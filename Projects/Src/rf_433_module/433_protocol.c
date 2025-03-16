/**
*********************************************************************************************************
*               Copyright(c) 2024, Seneasy. All rights reserved.
**********************************************************************************************************
* @file         433_protocol.c
* @brief        This file provides 433MHz module encode handler of protocol.
* @details
* @author       chenrongrong
* @date         2024-12-23
* @version      v1.0
*********************************************************************************************************
*/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "stdint.h"
#include "433_protocol.h"
#include "433_send_driver.h"

#if (UI_RF_ENABLE)
/*============================================================================*
 *                              Variables
 *============================================================================*/
const T_WM_SPEC SPEC =
{
    {0xff, 0xff, 0xff, 0xff},   //wake_up_code[WAKE_UP_CODE_LEN];
    WAKE_UP_CODE_LEN,
    10,                         //header_len;
    5                           //stop_len
};

/*============================================================================*
 *                              Local Functions
 *============================================================================*/
// CRC-8多项式：x^8 + x^2 + x + 1 = 0x07
#define CRC8_POLYNOMIAL 0x07

#define CRC8_INITIAL_VALUE 0x00

// 计算CRC-8值
uint8_t crc8(uint8_t *data, uint8_t length) {
    uint8_t crc = CRC8_INITIAL_VALUE; 
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {  
            if (crc & 0x80) {
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            } else {
                crc <<= 1;  
            }
        }
        // printf("CRC-8 %d: 0x%02X\n", i,crc);  // 打印CRC结果
    }
    return crc; 
}

/******************************************************************
 * @brief   bit_n
 * @param   uint32_t a
 * @param   uint32_t b
 * @return  result
 * @retval  uint8_t
 */
static uint8_t bit_n(uint32_t a, uint32_t b)
{
    return ((a >> b) & 0x01);
}

/******************************************************************
 * @brief   ir_protocol_nec_command_set_tx_buf
 * @param   T_IR_NEC_BUF *p_ir_data_buf
 * @param   T_IR_NEC_SPEC p_spec
 * @return  result
 * @retval  T_IRDA_RET
 */
static T_WMDA_RET protocol_command_set_tx_buf(T_WM_BUF *p_wm_data_buf,
                                                     T_WM_SPEC *p_spec)
{
    uint8_t i = 0, n = 0;
    uint16_t  buf_len = 0;
    uint8_t   Code = 0;

    //Wake-up code
    for (i = 0; i < p_spec->wake_up_code_len; i++)
    {
        Code = p_spec->wake_up_code[i];
        for (n = 0; n < 8; n++)
        {
            /* Change encoding bit order */
            if (bit_n(Code,7-n)== 0x01)
            {
                /* log 1 */
                p_wm_data_buf->p_buf[buf_len] = LEVEL_HIGH;
                p_wm_data_buf->p_buf[buf_len + 1] = LEVEL_LOW;
                buf_len += 2;
            }
            else
            {
                /* log 0 */
                p_wm_data_buf->p_buf[buf_len] = LEVEL_LOW;
                p_wm_data_buf->p_buf[buf_len + 1] = LEVEL_HIGH;
                buf_len += 2;
            }

        }
    }

    /* header */
    for (i = 0; i < p_spec->header_len; i++)
    {
        if(i<3)
            p_wm_data_buf->p_buf[buf_len + i] =  LEVEL_HIGH;
        else
            p_wm_data_buf->p_buf[buf_len + i] =  LEVEL_LOW;

    }
    buf_len += p_spec->header_len;

    //start bit log 1
    p_wm_data_buf->p_buf[buf_len] = LEVEL_HIGH;
    p_wm_data_buf->p_buf[buf_len + 1] = LEVEL_LOW;
    buf_len += 2;
    
    //code
    for (i = 0; i < p_wm_data_buf->code_len; i++)
    {
        Code = p_wm_data_buf->code[i];
        for (n = 0; n < 8; n++)
        {
            /* Change encoding bit order */
            if (bit_n(Code,7-n)== 0x01)
            {
                /* log 1 */
                p_wm_data_buf->p_buf[buf_len] = LEVEL_HIGH;
                p_wm_data_buf->p_buf[buf_len + 1] = LEVEL_LOW;
                buf_len += 2;
            }
            else
            {
                /* log 0 */
                p_wm_data_buf->p_buf[buf_len] = LEVEL_LOW;
                p_wm_data_buf->p_buf[buf_len + 1] = LEVEL_HIGH;
                buf_len += 2;
            }

        }
    }

    //stop
    for (i = 0; i < p_spec->stop_len; i++)
    {
        p_wm_data_buf->p_buf[buf_len + i] =  LEVEL_LOW;
    }
    buf_len += p_spec->stop_len;

    p_wm_data_buf->buf_len = buf_len;

    return IRDA_SUCCEED;
}

/*============================================================================*
 *                              Global Functions
 *============================================================================*/

/******************************************************************
 * @brief   encode ir command data with NEC protocol
 * @param   uint8_t ir_key_command
 * @param   T_IR_SEND_PARA *p_ir_send_parameters
 * @return  result
 * @retval  T_IRDA_RET
 */
T_WMDA_RET protocol_command_encode(uint8_t *data, uint8_t len,
                                          T_WM_SEND_PARA *p_send_parameters)
{
    T_WMDA_RET ret = IRDA_ERROR;

    /* set p_ir_send_buf and send_buf_len */
    T_WM_BUF data_buf;

    data_buf.code_len = len;
    //memcpy(data_buf.code, data, len);


    data_buf.p_buf = p_send_parameters->wm_send_buf;

    ret = protocol_command_set_tx_buf(&data_buf, (T_WM_SPEC *)&SPEC); 
    p_send_parameters->send_buf_len = data_buf.buf_len;

    return ret;
}

#endif



/******************* (C) COPYRIGHT 2024 Seneasy *****END OF FILE****/
