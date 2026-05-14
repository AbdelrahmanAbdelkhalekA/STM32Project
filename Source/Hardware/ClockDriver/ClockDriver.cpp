#include "ClockDriver.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "FreeRTOSConfig.h"
#include <cstdio>

extern "C" {
uint32_t SystemCoreClock = 16000000U;
}

// ======= Your preferred macro-bit style =======
// PLL source = HSE (8 MHz)
// Choose: VCO_in = 8/8 = 1 MHz
//         VCO_out = 1 * 384 = 384 MHz
//         SYSCLK  = 384 / 6 = 64 MHz (PLLP = /6)
//         PLL48CK = 384 / 8 = 48 MHz (for USB/SDIO)

// M = 8  -> set bit 3 in PLLM field
#define M_DIVIDER_PLL (RCC_PLLCFGR_PLLM_3)
// N = 384 -> set bits 7 and 8 in PLLN field
#define N_MULTIPLIER_PLL (RCC_PLLCFGR_PLLN_7 | RCC_PLLCFGR_PLLN_8)
// P = /6  -> encoding '10' in PLLP[1:0] -> set _1, clear _0
#define P_DIVIDER_PLL_DIV6 (RCC_PLLCFGR_PLLP_1)
// Q = 8  -> set bit 3 in PLLQ field (gives 48 MHz)
#define Q_DIVIDER_PLL (RCC_PLLCFGR_PLLQ_3)

// Optional mask you mentioned
#define DISABLE_HSE_HSI_MASK (~(RCC_CR_HSEON | RCC_CR_HSION))

static inline void wait_clear(volatile uint32_t *reg, uint32_t mask)
{
    while ((*reg & mask) != 0U)
    { /* wait */
    }
}

void ClockDriver_init(void)
    {
        // --- Ensure Power interface clock ON and set VOS = Scale 1 (high performance) ---
        RCC->APB1ENR |= RCC_APB1ENR_PWREN;
        // F411 uses PWR->CR VOS bit: VOS=1 => Scale 1
        PWR->CR |= PWR_CR_VOS;

        // --- 0) Safe baseline: make sure we run from HSI before reconfiguring ---
        if ((RCC->CR & RCC_CR_HSIRDY) == 0U)
        {
            RCC->CR |= RCC_CR_HSION;
            while ((RCC->CR & RCC_CR_HSIRDY) == 0U)
            { /* wait */
            }
        }

        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_HSI;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI)
        { /* wait */
        }

        // --- 1) Clean slate: turn off PLL & HSE before configuring ---
        if (RCC->CR & RCC_CR_PLLON)
        {
            RCC->CR &= ~RCC_CR_PLLON;
            wait_clear(&RCC->CR, RCC_CR_PLLRDY);
        }

        // --- 2) Enable HSE (crystal). If external clock (not crystal), set HSEBYP before HSEON ---
        // RCC->CR |= RCC_CR_HSEBYP; // <-- uncomment ONLY if feeding a digital clock on OSC_IN
        RCC->CR |= RCC_CR_HSEON;
        while ((RCC->CR & RCC_CR_HSERDY) == 0U)
        { /* wait */
        }

        // Optional: Clock Security System
        RCC->CR |= RCC_CR_CSSON;

        // --- 3) Configure PLL (bit-macro style, no positional shifts) ---
        RCC->PLLCFGR = 0;                       // start from a clean field
        RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE; // source = HSE
        RCC->PLLCFGR |= M_DIVIDER_PLL;          // M = 8  (VCO_in = 1 MHz)
        RCC->PLLCFGR |= N_MULTIPLIER_PLL;       // N = 384 (VCO_out = 384 MHz)
        RCC->PLLCFGR |= P_DIVIDER_PLL_DIV6;     // P = /6  (SYSCLK = 64 MHz)
        RCC->PLLCFGR |= Q_DIVIDER_PLL;          // Q = 8   (PLL48CK = 48 MHz)

        // --- 4) Prescalers: HCLK=64, PCLK1=32 (<=50 max), PCLK2=64 (<=100 max) ---
        uint32_t cfgr = RCC->CFGR;
        cfgr &= ~(RCC_CFGR_HPRE | RCC_CFGR_PPRE1 | RCC_CFGR_PPRE2);
        cfgr |= RCC_CFGR_HPRE_DIV1;  // AHB = 64
        cfgr |= RCC_CFGR_PPRE1_DIV2; // APB1 = 32
        cfgr |= RCC_CFGR_PPRE2_DIV1; // APB2 = 64
        RCC->CFGR = cfgr;

        // --- 5) FLASH latency & caches ---
        // STM32F411RE (VDD >= 2.7V, VOS=Scale1):
        // Table allows 1 WS up to 64 MHz. Enable caches + prefetch.
        FLASH->ACR = FLASH_ACR_LATENCY_1WS | FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_PRFTEN;
        while ((FLASH->ACR & FLASH_ACR_LATENCY) != FLASH_ACR_LATENCY_1WS)
        { /* wait */
        }

        // --- 6) Enable PLL and wait ready ---
        RCC->CR |= RCC_CR_PLLON;
        while ((RCC->CR & RCC_CR_PLLRDY) == 0U)
        { /* wait */
        }

        // --- 7) Switch SYSCLK to PLL ---
        RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW) | RCC_CFGR_SW_PLL;
        while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
        { /* wait */
        }

        SystemCoreClock = 64000000U;

        // Optional: power tidy-up (turn off HSI if unused)
        // RCC->CR &= ~RCC_CR_HSION;
    }

void EnablePeripheralClock(ClockBus bus, uint32_t reg)
{
    switch (bus)
    {
    case ClockBus::AHB1:
        RCC->AHB1ENR |= reg;
        break;
    case ClockBus::AHB2:
        RCC->AHB2ENR |= reg;
        break;
    case ClockBus::APB1:
        RCC->APB1ENR |= reg;
        break;
    case ClockBus::APB2:
        RCC->APB2ENR |= reg;
        break;
    }
}

void DisablePeripheralClock(ClockBus bus, uint32_t reg)
{
    switch (bus)
    {
    case ClockBus::AHB1:
        RCC->AHB1ENR &= ~reg;
        break;
    case ClockBus::AHB2:
        RCC->AHB2ENR &= ~reg;
        break;
    case ClockBus::APB1:
        RCC->APB1ENR &= ~reg;
        break;
    case ClockBus::APB2:
        RCC->APB2ENR &= ~reg;
        break;
    }
}
