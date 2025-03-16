/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy. All rights reserved.
*********************************************************************************************************
* @file      433_protocol.h
* @brief
* @details
* @author    chenrongrong
* @date      2024-12-23
* @version   v1.0
* *********************************************************************************************************
*/
#ifndef _433_PROTOCOL_H_
#define _433_PROTOCOL_H_

#include "stdint.h"
#include "433_send_driver.h"

#if (UI_RF_ENABLE)

#define BIT_PERIDO                  800 // 800us/bit
#define HALF_BIT                    BIT_PERIDO/2

#define DIVCE_ID                    0x112233
#define PID                         5

#define CODE_LEN                    9

#define WAKE_UP_CODE_LEN            4
#define MAX_CODE_SIZE               9

typedef struct
{
    uint8_t wake_up_code[WAKE_UP_CODE_LEN];
    uint8_t wake_up_code_len;
    uint8_t header_len;
    uint8_t stop_len;
} T_WM_SPEC;


typedef struct
{
    uint8_t code[MAX_CODE_SIZE];
    uint8_t code_len;
    uint16_t buf_len;
    uint8_t *p_buf;
} T_WM_BUF;

T_WMDA_RET protocol_command_encode(uint8_t *data, uint8_t len,
                                          T_WM_SEND_PARA *p_send_parameters);

#endif  //UI_RF_ENABLE

#endif



/******************* (C) COPYRIGHT 2024 Seneasy *****END OF FILE****/
