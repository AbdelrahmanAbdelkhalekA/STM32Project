#include "GPIOConfigurationDriver.h"

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "ClockDriver.h"

void GPIOConfigurationDriver_InitPWMPin(void)
{
    // TIM1 CH1 PWM: Set PE9 to be output of TIM1.
    EnablePeripheralClock(ClockBus::AHB1, RCC_AHB1Periph_GPIOA);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_8;
    gpio.GPIO_Mode  = GPIO_Mode_AF;
    gpio.GPIO_OType = GPIO_OType_PP;
    gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM1);
}

void GPIOConfigurationDriver_InitI2C1Pins(void)
{
    // I2C1 GPIO: PB6 = SCL, PB7 = SDA (AF4), open-drain with pull-up.
    EnablePeripheralClock(ClockBus::AHB1, RCC_AHB1Periph_GPIOB);

    GPIO_InitTypeDef i2c_gpio;
    GPIO_StructInit(&i2c_gpio);
    i2c_gpio.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
    i2c_gpio.GPIO_Mode  = GPIO_Mode_AF;
    i2c_gpio.GPIO_OType = GPIO_OType_OD;
    i2c_gpio.GPIO_PuPd  = GPIO_PuPd_UP;
    i2c_gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &i2c_gpio);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1);
}

void GPIOConfigurationDriver_InitAnalogPins(void){
    // TIM1 CH1 PWM: Set PE9 to be output of TIM1.
    EnablePeripheralClock(ClockBus::AHB1, RCC_AHB1Periph_GPIOA);

    GPIO_InitTypeDef gpio;
    GPIO_StructInit(&gpio);
    gpio.GPIO_Pin   = GPIO_Pin_6;
    gpio.GPIO_Mode  = GPIO_Mode_AN;
    gpio.GPIO_OType = GPIO_OType_OD;
    gpio.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);
}


void GPIOConfigurationDriver_Init(void)
{
    GPIOConfigurationDriver_InitPWMPin();
    GPIOConfigurationDriver_InitI2C1Pins();
    GPIOConfigurationDriver_InitAnalogPins();
}
