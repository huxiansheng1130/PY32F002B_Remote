/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     ntc_handle.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2025-03-06
 * @version  V_1.0
 *********************************************************************************************************/

#ifndef _NTC_HANDLE_H_
#define _NTC_HANDLE_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"
#include "py32f002b_ll_adc.h"
#include "../ntc_module/ntc_driver.h"

/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
#define NTC_HANDLE_ENABLE                           1

// Operating temperature range
#define NTC_MINIMUM_TEMPERATURE                     -20
#define NTC_MAXIMUM_TEMPERATURE                     69

// Define the size of the resistance list
#define RESISTANCE_LIST_SIZE                        (NTC_MAXIMUM_TEMPERATURE - NTC_MINIMUM_TEMPERATURE + 1)

// Through the calculation of B-value conversion                                                   
#define RESISTANCE_VALUE_LIST                       {3740, 3719, 3698, 3676, 3653, 3629, 3604, 3578, 3551, 3523, \
                                                     3494, 3465, 3434, 3403, 3370, 3337, 3302, 3267, 3231, 3194, \
                                                     3156, 3117, 3078, 3037, 2996, 2955, 2912, 2869, 2826, 2782, \
                                                     2737, 2692, 2647, 2602, 2556, 2510, 2463, 2417, 2370, 2324, \
                                                     2277, 2231, 2185, 2139, 2093, 2047, 2002, 1957, 1912, 1868, \
                                                     1824, 1781, 1738, 1696, 1654, 1614, 1573, 1533, 1494, 1456, \
                                                     1418, 1381, 1345, 1309, 1275, 1240, 1207, 1174, 1142, 1111, \
                                                     1081, 1051, 1022, 994,  966,  939,  913,  887,  863,  838,  \
                                                     815,  792,  770,  748,  727,  706,  686,  667,  648,  630}
                                                                                                    

#define CALCULATE_TEMPERATURE_INT(i)                (i + NTC_MINIMUM_TEMPERATURE)
      
#define CALCULATE_TEMPERATURE_FLAOT(adc_v, i)       ((10 * (ntc_list[i] - adc_v)) / \
                                                    (ntc_list[i] - ntc_list[i + 1]))

/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern _Bool temperature_calc(uint16_t adc_v, uint8_t *temp);
#endif
