#include "./drivers/i2c.h"
#include "./drivers/rp2040.h"
#include "./drivers/display.h"
#include <math.h>

void main(void)
{
    i2cInit();
    displayInit();
    
    // 1. Змиваємо стартове сміття! Екран стане ідеально чорним.
    displayClear(); 

    int vertex[4][3] = {
        {-7, -7, -7}, 
        { 7, -7, -7}, 
        { 7,  7, -7}, 
        {-7,  7, -7}
    };

    int oldVertex[8][3] =
    {
        {-15, -15, 0}, 
        { 15, -15, 0}, 
        { 15,  15, 0}, 
        {-15,  15, 0},

        {-15, -15, 0}, 
        { 15, -15, 0}, 
        { 15,  15, 0}, 
        {-15,  15, 0},
    };

    int newVertex[8][3] =
    {
        {-15, -15, 0}, 
        { 15, -15, 0}, 
        { 15,  15, 0}, 
        {-15,  15, 0},

        {-15, -15, 0}, 
        { 15, -15, 0}, 
        { 15,  15, 0}, 
        {-15,  15, 0},
    };
    
    float deg = 0;

    float s;
    float c;

    float x,y,z;
    
    int next_i = 0;
    
    while (1) {
        s = sin(deg);
        c = cos(deg);
        for (int i = 0; i < 4; i++)
        {

            x = vertex[i][0];
            y = vertex[i][1] * c - vertex[i][2] * s;
            z = vertex[i][1] * s + vertex[i][2] * c;

            float new_x = x * c - z * s;
            float new_z = x * s + z * c;

            newVertex[i][0] = (new_x * 128) / (new_z + 100) + 64;
            newVertex[i][1] = (y * 128) / (new_z + 100) + 32; // Зробили центр 32
            
        }

        for (int i = 0; i < 4; i++)
        {

            x = vertex[i][0];
            y = vertex[i][1] * c - (vertex[i][2]-15) * s;
            z = vertex[i][1] * s + (vertex[i][2]-15) * c;

            // Зберігаємо нові координати безпечно!
            float new_x = x * c - z * s;
            float new_z = x * s + z * c;

            newVertex[i + 4][0] = (new_x * 128) / (new_z + 100) + 64;
            newVertex[i + 4][1] = (y * 128) / (new_z + 100) + 32; // Зробили центр 32
            
        }
        for (int i = 0; i < 4; i++)
        {
            next_i = (i + 1) % 4;

            drawVector(oldVertex[i][0],oldVertex[i][1],oldVertex[next_i][0],oldVertex[next_i][1],0);
            drawVector(newVertex[i][0],newVertex[i][1],newVertex[next_i][0],newVertex[next_i][1], 1);

            drawVector(oldVertex[i+4][0],oldVertex[i + 4][1],oldVertex[next_i + 4][0],oldVertex[next_i + 4][1],0);
            drawVector(newVertex[i+4][0],newVertex[i + 4][1],newVertex[next_i + 4][0],newVertex[next_i + 4][1], 1);

            drawVector(oldVertex[i][0],oldVertex[i][1],oldVertex[i + 4][0],oldVertex[i + 4][1], 0);
            drawVector(newVertex[i][0],newVertex[i][1],newVertex[i + 4][0],newVertex[i + 4][1], 1);
        }

        for (int i = 0; i < 8; i++)
        {
            oldVertex[i][0] = newVertex[i][0];
            oldVertex[i][1] = newVertex[i][1];
        }
        updateDisplay();
        deg += 0.1;
    } 
}