#ifndef CLOCKDRIVER_H
#define CLOCKDRIVER_H

#include <stdint.h>

enum class ClockBus
{
    AHB1,
    AHB2,
    APB1,
    APB2
};

void ClockDriver_init(void);

void EnablePeripheralClock(ClockBus bus, uint32_t mask);
void DisablePeripheralClock(ClockBus bus, uint32_t mask);

#endif // CLOCKDRIVER_H
