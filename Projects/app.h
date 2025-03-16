/*********************************************************************************************************
*               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
**********************************************************************************************************
* @file     app.h
* @brief    
* @details
* @author   huzhuohuan
* @date     2024-11-09
* @version  v1.0
*********************************************************************************************************/

#ifndef _APP_H_
#define _APP_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "py32f002b_bsp_clock.h"

/*============================================================================*
*                               Function enable
*============================================================================*/
#define DEBUG_ENABLED                           0
#define	UI_KEYBOARD_ENABLE						1
#define NTC_SMAPLING_ENABLE	                    0
#define LOW_POWER_ENABLE	                    0
#define LED_FUNCTION_ENABLE	                    1
#define UI_RF_ENABLE                            1
#define FLASH_ID_READ_ENABLE	                1
#define GYROSCOPE_ENABLE	                    1
#define GEOMAGNERISM_ENABLE	                    0

/*============================================================================*
 *                           Export Global Variables
 *============================================================================*/
#define RF_SINGLE_SEND_DATA_NUM                 5
#define RF_SEND_TIMEOUT                         100 
#define RCU_ENTER_SLEEP_TIMEOUT                 100
#define KB_DITHER_TIMEOUT                       30
#define KB_SHORT_TIMEOUT                        200

enum
{
    POWER_ON,
    SLEEP_WAKEUP
};

enum
{
    WAIT_INTI,
    AREADY_INTI
};

enum
{
    SEND_IDLE,
    SENDING_DATA
};

typedef struct 
{
    _Bool wakeup_source;
    _Bool ret_init_flag;
    uint8_t device_temp[2];
    uint32_t run_time;
}Device_state_t;
extern Device_state_t dev_st;

typedef struct
{
    _Bool send_status;
    uint8_t send_num;
    uint32_t send_tick;
}Rf_send_status_t;
extern Rf_send_status_t rf_send_st;

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern void user_init(void);
extern void main_loop(void);
extern void device_status_init(void);
extern void device_status_loop(void);
extern void device_status_clear(void);
extern void re_send_enable(_Bool enable);
#endif