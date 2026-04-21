#include "i2c.h"
#include "rp2040.h"

void i2cInit(void)
{
    RESETS->RESET &= ~((1 << 3) | (1 << 5) | (1 << 8));

    while (!(RESETS->RESET_DONE & ((1 << 3) | (1 << 5) | (1 << 8)))) {
        __asm volatile("nop");
    }

    GPIO[4].CTRL = 3;
    GPIO[5].CTRL = 3;

    PADS_BANK0->PIN[4] |= (1 << 6) | (1 << 3);
    PADS_BANK0->PIN[5] |= (1 << 6) | (1 << 3);

    I2C->ENABLE = 0;

    I2C->CON = 0x63;
    I2C->SS_SCL_HCNT = 600;
    I2C->SS_SCL_LCNT = 600;
    I2C->ENABLE = 1;
}

void i2cSend(uint8_t target, const uint8_t *data, int length)
{
    __asm volatile ("cpsid i");
    I2C->ENABLE = 0;
    I2C->TAR = target;
    I2C->ENABLE = 1;

    for (int i = 0; i < length; i++)
    {
        while (I2C->TXFLR == 16) 
        {
            if (I2C->TX_ABRT_SOURCE != 0) {
                volatile uint32_t clear = I2C->CLR_TX_ABRT; 
                return;
            }
        }

        if (i == length - 1) I2C->DATA_CMD = data[i] | (1 << 9);
        else  I2C->DATA_CMD = data[i];
    }

    while (I2C->STATUS & 1) {
        if (I2C->TX_ABRT_SOURCE != 0) {
            volatile uint32_t clear = I2C->CLR_TX_ABRT;
            return;
        }
    }
    __asm volatile ("cpsie i");
}