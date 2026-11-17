/*
 * driverTof.h
 *
 *  Created on: 26.09.2026
 *      Author: mayerflo
 */

#ifndef DL_DRIVERTOF_H_
#define DL_DRIVERTOF_H_

#define TOF_ADDR_DEFAULT        0x29        // 7-Bit-Adresse nach dem Einschalten
#define TOF_ADDR_LEFT           0x30
#define TOF_ADDR_MID            0x31
#define TOF_ADDR_RIGHT          0x32

// Bits im Status, 1 = Sensor fehlerhaft / nicht bereit
#define TOF_LEFT                0x01        // BIT0
#define TOF_MID                 0x02        // BIT1
#define TOF_RIGHT               0x04        // BIT2
#define TOF_ALL                 (TOF_LEFT | TOF_MID | TOF_RIGHT)

// Messeinstellungen pro Sensor
// Modus: 1 = kurz (bis 1,3 m), 2 = lang (bis 4 m)
// Timing Budget: kurz 15, 20, 33, 50, 100, 200, 500 ms / lang 20, 33, 50, 100, 200, 500 ms
// ROI (Sichtfeld): 4..16 Zellen, 16 x 16 = ca. 27 Grad, 4 x 4 = ca. 15 Grad

// Mitte: nach vorne weit schauen, enges Sichtfeld -> kein Boden, keine Aufhaengung
#define TOF_MID_DIST_MODE       2
#define TOF_MID_TIMING_MS       33
#define TOF_MID_ROI_X           4
#define TOF_MID_ROI_Y           4

// Links / rechts: Wand ist nah -> Kurzmodus, dafuer schneller
#define TOF_SIDE_DIST_MODE      1
#define TOF_SIDE_TIMING_MS      20
#define TOF_SIDE_ROI_X          10
#define TOF_SIDE_ROI_Y          10

#define TOF_SENSOR_ID           0xEACC      // Model-ID des VL53L1X
#define TOF_BOOT_TIMEOUT_MS     100

// ##### Functions #####
void driverTofInit(void);
void driverTofSetAllAddresses(void);
void driverTofConfigAll(void);
void driverTofStartAll(void);
unsigned char driverTofStartSensor(unsigned char addr, unsigned char distMode, unsigned int timingMs,
                                   unsigned char roiX, unsigned char roiY);
unsigned char driverTofGetStatus(unsigned char sensor);     // 0 = OK, 1 = Fehler
unsigned char driverTofGetDistance(unsigned char addr, unsigned int *distance);  // 0 = neuer Wert, 1 = noch kein neuer, 2 = ungueltig
unsigned char driverTofWaitBoot(unsigned char addr);
unsigned char driverTofGetSensorId(unsigned char addr, unsigned int *id);

#endif /* DL_DRIVERTOF_H_ */
