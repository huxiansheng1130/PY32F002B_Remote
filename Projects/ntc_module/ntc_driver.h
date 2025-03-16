/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     ntc_driver.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2024-02-27
 * @version  V_1.0
 *********************************************************************************************************/

#ifndef _NTC_DRIVER_H_
#define _NTC_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"
#include "py32f002b_ll_adc.h"
#include "ntc_handle.h"

#if (NTC_SMAPLING_ENABLE)
/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
#define NTC_INTERRUPT_SMAP_ENABLE                   0
#define VDDA_APPLI                                  ((uint32_t)3300)

#if (NTC_INTERRUPT_SMAP_ENABLE == 0)
#define USER_ADC_SMAP_MODE                          LL_ADC_REG_TRIG_SOFTWARE
#else
#define USER_ADC_SMAP_MODE                          LL_ADC_REG_TRIG_EXT_TIM1_TRGO
#endif

/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern void ntc_smapling_init(void);
extern void ntc_smapling(uint8_t *temp);
#endif
#endif
