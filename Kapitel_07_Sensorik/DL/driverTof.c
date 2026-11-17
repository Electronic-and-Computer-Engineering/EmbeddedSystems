/*
 * driverTof.c
 *
 *  Created on: 26.09.2026
 *      Author: mayerflo
 */

#include "driverTof.h"
#include "VL53L1X_api.h"
#include "halGPIO.h"
#include "halUCS.h"

unsigned char tofStatus = TOF_ALL;      // alle Bits gesetzt = noch nicht initialisiert

void driverTofInit(void)
{
    driverTofSetAllAddresses();     // 1. jedem Sensor eine eigene I2C-Adresse geben
    driverTofConfigAll();           // 2. Default-Konfiguration laden
    driverTofStartAll();            // 3. Messeinstellungen setzen und Messung starten
}

void driverTofSetAllAddresses(void)
{
    tofStatus = 0;

    // alle Sensoren in Reset -> alle haben danach wieder Adresse 0x29
    halSetToFXshut(TOF_XSHUT_LEFT,  false);
    halSetToFXshut(TOF_XSHUT_MID,   false);
    halSetToFXshut(TOF_XSHUT_RIGHT, false);
    delayMsBlocked(2);

    // einzeln freigeben, dann antwortet immer nur einer auf 0x29
    // bei Fehler: Bit setzen und Sensor zurueck in Reset, sonst stoert er den naechsten auf 0x29
    halSetToFXshut(TOF_XSHUT_LEFT, true);
    delayMsBlocked(2);                          // Sensor erst booten lassen, vorher nicht ansprechen
    if (driverTofWaitBoot(TOF_ADDR_DEFAULT) != 0 ||
        VL53L1X_SetI2CAddress(TOF_ADDR_DEFAULT, TOF_ADDR_LEFT << 1) != 0)   // API erwartet 8-Bit-Adresse
    {
        tofStatus |= TOF_LEFT;
        halSetToFXshut(TOF_XSHUT_LEFT, false);
    }

    halSetToFXshut(TOF_XSHUT_MID, true);
    delayMsBlocked(2);                          // Sensor erst booten lassen, vorher nicht ansprechen
    if (driverTofWaitBoot(TOF_ADDR_DEFAULT) != 0 ||
        VL53L1X_SetI2CAddress(TOF_ADDR_DEFAULT, TOF_ADDR_MID << 1) != 0)
    {
        tofStatus |= TOF_MID;
        halSetToFXshut(TOF_XSHUT_MID, false);
    }

    halSetToFXshut(TOF_XSHUT_RIGHT, true);
    delayMsBlocked(2);                          // Sensor erst booten lassen, vorher nicht ansprechen
    if (driverTofWaitBoot(TOF_ADDR_DEFAULT) != 0 ||
        VL53L1X_SetI2CAddress(TOF_ADDR_DEFAULT, TOF_ADDR_RIGHT << 1) != 0)
    {
        tofStatus |= TOF_RIGHT;
        halSetToFXshut(TOF_XSHUT_RIGHT, false);
    }
}

void driverTofConfigAll(void)
{
    // nur Sensoren konfigurieren, die bei der Adressvergabe kein Fehler-Bit bekommen haben
    // SensorInit laedt die Default-Konfiguration und kalibriert einmal (dauert ca. 50 ms pro Sensor)
    if (!(tofStatus & TOF_LEFT))
    {
        if (VL53L1X_SensorInit(TOF_ADDR_LEFT) != 0) tofStatus |= TOF_LEFT;
    }

    if (!(tofStatus & TOF_MID))
    {
        if (VL53L1X_SensorInit(TOF_ADDR_MID) != 0) tofStatus |= TOF_MID;
    }

    if (!(tofStatus & TOF_RIGHT))
    {
        if (VL53L1X_SensorInit(TOF_ADDR_RIGHT) != 0) tofStatus |= TOF_RIGHT;
    }
}

void driverTofStartAll(void)
{
    // nur Sensoren starten, die bisher fehlerfrei sind
    if (!(tofStatus & TOF_LEFT))
    {
        if (driverTofStartSensor(TOF_ADDR_LEFT, TOF_SIDE_DIST_MODE, TOF_SIDE_TIMING_MS, TOF_SIDE_ROI_X, TOF_SIDE_ROI_Y) != 0) tofStatus |= TOF_LEFT;
    }

    if (!(tofStatus & TOF_MID))
    {
        if (driverTofStartSensor(TOF_ADDR_MID, TOF_MID_DIST_MODE, TOF_MID_TIMING_MS, TOF_MID_ROI_X, TOF_MID_ROI_Y) != 0) tofStatus |= TOF_MID;
    }

    if (!(tofStatus & TOF_RIGHT))
    {
        if (driverTofStartSensor(TOF_ADDR_RIGHT, TOF_SIDE_DIST_MODE, TOF_SIDE_TIMING_MS, TOF_SIDE_ROI_X, TOF_SIDE_ROI_Y) != 0) tofStatus |= TOF_RIGHT;
    }
}

unsigned char driverTofStartSensor(unsigned char addr, unsigned char distMode, unsigned int timingMs,
                                   unsigned char roiX, unsigned char roiY)
{
    unsigned char status = 0;

    status |= VL53L1X_SetDistanceMode(addr, distMode);          // zuerst Modus, dann Timing Budget
    status |= VL53L1X_SetTimingBudgetInMs(addr, timingMs);
    status |= VL53L1X_SetInterMeasurementInMs(addr, timingMs);  // = Timing Budget -> Messungen ohne Pause
    status |= VL53L1X_SetROI(addr, roiX, roiY);                 // Sichtfeld
    status |= VL53L1X_StartRanging(addr);                       // ab jetzt misst der Sensor laufend

    return status;      // 0 = OK
}

unsigned char driverTofGetStatus(unsigned char sensor)
{
    if (tofStatus & sensor) return 1;   // Fehler
    return 0;                           // OK
}

unsigned char driverTofGetDistance(unsigned char addr, unsigned int *distance)
{
    unsigned char ready = 0;
    unsigned char rangeStatus = 0;
    uint16_t dist = 0;

    if (VL53L1X_CheckForDataReady(addr, &ready) != 0 || !ready) return 1;   // noch keine neue Messung

    VL53L1X_GetRangeStatus(addr, &rangeStatus);     // 0 = gueltige Messung
    VL53L1X_GetDistance(addr, &dist);               // in mm
    VL53L1X_ClearInterrupt(addr);                   // quittieren, sonst misst der Sensor nicht weiter

    if (rangeStatus != 0) return 2;                 // z.B. kein Hindernis in Reichweite -> Wert unbrauchbar

    *distance = dist;
    return 0;
}

unsigned char driverTofWaitBoot(unsigned char addr)
{
    unsigned char booted = 0;
    unsigned int ms;

    for (ms = 0; ms < TOF_BOOT_TIMEOUT_MS; ms++)
    {
        // beim Booten antwortet der Sensor noch mit NACK -> Fehler ignorieren, weiter fragen
        if (VL53L1X_BootState(addr, &booted) == 0 && booted) return 0;
        delayMsBlocked(1);
    }

    return 1;           // Timeout
}

unsigned char driverTofGetSensorId(unsigned char addr, unsigned int *id)
{
    uint16_t sensorId = 0;
    unsigned char status;

    status = VL53L1X_GetSensorId(addr, &sensorId);
    *id = sensorId;

    return status;      // 0 = OK
}
