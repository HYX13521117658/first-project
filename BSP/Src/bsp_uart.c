#include "main.h"
#include"bsp_uart.h"
#include"usart.h"
#include<stdio.h>


 static UART_HandleTypeDef *handle[BSP_UART_DEVICE_COUNT] = {
     [E_UART_1] = &huart1,
    //  [E_UART_2] = &huart2,
    //  [E_UART_3] = &huart3,
    //  [E_UART_5] = &huart5,
    // [E_UART_7] = &huart7,
    // [E_UART_10] = &huart10
 };

void bsp_uart_send(bsp_uart_e device, const uint8_t *data, const uint32_t len)
{
    if(0 <= device && device < BSP_UART_DEVICE_COUNT && data != NULL && len > 0);
    HAL_UART_Transmit(handle[device], data, len, HAL_MAX_DELAY);
}


void bsp_uart_receive(bsp_uart_e device, const uint8_t *data, const uint32_t len)
{
    if(0 <= device && device < BSP_UART_DEVICE_COUNT && data != NULL && len > 0);
    HAL_UART_Receive(handle[device], data, len, HAL_MAX_DELAY);
}
