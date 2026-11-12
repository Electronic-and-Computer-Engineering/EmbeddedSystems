/*
 * halUSCIB0.h
 *
 *  Created on: 23.09.2026
 *      Author: mayerflo
 */

#ifndef HAL_HALUSCIB0_H_
#define HAL_HALUSCIB0_H_

/*  I2C-Master auf UCB0 (P2.6 SDA, P2.7 SCL, per Port-Mapping)
    400 kHz Fast Mode, Takt SMCLK 2,5 MHz, Übertragung läuft im Interrupt.
    Die übergebenen Arrays müssen gültig bleiben, bis halI2CBusy() 0 liefert.   */

void halUSCIB0Init(unsigned long baudrate);

unsigned char halI2CWrite(unsigned char addr, const unsigned char *data, unsigned char len);
unsigned char halI2CWriteRead(unsigned char addr, const unsigned char *wData, unsigned char wLen,
                              unsigned char *rData, unsigned char rLen);

unsigned char halI2CBusy(void);     // 1 = Übertragung läuft
unsigned char halI2CError(void);    // 1 = letzte Übertragung mit NACK abgebrochen

#endif /* HAL_HALUSCIB0_H_ */
