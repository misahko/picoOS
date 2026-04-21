#ifndef RP2040_H
#define CORE_H

#include <stdint.h>

#define SYST_CSR (*(volatile uint32_t *) 0xE000E010)
#define SYST_RVR (*(volatile uint32_t *) 0xE000E014)
#define SYST_CVR (*(volatile uint32_t *) 0xE000E018)

#define SCB_ICSR (*((volatile uint32_t *)0xE000ED04))
#define PENDSV_SET (1 << 28)

#define SCB_SHPR3 (*((volatile uint32_t *)0xE000ED20))

#define NONE 255

#define BLOCKED 0
#define READY 1

#define SMALL 0
#define MEDIUM 1
#define LARGE 2

typedef struct
{
    uint16_t queue;
} EVENT;

typedef struct 
{
    uint32_t *stackPointer;
    uint16_t sleepTime;
    uint16_t priority;
    uint8_t state;
    uint8_t index;
} TCB;

void eventEmit(EVENT *event);
void eventWait(EVENT *event);

void taskSleep(TCB *task);
void initTask(void (*task)(void), uint8_t priority, uint8_t size);

#define XOSC_BASE 0x40024000
typedef struct
{
    volatile uint32_t CTRL;      // Офсет: 0x00
    volatile uint32_t STATUS;    // Офсет: 0x04
    volatile uint32_t DORMANT;   // Офсет: 0x08 (Додали, щоб вирівняти пам'ять!)
    volatile uint32_t START_UP;  // Офсет: 0x0C
} XOSC_TypeDef;
#define XOSC ((XOSC_TypeDef *) XOSC_BASE)

#define CLOCKS_BASE 0x40008000
typedef struct 
{
    volatile uint32_t padding1[12];
    volatile uint32_t CLK_REF_CTRL;
    volatile uint32_t padding2[2];
    volatile uint32_t CLK_SYS_CTRL;
} CLOCKS_TypeDef;
#define CLOCKS ((CLOCKS_TypeDef *) CLOCKS_BASE)

#define PLL_SYS_BASE 0x40028000
typedef struct
{
    volatile uint32_t CS;
    volatile uint32_t PWR;
    volatile uint32_t FBDIV_INT;
    volatile uint32_t PRIM;
} PLL_SYS_TypeDef;
#define PLL_SYS ((PLL_SYS_TypeDef *) PLL_SYS_BASE)

void coreInit(void);

#endif