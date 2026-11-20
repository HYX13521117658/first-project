#include "bsp_can.h"
#include<stm32h7xx_hal_fdcan.h>

extern FDCAN_HandleTypeDef hfdcan1;

static uint32_t s_pkgId[BSP_CAN_FILTER_LIMIT];
static BSP_CAN_RxCallback_t s_callback[BSP_CAN_FILTER_LIMIT];
static uint8_t s_cnt = 0;

static uint32_t BSP_CAN_ConvertDLC(uint8_t len)
{
    switch (len)
    {
        case 0: return FDCAN_DLC_BYTES_0;
        case 1: return FDCAN_DLC_BYTES_1;
        case 2: return FDCAN_DLC_BYTES_2;
        case 3: return FDCAN_DLC_BYTES_3;
        case 4: return FDCAN_DLC_BYTES_4;
        case 5: return FDCAN_DLC_BYTES_5;
        case 6: return FDCAN_DLC_BYTES_6;
        case 7: return FDCAN_DLC_BYTES_7;
        default: return FDCAN_DLC_BYTES_8;
    }
}

BSP_CAN_StatusTypeDef BSP_CAN_Init(void)
{
    FDCAN_FilterTypeDef sFilterConfig = {0};

    sFilterConfig.IdType       = FDCAN_STANDARD_ID;
    sFilterConfig.FilterIndex  = 0;
    sFilterConfig.FilterType   = FDCAN_FILTER_MASK;
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    sFilterConfig.FilterID1    = 0x000;
    sFilterConfig.FilterID2    = 0x000;

    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
    {
        return BSP_CAN_ERROR;
    }

    if (HAL_FDCAN_ActivateNotification(&hfdcan1,FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
        return BSP_CAN_ERROR;
    }

    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        return BSP_CAN_ERROR;
    }

    return BSP_CAN_OK;
}

BSP_CAN_StatusTypeDef BSP_CAN_Send(uint32_t id,const uint8_t *data,uint8_t len)
{
    FDCAN_TxHeaderTypeDef txHeader = {0};
    uint8_t txData[8] = {0};
    uint8_t copyLen;
    uint8_t i;

    if (data == 0 || len == 0)
    {
        return BSP_CAN_PARAM;
    }
    if (id > 0x7FF)
    {
        return BSP_CAN_PARAM;
    }

    copyLen = (len > 8) ? 8 : len;
    for (i = 0; i < copyLen; i++)
    {
        txData[i] = data[i];
    }

    txHeader.Identifier  = id;
    txHeader.IdType      = FDCAN_STANDARD_ID;
    txHeader.TxFrameType = FDCAN_DATA_FRAME;
    txHeader.DataLength  = BSP_CAN_ConvertDLC(copyLen);
    txHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    txHeader.BitRateSwitch      = FDCAN_BRS_OFF;
    txHeader.FDMode             = FDCAN_CLASSIC_CAN;
    txHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    txHeader.MessageMarker      = 0;

    if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txHeader, txData) != HAL_OK)
    {
        return BSP_CAN_BUSY;
    }

    return BSP_CAN_OK;
}

BSP_CAN_StatusTypeDef BSP_CAN_RegisterRxCallback(uint32_t id,BSP_CAN_RxCallback_t func)
{
    uint8_t i;

    if (func == 0 || id > 0x7FF)
    {
        return BSP_CAN_PARAM;
    }
    if (s_cnt >= BSP_CAN_FILTER_LIMIT)
    {
        return BSP_CAN_FULL;
    }

    for (i = 0; i < s_cnt; i++)
    {
        if (s_pkgId[i] == id)
        {
            return BSP_CAN_DUP;
        }
    }

    s_pkgId[s_cnt]    = id;
    s_callback[s_cnt] = func;
    s_cnt++;

    return BSP_CAN_OK;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8] = {0};
    uint8_t rxLen;
    uint8_t i;

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) == 0)
    {
        return;
    }

    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0,&rxHeader, rxData) != HAL_OK)
    {
        return;
    }

    rxLen = (uint8_t)rxHeader.DataLength;
    if (rxLen > 8) rxLen = 8;

    for (i = 0; i < s_cnt; i++)
    {
        if (s_pkgId[i] == rxHeader.Identifier)
        {
            if (s_callback[i] != 0)
            {
                s_callback[i](rxHeader.Identifier, rxData, rxLen);
            }
            break;
        }
    }
}

