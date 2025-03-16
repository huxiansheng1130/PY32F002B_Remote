/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy. All rights reserved.
**********************************************************************************************************
* @file         433_send_driver.c
* @brief        This file provides MW sending data handler by interrupt.
* @details      Application users can use the global functions to send Modulated Wave data for 433MHz module.
* @author       chenrongrong
* @date         2024-12-23
* @version      v1.0
*********************************************************************************************************
*/


/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "string.h"
#include "433_send_driver.h"
#include "433_protocol.h"
#include "stdint.h"
#include <stdlib.h>
#include "py32f002b_bsp_clock.h"
#include "py32f002b_bsp_printf.h"
#include "py32f002b_ll_tim.h"

#if (UI_RF_ENABLE)
/*============================================================================*
 *                          Local Variables
 *============================================================================*/
static T_WM_SEND_STRUCT wm_send_struct;

static void rf_gpio_config(void);
static void rf_timer_config(void);

/*============================================================================*
 *                              Functions Declaration
 *============================================================================*/
/**
  * @brief  Initialization of pinmux settings and pad settings.
  * @param  No parameter.
  * @return void
  */
static void rf_gpio_config(void)
{
  /* Set PB5 as GPIO output */
  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
  LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_7, LL_GPIO_MODE_OUTPUT);

  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);//low
}

/**
 * @brief  Sets the Radio Frequency (RF) module's General Purpose Input/Output (GPIO) pin to a low level.
 * @details This function is used to control the RF module by setting its GPIO pin to a low logic level.
 * @note    The function assumes that the GPIO pin has been properly configured for output mode before calling this function.
 * @param   None
 * @retval  None
 */
void rf_gpio_set_low(void)
{
  LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7); // low
};

/**
  * @brief  Initialize gpio peripheral.
  * @param  No parameter.
  * @return void
  */
void rf_driver_init(void)
{
    /* Initialize gpio peripheral */
   rf_gpio_config();
   rf_timer_config();
   wm_send_struct.wm_send_state = WM_SEND_IDLE;

}
/**
  * @brief  Initialize tim peripheral.
  * @param  No parameter.
  * @return void
*/
static void rf_timer_config(void)
{
  LL_TIM_InitTypeDef TIM1CountInit = {0};

  LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM1EN);
  
  TIM1CountInit.ClockDivision       = LL_TIM_CLOCKDIVISION_DIV1;
  TIM1CountInit.CounterMode         = LL_TIM_COUNTERMODE_UP;
  TIM1CountInit.Prescaler           = 48 - 1;  // 48MHz/48 = 1MHz (1us/tick)
  TIM1CountInit.Autoreload          = BIT_PERIDO/2 - 1;//每半周期触发一次中断
  TIM1CountInit.RepetitionCounter   = 0;
  LL_TIM_Init(TIM1, &TIM1CountInit);

  LL_TIM_EnableIT_UPDATE(TIM1);
  LL_TIM_EnableCounter(TIM1);

  NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
  NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,0);
}

void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
  if(LL_TIM_IsActiveFlag_UPDATE(TIM1) && LL_TIM_IsEnabledIT_UPDATE(TIM1))
  {
    static uint16_t i = 0;
    LL_TIM_ClearFlag_UPDATE(TIM1);
    uint8_t level = wm_send_struct.p_wm_send_data->wm_send_buf[i];

    if(wm_send_struct.wm_send_state ==  WM_SEND_CAMMAND_COMPLETE)
        wm_send_struct.wm_send_state =  WM_SEND_IDLE;

    if(wm_send_struct.wm_send_state == WM_SEND_CAMMAND){
      if(i < wm_send_struct.p_wm_send_data ->send_buf_len){
          if(level == LEVEL_HIGH)
            LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_7);//HIGH
          else
            LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_7);//low

          i++;
        }else{
          i = 0;
          wm_send_struct.wm_send_state =  WM_SEND_CAMMAND_COMPLETE;
          memset(&wm_send_struct, 0, sizeof(T_WM_SEND_STRUCT));
        }
    }
  }
}

/**
  * @brief  Initialize gdma peripheral.
  * @param  No parameter.
  * @return void
*/
void mw_driver_tx_gdma_init(uint32_t source_addr, uint32_t buffer_size)
{
    /* Set gdma with GDMA_Handshake_TIM5, msize=1, transfer width = 32 */
   //source_addr :wave buffer
}

static T_WM_SEND_PARA wm_send_parameters;

/******************************************************************
 * @brief   check if 433 send is working.
 * @param   none
 * @return  if 433 send is working
 * @retval  true or false
 */
bool rf_send_is_working(void)
{
    if (wm_send_struct.wm_send_state == WM_SEND_IDLE)
    {
        return false;
    }
    return true;
}

/******************************************************************
 * @brief   get 433 send current state
 * @param   none
 * @return  ir send current state
 * @retval  T_IR_SEND_STATE
 */
T_WM_SEND_STATE rf_send_get_current_state(void)
{
    return wm_send_struct.wm_send_state;
}

/**
  * @brief  Demo code of operation about timer + gpio.
  * @param  No parameter.
  * @return void
*/
_Bool rf_send(uint8_t *data, uint8_t len)
{
    T_WMDA_RET ret = IRDA_ERROR;

    if(true == rf_send_is_working()){
      // printf("[433_RF_ERR] RF send is working, can't start RF send.\r\n");
      return 0;
    }

    memset(&wm_send_parameters, 0, sizeof(T_WM_SEND_PARA));

    ret = protocol_command_encode(data, len, &wm_send_parameters);

    if(ret == IRDA_SUCCEED){
    
      if(true == wm_send_module_init(&wm_send_parameters)){
        wm_send_struct.wm_send_state = WM_SEND_CAMMAND;
        // printf("[433_RF]:wave_len:%d...\r\n",wm_send_struct.p_wm_send_data ->send_buf_len);
      }
  }
  return 1;
}

bool wm_send_module_init(T_WM_SEND_PARA *p_wm_send_para)
{
    wm_send_struct.p_wm_send_data = p_wm_send_para;

    return true;
}

#endif




/******************* (C) COPYRIGHT 2024 Seneasy *****END OF FILE****/
