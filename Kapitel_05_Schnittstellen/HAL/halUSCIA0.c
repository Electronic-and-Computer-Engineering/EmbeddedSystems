/*
 * halUSCIA0.c
 *
 *  Created on: 22.09.2026
 *      Author: mayerflo
 */

#include <msp430.h>
#include "halUSCIA0.h"
#include "halUCS.h"
#include "halGeneral.h"

// ##### Empfangen #####
extern volatile unsigned char uartRxArray[UART_RX_MAX];
extern volatile unsigned char uartRxComplete;
extern unsigned char uartRxPos;
extern const unsigned char *uartSync;
extern unsigned char uartSyncLen;
extern unsigned char uartFrameLen;
extern volatile const unsigned char *uartTxPtr;
extern volatile unsigned char uartTxLen;

void halUSCIA0Init(unsigned int baudrate)
{
    unsigned int  brScaler      = SMCLK_FREQU / baudrate;                     // 2 500 000 / 57 600 = 43
    unsigned char brDecPlaces   = ((SMCLK_FREQU % baudrate) * 8) / baudrate;  // Rest 23 200 -> 3 Achtel

    UCA0CTL1 |= UCSWRST;                // USCI im Reset halten, solange konfiguriert wird
    UCA0CTL1 |= UCSSEL__SMCLK;          // Taktquelle: SMCLK (2,5 MHz)

    // Frame: 8 Datenbits, keine Parität, 1 Stoppbit, LSB first, UART-Mode
    UCA0CTL0 &= ~UCPEN;                 // keine Parität
    UCA0CTL0 &= ~UC7BIT;                // 8 Datenbits
    UCA0CTL0 &= ~UCSPB;                 // 1 Stoppbit
    UCA0CTL0 &= ~UCSYNC;                // asynchron (UART)
    UCA0CTL0 &= ~UCMSB;                 // LSB first

    UCA0BR0  = brScaler & 0xFF;         // Teiler, Low-Byte
    UCA0BR1  = brScaler >> 8;           // Teiler, High-Byte
    UCA0MCTL = brDecPlaces << 1;        // Nachkommaanteil in Achteln -> UCBRS

    UCA0CTL1 &= ~UCSWRST;               // USCI bereit, Interrupts noch aus
}

void halUSCIA0Start(const unsigned char *sync, unsigned char syncLen, unsigned char frameLen)
{
    if (frameLen > UART_RX_MAX) frameLen = UART_RX_MAX;

    uartSync     = sync;            // Array muss dauerhaft gültig bleiben!
    uartSyncLen  = syncLen;
    uartFrameLen = frameLen;

    uartRxPos = 0;
    uartRxComplete = 0;
    UCA0IFG &= ~UCRXIFG;            // altes Zeichen verwerfen
    UCA0IE  |= UCRXIE;              // Empfang an
}

void halUSCIA0Stop(void)
{
    UCA0IE &= ~UCRXIE;              // Empfang aus
}

unsigned char halUSCIA0RxComplete(void)
{
    return uartRxComplete;
}

unsigned char halUSCIA0GetFrame(unsigned char *dest)
{
    unsigned char i;

    if (uartRxComplete == 0) return 0;

    for (i = 0; i < uartFrameLen; i++) dest[i] = uartRxArray[i];

    uartRxComplete = 0;             // Rahmen freigeben, Empfang läuft weiter
    return uartFrameLen;
}

unsigned char halUSCIA0Send(const unsigned char *data, unsigned char length)
{
    if (uartTxLen > 0) return 0;    // Sender beschäftigt, nichts übernommen
    if (length == 0)   return 0;

    uartTxPtr = data;               // Array muss bis zum Ende der Übertragung gültig bleiben!
    uartTxLen = length;
    UCA0IFG |= UCTXIFG;             // Anforderung setzen
    UCA0IE |= UCTXIE;               // Sende-Interrupt anstoßen
    return 1;                       // übernommen
}

unsigned char halUSCIA0TxBusy(void)
{
    return (uartTxLen > 0);
}

#pragma vector = USCI_A0_VECTOR
__interrupt void usciA0Isr(void)
{
    unsigned char rxByte;

    switch (UCA0IV)                     // Lesen löscht das behandelte Flag
    {
        case USCI_UCRXIFG:
            rxByte = UCA0RXBUF;         // Lesen löscht UCRXIFG

            if ((uartRxPos < uartSyncLen) && (rxByte != uartSync[uartRxPos]))
            {
                if (rxByte == uartSync[0])          // Byte könnte selbst der Rahmenanfang sein
                {
                    uartRxArray[0] = rxByte;
                    uartRxPos = 1;
                }
                else
                {
                    uartRxPos = 0;
                }
            }
            else
            {
                uartRxArray[uartRxPos++] = rxByte;

                if (uartRxPos == uartFrameLen)
                {
                    uartRxComplete = 1;
                    uartRxPos = 0;
                }
            }
            break;

        case USCI_UCTXIFG:
            if (uartTxLen > 0)
            {
                UCA0TXBUF = *uartTxPtr++;       // Schreiben löscht UCTXIFG
                uartTxLen--;
            }
            else
            {
                UCA0IE &= ~UCTXIE;              // fertig, Sende-Interrupt wieder aus
            }
            break;

        default:
            break;
    }
}
