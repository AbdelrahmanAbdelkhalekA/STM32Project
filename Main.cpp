#include <stdint.h>
#include <stdio.h>

#include "ClockDriver.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <cstdio>

extern "C" int main(void)
{
    ClockDriver_init();

    return 0;
}
