/*********************************************************************************************************
 * @file      flash_handle.c
 *
 * @details
 *
 * @author    huzhuohuan
 * @date      2024-03-10
 * @version   V_1.0
 ********************************************************************************************************/

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "flash_handle.h"

#if (FLASH_ID_READ_ENABLE)
/*============================================================================*
 *                              Global Variables
 *============================================================================*/

/*============================================================================*
 *                              Function Definitions
 *============================================================================*/
/**
 * @brief  Reads data from the flash memory at a specified address.
 * @param  address: The memory address from which to read the data.
 * @retval Returns the data read from the flash memory.
 */
uint32_t flash_read_data(uint32_t address)
{
    return HW32_REG(address);
}

/**
 * @brief  Reads the device's pair ID from persistent storage.
 * @details This function retrieves the unique identifier used for pairing the device.
 * @param None
 * @retval None
 */
void device_pair_id_read()
{
    uint32_t ret_data = 0;
    ret_data = flash_read_data(DEVICE_PAIR_ID_ADDRESS);
    packet_dat.device_id[0] = ret_data;
    packet_dat.device_id[1] = ret_data >> 8;
    packet_dat.device_id[2] = ret_data >> 16;
}

#endif