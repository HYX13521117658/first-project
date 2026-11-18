//
// Created by HYX on 2026/11/16.
//

#ifndef PROJECT1_BSP_UART_H
#define PROJECT1_BSP_UART_H
#include <stdint.h>
#include "stm32h7xx_hal_uart.h"



#define DMA_RX_BUF_SIZE     128
#define FIFO_RX_SIZE        256
#define FIFO_TX_SIZE        128
#define FRAME_MAX_LEN       100


typedef enum {
    E_UART_1,
    E_UART_2,
    E_UART_3,
    E_UART_5,
    E_UART_7,
    E_UART_10,
    BSP_UART_MAX
} bsp_uart_e;

typedef struct
{
    uint8_t *buf;
    uint16_t wr;
    uint16_t rd;
    uint16_t size;
} fifo_t;

void bsp_uart_send(bsp_uart_e device, const uint8_t *data, uint32_t len);
void bsp_uart_receive(bsp_uart_e device, uint8_t *data, uint32_t len);
void bsp_uart_transmit_IT(bsp_uart_e device, const uint8_t *data, uint32_t len);
void bsp_uart_transmit_DMA(bsp_uart_e device, const uint8_t *data, uint32_t len);

typedef struct
{
    UART_HandleTypeDef *huart;
    uint8_t rx_dma_buf[DMA_RX_BUF_SIZE];
    fifo_t rx_fifo;
    uint8_t rx_fifo_buf[FIFO_RX_SIZE];
    fifo_t tx_fifo;
    uint8_t tx_fifo_buf[FIFO_TX_SIZE];
    uint8_t tx_busy;
} uart_ctx_t;

int8_t  bsp_uart_init(bsp_uart_e uart_idx, UART_HandleTypeDef *huart);
int16_t bsp_uart_fifo_DMA_send(bsp_uart_e uart_idx, const uint8_t *data, uint16_t len);
int16_t bsp_uart_fifo_read(bsp_uart_e uart_idx, uint8_t *data, uint16_t len);
uint16_t bsp_uart_get_rx_len(bsp_uart_e uart_idx);
#endif
