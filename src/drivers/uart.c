#include "uart.h"
#include "rp2040.h"

void UARTInit(void)
{
    uint32_t mask = (1 << 5) | (1 << 22);

    RESETS->RESET &= ~mask;
    while ((RESETS->RESET_DONE & mask) != mask) {}

    GPIO[0].CTRL = 2;
    //gpio[1].CTRL = 2;
    
    UART->IBRD = 42;
    UART->FBRD = 20;
    UART->LCR_H = 0x70;
    UART->CR = ((1 << 8) | 1);
}

void UARTSend(char str[])
{
    int i = 0;
    while (str[i] != '\0')
    {
        while (UART->FLAG & (1 << 5)) {}
        UART->DATA = str[i];
        i++;
    }
    
}