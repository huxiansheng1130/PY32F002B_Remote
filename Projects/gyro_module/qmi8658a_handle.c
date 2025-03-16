/*********************************************************************************************************
 * @file      qmi8658a_handle.c
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
#include "qmi8658a_handle.h"

#if (GYROSCOPE_ENABLE)
/*============================================================================*
 *                              Macro Definitions
 *============================================================================*/

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
extern QST_Filter gyro_filter;
extern QST_Filter accel_filter;
QST_Filter_Buffer gyro_buf[3];
QST_Filter_Buffer accel_buf[3];

float accl[3], gyro[3];
float accel_correct[3] = {0, 0, 0};
float gyro_correct[3] = {0, 0, 0};
float dt = 0.01f;
float euler_angle[3] = {0, 0, 0};
float quater[4] = {1, 0, 0, 0}; 
float line_acc[3] = {0, 0, 0};

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief Read accelerometer data from QMI8658A and convert to m/s²
 * @param accel_float Array to store X,Y,Z acceleration in m/s²
 */
void qmi8658a_read_accel_float(float accel_float[3])
{
    uint8_t data[6];
    const float accel_sensitivity = 4096.0f; // For ±8g range (from datasheet: 4096 LSB/g)
    const float gravity = 9.80665f;          // Standard gravity in m/s²

    for (int i = 0; i < 6; i++)
        data[i] = qmi8658a_read_byte(0x35 + i);

    int16_t raw_accel[3];
    raw_accel[0] = (int16_t)((data[1] << 8) | data[0]);
    raw_accel[1] = (int16_t)((data[3] << 8) | data[2]);
    raw_accel[2] = (int16_t)((data[5] << 8) | data[4]);

    for (int i = 0; i < 3; i++)
        accel_float[i] = ((float)raw_accel[i] / accel_sensitivity) * gravity;
}

/**
 * @brief Read gyroscope data from QMI8658A and convert to rad/s
 * @param gyro_float Array to store X,Y,Z angular rates in rad/s
 */
void qmi8658a_read_gyro_float(float gyro_float[3])
{
    uint8_t data[6];
    const float gyro_sensitivity = 128.0f; // For ±256dps range (from datasheet: 128 LSB/dps)
    const float dps_to_rads = 0.0174533f;  // Conversion factor from degrees/s to radians/s (pi/180)

    for (int i = 0; i < 6; i++)
        data[i] = qmi8658a_read_byte(0x3B + i);

    int16_t raw_gyro[3];
    raw_gyro[0] = (int16_t)((data[1] << 8) | data[0]);
    raw_gyro[1] = (int16_t)((data[3] << 8) | data[2]);
    raw_gyro[2] = (int16_t)((data[5] << 8) | data[4]);

    for (int i = 0; i < 3; i++)
        gyro_float[i] = ((float)raw_gyro[i] / gyro_sensitivity) * 10 / 573;
}

/**
 * @brief Read both accelerometer and gyroscope data from QMI8658A as floating point
 * @param accel_float Array to store X,Y,Z acceleration in g
 * @param gyro_float Array to store X,Y,Z angular rates in dps
 */
void qmi8658a_read_sensors_float(float accel_float[3], float gyro_float[3])
{
    qmi8658a_read_accel_float(accel_float);
    qmi8658a_read_gyro_float(gyro_float);
}

/**
 * @brief Reads data from a specified register of the QMI8658 sensor.
 *
 * @param reg The address of the register to read from.
 * @param buf Pointer to a buffer where the read data will be stored.
 * @param len The number of bytes to read.
 * @return uint8_t Status of the read operation (0 for success).
 */
uint8_t qmi8658_read_reg(unsigned char reg, unsigned char *buf, unsigned short len)
{
    i2c_read_byte(QMI8658A_ADDRESS, reg, buf, len);
    return 1;
}

/**
 * @brief Initializes the QST algorithm.
 *
 * This function sets up the necessary parameters and configurations for the QST (Quantitative Sensor Test) algorithm.
 * It is typically called during the initialization phase of the application to ensure that the algorithm is ready for use.
 */
void qst_algo_init(void)
{
    set_cutoff_frequency(50, 1, &gyro_filter); // First parameter is frequency 100Hz, related to algorithm library call period
    set_cutoff_frequency(50, 2, &accel_filter);
}

/**
 * @brief Processes data from the QMI8658A sensor.
 *
 * This function is responsible for handling the data collected from the QMI8658A sensor.
 * It may include operations such as filtering, calibration, and conversion of raw sensor data into usable formats.
 */
void qmi8658a_data_process(void)
{
    qmi8658a_read_sensors_float(accl, gyro);

    // Apply filter to raw sensor data
    accel_correct[0] = Filter_Apply(accl[0], &accel_buf[0], &accel_filter);
    accel_correct[1] = Filter_Apply(accl[1], &accel_buf[1], &accel_filter);
    accel_correct[2] = Filter_Apply(accl[2], &accel_buf[2], &accel_filter);

    gyro_correct[0] = Filter_Apply(gyro[0], &gyro_buf[0], &gyro_filter);
    gyro_correct[1] = Filter_Apply(gyro[1], &gyro_buf[1], &gyro_filter);
    gyro_correct[2] = Filter_Apply(gyro[2], &gyro_buf[2], &gyro_filter);

    // Update fusion algorithm
    qst_fusion_update(accel_correct, gyro_correct, &dt, euler_angle, quater, line_acc);
}

void qmi8658a_loop(void)
{
    if (task_run == 1)
    {
        task_run = 0;
        qmi8658a_data_process();
    }
    if (!(show_flag % 5))
    {
        show_flag = 0;
        rtt_printf("--->>> ptich:%ld,roll:%ld,yaw:%ld \r\n", (int)euler_angle[0], (int)euler_angle[1], (int)euler_angle[2]);
    }
}
#endif