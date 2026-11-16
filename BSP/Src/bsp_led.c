#include "main.h"
#include "spi.h"
#include "bsp_led.h"

uint8_t ws_buf[24];
#define WS_0 0xC0
#define WS_1 0xF8
void WS2812_SetColor(uint8_t G, uint8_t R, uint8_t B)
{
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        if(G & (0x80 >> i))
            ws_buf[i] = WS_1;
        else
            ws_buf[i] = WS_0;
    }

    for(i = 0; i < 8; i++)
    {
        if(R & (0x80 >> i))
            ws_buf[8 + i] = WS_1;
        else
            ws_buf[8 + i] = WS_0;
    }

    for(i = 0; i < 8; i++)
    {
        if(B & (0x80 >> i))
            ws_buf[16 + i] = WS_1;
        else
            ws_buf[16 + i] = WS_0;
    }
    HAL_SPI_Transmit(&hspi6, ws_buf, 24, 100);
    HAL_Delay(1);
}