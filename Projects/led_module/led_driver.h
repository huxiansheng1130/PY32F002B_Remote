/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     led_driver.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2024-02-27
 * @version  V_1.0
 *********************************************************************************************************/
#ifndef _LED_DRIVER_H_
#define _LED_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"

#if (LED_FUNCTION_ENABLE)
/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/

/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern void led_init(void);
extern void led_open(void);
extern void led_close(void);
#endif
#endif
