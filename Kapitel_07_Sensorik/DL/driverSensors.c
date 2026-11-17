/*
 * driverSensors.c
 *
 * UNVOLLSTAENDIG - an jedem TODO fehlt Code.
 */

#include "driverSensors.h"
#include "halADC12.h"

unsigned int driverSensorsGetBatteryMv(void)
{
    // TODO 1: Rohwert des Kanals ADC_CH_VBAT ueber halADC12GetValue() holen
    //         und in Millivolt umrechnen.
    //
    //         Zwei Schritte:
    //           1. Rohwert -> Spannung am ADC-Pin
    //                  U_ADC = roh * ADC_VREF_MV / ADC_MAX_VALUE
    //           2. Spannung am Pin -> Batteriespannung
    //                  VBAT  = U_ADC * VBAT_DIVIDER_NUM / VBAT_DIVIDER_DEN
    //
    //         Vorsicht beim Rechnen mit unsigned int: 4095 * 2500 passt nicht
    //         in 16 Bit. Rechnen Sie in unsigned long oder teilen Sie frueh
    //         genug - und begruenden Sie im Kommentar, welchen Weg Sie
    //         gewaehlt haben und welchen Fehler er kostet.

    return 0;
}

unsigned char driverSensorsBatteryLow(void)
{
    // TODO 2: 1 zurueckgeben, wenn die Batteriespannung unter VBAT_WARN_MV
    //         liegt, sonst 0. Eine Zeile, aber sie gehoert hierher und nicht
    //         in den Application Layer - dort steht sonst wieder eine Zahl
    //         ohne Einheit.

    return 0;
}
