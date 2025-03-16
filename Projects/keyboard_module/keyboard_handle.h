/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     keybroad_config.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2024-02-27
 * @version  V_1.0
 *********************************************************************************************************/

#ifndef _KEYBROAD_HANDLE_H_
#define _KEYBROAD_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"
#include "keyboard_driver.h"
/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
#define DATA_ONE                    0x01
#define DATA_TWO                    0x02
#define DATA_THREE                  0x03
#define DATA_FOUR                   0x04
#define DATA_FIVE                   0x05
#define DATA_SIX                    0x06
#define FAN_SWITCH                  0x07
#define GEAR_DEC                    0x08
#define LED_SWITCH                  0x09
#define GEAR_ADD                    0x0A
#define REVERSIBLE                  0x0B
#define RGB_SWITCH                  0x12
#define COLOR_TEMP                  0x0D
#define TIME_1_HOUR                 0x0F
#define TIME_4_HOUR                 0x10
#define TIME_8_HOUR                 0x11
 
#define RGB_HOLD                    0x14
#define COLOR_HOLD                  0x0E
#define LOW_POWER_WARN              0xFF

#define NOTE_TO_DEVICE_PAIR         0xF1
#define NOTE_TO_DEVICE_PAIR_WIFI    0xF2
#define NOTE_TO_WIFI_FACTORY_MODE   0x13

// This 433 remote key mapping relationship 
#define ROMORE_KEY_LIST            {DATA_ONE,   DATA_TWO,    DATA_THREE,  DATA_FOUR, \
                                    DATA_FIVE , DATA_SIX,    FAN_SWITCH,  GEAR_DEC,  \
                                    LED_SWITCH, GEAR_ADD,    REVERSIBLE,  RGB_SWITCH,\
                                    COLOR_TEMP, TIME_1_HOUR, TIME_4_HOUR, TIME_8_HOUR}

enum
{
    KB_CODE_0,
    KB_CODE_1,
    KB_CODE_2,
    KB_CODE_3,
    KB_CODE_4,
    KB_CODE_5,
    KB_CODE_6,
    KB_CODE_7,
    KB_CODE_8,
    KB_CODE_9,
    KB_CODE_10,
    KB_CODE_11,
    KB_CODE_12,
    KB_CODE_13,
    KB_CODE_14,
    KB_CODE_15,
    KB_CODE_16
};

/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern void keyboard_loop(void);
#endif
