#ifndef UART_DRIVAER
#define UART_DRIVER

#include <stdint.h>

#define UART0_BASE 0x40034000
typedef struct 
{
    volatile uint32_t DATA; //0x000
    volatile uint32_t RSR; //0x004
    uint32_t RESERVED0[4]; //
    volatile uint32_t FLAG;
    uint32_t RESERVED1;
    volatile uint32_t LPR;
    volatile uint32_t IBRD;
    volatile uint32_t FBRD;
    volatile uint32_t LCR_H;
    volatile uint32_t CR;
} UART_TypeDef;
#define UART ((UART_TypeDef *) UART0_BASE)

void uartInit(void);
void uartSend(void);

#endif