#include "bsp_uart.h"
#include "usart.h"

static UART_HandleTypeDef *bsp_uart_get_handle(bsp_uart_e device)
{
    switch (device)
    {
        case E_UART_10:
            return &huart10;
         // case E_UART_5;
         // return &huart5;
        default:
            return NULL;
    }
}

void bsp_uart_send(bsp_uart_e device, const uint8_t *data, uint32_t len)
{
    UART_HandleTypeDef *uart_handle = bsp_uart_get_handle(device);

    if (uart_handle != NULL && data != NULL && len > 0U)
    {
        HAL_UART_Transmit(uart_handle, data, len, HAL_MAX_DELAY);
    }
}

void bsp_uart_receive(bsp_uart_e device, uint8_t *data, uint32_t len)
{
    UART_HandleTypeDef *uart_handle = bsp_uart_get_handle(device);

    if (uart_handle != NULL && data != NULL && len > 0U)
    {
        HAL_UART_Receive(uart_handle, data, len, HAL_MAX_DELAY);
    }
}

void bsp_uart_transmit_IT(bsp_uart_e device, const uint8_t *data, uint32_t len)
{
    UART_HandleTypeDef *uart_handle = bsp_uart_get_handle(device);

    if (uart_handle != NULL && data != NULL && len > 0U)
    {
        HAL_UART_Transmit_IT(uart_handle, data, len);
    }
}

void bsp_uart_transmit_DMA(bsp_uart_e device, const uint8_t *data, uint32_t len)
{
    UART_HandleTypeDef *uart_handle = bsp_uart_get_handle(device);

    if (uart_handle != NULL && data != NULL && len > 0U)
    {
        HAL_UART_Transmit_DMA(uart_handle, data, len);
    }
}




static uart_ctx_t uart_ctx[BSP_UART_MAX];

static void fifo_init(fifo_t *fifo, uint8_t *buf, uint16_t size)
{
    fifo->buf = buf;
    fifo->wr = 0;
    fifo->rd = 0;
    fifo->size = size;
}

static uint8_t fifo_write(fifo_t *fifo, uint8_t *data, uint16_t len)
{
    uint16_t used = (fifo->wr - fifo->rd + fifo->size) % fifo->size;
    if (used + len > fifo->size)
    {
        return 1;
    }
    for (uint16_t i = 0; i < len; i++)
    {
        fifo->buf[fifo->wr] = data[i];
        fifo->wr = (fifo->wr + 1) % fifo->size;
    }
    return 0;
}

static uint16_t fifo_read(fifo_t *fifo, uint8_t *data, uint16_t len)
{
    uint16_t cnt = 0;
    while (cnt < len && fifo->wr != fifo->rd)
    {
        data[cnt++] = fifo->buf[fifo->rd];
        fifo->rd = (fifo->rd + 1) % fifo->size;
    }
    return cnt;
}

static uint16_t fifo_get_used(fifo_t *fifo)
{
    return (fifo->wr - fifo->rd + fifo->size) % fifo->size;
}

static void uart_tx_start(uart_ctx_t *ctx)
{
    if (ctx->tx_busy)
        return;
    uint16_t used = fifo_get_used(&ctx->tx_fifo);
    if (used == 0)
        return;

    uint8_t temp[FIFO_TX_SIZE];
    uint16_t copy_len = fifo_read(&ctx->tx_fifo, temp, used);
    if (HAL_UART_Transmit_DMA(ctx->huart, temp, copy_len) == HAL_OK)
    {
        ctx->tx_busy = 1;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0; i < BSP_UART_MAX; i++)
    {
        if (uart_ctx[i].huart == huart)
        {
            uart_ctx_t *ctx = &uart_ctx[i];
            ctx->tx_busy = 0;
            uart_tx_start(ctx);
            break;
        }
    }
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t len)
{
    for (uint8_t i = 0; i < BSP_UART_MAX; i++)
    {
        if (uart_ctx[i].huart == huart)
        {
            uart_ctx_t *ctx = &uart_ctx[i];

            if (len > FRAME_MAX_LEN)
            {
                HAL_UARTEx_ReceiveToIdle_DMA(huart, ctx->rx_dma_buf, DMA_RX_BUF_SIZE);
                if (huart->hdmarx != NULL)
                {
                    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
                }
                break;
            }

            fifo_write(&ctx->rx_fifo, ctx->rx_dma_buf, len);

            HAL_UARTEx_ReceiveToIdle_DMA(huart, ctx->rx_dma_buf, DMA_RX_BUF_SIZE);
            if (huart->hdmarx != NULL)
            {
                __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
            }
            break;
        }
    }
}

int8_t bsp_uart_init(bsp_uart_e uart_idx, UART_HandleTypeDef *huart)
{
    if (uart_idx >= BSP_UART_MAX || huart == NULL)
        return -1;

    uart_ctx_t *ctx = &uart_ctx[uart_idx];
    ctx->huart = huart;
    ctx->tx_busy = 0;

    fifo_init(&ctx->rx_fifo, ctx->rx_fifo_buf, FIFO_RX_SIZE);
    fifo_init(&ctx->tx_fifo, ctx->tx_fifo_buf, FIFO_TX_SIZE);

    HAL_UARTEx_ReceiveToIdle_DMA(huart, ctx->rx_dma_buf, DMA_RX_BUF_SIZE);
    if (huart->hdmarx != NULL)
    {
        __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
    }
    return 0;
}

int16_t bsp_uart_fifo_DMA_send(bsp_uart_e uart_idx, const uint8_t *data, uint16_t len)
{
    if (uart_idx >= BSP_UART_MAX || data == NULL || len == 0)
        return -1;
    uart_ctx_t *ctx = &uart_ctx[uart_idx];

    if (fifo_write(&ctx->tx_fifo, (uint8_t *)data, len) != 0)
    {
        return -2;
    }
    uart_tx_start(ctx);
    return len;
}

int16_t bsp_uart_fifo_read(bsp_uart_e uart_idx, uint8_t *data, uint16_t len)
{
    if (uart_idx >= BSP_UART_MAX || data == NULL)
        return -1;
    uart_ctx_t *ctx = &uart_ctx[uart_idx];
    return fifo_read(&ctx->rx_fifo, data, len);
}

uint16_t bsp_uart_get_rx_len(bsp_uart_e uart_idx)
{
    if (uart_idx >= BSP_UART_MAX)
        return 0;
    return fifo_get_used(&uart_ctx[uart_idx].rx_fifo);
}




