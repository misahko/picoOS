#ifndef RP2040_H
#define RP2040_H

#include <stdint.h>

#define RESETS_BASE 0x4000c000
typedef struct 
{
    volatile uint32_t RESET;
    volatile uint32_t WDSEL;
    volatile uint32_t RESET_DONE;
} RESETS_TypeDef;
#define RESETS ((RESETS_TypeDef *) RESETS_BASE)

#define IO_BANK_BASE 0x40014000
typedef struct
{
    volatile uint32_t STATUS;
    volatile uint32_t CTRL;
} GPIO_TypeDef;
#define GPIO ((GPIO_TypeDef *) IO_BANK_BASE)

#define SIO_BASE 0xd0000000
typedef struct
{
    volatile uint32_t CPUID;       // 0x00
    volatile uint32_t GPIO_IN;     // 0x04
    volatile uint32_t HI_IN;       // 0x08
    volatile uint32_t padding;     // 0x0C (цей регістр не використовуємо, тому пропускаємо 4 байти)
    volatile uint32_t GPIO_OUT;    // 0x10
    volatile uint32_t GPIO_OUT_SET;// 0x14
    volatile uint32_t GPIO_OUT_CLR;// 0x18
    volatile uint32_t GPIO_OUT_XOR;// 0x1C
    volatile uint32_t GPIO_OE;     // 0x20
    volatile uint32_t GPIO_OE_SET; // 0x24
    volatile uint32_t GPIO_OE_CLR; // 0x28
    volatile uint32_t GPIO_OE_XOR; // 0x2C
} SIO_TypeDef;
#define SIO ((SIO_TypeDef *) SIO_BASE)

#define PADS_BANK0_BASE 0x4001c000

typedef struct {
    volatile uint32_t VOLTAGE_SELECT; // Загальний для блоку
    volatile uint32_t PIN[30];       // Регістри для кожної ніжки
} PADS_BANK0_TypeDef;

#define PADS_BANK0 ((PADS_BANK0_TypeDef *) PADS_BANK0_BASE)

#endif