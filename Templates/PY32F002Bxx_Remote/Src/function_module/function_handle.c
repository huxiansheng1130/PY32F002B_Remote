/*********************************************************************************************************
 * @file      function_handle.c
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
#include "../function_module/function_handle.h"
#include "led_driver.h"

/*============================================================================*
 *                              Global Variables
 *============================================================================*/
Send_packet_t packet_dat;

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief  Sends a packet containing data to be checked and set.
 * @param  data: Array of data to be sent, which may include various types of information.
 * @retval None
 */
void send_data_check_set(uint8_t data[])
{
    data[4] = (data[0] ^ data[1] ^ data[2] ^ data[3]);
    data[4] += 0x11;
}
/**
 * @brief  Sends a packet containing key press and temperature data.
 * @param  key_1: First key press value.
 * @param  key_2: Second key press value.
 * @param  temp: Array of temperature data.
 * @retval None
 */
void send_key_ntc_packet(uint8_t key_1, uint8_t key_2)
{
#if (LED_FUNCTION_ENABLE)
    led_open();
#endif
    packet_dat.type = NTC_TYPE;
    packet_dat.data[0] = key_1;
    packet_dat.data[1] = key_2;
    packet_dat.data[2] = dev_st.device_temp[0];
    packet_dat.data[3] = dev_st.device_temp[1];
    send_data_check_set(packet_dat.data);
#if (UI_RF_ENABLE)
    re_send_enable(1);
#endif

#if 0
   printf("--->>> Device id : 0x%x, 0x%x, 0x%x\r\n",
          packet_dat.device_id[0],
          packet_dat.device_id[1],
          packet_dat.device_id[2]);
   printf("--->>> ntc_packet.type : %d\n", packet_dat.type);
   printf("--->>> ntc_packet.key.dat_1 : %d\n", packet_dat.data[0]);
   printf("--->>> ntc_packet.key.dat_2 : %d\n", packet_dat.data[1]);
   printf("--->>> ntc_packet.temp: %d.%d\n", packet_dat.data[2], packet_dat.data[3]);
   printf("--->>> ntc_packet.check :%d\r\n", packet_dat.dat_check);
#endif
}

void send_sensor_packet(void)
{
    // TODO: implement sensor data packet sending
}
