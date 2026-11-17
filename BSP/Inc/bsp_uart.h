//
// Created by HYX on 2026/11/16.
//

#ifndef PROJECT1_BSP_UART_H
#define PROJECT1_BSP_UART_H
#include <stdint.h>

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

typedef void (*uart_recv_callback_t)(bsp_uart_e device,const uint8_t *data,uint16_t len);

void bsp_uart_send(bsp_uart_e device, const uint8_t *data, uint32_t len);
void bsp_uart_receive(bsp_uart_e device, uint8_t *data, uint32_t len);
void bsp_uart_transmit_IT(bsp_uart_e device, const uint8_t *data, uint32_t len);
void bsp_uart_transmit_DMA(bsp_uart_e device, const uint8_t *data, uint32_t len);
void bsp_uartEx_init(bsp_uart_e device, uart_recv_callback_t cb);

#endif //PROJECT1_BSP_UART_H
