//
// Created by HYX on 2026/11/16.
//

#ifndef PROJECT1_BSP_UART_H
#define PROJECT1_BSP_UART_H
#include<stdint.h>
#define BSP_UART_DEVICE_COUNT 6
#define BSP_UART_BUFFER_SIZE 512

typedef enum {
    E_UART_1,
    E_UART_2,
    E_UART_3,
    E_UART_5,
    E_UART_7,
    E_UART_10
} bsp_uart_e;


void bsp_uart_send(bsp_uart_e device, const uint8_t *data, const uint32_t len);
void bsp_uart_receive(bsp_uart_e device, const uint8_t *data, const uint32_t len);



#endif //PROJECT1_BSP_UART_H
