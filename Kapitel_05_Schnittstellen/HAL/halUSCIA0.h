/*
 * halUSCIA0.h
 *
 *  Created on: 22.09.2026
 *      Author: mayerflo
 */

#ifndef HAL_HALUSCIA0_H_
#define HAL_HALUSCIA0_H_

/*  UART über eZ-FET Backchannel
    57600 Baud, 8 Datenbits, keine Parität, 1 Stoppbit (8N1)
    Takt: SMCLK 2,5 MHz
    PC: COM-Port "MSP Application UART1"                        */

#define UART_RX_MAX  32         // Größe des Empfangsarrays

// ##### Funktionen ######

void halUSCIA0Init(unsigned int baudrate);       // konfiguriert die USCI, Empfang bleibt aus

// ##### Empfangen #####
void halUSCIA0Start(const unsigned char *sync, unsigned char syncLen, unsigned char frameLen);
void halUSCIA0Stop(void);
unsigned char halUSCIA0RxComplete(void);                // 1 = Rahmen vollständig
unsigned char halUSCIA0GetFrame(unsigned char *dest);   // kopiert den Rahmen, gibt ihn frei

// ##### Senden #####
unsigned char halUSCIA0Send(const unsigned char *data, unsigned char length);   // 0 = Sender beschäftigt
unsigned char halUSCIA0TxBusy(void);

#endif /* HAL_HALUSCIA0_H_ */
