#include <stdint.h>
#include <stdlib.h>

#include "ClockDriver.h"
#include "GPIOConfigurationDriver.h"
#include "PWMDriver.h"
#include "UARTDriver.h"

#include "FreeRTOS.h"
#include "task.h"

#include "stm32f4xx_conf.h"

static constexpr uint16_t ESC_MIN_PULSE_US = 1000U;
static constexpr uint16_t ESC_MAX_PULSE_US = 2000U;
static constexpr uint16_t ESC_ARM_PULSE_US = 1000U;
static constexpr uint16_t UART_TASK_STACK_WORDS = 256U;

static StaticTask_t uartTaskTcb;
static StackType_t uartTaskStack[UART_TASK_STACK_WORDS];

static uint16_t clampPulseUs(uint16_t pulseUs)
{
    if (pulseUs < ESC_MIN_PULSE_US)
    {
        return ESC_MIN_PULSE_US;
    }

    if (pulseUs > ESC_MAX_PULSE_US)
    {
        return ESC_MAX_PULSE_US;
    }

    return pulseUs;
}

static void writeUnsigned(uint32_t value)
{
    char buffer[11];
    char *cursor = &buffer[sizeof(buffer) - 1];
    *cursor = '\0';

    do
    {
        cursor--;
        *cursor = static_cast<char>('0' + (value % 10U));
        value /= 10U;
    } while (value != 0U);

    UARTDriver_WriteString(cursor);
}

static void applyEscPulse(uint16_t pulseUs)
{
    const uint16_t clampedPulseUs = clampPulseUs(pulseUs);
    PWMDriver_SetDutyCycle(clampedPulseUs);

    UARTDriver_WriteString("\r\nESC pulse: ");
    writeUnsigned(clampedPulseUs);
    UARTDriver_WriteString(" us\r\n> ");
}

static void printHelp(void)
{
    UARTDriver_WriteString("\r\nESC PWM test\r\n");
    UARTDriver_WriteString("Send 1000..2000 for raw pulse width in us\r\n");
    UARTDriver_WriteString("Send p0..p100 for percent throttle mapped to 1000..2000 us\r\n");
    UARTDriver_WriteString("Send stop to command 1000 us\r\n> ");
}

static void handleCommand(char *command)
{
    while ((*command == ' ') || (*command == '\t'))
    {
        command++;
    }

    if (command[0] == '\0')
    {
        UARTDriver_WriteString("> ");
        return;
    }

    if ((command[0] == 's') && (command[1] == 't') && (command[2] == 'o') &&
        (command[3] == 'p') && (command[4] == '\0'))
    {
        applyEscPulse(ESC_ARM_PULSE_US);
        return;
    }

    if ((command[0] == 'p') || (command[0] == 'P'))
    {
        const long percent = strtol(&command[1], nullptr, 10);
        if ((percent >= 0L) && (percent <= 100L))
        {
            const uint16_t pulseUs =
                static_cast<uint16_t>(ESC_MIN_PULSE_US + ((percent * 1000L) / 100L));
            applyEscPulse(pulseUs);
            return;
        }
    }
    else
    {
        const long pulseUs = strtol(command, nullptr, 10);
        if ((pulseUs >= ESC_MIN_PULSE_US) && (pulseUs <= ESC_MAX_PULSE_US))
        {
            applyEscPulse(static_cast<uint16_t>(pulseUs));
            return;
        }
    }

    UARTDriver_WriteString("\r\nInvalid command\r\n> ");
}

static void UartEscCommandTask(void *parameters)
{
    (void)parameters;

    char commandBuffer[16];
    uint32_t commandLength = 0;

    applyEscPulse(ESC_ARM_PULSE_US);
    printHelp();

    for (;;)
    {
        const int received = UARTDriver_ReadChar();
        if (received < 0)
        {
            vTaskDelay(pdMS_TO_TICKS(10));
            continue;
        }

        const char character = static_cast<char>(received);
        if ((character == '\r') || (character == '\n'))
        {
            commandBuffer[commandLength] = '\0';
            handleCommand(commandBuffer);
            commandLength = 0;
        }
        else if ((character == '\b') || (character == 0x7F))
        {
            if (commandLength > 0U)
            {
                commandLength--;
                UARTDriver_WriteString("\b \b");
            }
        }
        else if (commandLength < (sizeof(commandBuffer) - 1U))
        {
            commandBuffer[commandLength] = character;
            commandLength++;
            UARTDriver_WriteChar(character);
        }
    }
}

extern "C" int main(void)
{
    ClockDriver_init();
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    GPIOConfigurationDriver_InitPWMPin();
    PWMDriver_Init();
    UARTDriver_Init();

    xTaskCreateStatic(UartEscCommandTask,
                      "esc",
                      UART_TASK_STACK_WORDS,
                      nullptr,
                      tskIDLE_PRIORITY + 1U,
                      uartTaskStack,
                      &uartTaskTcb);

    vTaskStartScheduler();

    for (;;)
    {
    }
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t task, char *taskName)
{
    (void)task;
    (void)taskName;

    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

extern "C" void vApplicationMallocFailedHook(void)
{
    taskDISABLE_INTERRUPTS();
    for (;;)
    {
    }
}

extern "C" void vApplicationGetIdleTaskMemory(StaticTask_t **taskTcb,
                                               StackType_t **taskStack,
                                               uint32_t *taskStackSize)
{
    static StaticTask_t idleTaskTcb;
    static StackType_t idleTaskStack[configMINIMAL_STACK_SIZE];

    *taskTcb = &idleTaskTcb;
    *taskStack = idleTaskStack;
    *taskStackSize = configMINIMAL_STACK_SIZE;
}
