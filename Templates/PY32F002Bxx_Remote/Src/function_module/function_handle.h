/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     function_handle.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2025-03-06
 * @version  V_1.0
 *********************************************************************************************************/

#ifndef _FUNCTION_HANDLE_H_
#define _FUNCTION_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"

/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
enum
{
    NTC_TYPE = 1,
    GYRO_TYPE,
};

typedef struct Send_packet_t
{
    uint8_t device_id[3];
    uint8_t type :4;
    uint8_t pid :4;
    uint8_t data[4];
    uint8_t dat_check;
}Send_packet_t;
extern Send_packet_t packet_dat;

#define PACKET_CHECK(dat_check, dat_1, dat_2, dat_3, dat_4)            (dat_check = (dat_1 ^ dat_2 ^ dat_3 ^ dat_4) + 0x11)
/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern void send_key_ntc_packet(uint8_t key_1, uint8_t key_2);
#endif
