//
// Created by HYX on 2026/11/20.
//

#ifndef PROJECT1__BSP_CAN_H
#define PROJECT1__BSP_CAN_H

#include <stdint.h>

#define BSP_CAN_FILTER_LIMIT    16

typedef enum
{
    BSP_CAN_OK = 0,
    BSP_CAN_ERROR,
    BSP_CAN_BUSY,
    BSP_CAN_PARAM,
    BSP_CAN_FULL,
    BSP_CAN_DUP
} BSP_CAN_StatusTypeDef;

typedef void (*BSP_CAN_RxCallback_t)(uint32_t id,
                                     const uint8_t *data,
                                     uint8_t len);

BSP_CAN_StatusTypeDef BSP_CAN_Init(void);

BSP_CAN_StatusTypeDef BSP_CAN_Send(uint32_t id,const uint8_t *data,uint8_t len);

BSP_CAN_StatusTypeDef BSP_CAN_RegisterRxCallback(uint32_t id,BSP_CAN_RxCallback_t func);

#endif





