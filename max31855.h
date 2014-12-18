/*
 * max31855.h
 *
 *  Created on: 15Dec, 2014
 *  Author: Ported to LPC2134 for use with T962/A Reflow oven by PWTL
 *
 ********************Original comments*******************************
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
 ************************************************************************/

#ifndef MAX31855_H_
#define MAX31855_H_


#define MAXDATABYTES 4
#define OCFAULT  1
#define SCGFAULT 2
#define SCVFAULT 4

void MAX31885_Init(void);
double readInternal(uint8_t deviceNum);
double readCelsius(uint8_t deviceNum);
double readFarenheit(uint8_t deviceNum);
uint8_t readError(uint8_t deviceNum);
uint32_t readDevice(uint8_t deviceNum);

#endif /* MAX31855_H_ */
