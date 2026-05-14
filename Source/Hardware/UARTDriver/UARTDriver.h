#ifndef UARTDRIVER_H
#define UARTDRIVER_H

#include <stdint.h>

void UARTDriver_Init(void);
void UARTDriver_WriteChar(char value);
void UARTDriver_WriteString(const char *text);
int UARTDriver_ReadChar(void);

#endif // UARTDRIVER_H
