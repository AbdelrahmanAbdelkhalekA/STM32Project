#ifndef GPIOCONFIGURATIONDRIVER_H
#define GPIOCONFIGURATIONDRIVER_H

#include <stdint.h>

// High-level init that calls the more granular helpers below.
void GPIOConfigurationDriver_Init(void);

// Configure a simple debug/test output pin (currently PA8).
void GPIOConfigurationDriver_InitPWMPin(void);

// Configure I2C1 pins (currently PB6 = SCL, PB7 = SDA).
void GPIOConfigurationDriver_InitI2C1Pins(void);

// Configure analog pins (currently PB6 = SCL, PB7 = SDA).
void GPIOConfigurationDriver_InitAnalogPins(void);

#endif // GPIOCONFIGURATIONDRIVER_H
