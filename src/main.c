#include "./drivers/i2c.h"
#include "./drivers/rp2040.h"
#include "./drivers/display.h"


void main(void)
{
    i2cInit();
    displayInit();
    
    // 1. Змиваємо стартове сміття! Екран стане ідеально чорним.
    displayClear(); 

    /*
    drawStr(0,0,"< 11x");
    drawStr(127 - 4*4,0,"9x >");
    drawStr(56,0,"pico");
    drawVector(0,6,127,6,1);
    updateDisplay();
    */

    int color = 1;
    
    while (1) {
        for (int i = 0; i < 128; i++)
        { 
            drawVector(i,0,127-i,63,color);
            updateDisplay();
        }

        for (int i = 63; i >= 0;i++)
        {
            drawVector(0,i,127,63-i,color);
            updateDisplay();
        }
        color = color == 1 ? 0 : 1;
    }
}