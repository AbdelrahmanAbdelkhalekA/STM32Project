#ifndef PWMDRIVER_H
#define PWMDRIVER_H

#include <stdint.h>

void PWMDriver_Init(void);

void PWMDriver_SetDutyCycle(uint16_t dutyCycle);

#endif // PWMDRIVER_H
