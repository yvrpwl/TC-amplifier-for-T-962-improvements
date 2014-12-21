/*
 * sc18is602.h
 *
 *  Version:	V0.1
 *  Created on: 14Dec, 2104
 *  Author: 	Ported to LPC2134 for use with T926/A reflow oven by Patrick Law
 *
 * Description:
 * This the header file for NXP SC18IS602B I2C to SPI bridge module.
 *
 * Some parts of the code are extracted from the paper written by the
 * Original Author :	Benjamin Edward Hastings
 *
 * http://scholar.lib.vt.edu/theses/available/etd-08142006-101925/unrestricted/uWIP.pdf
 *
 * and                  Tiequan Shao          info@sandboxelectronics.com
 *
 *
 * Lisence:			CC BY-NC-SA 3.0
 *
 * Please keep the above information when you use this code in your project.
*/

#ifndef SC18IS602_H_
#define SC18IS602_H_

//Device Address

#define     SC18IS602_BASE_ADDRESS    (0X50)
#define     SC18IS602_ADDRESS_001     (0X52)
#define     SC18IS602_ADDRESS_010     (0X54)
#define     SC18IS602_ADDRESS_011     (0X56)
#define     SC18IS602_ADDRESS_100     (0X58)
#define     SC18IS602_ADDRESS_101     (0X5A)
#define     SC18IS602_ADDRESS_110     (0X5C)
#define     SC18IS602_ADDRESS_111     (0X5E)

//Clock Divider
#define     SC18IS602_CLOCK_1843K     (0X00)
#define     SC18IS602_CLOCK_461K      (0X01)
#define     SC18IS602_CLOCK_115K      (0X02)
#define     SC18IS602_CLOCK_58K       (0X03)

#define     SC18IS602_SPI_MODE0       (0x00)
#define     SC18IS602_SPI_MODE1       (0x01)
#define     SC18IS602_SPI_MODE2       (0x02)
#define     SC18IS602_SPI_MODE3       (0x03)

#define     SC18IS602_SS_0   		 (0X00)
#define     SC18IS602_SS_1  		 (0X01)
#define     SC18IS602_SS_2   		 (0X02)
#define     SC18IS602_SS_3   		 (0X03)

#define BIT(x)	(1 << x)
#define I2C_EN 	BIT(6)
#define I2C_STA BIT(5)
#define I2C_STO BIT(4)
#define I2C_SI 	BIT(3)
#define I2C_AA 	BIT(2)

#ifndef INPUT
#define INPUT 0
#endif
#ifndef OUTPUT
#define OUTPUT 1
#endif

#define LSBFIRST 1
#define DEVADDR 0x50

void I2CtoSPI_init(void);
void ResetDevice(void);
void GPIOEnable(uint8_t pin);
void SSEnable(uint8_t pin);
//uint8_t transferByte(uint8_t value);
//uint8_t transfernBytes(uint8_t* buffer, uint8_t length);
void pinMode(uint8_t pin, uint8_t io);
void digitalWrite(uint8_t pin, uint8_t value);
uint8_t digitalRead(uint8_t pin);
//void WriteBytestoDevice(uint8_t* buffer, uint8_t length, uint8_t devSelect);
void SendStartCond(void);
void SendStopCond(void);
void WriteFunctionID(uint8_t DeviceAddr, uint8_t FnID);
void WriteByte(uint8_t ChipAddr, uint8_t FnID, uint8_t Data);
void WriteNBytes(uint8_t ChipAddr, uint8_t fnID, uint8_t* buffer, uint8_t length);
void WriteRegister(uint8_t reg_addr, uint8_t val);
void WriteCommand(uint8_t fnID);
uint8_t ReadNBytes(uint8_t DeviceAddr, uint8_t* buffer, uint8_t length);
uint8_t ReadByte(uint8_t DeviceAddr);
void I2CBridgeIRQClear(void);
void setBitOrder(uint8_t order);
void setClockDivider(uint8_t divider);
void setDataMode(uint8_t mode);
void enterIdleMode(void);


#endif /* SC18IS602_H_ */
