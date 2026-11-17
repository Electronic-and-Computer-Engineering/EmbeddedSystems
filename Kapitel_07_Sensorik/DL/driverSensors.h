/*
 * driverSensors.h
 *
 * Driver Layer der analogen Sensorik: rechnet ADC-Rohwerte in physikalische
 * Groessen um. Hier endet die Welt der Zahlen ohne Einheit.
 */

#ifndef DL_DRIVERSENSORS_H_
#define DL_DRIVERSENSORS_H_

/*
 * Spannungsteiler der Batteriemessung.
 * Lesen Sie die beiden Widerstaende im Schaltplan ab und tragen Sie das
 * Verhaeltnis hier ein:  VBAT = U_ADC * (R1 + R2) / R2
 */
#define VBAT_DIVIDER_NUM    1       // TODO: Zaehler   (R1 + R2)
#define VBAT_DIVIDER_DEN    1       // TODO: Nenner    (R2)

#define VBAT_WARN_MV        6600    // darunter: Akku leer, Fahrbetrieb einstellen

// ##### Funktionen #####
unsigned int driverSensorsGetBatteryMv(void);           // Batteriespannung in Millivolt
unsigned char driverSensorsBatteryLow(void);            // 1 = unter VBAT_WARN_MV

#endif /* DL_DRIVERSENSORS_H_ */
