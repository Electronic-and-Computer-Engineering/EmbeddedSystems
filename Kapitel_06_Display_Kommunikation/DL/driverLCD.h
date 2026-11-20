/*
 * driver_LCD.h
 *
 *  Created on: 07.11.2017
 *      Author: mayerflo
 */

#ifndef DL_DRIVER_LCD_H_
#define DL_DRIVER_LCD_H_

#define LCD_RESET               0xE2
#define LCD_BIAS                0xA3
#define ADC_SEL_NORMAL          0xA0
#define COMMON_REVERSE          0xC8
#define RES_RATIO               0x24
#define ELEC_VOL_MODE           0x81
#define ELEC_VOL_VALUE          0x0F
#define POWER_CONT              0x2F
#define DISPLAY_ON              0xAF

#define LCD_CMD_ALL_POINTS_ON   0xA5
#define LCD_CMD_ALL_POINTS_OFF  0xA4

#define LCD_MAX_COLM 128
#define LCD_MAX_ROW 7
#define MAX_DIGIT 8
#define DEZIMAL 10

#define SET_PAGE 0xB0 //0xB0 f�r erste Page , 0xB1 f�r zweite Page
#define LAST_PAGE 0xB7

#define MSB_COL_ADDR 0x10 //0x10, 0x11 ,..,0x1F f�r spalten �ber 16
#define LSB_COL_ADDR 0x00 //0x01, 0x02 ,..,0x0F f�r spalten bis 15

// ##### Functions #####
void driverLCDSend(const unsigned char *data, unsigned char dataLength, unsigned char isData);

void driverLCDInit(void);
void driverLCDClear(void);
void driverLCDWriteCommand(const unsigned char *data, unsigned char dataLength);
void driverLCDWriteData(const unsigned char *data, unsigned char dataLength);
void driverLCDSetPosition(unsigned char page, unsigned char col);
void driverLCDWriteText(const char *text, unsigned char textLength, unsigned char page, unsigned char col);
void driverLCDWriteUInt(unsigned int number, unsigned char page, unsigned char col);

#endif /* DL_DRIVER_LCD_H_ */
