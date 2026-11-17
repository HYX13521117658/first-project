#include "bsp_uart.h"
#include "usart.h"

static UART_HandleTypeDef *bsp_uart_get_handle(bsp_uart_e device)
{
    switch (device)
    {
        case E_UART_10:
            return &huart10;
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





typedef struct
{
    UART_HandleTypeDef *uart_handle;
    uint8_t rx_buf[BSP_UART_BUFFER_SIZE];
    uart_recv_callback_t recv_cb;
    uint8_t tx_busy;
} uart_ctx_t;

static uart_ctx_t uart_ctx[BSP_UART_DEVICE_COUNT];

void bsp_uartEx_init(bsp_uart_e  device, uart_recv_callback_t cb)
{
    UART_HandleTypeDef *uart_handle = bsp_uart_get_handle(device);

    if (uart_handle == NULL)
    {
        return;
    }
    uart_ctx[device].uart_handle = uart_handle;
    uart_ctx[device].recv_cb = cb;
    uart_ctx[device].tx_busy = 0;

    HAL_UARTEx_ReceiveToIdle_DMA(uart_handle, uart_ctx[device].rx_buf, BSP_UART_BUFFER_SIZE);
    if (uart_handle->hdmarx != NULL)
    {
        __HAL_DMA_DISABLE_IT(uart_handle->hdmarx, DMA_IT_HT);
    }
}


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *uart_handle, uint16_t len)
{
    for (uint8_t i = 0; i < BSP_UART_DEVICE_COUNT; i++)
    {
        if (uart_ctx[i].uart_handle == uart_handle)
        {
            uart_ctx_t *ctx = &uart_ctx[i];

            if (ctx->recv_cb != NULL)
            {
                ctx->recv_cb((bsp_uart_e)i, ctx->rx_buf, len);
            }
            HAL_UARTEx_ReceiveToIdle_DMA(uart_handle, ctx->rx_buf, BSP_UART_BUFFER_SIZE);
            if (uart_handle->hdmarx != NULL)
            {
                __HAL_DMA_DISABLE_IT(uart_handle->hdmarx, DMA_IT_HT);
            }
            break;
        }
    }
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart_handle)
{
    for (uint8_t i = 0; i < BSP_UART_DEVICE_COUNT; i++)
    {
        if (uart_ctx[i].uart_handle == uart_handle)
        {
            uart_ctx[i].tx_busy = 0;
            break;
        }
    }
}


void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart_handle)
{
    for (uint8_t i = 0; i < BSP_UART_DEVICE_COUNT; i++)
    {
        if (uart_ctx[i].uart_handle == uart_handle)
        {
            HAL_UARTEx_ReceiveToIdle_DMA(uart_handle, uart_ctx[i].rx_buf, BSP_UART_BUFFER_SIZE);
            if (uart_handle->hdmarx != NULL)
            {
                __HAL_DMA_DISABLE_IT(uart_handle->hdmarx, DMA_IT_HT);
            }
            break;
        }
    }
}
