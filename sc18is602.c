/*
 * sc18is602.c
 *
 * This is the library code for accessing SPI version of the Thermocouple amplifier (MAX31855) using
 * I2C interface via the SC18IS602B I2C to SPI bridge IC.
 * Due to the lack of usable SPI pins on the LPC2134, (SPI interface pins are occupied by other functions)
 * instead from using the MAX31850 one-wire interface version, this is an alternative way to use the SPI
 * version of the thermocouple amplifier(MAX31855)
 *
 *  Version:	V0.1
 *  Created on: 14Dec, 2014
 *  Author: 	Ported to LPC2134 for use with T926/A reflow oven by Patrick Law
 *
 * Some parts of the code are extracted from the paper written by the
 * Original Author :	Benjamin Edward Hastings
 *
 * http://scholar.lib.vt.edu/theses/available/etd-08142006-101925/unrestricted/uWIP.pdf
 *
 * and                  Tiequan Shao          info@sandboxelectronics.com
 *
 * Lisence:			CC BY-NC-SA 3.0
 *
 * Please keep the above information when you use this code in your project.
 *
*/

#include "LPC214x.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "t962.h"
#include "sc18is602.h"
#include "i2c.h"

//#define __DEBUG

#define SETSTARTCON() 	{ I2C0CONSET = I2C_STA | I2C_SI; //setup start condition }
#define SENDSTARTCON()	{ I2C0CONCLR = I2C_SI; //send start condition  }
#define WAITUNTILSENT()	{ while (~I2C0CONSET & I2C_SI); //wait for start to be sent }

#ifndef DEVADDR_WR
#define DEVADDR_WR SC18IS602_BASE_ADDRESS
#endif

#ifndef DEVADDR_RD
#define DEVADDR_RD SC18IS602_BASE_ADDRESS + 1
#endif

#define I2C0CONSET	I20CONSET
#define I2C0CONCLR	I20CONCLR
#define I2C0STAT	I20STAT
#define I2C0DAT		I20DAT

uint8_t reg_f0_config; //configuration register 0xF0
uint8_t reg_f4_gpio_w; //gpio write register
uint8_t reg_f5_gpio_r; //gpio read register
uint8_t reg_f6_gpio_e; //gpio enable register
uint8_t reg_f7_gpio_d; //gpio direction

void I2CtoSPI_init(void)
{
#ifdef __DEBUG
	printf("\n%s called\r\n",__FUNCTION__);
#endif
	ResetDevice();
	I2CBridgeIRQClear();
}
void SendStartCond(void)
{
	I2C0CONSET = I2C_STA | I2C_SI; 		//setup start condition
	I2C0CONCLR = I2C_SI; 				//send start condition
	while (~I2C0CONSET & I2C_SI)	; 	//wait for start to be sent
}
void SendStopCond(void)
{
	I2C0CONSET = I2C_STO; 				//setup stop condition
	I2C0CONCLR = I2C_SI; 				//start sending stop
	while (I2C0CONSET & I2C_STO)	; 	//wait for stop to be sent
}
/**************************************************************************
 * Write chip address and Function ID to device
 *
 *************************************************************************/
void WriteFunctionID(uint8_t DeviceAddr, uint8_t FnID)
{
	do {
		SendStartCond();					// Send start condition
		I2C0DAT = DeviceAddr;				//load slave address
		I2C0CONCLR = I2C_STA | I2C_SI; 		//send slave address
		while (~I2C0CONSET & I2C_SI)	; 	//wait for address to be sent

	}	while (I2C0STAT != 0x18); 			//repeat if no ACK received

	I2C0DAT = FnID; 						//load function ID
	I2C0CONCLR = I2C_SI; 					//send data
	while (~I2C0CONSET & I2C_SI)	; 		//wait
}	//WriteFunctionID

/**************************************************************************
 * Write chip address, Function ID with one byte of data
 *
 *************************************************************************/
void WriteByte(uint8_t ChipAddr, uint8_t FnID, uint8_t Data)
{
	WriteFunctionID(ChipAddr, FnID);
	I2C0DAT = Data; 					//load data
	I2C0CONCLR = I2C_SI; 				//send data
	while (~I2C0CONSET & I2C_SI)	; 	//wait for data to be sent
	SendStopCond();
}	//WriteByte

/**************************************************************************
 * Write a number of bytes to selected device (0-3) connected to SC18IS602
 *
 * device connected to SS0 = 1, SS1 = 2, SS2 = 4, SS3 = 8.
 * only 4 devices allowed.
 * data are stored in buffer and it's passed into the function via the
 * byte pointer
 **************************************************************************/
void WriteNBytes(uint8_t ChipAddr, uint8_t fnID, uint8_t* buffer, uint8_t length)
{
	uint32_t i;

#ifdef __DEBUG
	printf("\r\n%s called\r\n",__FUNCTION__);
#endif

	WriteFunctionID(ChipAddr, fnID);
	for(i = 0; i < length; i++)
	{
		I2C0DAT = *buffer++; 				//load data
		I2C0CONCLR = I2C_SI; 				//send data
		while (~I2C0CONSET & I2C_SI)	; 	//wait for data to be sent
	}
	SendStopCond();
}	// WriteNBytes

void WriteRegister(uint8_t reg_addr, uint8_t val)
{
	WriteByte(DEVADDR_WR, reg_addr, val);
}

void WriteCommand(uint8_t fnID)
{
	WriteFunctionID(DEVADDR_WR, fnID);
	SendStopCond();
}

/*******************************************************************************
 * Read a byte from the device
 ******************************************************************************/
uint8_t ReadByte(uint8_t DeviceAddr)
{
	uint32_t Data;

#ifdef __DEBUG
	printf("\r\n%s called\r\n",__FUNCTION__);
#endif

	do {
		SendStartCond();					// send start condition
		I2C0DAT = DeviceAddr | 0x01;		//Load slave base address read bit set
		I2C0CONCLR = I2C_STA | I2C_SI; 		//Send slave address
		while (~I2C0CONSET & I2C_SI)	; 	//wait for address to be sent
	} while (I2C0STAT != 0x40); 			//repeat if no ack received

	I2C0CONCLR = I2C_SI; 				//start reading data
	while (~I2C0CONSET & I2C_SI)	; 	//wait for data to be received
	Data = I2C0DAT;						//retrieve data
	SendStopCond();						// send stop condition
	return (uint8_t)Data; 						//return the data
}	// ReadByte

/*******************************************************************************
 * Read number of bytes from the device
 ******************************************************************************/
uint8_t ReadNBytes(uint8_t DeviceAddr, uint8_t* buffer, uint8_t length)
{
	uint32_t i;

#ifdef __DEBUG
	printf("\r\n%s called\r\n",__FUNCTION__);
#endif

	do {
		SendStartCond();
		I2C0DAT = DeviceAddr | 0x01;		//Load slave base address with read bit set
		I2C0CONCLR = I2C_STA | I2C_SI; 		//Send slave address
		while (~I2C0CONSET & I2C_SI)	; 	//wait for address to be sent
	} while (I2C0STAT != 0x40); 			//repeat if no ack received

	for(i = 0; i < length; i++)
	{
		I2C0CONSET = I2C_AA;				// send ACK
		I2C0CONCLR = I2C_SI; 				//start reading data
		while (~I2C0CONSET & I2C_SI)	; 	//wait for data to be received
		*buffer++ = I2C0DAT;				//retrieve data

	}

	I20CONSET = I2C_STO; 				// send STOP condition
	I2C0CONCLR = I2C_SI | I2C_STO ;		// clear flags
	while(I20CONSET & I2C_STO); 		// Wait for STO to clear
	return length;
}

// Function ID 0xF0

//LSBFIRST MSBFIRST
void setBitOrder(uint8_t order)
{

	reg_f0_config &= 0xDF;
	if ( order == LSBFIRST) {
		reg_f0_config |= 0x20;
	} else {
		reg_f0_config &= 0xDF;
	}

	WriteRegister(0xF0, reg_f0_config);

	return;
}

// Valid divider value = 0, 1, 2, 3
void setClockDivider(uint8_t divider)
{
	reg_f0_config &= 0xFC;

	reg_f0_config |= divider;

	WriteRegister(0xF0, reg_f0_config);

	return;
}

// Valid mode, (use macro SC18IS602_SPI_MODE0|1|2|3)
void setDataMode(uint8_t mode)
{
	reg_f0_config &= 0xF3;
	switch (mode) {
		case SC18IS602_SPI_MODE0:
			break;
		case SC18IS602_SPI_MODE1:
			reg_f0_config |=0x04;
			break;
		case SC18IS602_SPI_MODE2:
			reg_f0_config |=0x08;
			break;
		case SC18IS602_SPI_MODE3:
			reg_f0_config |=0x0C;
			break;
		default:
			break;
	}

	WriteRegister(0xF0, reg_f0_config);

	return;
}

// Function ID 0xF1. Clear Interrupt
void I2CBridgeIRQClear(void)
{
	uint8_t fn_ID = 0xF1;

	WriteCommand(fn_ID);

	return;
}

// Function ID 0xF2. Enter idle mode
void enterIdleMode(void)
{
	uint8_t fn_ID = 0xF2;

	WriteCommand(fn_ID);

	return;
}

void ResetDevice(void)
{

	reg_f0_config = 0x00; 	//configuration register 0xF0
	reg_f4_gpio_w = 0x00; 	//gpio write register
	reg_f5_gpio_r = 0x0F; 	//gpio read register
	reg_f6_gpio_e = 0x00; 	//gpio enable register
	reg_f7_gpio_d = 0xAA; 	//gpio direction
	WriteRegister(0xf0,reg_f0_config);
	setClockDivider(1);		//set SPI frequency to 461KHz
	setDataMode(SC18IS602_SPI_MODE0);	// using mode 0
}
/****************************************************************
 * Change Slave select pins to GPIO function
 * valid pin value = 0, 1, 2, 3
 ***************************************************************/
void GPIOEnable(uint8_t pin)
{
	reg_f6_gpio_e |= ( 0x01 << pin );
	WriteRegister(0xF6,reg_f6_gpio_e);
}

/***************************************************************
 * Change GPIO pin back to slave select pin function
 * valid pin value = 0, 1, 2, 3
 ***************************************************************/
void SSEnable(uint8_t pin)
{
	reg_f6_gpio_e &= (uint8_t)(~(0x01 << pin) );
	WriteRegister(0xF6,reg_f6_gpio_e);
}
#if 0
uint8_t transferByte(uint8_t value)
{
	uint8_t result = 0;

	WriteBytes(&value, 1);
	ReadBytes(&result, 1);

	return result;
}
uint8_t transfernBytes(uint8_t* buffer, uint8_t length)
{
	WriteBytes(buffer,length);
	return (ReadBytes(buffer,length));
}
#endif
/********************************************************************
 * Set pin mode for the 4 slave select pins
 *
 * Valid value for pin is 0,1,2,3
 * Valid value for i_o is INPUT or OUTPUT
 *******************************************************************/
void pinMode(uint8_t pin, uint8_t i_o)
{
    reg_f7_gpio_d &= (((uint8_t)~(0x03))<<(pin<1)); //clear the two control bit of the pin
	if (i_o == INPUT) {
		reg_f7_gpio_d |= (0x02<<(pin<<1)); //10 for input
	} else {
		reg_f7_gpio_d |= (0x01<<(pin<<1)); //01 for output
	}
	WriteRegister(0xF7, reg_f7_gpio_d);
}

/**********************************************************************
 * When the slave select pin has been set as GPIO with output function
 * use this function to set the pin value to 1 or 0
 *
 * Valid pin value = 0, 1, 2, 3
 * Valid value to output 0,1
 **********************************************************************/
void digitalWrite(uint8_t pin, uint8_t value)
{
	if ( value == 0 ) {
		reg_f4_gpio_w &= (((uint8_t)~(0x01)) << pin);
	} else {
		reg_f4_gpio_w |= (0x01 << pin);
	}

    WriteRegister(0xF4, reg_f4_gpio_w);
}

/**********************************************************************
 * When the slave select pin has been set as GPIO with input function
 * use this function to read the pin
 *
 * Valid pin value = 0, 1, 2, 3
 * Return either as 1 for high or 0 for low
 *********************************************************************/
uint8_t digitalRead(uint8_t pin)
{
	WriteRegister(0xF5, reg_f5_gpio_r);

	reg_f5_gpio_r = ReadByte(DEVADDR_RD);

    if (reg_f5_gpio_r & (0x01 << pin) ){
    	return 1;
    } else {
		return 0;
    }
}
