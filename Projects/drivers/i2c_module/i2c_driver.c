/*********************************************************************************************************
 * @file      i2c_driver.c
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
#include "i2c_driver.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
__IO static uint32_t i2cState = I2C_STATE_READY;

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief Configures the IIC device.
 *
 * @param None
 * @return None
 */
void dev_iic_config(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    IIC_SCL_GPIO_CLK_ENABLE();
    IIC_SDA_GPIO_CLK_ENABLE();
    
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    // PB3 SCL
    GPIO_InitStruct.Pin = IIC_SCL_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Alternate = IIC_SCL_AF;
    LL_GPIO_Init(IIC_SCL_GPIO_PORT, &GPIO_InitStruct);

    // PB4 SDA
    GPIO_InitStruct.Pin = IIC_SDA_PIN;
    GPIO_InitStruct.Alternate = IIC_SDA_AF;
    LL_GPIO_Init(IIC_SDA_GPIO_PORT, &GPIO_InitStruct);

    LL_APB1_GRP1_ForceReset(LL_APB1_GRP1_PERIPH_I2C1);
    LL_APB1_GRP1_ReleaseReset(LL_APB1_GRP1_PERIPH_I2C1);

    LL_I2C_InitTypeDef I2C_InitStruct;
    /*
     * Clock speed:
     * - standard = 100khz
     * - fast     = 400khz
     */
    I2C_InitStruct.ClockSpeed = LL_I2C_MAX_SPEED_FAST;
    I2C_InitStruct.DutyCycle = LL_I2C_DUTYCYCLE_16_9;
    I2C_InitStruct.OwnAddress1 = MASTER_ADDRESS;
    I2C_InitStruct.TypeAcknowledge = LL_I2C_NACK;
    LL_I2C_Init(I2C1, &I2C_InitStruct);
}

/**
 * @brief Writes data to an I2C device.
 *
 * @param devAddress The address of the I2C device.
 * @param memAddress The memory address to write to on the device.
 * @param pData Pointer to the data buffer for writing.
 * @param size Number of bytes to write.
 */
void i2c_write_byte(uint8_t devAddress, uint8_t memAddress, uint8_t *pData, uint16_t size)
{
    while (i2cState != I2C_STATE_READY)
        ;
    i2cState = I2C_STATE_BUSY_TX;
    /* Disable Pos */
    LL_I2C_DisableBitPOS(I2C1);

    /* Generate Start */
    LL_I2C_GenerateStartCondition(I2C1);
    /* Wait until SB flag is set */
    while (LL_I2C_IsActiveFlag_SB(I2C1) != 1)
        ;
    /* Send slave address */
    LL_I2C_TransmitData8(I2C1, (devAddress & (uint8_t)(~0x01)));
    /* Wait until ADDR flag is set */
    while (LL_I2C_IsActiveFlag_ADDR(I2C1) != 1)
        ;
    /* Clear ADDR flag */
    LL_I2C_ClearFlag_ADDR(I2C1);

    /* Wait until TXE flag is set */
    while (LL_I2C_IsActiveFlag_TXE(I2C1) != 1)
        ;
    /* Send memory address */
    LL_I2C_TransmitData8(I2C1, memAddress);

    /* Transfer data */
    while (size > 0)
    {
        while (LL_I2C_IsActiveFlag_TXE(I2C1) != 1)
            ;
        LL_I2C_TransmitData8(I2C1, *pData++);
        size--;

        if ((LL_I2C_IsActiveFlag_BTF(I2C1) == 1) && (size != 0U))
        {
            LL_I2C_TransmitData8(I2C1, *pData++);
            size--;
        }

        while (LL_I2C_IsActiveFlag_BTF(I2C1) != 1)
            ;
    }

    /* Stop */
    LL_I2C_GenerateStopCondition(I2C1);

    i2cState = I2C_STATE_READY;
}

/**
 * @brief Reads data from an I2C device.
 * 
 * @param devAddress The 7-bit or 10-bit address of the I2C device.
 * @param memAddress The memory address on the device from which to read.
 * @param buf Pointer to the buffer where the read data will be stored.
 * @param size The number of bytes to read from the device.
 * 
 * @return None
 */
void i2c_read_byte(uint16_t devAddress, uint16_t memAddress, uint8_t *buf, uint16_t size)
{
    while (i2cState != I2C_STATE_READY)
        ;
    i2cState = I2C_STATE_BUSY_RX;
    /* Disable Pos */
    LL_I2C_DisableBitPOS(I2C1);

    /***** Send device address + memory address *****/

    /* Enable Acknowledge */
    LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
    /* Generate Start */
    LL_I2C_GenerateStartCondition(I2C1);
    /* Wait until SB flag is set */
    while (LL_I2C_IsActiveFlag_SB(I2C1) != 1)
        ;

    /* Send slave address */
    LL_I2C_TransmitData8(I2C1, (devAddress & (uint8_t)(~0x01)));
    /* Wait until ADDR flag is set */
    while (LL_I2C_IsActiveFlag_ADDR(I2C1) != 1)
        ;
    /* Clear ADDR flag */
    LL_I2C_ClearFlag_ADDR(I2C1);

    /* Wait until TXE flag is set */
    while (LL_I2C_IsActiveFlag_TXE(I2C1) != 1)
        ;
    /* Send memory address */
    LL_I2C_TransmitData8(I2C1, (uint8_t)(memAddress & 0x00FF));
    while (LL_I2C_IsActiveFlag_BTF(I2C1) != 1)
        ;

    /***** Restart to read *****/

    /* Generate Start */
    LL_I2C_GenerateStartCondition(I2C1);
    /* Wait until SB flag is set */
    while (LL_I2C_IsActiveFlag_SB(I2C1) != 1)
        ;

    /* Send slave address */
    LL_I2C_TransmitData8(I2C1, (devAddress | 0x1));
    /* Wait until ADDR flag is set */
    while (LL_I2C_IsActiveFlag_ADDR(I2C1) != 1)
        ;

    if (size == 0U)
    {
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_GenerateStopCondition(I2C1);
    }
    else if (size == 1U)
    {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);

        __disable_irq();
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_GenerateStopCondition(I2C1);
        __enable_irq();
    }
    else if (size == 2U)
    {
        LL_I2C_EnableBitPOS(I2C1);

        __disable_irq();
        LL_I2C_ClearFlag_ADDR(I2C1);
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
        __enable_irq();
    }
    else
    {
        LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
        LL_I2C_ClearFlag_ADDR(I2C1);
    }

    while (size > 0U)
    {
        if (size <= 3U)
        {
            if (size == 1U)
            {
                while (LL_I2C_IsActiveFlag_RXNE(I2C1) != 1)
                    ;
                *buf++ = LL_I2C_ReceiveData8(I2C1);
                size--;
            }
            else if (size == 2U)
            {
                while (LL_I2C_IsActiveFlag_BTF(I2C1) != 1)
                    ;

                __disable_irq();
                LL_I2C_GenerateStopCondition(I2C1);
                *buf++ = LL_I2C_ReceiveData8(I2C1);
                size--;
                __enable_irq();

                *buf++ = LL_I2C_ReceiveData8(I2C1);
                size--;
            }
            else
            {
                while (LL_I2C_IsActiveFlag_BTF(I2C1) != 1)
                    ;

                LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);

                __disable_irq();
                *buf++ = LL_I2C_ReceiveData8(I2C1);
                size--;
                while (LL_I2C_IsActiveFlag_BTF(I2C1) != 1)
                    ;
                LL_I2C_GenerateStopCondition(I2C1);
                *buf++ = LL_I2C_ReceiveData8(I2C1);
                size--;
                __enable_irq();

                *buf++ = LL_I2C_ReceiveData8(I2C1);
                size--;
            }
        }
        else
        {
            while (LL_I2C_IsActiveFlag_RXNE(I2C1) != 1)
                ;

            *buf++ = LL_I2C_ReceiveData8(I2C1);
            size--;

            if (LL_I2C_IsActiveFlag_BTF(I2C1) == 1)
            {
                *buf++ = LL_I2C_ReceiveData8(I2C1);
                size--;
            }
        }
    }

    i2cState = I2C_STATE_READY;
}