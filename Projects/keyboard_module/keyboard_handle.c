/*********************************************************************************************************
 * @file      keybroad_driver.c
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
#include "keyboard_handle.h"
#include "led_driver.h"
#include "function_handle.h"

#if (UI_KEYBOARD_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
const uint8_t kb_list[KB_TATOL] = ROMORE_KEY_LIST;

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief  Detects if a specific key combination has been pressed.
 * @param  key_1: The first key code or identifier.
 * @param  key_2: The second key code or identifier.
 * @retval Returns true if the specified key combination is detected, otherwise false.
 */
_Bool key_combination_events(uint8_t key_1, uint8_t key_2)
{
    if (((kb_code.kb_now_code[0] == key_1) && (kb_code.kb_now_code[1] == key_2)) ||
        ((kb_code.kb_now_code[0] == key_2) && (kb_code.kb_now_code[1] == key_1)))
        return 1;
    return 0;
}

/**
 * @brief  Handles a short press event for a single key.
 * @param  key: The key code or identifier of the pressed key.
 * @param  temp: Array containing temperature data.
 * @retval None
 */
void one_key_short_press_event()
{
    for (uint8_t i = 1; i <= KB_TATOL; i++)
        if (kb_code.kb_now_code[0] == i)
            send_key_ntc_packet(kb_list[i - 1], 0);
}

/**
 * @brief  Handles the event of a single key being pressed for a long duration.
 * @details This function is invoked when a key press is detected that exceeds
 *          a predefined duration threshold, indicating a long press event.
 * @param  None
 * @retval None
 */
void one_key_long_press_event()
{
    if (kb_code.kb_now_code[0] == KB_CODE_12)
        send_key_ntc_packet(RGB_HOLD, 0);
    else if (kb_code.kb_now_code[0] == KB_CODE_13)
        send_key_ntc_packet(COLOR_HOLD, 0);
    else
    {
        one_key_short_press_event();
    }
}

/**
 * @brief  Handles the event of two keys being pressed simultaneously in a short press.
 * @details This function is triggered when two keys are detected as pressed at the same time
 *          for a short duration. 
 * @param  None
 * @retval None
 */
void two_key_short_press_event()
{
    if (key_combination_events(KB_CODE_7, KB_CODE_9) ||
        key_combination_events(KB_CODE_9, KB_CODE_13) ||
        key_combination_events(KB_CODE_14, KB_CODE_16))
    {
#if (LED_FUNCTION_ENABLE)
        led_open();
#endif
    }
    else
    {
#if (LED_FUNCTION_ENABLE)
        led_close();
#endif
    }
}

/**
 * @brief  Handles the event of two keys being pressed simultaneously for a long duration.
 * @details This function is invoked when two keys are detected as pressed at the same time
 *          and held for a period exceeding a predefined threshold, indicating a long press event.
 * @param  None
 * @retval None
 */
void two_key_long_press_event()
{
    if (key_combination_events(KB_CODE_7, KB_CODE_9))
        send_key_ntc_packet(NOTE_TO_DEVICE_PAIR, 0);
    else if (key_combination_events(KB_CODE_9, KB_CODE_13))
        send_key_ntc_packet(NOTE_TO_DEVICE_PAIR_WIFI, 0);
    else if (key_combination_events(KB_CODE_14, KB_CODE_16))
        send_key_ntc_packet(NOTE_TO_WIFI_FACTORY_MODE, 0);
}

/**
 * @brief  Handles the event of a key press timeout.
 * @details This function is triggered when a key press does not receive any further input
 *          within a specified time period, indicating that the key press event has timed out.
 * @param None
 * @retval None
 */
void key_press_timeout_event(void)
{
#if (UI_RF_ENABLE)
    re_send_enable(0);
#endif

#if (LED_FUNCTION_ENABLE)
    led_close();
#endif
}

/**
 * @brief  Handles the event where all keys have been released.
 * @details This function is triggered when the system detects that all previously pressed keys
 *          have been released. It is typically used to perform actions that require all keys
 *          to be in the released state, such as exiting a special mode or completing a command.
 * @param None
 * @retval None
 */
void all_key_release_event(void)
{
    if (kb_code.cnt == NULL_KEY)
    {
#if (LED_FUNCTION_ENABLE)
        led_close();
#endif
    }
}

/**
 * @brief  Updates the status of the key press events.
 * @details This function evaluates the current state of key presses and sets the 
 *          corresponding status flags. 
 * @param None
 * @retval None
 */
void key_status_set_event(void)
{
    if (kb_code.cnt)
    {
        if (kb_code.status == BK_RELEASE && kb_code.kb_trigger_time == 0)
        {
            kb_code.kb_trigger_time = clock_time() | 1;
            kb_code.status = KB_SHORT_PRESS;
        }
        if (kb_code.status == KB_SHORT_PRESS && kb_code.kb_trigger_time && clock_time_exceed(kb_code.kb_trigger_time, 3 * 1000 * 1000))
        {
            kb_code.status = KBLONG_PRESS_3S;
        }
        else if (kb_code.status == KBLONG_PRESS_3S && kb_code.kb_trigger_time && clock_time_exceed(kb_code.kb_trigger_time, 45 * 100 * 1000))
        {
            kb_code.status = KB_TIMEOUT_5S;
        }
    }
}
/**
 * @brief  Scan the keyboard matrix to detect key presses and releases.
 * @retval Key status (KEY_DEBOUNCE, KEY_PRESSED, KEY_RELEASED)
 */
void keyboard_loop(void)
{
    static uint32_t tick;

    if (clock_time_exceed(tick, 8000))
        tick = clock_time() | 1;
    else
        return;

    kb_matrix_scan();

    key_status_set_event();

    if (kb_code.status == KB_SHORT_PRESS)
    {
        if (kb_code.cnt == ONE_KEY && clock_time_exceed(kb_code.kb_short_time, KB_SHORT_TIMEOUT * 1000))
        {
            kb_code.kb_short_time = clock_time() | 1;
            one_key_short_press_event();
        }
        if (kb_code.cnt == TWO_KEY)
        {
            two_key_short_press_event();
        }
    }
    else if (kb_code.status == KBLONG_PRESS_3S)
    {
        if (kb_code.cnt == ONE_KEY)
        {
            one_key_long_press_event();
        }
        if (kb_code.cnt == TWO_KEY)
        {
            two_key_long_press_event();
        }
    }
    else if (kb_code.status == KB_TIMEOUT_5S)
    {
        key_press_timeout_event();
    }

    all_key_release_event();
}
#endif