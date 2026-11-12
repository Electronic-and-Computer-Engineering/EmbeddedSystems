/*
 * halUSCIB0.c
 *
 *  Created on: 23.09.2026
 *      Author: mayerflo
 */

#include <msp430.h>
#include "halUCS.h"
#include "halUSCIB0.h"

const unsigned char *i2cTxPtr = 0;
unsigned char *i2cRxPtr = 0;
volatile unsigned char i2cTxLen = 0;
volatile unsigned char i2cRxLen = 0;
volatile unsigned char i2cBusy  = 0;
volatile unsigned char i2cError = 0;
volatile unsigned char i2cDummy = 0;         // zum Verwerfen unerwarteter Bytes

void halUSCIB0Init(unsigned long baudrate)
{
    unsigned int br = (SMCLK_FREQU + baudrate - 1) / baudrate;  // aufrunden: nie schneller als gewuenscht
                                                                // 400 kHz -> 7 -> 357 kHz (Fast Mode)
    UCB0CTL1 |= UCSWRST;                        // USCI im Reset halten
    UCB0CTL0  = UCMST | UCMODE_3 | UCSYNC;      // Master, I2C, synchron
    UCB0CTL1  = UCSSEL__SMCLK | UCSWRST;        // Taktquelle SMCLK
    UCB0BR0   = br & 0xFF;
    UCB0BR1   = br >> 8;
    UCB0CTL1 &= ~UCSWRST;                       // USCI freigeben

    UCB0IE = UCTXIE | UCRXIE | UCNACKIE;        // Interrupts an
}

unsigned char halI2CWrite(unsigned char addr, const unsigned char *data, unsigned char len)
{
    if (i2cBusy) return 0;                      // Bus beschaeftigt
    if (len == 0) return 0;

    while (UCB0CTL1 & UCTXSTP);                 // vorherige Stoppbedingung erst zu Ende senden lassen

    i2cTxPtr = data;
    i2cTxLen = len;
    i2cRxLen = 0;                               // kein Lesen danach
    i2cBusy  = 1;
    i2cError = 0;

    UCB0I2CSA = addr;                           // 7-Bit-Adresse
    UCB0CTL1 |= UCTR | UCTXSTT;                 // Schreiben, Startbedingung
    return 1;
}

unsigned char halI2CWriteRead(unsigned char addr, const unsigned char *wData, unsigned char wLen,
                              unsigned char *rData, unsigned char rLen)
{
    if (i2cBusy) return 0;
    if (wLen == 0 || rLen == 0) return 0;

    while (UCB0CTL1 & UCTXSTP);                 // vorherige Stoppbedingung erst zu Ende senden lassen

    i2cTxPtr = wData;                           // Registeradresse
    i2cTxLen = wLen;
    i2cRxPtr = rData;
    i2cRxLen = rLen;
    i2cBusy  = 1;
    i2cError = 0;

    UCB0I2CSA = addr;
    UCB0CTL1 |= UCTR | UCTXSTT;                 // erst schreiben, dann Repeated Start
    return 1;
}

unsigned char halI2CBusy(void)  { return i2cBusy;  }
unsigned char halI2CError(void) { return i2cError; }

#pragma vector = USCI_B0_VECTOR
__interrupt void usciB0Isr(void)
{
    switch (UCB0IV)                             // Lesen loescht das behandelte Flag
    {
        case USCI_I2C_UCNACKIFG:                // Slave antwortet nicht
            UCB0CTL1 |= UCTXSTP;
            UCB0IFG  &= ~UCTXIFG;               // anstehendes TXIFG verwerfen, sonst sendet die ISR weiter
            i2cTxLen = 0;                       // Uebertragung komplett abbrechen,
            i2cRxLen = 0;                       //      sonst macht die ISR mit alten Werten weiter
            i2cError = 1;
            i2cBusy  = 0;
            break;

        case USCI_I2C_UCTXIFG:
            if (i2cTxLen > 0)                   // Schreibphase
            {
                UCB0TXBUF = *i2cTxPtr++;        // Schreiben loescht UCTXIFG
                i2cTxLen--;
            }
            else if (i2cRxLen > 0)              // auf Lesen umschalten
            {
                UCB0CTL1 &= ~UCTR;              // Empfangen
                UCB0CTL1 |= UCTXSTT;            // Repeated Start

                if (i2cRxLen == 1)              // Sonderfall: nur ein Byte
                {
                    while ((UCB0CTL1 & UCTXSTT) && !(UCB0IFG & UCNACKIFG));  // bei NACK nicht ewig auf das Startbit warten
                    UCB0CTL1 |= UCTXSTP;        // Stopp direkt nach dem einen Byte
                }
            }
            else                                // reines Schreiben, alles raus
            {
                UCB0CTL1 |= UCTXSTP;            // Stoppbedingung
                UCB0IFG  &= ~UCTXIFG;
                i2cBusy = 0;
            }
            break;

        case USCI_I2C_UCRXIFG:
            if (i2cRxLen == 0)                  // unerwartetes Byte
            {
                i2cDummy = UCB0RXBUF;           // nur lesen (loescht RXIFG), nicht in fremden Speicher schreiben
                break;
            }

            if (i2cRxLen == 2) UCB0CTL1 |= UCTXSTP;     // Stopp vor dem letzten Byte anfordern

            *i2cRxPtr++ = UCB0RXBUF;            // Lesen loescht UCRXIFG
            i2cRxLen--;

            if (i2cRxLen == 0) i2cBusy = 0;
            break;

        default:
            break;
    }
}
