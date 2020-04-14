/**
 ******************************************************************************
 * @file           : gps.c
 * @brief          : GPS library body
 ******************************************************************************
 * This is a library for the UBLOX M8Q-0 GPS
 * 
 */

/* Includes */
#include "stm32f1xx_hal.h"
#include "gps.h"
#include "i2c.h"
#include "usart.h"

/**
 * 
 * @brief  Calculate checksum from UBX packet
 * @param  frame UBX Frame
 * @retval Nothing (void)
 * 
 */
void UBX_checksum(ubxPacket *msg)
{

    msg->checksumA = 0;
    msg->checksumB = 0;

    msg->checksumA += msg->class;
    msg->checksumB += msg->checksumA;

    msg->checksumA += msg->id;
    msg->checksumB += msg->checksumA;

    msg->checksumA += (msg->len & 0xFF);
    msg->checksumB += msg->checksumA;

    msg->checksumA += (msg->len >> 8);
    msg->checksumB += msg->checksumA;

    for (uint16_t i = 0; i < msg->len; i++)
    {
        msg->checksumA += msg->payload[i];
        msg->checksumB += msg->checksumA;
    }
}

/**
 * 
 * @brief  Check if calculated checksum of message corresponds to provided message checksum
 * @param  msg UBX Frame
 * @retval 0 (false) if checksum is wrong, 1 (true) if checksum corresponds
 * 
 * Using a copy of msg and not pointing to adress so that checksum 
 * computation doesn't modify the "original" ubx packet. (Takes up more room but too lazy to modify 
 * compute checksum function)
 * 
 */
uint8_t UBX_isChecksum(ubxPacket msg)
{
    uint8_t checksumA = msg.checksumA;
	uint8_t checksumB = msg.checksumB;
    UBX_checksum(&msg);
    if(checksumA == msg.checksumA && checksumB == msg.checksumB)
        return 1;
    else
        return 0;
}

/**
 * TODO:
 * @brief  Calculate checksum from NMEA frame (without the header '$' and footer <CR><LF>)
 * @param  frame UBX Frame (without the header '$' and footer <CR><LF>)
 * @param  size Length of UBX frame (without the header '$' and footer <CR><LF>)
 * @retval uint8_t Result of checksum 
 * 
 */
uint8_t NMEA_checksum(uint8_t* frame, int size)
{
    return 0;
}