/**
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/**
 * @file  vl53l1_platform.h
 * @brief Plattform-Schicht der VL53L1X-ULD-API, implementiert in vl53l1_platform.c
 *        auf Basis von halUSCIB0 (I2C) und delayMsBlocked().
 *
 *  dev     = 7-Bit-I2C-Adresse des Sensors (Default 0x29)
 *  index   = 16-Bit-Registeradresse
 *  Rueckgabe: 0 = OK, -1 = Fehler (NACK oder Laenge zu gross)
 *
 *  Alle Funktionen blockieren, bis die I2C-Uebertragung abgeschlossen ist.
 *  Voraussetzung: halI2CInit() aufgerufen und Interrupts freigegeben (halInit()).
 */

#ifndef _VL53L1_PLATFORM_H_
#define _VL53L1_PLATFORM_H_

#include <extLib/VL53L1X/vl53l1_types.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define VL53L1_PLATFORM_OK          0
#define VL53L1_PLATFORM_ERROR     (-1)

#define VL53L1_MAX_WRITE_LEN        16      // max. Datenbytes pro Schreibzugriff (ohne Registeradresse)
#define VL53L1_MAX_READ_LEN         255     // Grenze durch unsigned char Laenge in halUSCIB0

int8_t VL53L1_WriteMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count);
int8_t VL53L1_ReadMulti (uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count);

int8_t VL53L1_WrByte (uint16_t dev, uint16_t index, uint8_t  data);
int8_t VL53L1_WrWord (uint16_t dev, uint16_t index, uint16_t data);
int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data);

int8_t VL53L1_RdByte (uint16_t dev, uint16_t index, uint8_t  *pdata);
int8_t VL53L1_RdWord (uint16_t dev, uint16_t index, uint16_t *pdata);
int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *pdata);

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms);

#ifdef __cplusplus
}
#endif

#endif
