/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     keybroad_driver.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2024-02-27
 * @version  V_1.0
 *********************************************************************************************************/

#ifndef _KEYBROAD_DRIVER_H_
#define _KEYBROAD_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"
#include "../keyboard_module/keyboard_config.h"

#if (UI_KEYBOARD_ENABLE)
/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
#ifndef KB_DITHER_TIMEOUT
#define KB_DITHER_TIMEOUT                       30
#endif

#ifndef KB_SHORT_TIMEOUT
#define KB_SHORT_TIMEOUT                        150
#endif

enum
{
    BK_RELEASE,
    KB_SHORT_PRESS,
    KBLONG_PRESS_3S,
    KB_TIMEOUT_5S
};

typedef struct
{
    uint8_t cnt;
    uint8_t kb_now_code[2];
    uint8_t status;
    uint32_t kb_short_time;
    uint32_t kb_trigger_time;
} Kb_code;
extern Kb_code kb_code;

typedef void (*Kb_gpio_event)           (GPIO_TypeDef *group, uint8_t pin_num);

#define BIT(n)                  		(1<<(n))
#define ARRAY_SIZE(a) 			        (sizeof(a) / sizeof(*a))

#define RCC_IOPENR_GPIOA_BEN            (RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN)
#define RCC_IOPENR_GPIOA_CEN            (RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOCEN)
#define RCC_IOPENR_GPIOB_CEN            (RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN)
#define RCC_IOPENR_GPIOA_B_CEN          (RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN)

enum
{
    NULL_KEY,
    ONE_KEY,
    TWO_KEY,
}; 

typedef enum
{
    GPIO_GROUPA = 0x000,
    GPIO_GROUPB = 0x400,
    GPIO_GROUPC = 0x800,

    GPIO_PA0  = GPIO_GROUPA | BIT(0U),
    GPIO_PA1  = GPIO_GROUPA | BIT(1U),
    GPIO_PA2  = GPIO_GROUPA | BIT(2U),
    GPIO_PA3  = GPIO_GROUPA | BIT(3U),
    GPIO_PA4  = GPIO_GROUPA | BIT(4U),
    GPIO_PA5  = GPIO_GROUPA | BIT(5U),
    GPIO_PA6  = GPIO_GROUPA | BIT(6U),
    GPIO_PA7  = GPIO_GROUPA | BIT(7U),

    GPIO_PB0  = GPIO_GROUPB | BIT(0U),
    GPIO_PB1  = GPIO_GROUPB | BIT(1U),
    GPIO_PB2  = GPIO_GROUPB | BIT(2U),
    GPIO_PB3  = GPIO_GROUPB | BIT(3U),
    GPIO_PB4  = GPIO_GROUPB | BIT(4U),
    GPIO_PB5  = GPIO_GROUPB | BIT(5U),
    GPIO_PB6  = GPIO_GROUPB | BIT(6U),
    GPIO_PB7  = GPIO_GROUPB | BIT(7U),
 
    GPIO_PC0  = GPIO_GROUPC | BIT(0U),
    GPIO_PC1  = GPIO_GROUPC | BIT(1U),
    GPIO_PC2  = GPIO_GROUPC | BIT(2U),
    GPIO_PC3  = GPIO_GROUPC | BIT(3U),
    GPIO_PC4  = GPIO_GROUPC | BIT(4U),
    GPIO_PC5  = GPIO_GROUPC | BIT(5U),
    GPIO_PC6  = GPIO_GROUPC | BIT(6U),
    GPIO_PC7  = GPIO_GROUPC | BIT(7U),
} GPIO_PinTypeDef;

#define GET_GPIO_GROUP(port_pin)                ((port_pin) & 0xF00)
#define GET_GPIO_PIN(port_pin)                  ((port_pin) & 0x00FF)

#define GET_KB_POS(tatol_row, col)              ((tatol_row) + (col))
#define GET_KB_POS_STATUS(list,pos)             ((list >> (pos * 2)) & 0x03)   

#define CLEAR_KB_POS_STATUS(pos)                (~(0x03 << (pos * 2)))
#define SET_KB_POS_STATUS(pos, st)              ((st << (pos * 2)))

#define CLEAR_MODER(port, pin)                  (port->MODER &= ~(0x3 *((pin) * (pin))))
#define SET_MODER_OUTPUT(port, pin)             (port->MODER |= (((pin) * (pin))))
#define SET_ODR_HIGH(port, pin)                 (port->ODR   |= ((pin)))
#define SET_ODR_LOW(port, pin)                  (port->ODR   &= ~((pin)))

#define CLEAR_PUPDR(port, pin)                  (port->PUPDR &= ~(0x3 *((pin) * (pin))))
#define SET_PUPDR_UP(port, pin)                 (port->PUPDR |= (((pin) * (pin))))

#define GPIO_GROUP_TO_PORT(group)               ((group) == GPIOA ? 0 : ((group) == GPIOB ? 1 : ((group) == GPIOC ? 2 : -1)))
#define EXTI_PIN_TO_CONFIG_LINE(pin)            ((pin) == BIT(0) ? LL_EXTI_CONFIG_LINE0 : \
                                                 (pin) == BIT(1) ? LL_EXTI_CONFIG_LINE1 : \
                                                 (pin) == BIT(2) ? LL_EXTI_CONFIG_LINE2 : \
                                                 (pin) == BIT(3) ? LL_EXTI_CONFIG_LINE3 : \
                                                 (pin) == BIT(4) ? LL_EXTI_CONFIG_LINE4 : \
                                                 (pin) == BIT(5) ? LL_EXTI_CONFIG_LINE5 : \
                                                 (pin) == BIT(6) ? LL_EXTI_CONFIG_LINE6 : \
                                                 (pin) == BIT(7) ? LL_EXTI_CONFIG_LINE7 : -1)


#define KEY_DEBOUNCE                            0x01 
#define KEY_PRESSED                             0x02  
#define KEY_RELEASED                            0x00 
/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
extern uint8_t kb_matrix_scan(void);
extern void kb_gpio_set_low_event(void);
extern void keybroad_init(void);

#endif
#endif