/*
 * driverCom.h
 *
 *  Created on: 22.09.2026
 *      Author: mayerflo
 */

#ifndef DL_DRIVERCOM_H_
#define DL_DRIVERCOM_H_

#define COM_USE_BT             0       // 0 = USB (UCA0), 1 = Bluetooth (UCA1)

#if COM_USE_BT
    #define COM_BAUDRATE       9600
    #define halComInit         halUSCIA1Init
    #define halComStart        halUSCIA1Start
    #define halComSend         halUSCIA1Send
    #define halComTxBusy       halUSCIA1TxBusy
    #define halComGetFrame     halUSCIA1GetFrame
#else
    #define COM_BAUDRATE       57600
    #define halComInit         halUSCIA0Init
    #define halComStart        halUSCIA0Start
    #define halComSend         halUSCIA0Send
    #define halComTxBusy       halUSCIA0TxBusy
    #define halComGetFrame     halUSCIA0GetFrame
#endif

// ##### Framekennungen #####
// Im Code und in den Tests gegen diese Namen vergleichen, nie gegen Zeichen.
#define COM_CAR_START_1  'C'
#define COM_CAR_START_2  'S'
#define COM_CAR_END_1    'E'
#define COM_CAR_END_2    'C'

#define COM_HOST_START_1 'H'
#define COM_HOST_START_2 'S'
#define COM_HOST_END_1   'E'
#define COM_HOST_END_2   'H'

// ##### Frame vom Host: 'H','S', cmd, valHigh, valLow, 'E','H' #####
#define COM_RX_LEN      7
#define COM_RX_CMD      2
#define COM_RX_VAL_HIGH 3
#define COM_RX_VAL_LOW  4
#define COM_RX_END_1    5
#define COM_RX_END_2    6

// ##### Frame vom Auto: 'C','S', Werte (je high/low), 'E','C' #####
#define COM_TX_MAX_VAL  10                              // Sensorwerte
#define COM_TX_HEADER   2       // 'C','S'
#define COM_TX_FOOTER   2       // 'E','C'
#define COM_TX_MAX_LEN  (COM_TX_HEADER + 2 * COM_TX_MAX_VAL + COM_TX_FOOTER)
#define COM_TX_LEN(n)   (COM_TX_HEADER + 2 * (n) + COM_TX_FOOTER)   // Laenge bei n Werten

// ##### Testwerte #####
// Fester Satz zum Pruefen der Uebertragung. Diese vier Zahlen muessen am PC
// genau so wieder erscheinen - siehe README. Definiert in driverCom.c.
#define COM_TEST_COUNT  4
extern const unsigned int comTestVals[COM_TEST_COUNT];

// ##### Funktionen #####
void driverComInit(void);
unsigned char driverComSend(const unsigned int *values, unsigned char count);
unsigned char driverComReceive(unsigned char *cmd, unsigned int *value);   // 0 = nichts empfangen

#endif /* DL_DRIVERCOM_H_ */
