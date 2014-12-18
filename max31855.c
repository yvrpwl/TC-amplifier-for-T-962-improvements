/*****************************************************************************
 *  max31855.c
 *
 *  Created on: 15Dec, 2014
 *  Author: Ported to LPC2134 for use with T962/A Reflow Oven by PWTL
 *
 ********************** Original Comments ************************************
 * This is a library for the Adafruit Thermocouple Sensor w/MAX31855K
 *
 * Designed specifically to work with the Adafruit Thermocouple Sensor
 * ----> https://www.adafruit.com/products/269
 *
 * These displays use SPI to communicate, 3 pins are required to interface
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 * BSD license, all text above must be included in any redistribution
 *****************************************************************************/

#include "LPC214x.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "t962.h"
#include "max31855.h"
#include "sc18is602.h"


//#define __DEBUG__

double readInternal(uint8_t deviceNum)
{
  uint32_t value;
  float internal;
  uint8_t fault;

  value = readDevice(deviceNum);
  fault = value & 0x07;
  if(fault) {
	  return -999.99f;
  }
  // ignore bottom 4 bits - they're just Thermocouple data
  value >>= 4;

  // pull the bottom 11 bits off
  internal = value & 0x7FF;
  // check sign bit!
  if (value & 0x800) {
    // Convert to negative value by extending sign and casting to signed type.
    int16_t tmp = 0xF800 | (value & 0x7FF);
    internal = tmp;
  }
  internal *= 0.0625; // LSB = 0.0625 degrees
  //printf("\tInternal Temp: %03.1fC", internal);

  return internal;
}

double readCelsius(uint8_t deviceNum)
{
  int32_t value;
  double centigrade;
  uint8_t fault;

  value = readDevice(deviceNum);

  fault = value & 0x07;
  if(fault) {
	  return -999.99f;
  }

  if (value & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    value = 0xFFFFC000 | ((value >> 18) & 0x00003FFFF);
  } else {
    // Positive value, just drop the lower 18 bits.
    value >>= 18;
  }

  centigrade = value;
  // LSB = 0.25 degrees C
  centigrade *= 0.25;

  return centigrade;
}

double readFarenheit(uint8_t deviceNum)
{
  double f = readCelsius(deviceNum);

  if(f == -999.99f){
	  return f;
  }
  f *= 9.0;
  f /= 5.0;
  f += 32;

  return f;
}

uint8_t readError(uint8_t deviceNum)
{
	uint32_t value;

	value = readDevice(deviceNum);

	return (value & 0x07);
}

void MAX31885_Init(void)
{
	uint8_t retval;
	uint16_t i;

	printf("\r\n%s called\r\n",__FUNCTION__);
	for(i = 0; i < 2; i++)
	{
		retval = readError(i);		// read device 1 status
		if(!retval){
			printf("\r\nDevice %d :- No Error, code = 0x%02X\r\n", i, retval);
		}else {
			printf("\r\nDevice %d :- Error found, code = 0x%02X\r\n", i, retval);
		}
		if(retval && OCFAULT) {
			printf("\r\n%s : Device %d - TC open circuit fault!\r\n",__FUNCTION__, i);
		} else if (retval && SCGFAULT) {
			printf("\r\n%s : Device %d - TC short-circuit to ground fault!\r\n", __FUNCTION__, i);
		} else if (retval && SCVFAULT) {
			printf("\r\n%s : Device %d - TC short-circuit to Vcc fault!\r\n", __FUNCTION__, i);
		} else {
			printf("Device %d - Cold Junction : %3.2fC\r\n", i, readInternal(i));
			printf("Device %d - Hot  Junction : %3.2fC\r\n", i, readCelsius(i));
#ifdef __DEBUG__
			printf("Device %d done....", i);
#endif
		}
	}
}
uint32_t readDevice(uint8_t deviceNum)
{
  uint32_t value, i;
  uint8_t chipSelect = 1, counts;
  uint8_t tempbuff[MAXDATABYTES];	// buffer to hold dummy bytes

  if(deviceNum > 1)  {
 	  deviceNum = 1;	// only allow device 0 or 1
  }
  chipSelect = (1 << deviceNum);
#ifdef __DEDBUG__
  printf("Prime buffer, prepare to receive data.....\r\n");
  for(i = 0; i < 4; i++) 	{ tempbuff[i] = 0xff; 	}
#endif
  // send dummy data out to the MAX31855, the device is read only but SC18IS602 will read in data from it's MISO pin
  //WriteBytes(chipSelect, tempbuff, MAXDATABYTES);	// dummy write out the data to retrieve 4 bytes from the device via MISO pin
  //counts = ReadnBytes(tempbuff, MAXDATABYTES);		// now read in the buffered data from the SC18IS602
  WriteNBytes(SC18IS602_BASE_ADDRESS, chipSelect, tempbuff, MAXDATABYTES);	// dummy write data to SPI bus in order to retrieve data bytes from the device via MISO pin
  counts = ReadNBytes(SC18IS602_BASE_ADDRESS, tempbuff, MAXDATABYTES);	// now read in the buffered data from the SC18IS602

  value = 0;

  for(i = 0; i < MAXDATABYTES; i++)
  {
	  value = value << 8;		// make room for next incoming byte
	  value |= tempbuff[i];		// assemble the 32bit data
  }

#ifdef __DEBUG__
  printf("\r\n%s called\r\n",__FUNCTION__);
  printf("Device returns...\r\n\r\n");
  for(i = 0; i < 4; i++)	{ printf("0x%02X | ", tempbuff[i]);	}
  printf("Value read : 0x%04X\r\n", value);
#endif

  return value;
}
