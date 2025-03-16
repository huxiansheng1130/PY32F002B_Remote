/*********************************************************************************************************
 *               Copyright(c) 2024, Seneasy Corporation. All rights reserved.
 **********************************************************************************************************
 * @file     i2c_driver.h
 * @brief
 * @details
 * @author   huzhuohuan
 * @date     2024-02-27
 * @version  V_1.0
 *********************************************************************************************************/

#ifndef _I2C_DRIVER_H_
#define _I2C_DRIVER_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "main.h"
#include "app.h"
#include "py32f002b_ll_i2c.h"

/*============================================================================*
 *                        Export Global Variables
 *============================================================================*/
#define IIC_SCL_GPIO_PORT                       GPIOB
#define IIC_SCL_GPIO_CLK_ENABLE()               LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB)
#define IIC_SCL_PIN                             LL_GPIO_PIN_3
#define IIC_SCL_AF                              LL_GPIO_AF_6

#define IIC_SDA_GPIO_PORT                       GPIOB
#define IIC_SDA_GPIO_CLK_ENABLE()               LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB)
#define IIC_SDA_PIN                             LL_GPIO_PIN_4
#define IIC_SDA_AF                              LL_GPIO_AF_6

#define I2C_STATE_READY                         0
#define I2C_STATE_BUSY_TX                       1
#define I2C_STATE_BUSY_RX                       2

#define MASTER_ADDRESS                          0xA0

/*============================================================================*
 *                          Functions
 *============================================================================*/

/*============================================================================*
 *                      Extern Functions
 *============================================================================*/
void dev_iic_config(void);
void i2c_write_byte(uint8_t devAddress, uint8_t memAddress, uint8_t *pData, uint16_t size);
void i2c_read_byte(uint16_t devAddress, uint16_t memAddress, uint8_t *buf, uint16_t size);
#endif
