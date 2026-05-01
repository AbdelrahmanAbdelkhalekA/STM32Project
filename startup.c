#include <stdint.h>

// Core exception prototypes.
void Reset_Handler(void);
void Default_Handler(void);

void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManage_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
void DebugMon_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));

// Peripheral interrupt handlers (weak aliases so the application can override as needed).
void WWDG_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void PVD_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TAMP_STAMP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RCC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void ADC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI9_5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_BRK_TIM9_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_UP_TIM10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_TRG_COM_TIM11_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM1_CC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void EXTI15_10_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void RTC_Alarm_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_WKUP_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA1_Stream7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SDIO_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void SPI3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void UART5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM6_DAC_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void TIM7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream0_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream1_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream2_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream3_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream4_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void OTG_FS_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream5_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void DMA2_Stream7_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void USART6_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_EV_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void I2C3_ER_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));
void FPU_IRQHandler(void) __attribute__((weak, alias("Default_Handler")));

extern uint32_t _estack;
extern uint32_t _sidata;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;

void SystemInit(void) __attribute__((weak));
void SystemInit(void) {}

// Vector table placed at flash base. Reserved slots left as 0 (hard fault if ever hit).
__attribute__((section(".isr_vector"), used)) void (*const g_pfnVectors[])(void) = {
    [0] = (void (*)(void))(&_estack),
    [1] = Reset_Handler,
    [2] = NMI_Handler,
    [3] = HardFault_Handler,
    [4] = MemManage_Handler,
    [5] = BusFault_Handler,
    [6] = UsageFault_Handler,
    [11] = SVC_Handler,
    [12] = DebugMon_Handler,
    [14] = PendSV_Handler,
    [15] = SysTick_Handler,

    [16] = WWDG_IRQHandler,
    [17] = PVD_IRQHandler,
    [18] = TAMP_STAMP_IRQHandler,
    [19] = RTC_WKUP_IRQHandler,
    [20] = FLASH_IRQHandler,
    [21] = RCC_IRQHandler,
    [22] = EXTI0_IRQHandler,
    [23] = EXTI1_IRQHandler,
    [24] = EXTI2_IRQHandler,
    [25] = EXTI3_IRQHandler,
    [26] = EXTI4_IRQHandler,
    [27] = DMA1_Stream0_IRQHandler,
    [28] = DMA1_Stream1_IRQHandler,
    [29] = DMA1_Stream2_IRQHandler,
    [30] = DMA1_Stream3_IRQHandler,
    [31] = DMA1_Stream4_IRQHandler,
    [32] = DMA1_Stream5_IRQHandler,
    [33] = DMA1_Stream6_IRQHandler,
    [34] = ADC_IRQHandler,
    [40] = EXTI9_5_IRQHandler,
    [41] = TIM1_BRK_TIM9_IRQHandler,
    [42] = TIM1_UP_TIM10_IRQHandler,
    [43] = TIM1_TRG_COM_TIM11_IRQHandler,
    [44] = TIM1_CC_IRQHandler,
    [45] = TIM2_IRQHandler,
    [46] = TIM3_IRQHandler,
    [47] = TIM4_IRQHandler,
    [48] = I2C1_EV_IRQHandler,
    [49] = I2C1_ER_IRQHandler,
    [50] = I2C2_EV_IRQHandler,
    [51] = I2C2_ER_IRQHandler,
    [52] = SPI1_IRQHandler,
    [53] = SPI2_IRQHandler,
    [54] = USART1_IRQHandler,
    [55] = USART2_IRQHandler,
    [56] = USART3_IRQHandler,
    [57] = EXTI15_10_IRQHandler,
    [58] = RTC_Alarm_IRQHandler,
    [59] = OTG_FS_WKUP_IRQHandler,
    [63] = DMA1_Stream7_IRQHandler,
    [65] = SDIO_IRQHandler,
    [66] = TIM5_IRQHandler,
    [67] = SPI3_IRQHandler,
    [68] = UART4_IRQHandler,
    [69] = UART5_IRQHandler,
    [70] = TIM6_DAC_IRQHandler,
    [71] = TIM7_IRQHandler,
    [72] = DMA2_Stream0_IRQHandler,
    [73] = DMA2_Stream1_IRQHandler,
    [74] = DMA2_Stream2_IRQHandler,
    [75] = DMA2_Stream3_IRQHandler,
    [76] = DMA2_Stream4_IRQHandler,
    [83] = OTG_FS_IRQHandler,
    [84] = DMA2_Stream5_IRQHandler,
    [85] = DMA2_Stream6_IRQHandler,
    [86] = DMA2_Stream7_IRQHandler,
    [87] = USART6_IRQHandler,
    [88] = I2C3_EV_IRQHandler,
    [89] = I2C3_ER_IRQHandler,
    [97] = FPU_IRQHandler,
};

static void initialize_data(void);
static void zero_bss(void);

void Reset_Handler(void)
{
    initialize_data();
    zero_bss();
    SystemInit();

    extern int main(void);
    (void)main();

    while (1)
    {
        // Halt if main returns.
    }
}

static void initialize_data(void)
{
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata)
    {
        *dst++ = *src++;
    }
}

static void zero_bss(void)
{
    uint32_t *dst = &_sbss;
    while (dst < &_ebss)
    {
        *dst++ = 0u;
    }
}

void Default_Handler(void)
{
    while (1)
    {
    }
}
