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
#include "can.h"
#include <string.h>

///UBX Stuff

/**
 * 
 * @brief  Parses a string of uint8_t(char) into a ubxPacket
 * @param  msg String of uint8_t to parse
 * @retval If parsing successful = 0
 * 
 */
uint8_t UBX_parser(UART_HandleTypeDef* huart,uint8_t* rx_string, ubxPacket* msg)
{
    HAL_UART_Receive(huart,&rx_string[1],5,HAL_MAX_DELAY);
    //Verify that it's a UBX packet
    if(rx_string[1] == 0x62)
    {
        msg->class = rx_string[2];
        msg->id = rx_string[3];
        msg->len = rx_string[5]<<8 | rx_string[4]; //Little endian
        HAL_UART_Receive(&huart2,&rx_string[6],msg->len + 2,HAL_MAX_DELAY);
        for(int i=0;i<msg->len;i++)
        {
            msg->payload[i] = rx_string[i+5+1];
        }
        msg->checksumA = rx_string[5+msg->len-1];
        msg->checksumB = rx_string[5+msg->len];
        if(UBX_isChecksum(*msg))
            return 0;
    }
    return 1;
}

/**
 * 
 * @brief  Calculate checksum from UBX packet
 * @param  frame UBX Frame
 * @retval Nothing (void)
 * 
 */
void UBX_checksum(ubxPacket* msg)
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

///NMEA Stuff (Should be enough to checksum and not anything else because
// no need compute the GPS data on board)

/**
 * 
 * @brief  Parses a string of uint8_t(char) into a nmeaPacket
 * @param  msg String of uint8_t to parse
 * @retval If parsing successful = 0
 * 
 */
uint8_t NMEA_parser(uint8_t* rx_string, nmeaPacket* msg)
{
    //msg->counter = 1;
    //Parse Talker ID and sentence form
    for(int i=0;i<2;i++)
    {
        msg->talker[i] = rx_string[i+1];
    }
    //msg->counter +=2;

    for(int i=0;i<3;i++)
    {
        msg->sen_form[i] = rx_string[i+3];
    }
    //msg->counter +=3;
    msg->len=0;
    if(strcmp(msg->sen_form,"GGA"))
    {
        /* for(int i = 0; i<msg->counter-4;i++,msg->len++)
        {
            msg->data_field[i]=rx_string[i+5];
        } */

        /*
        //Find starting index of checksum
        uint8_t p = strcspn(rx_string,"*");
        //Copy payload from after $GPXXX to before checksum (includes the first ',' 
        //but excludes the '*' for checksum)
        strncpy(msg->payload,rx_string+6, p-6-1);
        msg->len = (p-6-1);
        //msg->counter += msg->len;

        msg->checksum = rx_string[p+1];
        //msg->counter += 4; //Includes '*'+Checksum + <CR><LF>
        */
       msg->checksum=rx_string[msg->counter-3];

    if(NMEA_isChecksum(*msg))
        return 0;
    }
    return 1;
}

/**
 * TODO:
 * @brief  Calculate checksum from NMEA frame (without the header '$' and footer <CR><LF>)
 * @param  frame Full NMEA frame
 * @param  size Length of NMEA frame (without the header '$' and footer <CR><LF>)
 * @retval uint8_t Result of checksum 
 * 
 */
void NMEA_checksum(nmeaPacket* msg)
{
    msg->checksum = 0;
    for(int i=0;i<2;i++)
    {
        msg->checksum ^= msg->talker[i];
    }
        
    for(int i=0;i<3;i++)
    {
        msg->checksum ^= msg->sen_form[i];
    }

    for(int i = 0; i<msg->len ; i++)
    {
        /*for(int p = 0;p<sizeof(msg->payload[i])/sizeof(msg->payload[i][0]);p++)
        {*/
            msg->checksum ^= msg->data_field[i]/*[p]*/;
        /*}*/
    }
}

/**
 * 
 * @brief  Check if calculated checksum of message corresponds to provided message checksum
 * @param  msg NMEA packet
 * @retval 0 (false) if checksum is wrong, 1 (true) if checksum corresponds
 * 
 * Using a copy of msg and not pointing to adress so that checksum 
 * computation doesn't modify the "original" NMEA packet. (Takes up more room but too lazy to modify 
 * compute checksum function)
 * 
 */
uint8_t NMEA_isChecksum(nmeaPacket msg)
{
    uint8_t checksum = msg.checksum;
    NMEA_checksum(&msg);
    if(checksum == msg.checksum)
        return 1;
    else
        return 0;
}