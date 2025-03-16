/*********************************************************************************************************
 * @file      keybroad_driver.c
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
#include "../keyboard_module/keyboard_driver.h"

#if (UI_KEYBOARD_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
Kb_code kb_code;

volatile uint32_t key_state = 0;
volatile uint32_t key_timestamp = 0;
volatile uint8_t current_row = 0;

const uint16_t row_pin[] = KB_ROW_PINS;
const uint16_t col_pin[] = KB_COL_PINS;
const uint16_t kb_map_num[ARRAY_SIZE(col_pin)][ARRAY_SIZE(row_pin)] = KB_MAP_NORMAL;
/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief  Configure GPIO pin for EXTI wake-up.
 * @param  group Pointer to GPIO port (e.g., GPIOA, GPIOB).
 * @param  pin Pin number to configure.
 * @retval None
 */
void kb_wakeup_exti_config(GPIO_TypeDef *group, uint8_t pin)
{
  LL_EXTI_InitTypeDef EXTI_InitStruct;

  /* Configure Port as the EXTI Line interrupt input */
  LL_EXTI_SetEXTISource(GPIO_GROUP_TO_PORT(group), EXTI_PIN_TO_CONFIG_LINE(pin));

  /* EXTI Line */
  EXTI_InitStruct.Line = pin;

  /* Line command enable*/
  EXTI_InitStruct.LineCommand = ENABLE;

  /* Event mode */
  EXTI_InitStruct.Mode = LL_EXTI_MODE_EVENT;

  /* Trigger Falling Mode */
  EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_FALLING;

  /* Initialize the EXTI registers according to the specified parameters in EXTI_InitStruct  */
  LL_EXTI_Init(&EXTI_InitStruct);
}

/**
 * @brief  Handle GPIO event for a specific group and pin.
 * @param  group GPIO group identifier (e.g., GROUP_GPIOA, GROUP_GPIOB).
 * @param  pin GPIO pin number within the specified group.
 * @param  kb_gpio_event Pointer to a callback function to handle the GPIO event.
 *                       The callback function must have the signature:
 *                       void callback(uint16_t group, uint8_t pin);
 * @retval None
 */
void kb_gpio_event_handler(uint16_t group, uint8_t pin, Kb_gpio_event kb_gpio_event)
{
  for (uint8_t i = 0; i < 3; i++)
  {
    if (group == GPIO_GROUPA)
      kb_gpio_event(GPIOA, pin);
    else if (group == GPIO_GROUPB)
      kb_gpio_event(GPIOB, pin);
    else if (group == GPIO_GROUPC)
      kb_gpio_event(GPIOC, pin);
  }
}

/**
 * @brief  Initialize a GPIO pin as a keyboard row input.
 * @param  group Pointer to the GPIO group (e.g., GPIOA, GPIOB).
 * @param  pin_num GPIO pin number within the specified group.
 * @retval None
 */
void kb_gpio_init_row(GPIO_TypeDef *group, uint8_t pin_num)
{
  CLEAR_MODER(group, pin_num);
  SET_MODER_OUTPUT(group, pin_num);
  SET_ODR_HIGH(group, pin_num);
}

/**
 * @brief  Initialize a GPIO pin as a column input with pull-up.
 * @param  group Pointer to the GPIO port (e.g., GPIOA, GPIOB).
 * @param  pin_num Pin number of the column.
 * @retval None
 */
void kb_gpio_init_col(GPIO_TypeDef *group, uint8_t pin_num)
{
  CLEAR_MODER(group, pin_num);
  CLEAR_PUPDR(group, pin_num);
  SET_PUPDR_UP(group, pin_num);
}

/**
 * @brief  Set a GPIO pin to high level.
 * @param  group Pointer to the GPIO port (e.g., GPIOA, GPIOB).
 * @param  pin_num Pin number to set high.
 * @retval None
 */
void kb_gpio_set_high(GPIO_TypeDef *group, uint8_t pin_num)
{
  SET_ODR_HIGH(group, pin_num);
}

/**
 * @brief  Set a GPIO pin to low level.
 * @param  group Pointer to the GPIO port (e.g., GPIOA, GPIOB).
 * @param  pin_num Pin number to set low.
 * @retval None
 */
void kb_gpio_set_low(GPIO_TypeDef *group, uint8_t pin_num)
{
  SET_ODR_LOW(group, pin_num);
}

/**
 * @brief  Initialize the keyboard matrix by configuring row and column pins.
 * @retval None
 */
void keybroad_init(void)
{
  RCC->IOPENR |= KB_IOPENR_GPIO_EN;

  // 矩阵键盘行初始化
  for (uint8_t i = 0; i < ARRAY_SIZE(row_pin); i++)
    kb_gpio_event_handler(GET_GPIO_GROUP(row_pin[i]), GET_GPIO_PIN(row_pin[i]), kb_gpio_init_row);

  // 矩阵键盘列初始化
  for (uint8_t i = 0; i < ARRAY_SIZE(col_pin); i++)
    kb_gpio_event_handler(GET_GPIO_GROUP(col_pin[i]), GET_GPIO_PIN(col_pin[i]), kb_gpio_init_col);

  // 矩阵键盘唤醒初始化
  for (uint8_t i = 0; i < ARRAY_SIZE(col_pin); i++)
    kb_gpio_event_handler(GET_GPIO_GROUP(col_pin[i]), GET_GPIO_PIN(col_pin[i]), kb_wakeup_exti_config);
}

/**
 * @brief  Set GPIO pin low to trigger an event.
 * @retval None
 */
void kb_gpio_set_low_event(void)
{
  for (uint8_t i = 0; i < ARRAY_SIZE(row_pin); i++)
    kb_gpio_event_handler(GET_GPIO_GROUP(row_pin[i]), GET_GPIO_PIN(row_pin[i]), kb_gpio_set_low);
}

/**
 * @brief  Scan the keyboard matrix to detect key presses and releases.
 * @retval Key status (KEY_DEBOUNCE, KEY_PRESSED, KEY_RELEASED)
 */
uint8_t kb_matrix_scan(void)
{
  uint8_t ret_status = KEY_DEBOUNCE;

  // 拉高所有行线
  for (uint8_t i = 0; i < ARRAY_SIZE(row_pin); i++)
    kb_gpio_event_handler(GET_GPIO_GROUP(row_pin[i]), GET_GPIO_PIN(row_pin[i]), kb_gpio_set_high);

  // 激活当前行
  kb_gpio_event_handler(GET_GPIO_GROUP(row_pin[current_row]), GET_GPIO_PIN(row_pin[current_row]), kb_gpio_set_low);

  // 读取列线状态
  uint8_t col_state = 0;
  for (uint8_t i = 0; i < ARRAY_SIZE(col_pin); i++)
  {
    uint16_t group_col = GET_GPIO_GROUP(col_pin[i]);
    uint8_t pin_col = GET_GPIO_PIN(col_pin[i]);

    // 根据组别读取对应端口的输入数据
    if (group_col == GPIO_GROUPA)
      col_state |= ((~GPIOA->IDR) & pin_col) ? (1 << i) : 0;
    else if (group_col == GPIO_GROUPB)
      col_state |= ((~GPIOB->IDR) & pin_col) ? (1 << i) : 0;
    else if (group_col == GPIO_GROUPC)
      col_state |= ((~GPIOC->IDR) & pin_col) ? (1 << i) : 0;
  }

  for (uint8_t c = 0; c < ARRAY_SIZE(col_pin); c++)
  {
    uint32_t pos = GET_KB_POS((current_row * ARRAY_SIZE(col_pin)), c);
    uint32_t state = GET_KB_POS_STATUS(key_state, pos);

    if (col_state & (1 << c))
    {
      if (state == KEY_RELEASED)
      {
        state = KEY_DEBOUNCE;
        key_timestamp = clock_time() | 1;
      }
      else if (state == KEY_DEBOUNCE && clock_time_exceed(key_timestamp, KB_DITHER_TIMEOUT * 1000))
      {
        state = KEY_PRESSED;
        ret_status = state;
        kb_code.status = KB_SHORT_PRESS;
        kb_code.kb_short_time = 0;
        kb_code.kb_trigger_time = clock_time() | 1;

        // 按键状态按下处理
        if (kb_code.cnt < 0x02)
        {
          kb_code.kb_now_code[kb_code.cnt] = kb_map_num[c][current_row];
          kb_code.cnt++;
        }
      }
    }
    else
    {
      if (state == KEY_PRESSED || state == KEY_DEBOUNCE)
      {
        state = KEY_RELEASED;
        ret_status = state;
        key_timestamp = clock_time() | 1;
        kb_code.kb_trigger_time = 0;
        kb_code.status = BK_RELEASE;

        // 按键状态释放处理
        if (kb_code.cnt)
        {
          kb_code.cnt--;
          if (kb_code.kb_now_code[kb_code.cnt] != kb_map_num[c][current_row])
          {
            kb_code.kb_now_code[0] = kb_code.kb_now_code[kb_code.cnt];
          }
          kb_code.kb_now_code[kb_code.cnt] = 0;
        }
      }
    }

    key_state &= CLEAR_KB_POS_STATUS(pos);
    key_state |= SET_KB_POS_STATUS(pos, state);
  }

  // 切换下一行线
  current_row = (current_row + 1) % ARRAY_SIZE(row_pin);

  return ret_status;
}

#if(KEYBOARD_TEST)
/**
 * @brief  keyboard_loop for keyboard scanning and event handling.
 * @retval None
 */
static void keyboard_test_loop(void)
{
  static uint32_t tick;

  if (clock_time_exceed(tick, 8000))
  {
    tick = clock_time() | 1;
  }
  else
  {
    return;
  }

  uint8_t det_key = kb_matrix_scan();

  if (det_key == KEY_PRESSED)
  {
    printf("--->>> kb_now_code[0] : %d ,kb_now_code[1] :%d \r\n", kb_code.kb_now_code[0], kb_code.kb_now_code[1]);
  }
  else if (det_key == KEY_RELEASED)
  {
    printf("--->>> kb_now_code[0] : %d ,kb_now_code[1] :%d \r\n", kb_code.kb_now_code[0], kb_code.kb_now_code[1]);
  }
}
#endif
#endif