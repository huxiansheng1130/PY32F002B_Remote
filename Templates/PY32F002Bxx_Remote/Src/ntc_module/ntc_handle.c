/*********************************************************************************************************
 * @file      ntc_driver.c
 *
 * @details
 *
 * @author    huzhuohuan
 * @date      2025-03-06
 * @version   V_1.0
 ********************************************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "../ntc_module/ntc_handle.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
const uint16_t ntc_list[RESISTANCE_LIST_SIZE] = RESISTANCE_VALUE_LIST;

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief Calculates the temperature based on the ADC value.
 * 
 * @param adc_v The ADC reading corresponding to the thermistor's resistance.
 * @param temp A pointer to store the calculated temperature.
 * @retval 1 if temperature is successfully calculated, 0 otherwise.
 */
_Bool temperature_calc(uint16_t adc_v, uint8_t *temp)
{
    if ((adc_v > ntc_list[0]) || (adc_v < ntc_list[RESISTANCE_LIST_SIZE - 1]))
        return 0;

    for (int8_t i = 0; i < RESISTANCE_LIST_SIZE; i++)
    {
        if (adc_v == ntc_list[i])
        {
            temp[0] = CALCULATE_TEMPERATURE_INT(i);
            temp[1] = CALCULATE_TEMPERATURE_FLAOT(adc_v, i);
            return 1;
        }
        else if (adc_v > ntc_list[i])
        {
            i--;
            temp[0] = CALCULATE_TEMPERATURE_INT(i);
            temp[1] = CALCULATE_TEMPERATURE_FLAOT(adc_v, i);
            return 1;
        }
    }
    return 0;
}