#include "display.h"
#include "i2c.h"

uint8_t frameBuffer[1024];
uint8_t minX = 128;
uint8_t maxX = 0;
uint8_t minPage = 8;
uint8_t maxPage = 0;

void displayInit(void) {
    for (int i = 0; i < 1024; i++) frameBuffer[i] = 0;
    uint8_t init_cmds[] = {
        0x00,       // Control Byte: далі йдуть КОМАНДИ
        0xAE,       // Display OFF (вимикаємо екран на час налаштування)
        0xD5, 0x80, // Частота оновлення генератора
        0xA8, 0x3F, // Multiplex Ratio (висота екрана: 64 пікселі)
        0xD3, 0x00, // Без зсуву екрана
        0x40,       // Починати малювати з нульової лінії
        0x8D, 0x14, // УВІМКНУТИ Charge Pump (генератор високої напруги для OLED) - КРИТИЧНО!
        0x20, 0x00, // Горизонтальний режим адресації пам'яті (ідеально для тексту)
        0xA1,       // Розворот по X (щоб текст не був дзеркальним)
        0xC8,       // Розворот по Y (щоб екран не був догори дригом)
        0xDA, 0x12, // Налаштування апаратних пінів COM
        0x81, 0x7F, // Контрастність (яскравість)
        0xD9, 0xF1, // Pre-charge period (покращує чіткість)
        0xDB, 0x40, // VCOMH deselect level
        0xA4,       // Виводити дані з RAM пам'яті
        0xA6,       // Нормальні кольори (0 - чорний, 1 - світиться)
        0xAF        // Display ON (ПРОКИДАЙСЯ!)
    };

    i2cSend(DISPLAY_ADDR, init_cmds, sizeof(init_cmds));
}

void displayClear(void) {
    minPage = 0;
    maxPage = 7;
    minX = 0;
    maxX = 127;
    for (int i = 0; i < 1024; i++) {
        frameBuffer[i] = 0xff;
    }
    updateDisplay();
}

void setCursor(uint8_t x, uint8_t page) {
    uint8_t cmds[] = {
        0x00, 
        0x21, x, 127,  
        0x22, page, 7  
    };
    i2cSend(DISPLAY_ADDR, cmds, sizeof(cmds));
}

void setPixel(int x, int y, int color)
{
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;

    if (x < minX) minX = x;
    if (x > maxX) maxX = x;

    if (y/8 < minPage) minPage = y/8;
    if (y/8 > maxPage) maxPage = y/8;

    int index = (y/8)*128 + x;
    if (color) frameBuffer[index] |= (1 << (y % 8));
    else frameBuffer[index] &= ~(1 << (y % 8));
}

void drawVectorH(int x0, int y0, int x1, int y1,int color)
{
    if (x0 > x1) {x0 ^= x1; x1 ^= x0; x0 ^= x1;}

    while (x0 != x1+1) setPixel(x0++,y0,color);
}

void drawVectorV(int x0, int y0, int x1, int y1,int color)
{
    if (y0 > y1) {y0 ^= y1; y1 ^= y0; y0 ^= y1;}

    while (y0 != y1+1) setPixel(x0,y0++,color);
}

void drawVector(int x0, int y0, int x1, int y1,int color)
{
    if (y0 == y1)
    {
        drawVectorH(x0, y0, x1, y1, color);
        return;
    }
    else if (x0 == x1) 
    {
        drawVectorV(x0, y0, x1, y1, color);
        return;
    }
    int steep = 0;
    int abs_dx = (x1 - x0) < 0 ? (x0 - x1) : (x1 - x0);
    int abs_dy = (y1 - y0) < 0 ? (y0 - y1) : (y1 - y0);

    if (abs_dy > abs_dx)
    {
        x0 ^= y0; y0 ^= x0; x0 ^= y0;
        x1 ^= y1; y1 ^= x1; x1 ^= y1;
        steep = 1;
    }

    if (x0 > x1)
    {
        x0 ^= x1; x1 ^= x0; x0 ^= x1;
        y0 ^= y1; y1 ^= y0; y0 ^= y1;
    }

    int dx = x1 - x0;
    int dy = (y1 - y0) < 0 ? (y0 - y1) : (y1 - y0);
    
    int y = y0;
    int p = 2 * dy - dx;
    
    int dir = (y0 < y1) ? 1 : -1;

    if (steep) 
    {
        for (int i = 0; i <= dx; i++)
        {
            setPixel(y, x0 + i, color); 
            if (p >= 0) {
                y += dir;
                p -= 2 * dx;
            }
            p += 2 * dy;
        }
    } 
    else 
    {
        for (int i = 0; i <= dx; i++)
        {
            setPixel(x0 + i, y, color);
            if (p >= 0) {
                y += dir;
                p -= 2 * dx;
            }
            p += 2 * dy;
        }
    }
}

void drawChar(int x, int y,char c)
{
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;

    if (x < minX) minX = x;
    if (x+4 > maxX) maxX = x+4 > 127 ? 127 : x+4;

    if (y/8 < minPage) minPage = y/8;
    if ((y+5)/8 > maxPage) maxPage = ((y+5)/8 > 7 ? 7 : (y+5)/8);

    int shift = y%8;
    int page = y/8;

    if (shift > 3)
    {
        for (int i = 0; i < 3; i++)
        {
            if (x + i >= 128) break;
            frameBuffer[x + i + page*128] |= (FONT_DATA[(c - 0x20) * 3 + i] << shift);
            if (page < 7 )frameBuffer[x + i + page*128+128] |= (FONT_DATA[(c - 0x20) * 3 + i] >> (8- shift));
        }
    }
    else for (int i = 0; i < 3; i++) 
    {
        if (x + i >= 128) break;
        frameBuffer[x + i + (y/8)*128] |= (FONT_DATA[(c - 0x20) * 3 + i] << shift);
    }
}

void drawStr(int x, int y, char str[])
{
    int i = 0;
    while (str[i] != '\0')
    {
        drawChar(x + (i * 4),y,str[i++]);
    }
}

void drawRectangle(int x0, int y0, int w, int h,int color)
{
    drawVectorH(x0,y0,x0 + w,y0,color);
    drawVectorH(x0,y0 + h,x0 + w,y0 + h,color);
    drawVectorV(x0,y0,x0,y0 + h,color);
    drawVectorV(x0 + w,y0,x0 + w,y0 + h,color);
}


void updateDisplay(void)
{
    if (minX > maxX) return;

    i2cSend(DISPLAY_ADDR, (uint8_t []){0x00,0x21,minX,maxX,0x22,minPage,maxPage}, 7);

    int totalBytes = (maxX - minX + 1) * (maxPage - minPage + 1) + 1;

    static uint8_t cmdToSend[1025];
    cmdToSend[0] = 0x40;

    int index = 1;
    for (int i = minPage; i <= maxPage; i++)
    {
        for (int j = minX; j <= maxX; j++)
        {
            cmdToSend[index++] = frameBuffer[j + i*128]; 
        }
    }
    
    i2cSend(DISPLAY_ADDR, cmdToSend, totalBytes);

    minX = 128;
    maxX = 0;
    minPage = 8;
    maxPage = 0;
}