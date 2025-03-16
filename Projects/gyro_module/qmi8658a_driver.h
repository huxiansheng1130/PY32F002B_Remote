/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     qmi8658a_driver.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2024-02-27
 * @version  V_1.0
 *********************************************************************************************************/

#ifndef _QMI8658A_DRIVER_H_
#define _QMI8658A_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"
#include "SEGGER_RTT.h"
#include "py32f002b_ll_tim.h"
#include "i2c_driver.h"
#include "imualgo_axis9.h"
#include "qmi8658a_handle.h"

#if (GYROSCOPE_ENABLE)
/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
#define QMI8658A_ADDRESS                        0xD6
#define QMI8658A_WHO_AM_I                       0x00
#define QMI8658A_REVISION_ID                    0x01
#define QMI8658A_CTRL1                          0x02
#define QMI8658A_CTRL2                          0x03
#define QMI8658A_CTRL3                          0x04
#define QMI8658A_CTRL5                          0x06
#define QMI8658A_CTRL7                          0x08
#define QMI8658A_CTRL8                          0x09
#define QMI8658A_CTRL9                          0x0A
#define QMI8658A_FIFO_WTM_TH                    0x13
#define QMI8658A_FIFO_CTRL                      0x14
#define QMI8658A_RESET                          0x60
#define QMI8658A_WHO_AM_I_VAL                   0x05

extern _Bool task_run;
extern uint8_t show_flag;

/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern void qmi8658a_setup_init(void);
extern void qmi8658a_write_byte(uint8_t reg, uint8_t value);
extern uint8_t qmi8658a_read_byte(uint8_t reg);

#endif
#endif
