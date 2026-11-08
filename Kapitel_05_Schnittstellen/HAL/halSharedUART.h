/*
 * halSharedUART.h
 *
 *  Created on: 23.09.2026
 *      Author: mayerflo
 */

#ifndef HAL_HALSHAREDUART_H_
#define HAL_HALSHAREDUART_H_

#define UART_RX_MAX     32         // Größe des Empfangsarrays

// ###### Shared UART Vars //
volatile unsigned char uartRxArray[UART_RX_MAX];
volatile unsigned char uartRxComplete = 0;
unsigned char uartRxPos = 0;
const unsigned char *uartSync = 0;
unsigned char uartSyncLen  = 0;
unsigned char uartFrameLen = 0;
volatile const unsigned char *uartTxPtr = 0;
volatile unsigned char uartTxLen = 0;

#endif /* HAL_HALSHAREDUART_H_ */
