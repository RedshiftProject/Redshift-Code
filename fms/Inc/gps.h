#ifndef __GPS_H
#define __GPS_H
// Struct defines
typedef struct
{
	uint8_t class;
	uint8_t id;
	uint16_t len;		   //Length of the payload. Does not include cls, id, or checksum bytes
	uint16_t counter;	  //Keeps track of number of overall bytes received. Some responses are larger than 255 bytes.
	//uint16_t startingSpot; //The counter value needed to go past before we begin recording into payload array
	uint8_t payload[512];
	uint8_t checksumA; //Given to us from module. Checked against the rolling calculated A/B checksums.
	uint8_t checksumB;
} ubxPacket;

typedef struct
{
	uint8_t talker[2];
	uint8_t sen_form[3];
	uint16_t len;		//Length of the payload (number of elements, not necessarily uint8_t). Does not include cls, id, or checksum bytes
	uint8_t data_field[74];	//Only the data field (starting with ',' / ending before '*'). NMEA data field is limited to 74 characters
	uint16_t counter;	//Length of the whole NMEA sentence
	uint8_t checksum;
} nmeaPacket;

// Function prototypes
uint8_t UBX_parser(UART_HandleTypeDef* huart,uint8_t* rx_string, ubxPacket* msg);
void UBX_checksum(ubxPacket* msg);
uint8_t UBX_isChecksum(ubxPacket msg);
uint8_t NMEA_parser(uint8_t* rx_string, nmeaPacket* msg);
void NMEA_checksum(nmeaPacket* msg);
uint8_t NMEA_isChecksum(nmeaPacket msg);
#endif