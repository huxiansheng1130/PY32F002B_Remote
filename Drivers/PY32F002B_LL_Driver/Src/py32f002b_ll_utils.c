/**
  ******************************************************************************
  * @file    py32f002b_ll_utils.c
  * @author  MCU Application Team
  * @brief   UTILS LL module driver.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "py32f002b_ll_utils.h"
#include "py32f002b_ll_rcc.h"
#include "py32f002b_ll_system.h"
#ifdef  USE_FULL_ASSERT
#include "py32_assert.h"
#else
#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

/** @addtogroup PY32F002B_LL_Driver
  * @{
  */

/** @addtogroup UTILS_LL
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/** @addtogroup UTILS_LL_Private_Constants
  * @{
  */

/* Defines used for FLASH latency according to HCLK Frequency */
#define UTILS_SCALE1_LATENCY1_FREQ  24000000U       /*!< HCLK frequency to set FLASH latency 1 in power scale 1  */
#define UTILS_SCALE1_LATENCY2_FREQ  48000000U       /*!< HCLK frequency to set FLASH latency 2 in power scale 1  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/
/** @addtogroup UTILS_LL_Private_Macros
  * @{
  */
#define IS_LL_UTILS_SYSCLK_DIV(__VALUE__) (((__VALUE__) == LL_RCC_SYSCLK_DIV_1)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_2)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_4)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_8)   \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_16)  \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_64)  \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_128) \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_256) \
                                        || ((__VALUE__) == LL_RCC_SYSCLK_DIV_512))

#define IS_LL_UTILS_APB1_DIV(__VALUE__) (((__VALUE__) == LL_RCC_APB1_DIV_1) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_2) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_4) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_8) \
                                      || ((__VALUE__) == LL_RCC_APB1_DIV_16))


#define IS_LL_UTILS_HSE_BYPASS(__STATE__) (((__STATE__) == LL_UTILS_HSEBYPASS_ON) \
                                        || ((__STATE__) == LL_UTILS_HSEBYPASS_OFF))

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup UTILS_LL_Exported_Functions
  * @{
  */

/** @addtogroup UTILS_LL_EF_DELAY
  * @{
  */

/**
  * @brief  This function configures the Cortex-M SysTick source to have 1ms time base.
  * @note   When a RTOS is used, it is recommended to avoid changing the Systick
  *         configuration by calling this function, for a delay use rather osDelay RTOS service.
  * @param  HCLKFrequency HCLK frequency in Hz
  * @note   HCLK frequency can be calculated thanks to RCC helper macro or function @ref LL_RCC_GetSystemClocksFreq
  * @retval None
  */
void LL_Init1msTick(uint32_t HCLKFrequency)
{
  /* Use frequency provided in argument */
  LL_InitTick(HCLKFrequency, 1000U);
}

/**
  * @brief  This function provides accurate delay (in milliseconds) based
  *         on SysTick counter flag
  * @note   When a RTOS is used, it is recommended to avoid using blocking delay
  *         and use rather osDelay service.
  * @note   To respect 1ms timebase, user should call @ref LL_Init1msTick function which
  *         will configure Systick to 1ms
  * @param  Delay specifies the delay time length, in milliseconds.
  * @retval None
  */
void LL_mDelay(uint32_t Delay)
{
  __IO uint32_t  tmp = SysTick->CTRL;  /* Clear the COUNTFLAG first */
  uint32_t tmpDelay; /* MISRAC2012-Rule-17.8 */
  /* Add this code to indicate that local variable is not used */
  ((void)tmp);
  tmpDelay  = Delay;
  /* Add a period to guaranty minimum wait */
  if (tmpDelay  < LL_MAX_DELAY)
  {
    tmpDelay ++;
  }

  while (tmpDelay  != 0U)
  {
    if ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != 0U)
    {
      tmpDelay --;
    }
  }
}

/**
 * @brief Initialize SysTick as a free-running counter without enabling interrupts.
 * 
 * This function configures the SysTick timer to run freely without generating interrupts.
 * It sets the SysTick to use the core clock (48 MHz) and enables the counter.
 */
void systick_init(void)
{
  SysTick->LOAD = 0xFFFFFF;  
  SysTick->VAL = 0;          
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |  
                  SysTick_CTRL_ENABLE_Msk;      
}

/**
 * @brief Get the current system time in milliseconds.
 * @return Current timestamp as a 32-bit unsigned integer.
 */
uint32_t clock_time(void)
{
  static uint32_t overflow_count = 0;  
  static uint32_t last_value = 0;     
  
  uint32_t current_value = SysTick->VAL & 0xFFFFFF;  
  
  if (current_value > last_value) 
    overflow_count++;  
  
  last_value = current_value;
  
  uint32_t ticks_per_ms = 48;  
  uint64_t total_ticks = (uint64_t)overflow_count * 0x1000000 + (0xFFFFFF - current_value);
  return (uint32_t)(total_ticks / ticks_per_ms);
}

/**
 * @brief Check if the time has exceeded the timeout threshold.
 * @param start_time The recorded start time (obtained by clock_time()).
 * @param timeout_ms The timeout threshold (unit: milliseconds).
 * @return 1 if timeout has occurred, 0 if not.
 */
_Bool clock_time_exceed(uint32_t start_time, uint32_t timeout_ms)
{
    return (((uint32_t)clock_time() - start_time) >= timeout_ms);
}

void WaitUs(uint32_t Delay) 
{
  uint32_t start = clock_time();  
  while ((clock_time() - start) < Delay);  
}

void WaitMs(uint32_t Delay) 
{
  uint32_t start = clock_time();  
  while ((clock_time() - start) < (Delay * 1000));  
}


/**
  * @brief  This function sets directly SystemCoreClock CMSIS variable.
  * @note   Variable can be calculated also through SystemCoreClockUpdate function.
  * @param  HCLKFrequency HCLK frequency in Hz (can be calculated thanks to RCC helper macro)
  * @retval None
  */
void LL_SetSystemCoreClock(uint32_t HCLKFrequency)
{
  /* HCLK clock frequency */
  SystemCoreClock = HCLKFrequency;
}

/**
  * @brief  Update number of Flash wait states in line with new frequency and current
  *         voltage range.
  * @param  HCLKFrequency  HCLK frequency
  * @retval An ErrorStatus enumeration value:
  *          - SUCCESS: Latency has been modified
  *          - ERROR: Latency cannot be modified
  */
ErrorStatus LL_SetFlashLatency(uint32_t HCLKFrequency)
{
  uint32_t timeout;
  uint32_t getlatency;
  uint32_t latency;
  ErrorStatus status;

  /* Frequency cannot be equal to 0 or greater than max clock */
  if ((HCLKFrequency == 0U) || (HCLKFrequency > UTILS_SCALE1_LATENCY2_FREQ))
  {
    return ERROR;
  }

  if (HCLKFrequency > UTILS_SCALE1_LATENCY1_FREQ)
  {
    /* 24 < HCLK <= 48 => 1WS (2 CPU cycles) */
    latency = LL_FLASH_LATENCY_1;
  }
  else
  {
    /* else HCLKFrequency < 24MHz default LL_FLASH_LATENCY_0 0WS */
    latency = LL_FLASH_LATENCY_0;
  }

  LL_FLASH_SetLatency(latency);

  /* Check that the new number of wait states is taken into account to access the Flash
     memory by reading the FLASH_ACR register */
  timeout = 2u;
  do
  {
    /* Wait for Flash latency to be updated */
    getlatency = LL_FLASH_GetLatency();
    timeout--;
  }
  while ((getlatency != latency) && (timeout > 0u));

  if (getlatency != latency)
  {
    status = ERROR;
  }
  else
  {
    status = SUCCESS;
  }

  return status;
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
