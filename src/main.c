#include "./drivers/i2c.h"
#include "./drivers/display.h"
#include "./core/core.h"
#include "./src/drivers/rp2040.h"

void print(void)
{

    drawRectangle(1,1,5,5,1);
    updateDisplay();
    while (1) {
        for (volatile uint32_t i = 0; i < 5000000; i++) { __asm volatile("nop"); }


    }
}

void blink(void)
{
    while (1)
    {
        SIO->GPIO_OUT_XOR = (1 << 25);
        
        // Тимчасова затримка-пустушка (доки немає taskSleep)
        for (volatile uint32_t i = 0; i < 5000000; i++) { __asm volatile("nop"); }
    }
}


void main(void)
{
    // Ініціалізація світлодіода
    RESETS->RESET &= ~(1 << 5);
    while ((RESETS->RESET_DONE & (1 << 5)) == 0) {}
    GPIO[25].CTRL = 5;
    SIO->GPIO_OE_SET = (1 << 25);

    // УВІМКНУТИ світлодіод і зависнути
    //SIO->GPIO_OUT_SET = (1 << 25);

    //i2cInit();
    //for (volatile int i = 0; i < 100000; i++); // Маленька пауза
    //displayInit();
    for (volatile int i = 0; i < 100000; i++);
    //displayClear();
    initTask(blink,3,SMALL);
    //initTask(print,3,MEDIUM);
    coreInit();
}