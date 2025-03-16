/**
*********************************************************************************************************
*               Copyright(c) 2024, Modulated Wave data for 433MHz module. All rights reserved.
*********************************************************************************************************
* @file         433_send_driver.h
* @brief
* @details
* @author       chenrongrong
* @date         2024-12-23
* @version      v1.0
* *********************************************************************************************************
*/

#ifndef __433_SEND_DRIVER_H_
#define __433_SEND_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"
#include "main.h"
#include "app.h"

#if (UI_RF_ENABLE)
/*============================================================================*
 *                          MW Send config
 *============================================================================*/
#define WM_SEND_WAVE_MAX_LEN            224

#define LEVEL_HIGH                      ((uint8_t)0xff)
#define LEVEL_LOW                       0x0

enum {
    DATA_RESERVE,
	DATA_KEY,
	DATA_SENSOR,
};

typedef enum
{
    WM_SEND_IDLE,
    WM_SEND_CAMMAND,
    WM_SEND_CAMMAND_COMPLETE,
} T_WM_SEND_STATE;

typedef enum
{
    IRDA_ERROR        = -1,
    IRDA_SUCCEED      = 0,
    IRDA_HEADER_ERROR = 1,
    IRDA_DATA_ERROR
} T_WMDA_RET;

typedef struct
{
    uint8_t wm_send_buf[WM_SEND_WAVE_MAX_LEN];
    uint16_t send_buf_len;
} T_WM_SEND_PARA;

typedef struct
{
    T_WM_SEND_STATE  wm_send_state;             /*wm send state*/
    T_WM_SEND_PARA   *p_wm_send_data;
} T_WM_SEND_STRUCT;

_Bool rf_send(uint8_t *data, uint8_t type);

bool wm_send_module_init(T_WM_SEND_PARA *p_wm_send_para);

bool rf_send_is_working(void);

void rf_driver_init(void);

extern void rf_gpio_set_low(void);

#ifdef __cplusplus
}
#endif

#endif //(UI_RF_ENABLE)

#endif /*__433_SEND_DRIVER_H_*/




/******************* (C) COPYRIGHT 2024 Seneasy *****END OF FILE****/
