#include "./drivers/i2c.h"
#include "./drivers/rp2040.h"
#include "./drivers/display.h"


void main(void)
{
    i2cInit();
    displayInit();
    
    // 1. Змиваємо стартове сміття! Екран стане ідеально чорним.
    displayClear(); 
    
    int color = 1;
    while (1) {
        for (int i = 1; i < 120;i += 5)
        {
            drawVector(1 + i,30 + i/2,30 + i,30 + i/2,color);
            drawVector(1 + i,30 + i/2,15 + i,1 + i/2,color);
            drawVector(16 + i,1 + i/2,30+ i,30 + i/2,color);
            updateDisplay();
            //delay();
        }
        color = (color == 1 ? 0 : 1);
        
    }
}