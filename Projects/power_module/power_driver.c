/*********************************************************************************************************
 * @file      power_driver.c
 *
 * @details
 *
 * @author    huzhuohuan
 * @date      2024-02-27
 * @version   V_1.0
 ********************************************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "power_driver.h"

#if (LOW_POWER_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
_Bool power_on = 0;
volatile uint32_t tick = 0;

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief  Periodic loop to update and manage device status.
 * @retval None
 */
void enter_sleep_clear_event(void)
{
    device_status_clear();

#if (UI_RF_ENABLE)
    rf_gpio_set_low();
#endif

#if (UI_KEYBOARD_ENABLE)
    kb_gpio_set_low_event();
#endif

#if (LED_FUNCTION_ENABLE)
    led_close();
#endif
}

/**
 * @brief  Checks if the system should enter deep stop condition.
 * @details This function evaluates various system parameters and conditions to determine
 *          if the system should enter a deep stop state. 
 * @param None
 * @retval Returns true if the deep stop condition is met, false otherwise.
 */
_Bool enter_deepstop_condition()
{
    if (!(tick && clock_time_exceed(tick, RCU_ENTER_SLEEP_TIMEOUT * 1000)))
        return 0;
    if (rf_send_st.send_status != SEND_IDLE)
        return 0;
    return 1;
}

/**
 * @brief  Enter deep stop mode
 * @param  None
 * @retval None
 */
void app_enter_deepstop(void)
{
#if (UI_KEYBOARD_ENABLE)
    if (kb_code.cnt)
    {
        tick = clock_time() | 1;
        return;
    }
    else 
#endif
    if (enter_deepstop_condition())
    {
        tick = 0;
    }
    else
    {
        if (power_on == 0)
        {
            power_on = 1;
            tick = clock_time() | 1;
        }
        return;
    }

    enter_sleep_clear_event();

    /* Enable PWR clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    /* STOP mode with deep low power regulator ON */
    LL_PWR_SetLprMode(LL_PWR_LPR_MODE_DLPR);

    /* SRAM retention voltage aligned with digital LDO output */
    LL_PWR_SetStopModeSramVoltCtrl(LL_PWR_SRAM_RETENTION_VOLT_CTRL_LOW);

    /* Enter DeepSleep mode */
    LL_LPM_EnableDeepSleep();

    /* Request Wait For Event */
    __SEV();
    __WFE();
    __WFE();

    LL_LPM_EnableSleep();
}
#endif