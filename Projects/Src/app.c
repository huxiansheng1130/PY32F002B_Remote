/*********************************************************************************************************
 * @file      app.c
 *
 * @details   This file contains the implementation of functions for handling received data and related operations.
 *            It includes functions for writing a generated MAC address to flash memory, reading paired device IDs,
 *            initializing device data, checking the duration of the pairing process, handling tick events,
 *            and initializing custom settings and device configurations.
 *
 * @author    huzhuohuan
 * @date      2024-11-09
 * @version   1.0
 ********************************************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
Device_state_t dev_st;
Rf_send_status_t rf_send_st;
/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief  Initializes the device status.
 * @retval None
 */
void device_status_init(void)
{
    dev_st.wakeup_source = POWER_ON;
    dev_st.ret_init_flag = WAIT_INTI;
    dev_st.run_time = clock_time() | 1;
}

/**
 * @brief  Periodic loop to update and manage device status.
 * @retval None
 */
void device_status_clear(void)
{
    dev_st.wakeup_source = SLEEP_WAKEUP;
    dev_st.ret_init_flag = WAIT_INTI;
    dev_st.run_time = 0;
}

/**
 * @brief  Periodic loop to update and manage device status.
 *         This function is typically called repeatedly to monitor
 *         and update the device status, handle state changes, and
 *         perform any necessary maintenance tasks.
 * @retval None
 */
void device_status_loop(void)
{
    if (dev_st.wakeup_source == SLEEP_WAKEUP)
    {
        if (dev_st.ret_init_flag == WAIT_INTI)
        {
            dev_st.ret_init_flag = AREADY_INTI;
#if (NTC_SMAPLING_ENABLE)
            ntc_smapling(dev_st.device_temp);
#endif
        }
    }
}

#if (UI_RF_ENABLE)
/**
 * @brief   Continuously sends data over RF (Radio Frequency) connection.
 * @details This function runs in a loop, sending data packets at regular intervals
 *          over an RF interface. It is typically used for maintaining a steady stream
 *          of data transmission in applications such as wireless sensor networks,
 *          remote controls, etc.
 * @param  None
 * @retval None
 */
void rf_send_loop()
{
    if (rf_send_st.send_status == SENDING_DATA)
    {
        if (clock_time_exceed(rf_send_st.send_tick, RF_SEND_TIMEOUT * 1000))
        {
            if (rf_send((uint8_t *)&packet_dat, sizeof(packet_dat)))
            {
                rf_send_st.send_tick = clock_time() | 1;
                rf_send_st.send_num--;
            }
        }

        if (rf_send_st.send_num == 0)
        {
            if (!rf_send_is_working())
                rf_send_st.send_status = SEND_IDLE;
        }
    }
}

/**
 * @brief  Enables or disables the re-sending of data.
 * @param  enable: A boolean value indicating whether to enable (true) or disable (false)
 *                 the re-sending functionality.
 * @retval None
 */
void re_send_enable(_Bool enable)
{
    if (enable)
    {
        rf_send_st.send_tick = 0;
        rf_send_st.send_status = SENDING_DATA;
        rf_send_st.send_num = RF_SINGLE_SEND_DATA_NUM;
    }
    else
    {
        rf_send_st.send_tick = 0;
        rf_send_st.send_status = SEND_IDLE;
        rf_send_st.send_num = 0;
    }
}
#endif

/**
 * @brief  Initializes the user-specific settings and configurations.
 *         This function is called once at the beginning of the program
 *         to set up user-related configurations, such as hardware
 *         initialization, default values, or other startup tasks.
 * @retval None
 */
void user_init(void)
{
    BSP_RCC_HSI_48MConfig();

    systick_init();

    device_status_init();

#if (DEBUG_ENABLED)
    BSP_USART_Config(115200);
#endif

#if LED_FUNCTION_ENABLE
    led_init();
#endif

#if (GYROSCOPE_ENABLE || GEOMAGNERISM_ENABLE)
    dev_iic_config();
#endif

#if (GYROSCOPE_ENABLE)
    qmi8658a_setup_init();
#endif
    

#if (FLASH_ID_READ_ENABLE)
    device_pair_id_read();
#endif

#if (UI_KEYBOARD_ENABLE)
    keybroad_init();
#endif

#if (UI_RF_ENABLE)
    rf_driver_init();
#endif

#if (NTC_SMAPLING_ENABLE)
    ntc_smapling_init();

    ntc_smapling(dev_st.device_temp);
#endif
}

/**
 * @brief  Main loop function that continuously executes the core logic of the application.
 *         This function is responsible for repeatedly processing tasks, checking for user input,
 *         updating system states, and handling events in a non-blocking manner.
 * @retval None
 */
void main_loop(void)
{
    device_status_loop();

#if (GYROSCOPE_ENABLE)
    qmi8658a_loop();
#endif

#if (UI_KEYBOARD_ENABLE)
    keyboard_loop();
#endif

#if (LOW_POWER_ENABLE)
    app_enter_deepstop();
#endif

#if (UI_RF_ENABLE)
    rf_send_loop();
#endif
}