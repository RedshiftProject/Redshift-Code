#ifndef __GPS_H
#define __GPS_H
// Struct defines
typedef struct
{
	uint8_t class;
	uint8_t id;
	uint16_t len;		   //Length of the payload. Does not include cls, id, or checksum bytes
	uint16_t counter;	  //Keeps track of number of overall bytes received. Some responses are larger than 255 bytes.
	uint16_t startingSpot; //The counter value needed to go past before we begin recording into payload array
	uint8_t *payload;
	uint8_t checksumA; //Given to us from module. Checked against the rolling calculated A/B checksums.
	uint8_t checksumB;
} ubxPacket;

// Function prototypes

uint16_t ubx_checksum(uint8_t* frame, int size);
uint8_t UBX_isChecksum(ubxPacket msg);
uint8_t NMEA_checksum(uint8_t* frame, int size);
#endif