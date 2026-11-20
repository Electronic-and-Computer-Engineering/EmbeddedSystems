/*
 * halUSCIA1.h
 *
 *  Created on: 22.09.2026
 *      Author: mayerflo
 */

#ifndef HAL_HALUSCIA1_H_
#define HAL_HALUSCIA1_H_

/*  AUX-UART auf den Steckerpins P8.2 / P8.3
    9600 Baud, 8 Datenbits, keine Parität, 1 Stoppbit (8N1)
    Takt: SMCLK 2,5 MHz
    Feste Pinfunktion, kein Port-Mapping nötig.
    Kein virtueller COM-Port - die Gegenstelle ist das Analog Discovery
    oder später ein Bluetooth-Modul.                           */

#define UART_RX_MAX  32         // Gröﬂe des Empfangsarrays

// ##### Funktionen ######

void halUSCIA1Init(unsigned int baudrate);       // konfiguriert die USCI, Empfang bleibt aus

// ##### Empfangen #####
void halUSCIA1Start(const unsigned char *sync, unsigned char syncLen, unsigned char frameLen);
void halUSCIA1Stop(void);
unsigned char halUSCIA1RxComplete(void);                // 1 = Frame vollständig
unsigned char halUSCIA1GetFrame(unsigned char *dest);   // kopiert den Frame, gibt ihn frei

// ##### Senden #####
unsigned char halUSCIA1Send(const unsigned char *data, unsigned char length);   // 0 = Sender beschäftigt
unsigned char halUSCIA1TxBusy(void);

#endif /* HAL_HALUSCIA1_H_ */
