#ifndef I2C_H
#define I2C_H

#include <stdint.h>

#define I2C0_BASE 0x40044000
typedef struct
{
    volatile uint32_t CON;          // 0x00
    volatile uint32_t TAR;          // 0x04
    volatile uint32_t SAR;          // 0x08
    volatile uint32_t HS_MADDR;     // 0x0C
    volatile uint32_t DATA_CMD;     // 0x10
    volatile uint32_t SS_SCL_HCNT;  // 0x14
    volatile uint32_t SS_SCL_LCNT;  // 0x18
    volatile uint32_t FS_SCL_HCNT;  // 0x1C
    volatile uint32_t FS_SCL_LCNT;  // 0x20
    uint32_t RESERV1[12];           // 0x24 - 0x50
    volatile uint32_t CLR_TX_ABRT;  // 0x54
    uint32_t RESERV2[5];            // 0x58 - 0x68
    volatile uint32_t ENABLE;       // 0x6C
    volatile uint32_t STATUS;       // 0x70
    volatile uint32_t TXFLR;        // 0x74
    volatile uint32_t RXFLR;        // 0x78
    volatile uint32_t SDA_HOLD;     // 0x7C
    volatile uint32_t TX_ABRT_SOURCE; // 0x80
} I2C_TypeDef;

#define I2C ((volatile I2C_TypeDef *) I2C0_BASE)

void i2cInit(void);
void i2cSend(uint8_t target, const uint8_t *data, int length);

#endif