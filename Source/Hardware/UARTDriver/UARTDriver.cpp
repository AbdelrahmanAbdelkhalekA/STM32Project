#include "UARTDriver.h"

#include "ClockDriver.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"

void UARTDriver_Init(void)
{
    EnablePeripheralClock(ClockBus::AHB1, RCC_AHB1Periph_GPIOA);
    EnablePeripheralClock(ClockBus::APB1, RCC_APB1Periph_USART2);

    GPIO_InitTypeDef gpio = {};
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_AF;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    USART_InitTypeDef usart = {};
    USART_StructInit(&usart);
    usart.USART_BaudRate = 9600;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &usart);

    USART_Cmd(USART2, ENABLE);
}

void UARTDriver_WriteChar(char value)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
    {
    }

    USART_SendData(USART2, static_cast<uint16_t>(value));
}

void UARTDriver_WriteString(const char *text)
{
    while (*text != '\0')
    {
        UARTDriver_WriteChar(*text);
        text++;
    }
}

int UARTDriver_ReadChar(void)
{
    if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == RESET)
    {
        return -1;
    }

    return static_cast<int>(USART_ReceiveData(USART2) & 0xFFU);
}
