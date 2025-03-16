/*********************************************************************************************************
 * @file      led_driver.c
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
#include "led_driver.h"

#if (LED_FUNCTION_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief  Initializes the LED hardware.
 * @retval None
 */
void led_init(void)
{
    /* Enable GPIOC clock */
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);

    /* Configure PAC in output mode */
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_3, LL_GPIO_MODE_OUTPUT);

    /* LED off*/
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_3);
}

/**
 * @brief  Turns on the LED.
 * @retval None
 */
void led_open(void)
{
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_3);
}

/**
 * @brief  Turns off the LED.
 * @retval None
 */
void led_close(void)
{
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_3);
}
#endif