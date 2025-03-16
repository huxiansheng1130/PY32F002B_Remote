/*********************************************************************************************************
 * @file      qmi8658a_driver.c
 *
 * @details   Improved driver for QMI8658A sensor with floating point conversion and output
 *
 * @author    huzhuohuan (original), updated
 * @date      2025-03-12
 * @version   V_1.1
 ********************************************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "qmi8658a_driver.h"

#if (GYROSCOPE_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/
_Bool task_run = 0;
uint8_t show_flag = 0;

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief Write a byte to QMI8658A register
 * @param reg Register address
 * @param value Value to write
 */
void qmi8658a_write_byte(uint8_t reg, uint8_t value)
{
    i2c_write_byte(QMI8658A_ADDRESS, reg, &value, 1);
    WaitMs(1);
}

/**
 * @brief Read a byte from QMI8658A register
 * @param reg Register address
 * @return Value read from register
 */
uint8_t qmi8658a_read_byte(uint8_t reg)
{
    uint8_t ret;
    i2c_read_byte(QMI8658A_ADDRESS, reg, &ret, 1);
    return ret;
}

/**
 * @brief Initialize QMI8658A sensor with proper configuration
 * @return true if initialization successful, false otherwise
 */
_Bool qmi8658a_init(void)
{
    uint8_t chip_id = 0;

    /* Soft reset the sensor */
    qmi8658a_write_byte(QMI8658A_RESET, 0xB6);

    /* Wait for reset to complete */
    WaitMs(10);

    /* Verify chip ID */
    chip_id = qmi8658a_read_byte(QMI8658A_WHO_AM_I);

    if (chip_id != QMI8658A_WHO_AM_I_VAL)
        return 0;

    /* Configure SPI/I2C interface - use I2C, bit4=1 */
    qmi8658a_write_byte(QMI8658A_CTRL1, 0x10);

    /* Configure accelerometer:
     * - Set full scale to ±8g (bits 6:4 = 010)
     * - Set ODR to 125Hz (bits 3:0 = 0110)
     */
    qmi8658a_write_byte(QMI8658A_CTRL2, 0x26);

    /* Configure gyroscope:
     * - Set full scale to ±256dps (bits 6:4 = 100)
     * - Set ODR to 112Hz (bits 3:0 = 0111)
     */
    qmi8658a_write_byte(QMI8658A_CTRL3, 0x47);

    /* Low pass filter setting - use default settings */
    qmi8658a_write_byte(QMI8658A_CTRL5, 0x00);

    /* Enable sensors (Accelerometer and Gyroscope) */
    qmi8658a_write_byte(QMI8658A_CTRL7, 0x03);

    /* Motion detection control - disabled */
    qmi8658a_write_byte(QMI8658A_CTRL8, 0x00);

    /* Host commands - normal operation */
    qmi8658a_write_byte(QMI8658A_CTRL9, 0x00);

    /* Configure FIFO settings - disabled */
    qmi8658a_write_byte(QMI8658A_FIFO_WTM_TH, 0x00);
    qmi8658a_write_byte(QMI8658A_FIFO_CTRL, 0x00);

    return 1;
}

/**
 * @brief Enables the QMI8658A sensor driver.
 *
 * Initializes and activates the driver for the QMI8658A sensor, preparing it for data collection.
 * This function may configure the sensor's settings, power management, and interrupt handling.
 */
void qmi8658a_driver_enable()
{
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_0);
}

/**
 * @brief Disables the QMI8658A sensor driver.
 *
 * This function disables the driver for the QMI8658A sensor.
 */
void qmi8658a_driver_disable()
{
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_0);
}

/**
 * @brief Configures the driver for the QMI8658A sensor.
 *
 * Sets up the sensor's operational parameters, such as scale, filtering, and interrupt settings.
 * This is a crucial step to ensure the sensor functions correctly and provides accurate data.
 *
 * @note Should be called after initialization and before starting data collection.
 */
void qmi8658a_driver_config(void)
{
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);

    LL_GPIO_SetPinMode(GPIOB, LL_GPIO_PIN_0, LL_GPIO_MODE_OUTPUT);

    qmi8658a_driver_enable();
}

/**
 * @brief Configures the timing-related settings for the QMI8658A sensor.
 *
 * This function is responsible for setting up the timing configurations of the QMI8658A sensor,
 * such as sample rate, timing mode, and synchronization with other sensors or systems.
 * Proper timing configuration is essential for accurate sensor data and system performance.
 */
void qmi8658a_tim_config(void)
{
    LL_TIM_InitTypeDef TIM_InitTypeDef = {0};

    LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM14EN);

    TIM_InitTypeDef.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitTypeDef.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitTypeDef.Prescaler = 48 - 1;
    TIM_InitTypeDef.Autoreload = 5000 - 1;
    TIM_InitTypeDef.RepetitionCounter = 0;
    LL_TIM_Init(TIM14, &TIM_InitTypeDef);

    LL_TIM_EnableIT_UPDATE(TIM14);
    LL_TIM_EnableCounter(TIM14);

    NVIC_EnableIRQ(TIM14_IRQn);
    NVIC_SetPriority(TIM14_IRQn, 1);
}

/**
 * @brief TIM14 interrupt handler.
 *
 * Handles interrupts generated by TIM14, which is a timer peripheral.
 * This function can be used to manage timing-related tasks such as sensor data sampling,
 * periodic processing, or triggering other system events.
 */
void TIM14_IRQHandler(void)
{
    if (LL_TIM_IsActiveFlag_UPDATE(TIM14) && LL_TIM_IsEnabledIT_UPDATE(TIM14))
    {
        LL_TIM_ClearFlag_UPDATE(TIM14);
        task_run = 1;
        show_flag++;
    }
}

/**
 * @brief Main loop function for the QMI8658A sensor operations.
 *
 * This function likely contains the main processing loop for the QMI8658A sensor,
 * where it continuously reads data, processes it, and performs any necessary actions.
 * It may also handle sensor configuration, error checking, and data output.
 */
void qmi8658a_setup_init(void)
{
    qmi8658a_tim_config();

    qmi8658a_driver_config();

    if (qmi8658a_init())
    {
        qst_algo_init();

        WaitUs(50);

        init_state_recognition(&qmi8658_read_reg);

        WaitUs(300);
    }
}


#endif