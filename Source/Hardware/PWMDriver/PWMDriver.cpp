#include "PWMDriver.h"
#include "stm32f4xx.h"
#include "stm32f4xx_rcc.h"
#include "ClockDriver.h"

void PWMDriver_Init(void)
{
    /*
     * Configure TIM1 Channel 1 to generate a 50 Hz PWM (20 ms period)
     * with a 1 µs time base and CCR1 as the duty control register.
     *
     * Assumptions (from ClockDriver):
     * - SYSCLK = 64 MHz
     * - APB2 prescaler = 1  => PCLK2 = 64 MHz
     * - APB2 timer clock rule (PPRE2 = 1): TIM1CLK = PCLK2 = 64 MHz
     *
     * PWM math (edge-aligned, upcounting):
     *   f_cnt   = TIM1CLK / (PSC + 1)
     *   f_pwm   = TIM1CLK / ((PSC + 1) * (ARR + 1))
     *   pulse   = CCR1 counts (in f_cnt ticks)
     *
     * Here we target f_cnt = 1 MHz (1 µs per tick) and f_pwm = 50 Hz:
     *   PSC = 64 MHz / 1 MHz - 1 = 63
     *   ARR = 1 MHz / 50 Hz - 1 = 19,999
     *   Example duty: CCR1 = 1000 → 1,000 µs high pulse
     */

    // Enable TIM1 clock on APB2.
    EnablePeripheralClock(ClockBus::APB2, RCC_APB2Periph_TIM1);

    // 0) Put the timer in a known, safe state while configuring
    TIM1->CR1 &= ~TIM_CR1_CEN;    // stop counter
    TIM1->CCER &= ~TIM_CCER_CC1E; // disable CH1 output to avoid glitches during setup

    // 1) Time base: 1 µs tick at 64 MHz
    TIM1->PSC = 63;        // 64 MHz / (63 + 1) = 1 MHz
    TIM1->ARR = 20000 - 1; // 20 ms period (50 Hz) → 19,999

    // 2) Initial duty: 1000 us pulse (ESC min throttle).
    TIM1->CCR1 = 1000;

    // 3) Channel configuration: PWM mode 1 on CH1 with preload
    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;                       // clear mode
    TIM1->CCMR1 |= (TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2); // OC1M = 110 (PWM mode 1)
    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;                       // enable CCR1 preload (shadow register)

    // 4) Enable ARR preload (buffering ARR, transferred on update)
    TIM1->CR1 |= TIM_CR1_ARPE;

    // 5) Push preloaded values (PSC/ARR/CCR) to the active registers before enabling
    TIM1->EGR = TIM_EGR_UG; // generate an update event

    // 6) Output stage: enable main output (advanced timer) and CH1 output
    TIM1->BDTR |= TIM_BDTR_MOE;  // main output enable
    TIM1->CCER |= TIM_CCER_CC1E; // CH1 enable, active high (default polarity)

    // 7) Start the counter
    TIM1->CR1 |= TIM_CR1_CEN;
}

void PWMDriver_SetDutyCycle(uint16_t dutyCycle)
{
    if (dutyCycle < 1000U)
    {
        dutyCycle = 1000U;
    }
    else if (dutyCycle > 2000U)
    {
        dutyCycle = 2000U;
    }

    TIM1->CCR1 = dutyCycle;
}
