/*
 * halUSCIA1.c
 *
 *  Created on: 22.09.2026
 *      Author: mayerflo
 */

#include <msp430.h>
#include "halUSCIA1.h"
#include "halUCS.h"
#include "halGeneral.h"

// ##### Empfangen ##### var share via hal general
extern volatile unsigned char uartRxArray[UART_RX_MAX];
extern volatile unsigned char uartRxComplete;
extern unsigned char uartRxPos;
extern const unsigned char *uartSync;
extern unsigned char uartSyncLen;
extern unsigned char uartFrameLen;
extern volatile const unsigned char *uartTxPtr;
extern volatile unsigned char uartTxLen;

void halUSCIA1Init(unsigned int baudrate)
{
    unsigned int  brScaler      = SMCLK_FREQU / baudrate;                     // 2 500 000 / 57 600 = 43
    unsigned char brDecPlaces   = ((SMCLK_FREQU % baudrate) * 8) / baudrate;  // Rest 23 200 -> 3 Achtel

    UCA1CTL1 |= UCSWRST;                // USCI im Reset halten, solange konfiguriert wird
    UCA1CTL1 |= UCSSEL__SMCLK;          // Taktquelle: SMCLK (2,5 MHz)

    // Frame: 8 Datenbits, keine Parität, 1 Stoppbit, LSB first, UART-Mode
    UCA1CTL0 &= ~UCPEN;                 // keine Parität
    UCA1CTL0 &= ~UC7BIT;                // 8 Datenbits
    UCA1CTL0 &= ~UCSPB;                 // 1 Stoppbit
    UCA1CTL0 &= ~UCSYNC;                // asynchron (UART)
    UCA1CTL0 &= ~UCMSB;                 // LSB first

    UCA1BR0  = brScaler & 0xFF;         // Teiler, Low-Byte
    UCA1BR1  = brScaler >> 8;           // Teiler, High-Byte
    UCA1MCTL = brDecPlaces << 1;        // Nachkommaanteil in Achteln -> UCBRS

    UCA1CTL1 &= ~UCSWRST;               // USCI bereit, Interrupts noch aus
}

void halUSCIA1Start(const unsigned char *sync, unsigned char syncLen, unsigned char frameLen)
{
    if (frameLen > UART_RX_MAX) frameLen = UART_RX_MAX;

    uartSync     = sync;            // Array muss dauerhaft gültig bleiben!
    uartSyncLen  = syncLen;
    uartFrameLen = frameLen;

    uartRxPos = 0;
    uartRxComplete = 0;
    UCA1IFG &= ~UCRXIFG;            // altes Zeichen verwerfen
    UCA1IE  |= UCRXIE;              // Empfang an
}

void halUSCIA1Stop(void)
{
    UCA1IE &= ~UCRXIE;              // Empfang aus
}

unsigned char halUSCIA1RxComplete(void)
{
    return uartRxComplete;
}

unsigned char halUSCIA1GetFrame(unsigned char *dest)
{
    unsigned char i;

    if (uartRxComplete == 0) return 0;

    for (i = 0; i < uartFrameLen; i++) dest[i] = uartRxArray[i];

    uartRxComplete = 0;             // Rahmen freigeben, Empfang läuft weiter
    return uartFrameLen;
}

unsigned char halUSCIA1Send(const unsigned char *data, unsigned char length)
{
    if (uartTxLen > 0) return 0;    // Sender beschäftigt, nichts übernommen
    if (length == 0)   return 0;

    uartTxPtr = data;               // Array muss bis zum Ende der Übertragung gültig bleiben!
    uartTxLen = length;
    UCA1IFG |= UCTXIFG;
    UCA1IE  |= UCTXIE;               // Sende-Interrupt anstoßen
    return 1;                       // übernommen
}

unsigned char halUSCIA1TxBusy(void)
{
    return (uartTxLen > 0);
}

#pragma vector = USCI_A1_VECTOR
__interrupt void usciA1Isr(void)
{
    unsigned char rxByte;

    switch (UCA1IV)                     // Lesen löscht das behandelte Flag
    {
        case USCI_UCRXIFG:
            rxByte = UCA1RXBUF;         // Lesen löscht UCRXIFG

            if((uartRxPos < uartSyncLen) && (rxByte != uartSync[uartRxPos]))
            {
                uartRxPos = 0;          // passt nicht: wieder auf den Rahmenanfang warten
                break;
            }
            uartRxArray[uartRxPos++] = rxByte;

            if(uartRxPos == uartFrameLen)      // Rahmen vollständig
            {
                uartRxComplete = 1;
                uartRxPos = 0;
            }
            break;

        case USCI_UCTXIFG:
            if (uartTxLen > 0)
            {
                UCA1TXBUF = *uartTxPtr++;       // Schreiben löscht UCTXIFG
                uartTxLen--;
            }
            else
            {
                UCA1IE &= ~UCTXIE;              // fertig, Sende-Interrupt wieder aus
            }
            break;

        default:
            break;
    }
}
