/*
 * vl53l1_platform.c
 *
 * Plattform-Schicht fuer die VL53L1X-ULD-API von ST.
 * Setzt die Register-Zugriffe der API auf die I2C-Funktionen aus halUSCIB0 um.
 *
 * Der VL53L1X hat 16-Bit-Registeradressen und ist Big-Endian:
 *   Schreiben: [Reg High][Reg Low][Daten MSB ... LSB]
 *   Lesen:     [Reg High][Reg Low] -> Repeated Start -> [Daten MSB ... LSB]
 * Der MSP430 ist Little-Endian, deshalb wird per Shift zerlegt/zusammengesetzt.
 */

#include <msp430.h>
#include <extLib/VL53L1X/vl53l1_platform.h>
#include "halUSCIB0.h"
#include "halUCS.h"

// ##### interne Hilfsfunktionen #####

static int8_t i2cWaitDone(void)
{
    while (halI2CBusy());                               // Uebertragung laeuft im Interrupt
    return halI2CError() ? VL53L1_PLATFORM_ERROR : VL53L1_PLATFORM_OK;
}

static int8_t i2cWriteReg(uint16_t dev, uint16_t index, const uint8_t *data, uint8_t count)
{
    uint8_t buf[2 + VL53L1_MAX_WRITE_LEN];              // Registeradresse + Daten in einem Puffer
    uint8_t i;

    if (count > VL53L1_MAX_WRITE_LEN) return VL53L1_PLATFORM_ERROR;

    buf[0] = (uint8_t)(index >> 8);                     // Registeradresse High
    buf[1] = (uint8_t)(index & 0xFF);                   // Registeradresse Low
    for (i = 0; i < count; i++)
    {
        buf[2 + i] = data[i];
    }

    while (halI2CBusy());                               // falls noch eine Uebertragung laeuft
    if (!halI2CWrite((unsigned char)dev, buf, 2 + count)) return VL53L1_PLATFORM_ERROR;

    return i2cWaitDone();                               // buf bleibt gueltig, bis fertig
}

static int8_t i2cReadReg(uint16_t dev, uint16_t index, uint8_t *data, uint8_t count)
{
    uint8_t reg[2];

    reg[0] = (uint8_t)(index >> 8);
    reg[1] = (uint8_t)(index & 0xFF);

    while (halI2CBusy());
    if (!halI2CWriteRead((unsigned char)dev, reg, 2, data, count)) return VL53L1_PLATFORM_ERROR;

    return i2cWaitDone();
}

// ##### Mehrere Bytes #####

int8_t VL53L1_WriteMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    if (count == 0 || count > VL53L1_MAX_WRITE_LEN) return VL53L1_PLATFORM_ERROR;
    return i2cWriteReg(dev, index, pdata, (uint8_t)count);
}

int8_t VL53L1_ReadMulti(uint16_t dev, uint16_t index, uint8_t *pdata, uint32_t count)
{
    if (count == 0 || count > VL53L1_MAX_READ_LEN) return VL53L1_PLATFORM_ERROR;
    return i2cReadReg(dev, index, pdata, (uint8_t)count);
}

// ##### Schreiben #####

int8_t VL53L1_WrByte(uint16_t dev, uint16_t index, uint8_t data)
{
    return i2cWriteReg(dev, index, &data, 1);
}

int8_t VL53L1_WrWord(uint16_t dev, uint16_t index, uint16_t data)
{
    uint8_t buf[2];

    buf[0] = (uint8_t)(data >> 8);                      // MSB zuerst
    buf[1] = (uint8_t)(data & 0xFF);

    return i2cWriteReg(dev, index, buf, 2);
}

int8_t VL53L1_WrDWord(uint16_t dev, uint16_t index, uint32_t data)
{
    uint8_t buf[4];

    buf[0] = (uint8_t)(data >> 24);                     // MSB zuerst
    buf[1] = (uint8_t)(data >> 16);
    buf[2] = (uint8_t)(data >> 8);
    buf[3] = (uint8_t)(data & 0xFF);

    return i2cWriteReg(dev, index, buf, 4);
}

// ##### Lesen #####

int8_t VL53L1_RdByte(uint16_t dev, uint16_t index, uint8_t *pdata)
{
    return i2cReadReg(dev, index, pdata, 1);
}

int8_t VL53L1_RdWord(uint16_t dev, uint16_t index, uint16_t *pdata)
{
    uint8_t buf[2];
    int8_t status;

    status = i2cReadReg(dev, index, buf, 2);
    *pdata = ((uint16_t)buf[0] << 8) | buf[1];          // Big-Endian -> uint16_t

    return status;
}

int8_t VL53L1_RdDWord(uint16_t dev, uint16_t index, uint32_t *pdata)
{
    uint8_t buf[4];
    int8_t status;

    status = i2cReadReg(dev, index, buf, 4);
    *pdata = ((uint32_t)buf[0] << 24) |                 // Cast noetig: int ist beim MSP430 nur 16 Bit
             ((uint32_t)buf[1] << 16) |
             ((uint32_t)buf[2] << 8)  |
              (uint32_t)buf[3];

    return status;
}

// ##### Warten #####

int8_t VL53L1_WaitMs(uint16_t dev, int32_t wait_ms)
{
    (void)dev;                                          // wird hier nicht gebraucht

    if (wait_ms <= 0) return VL53L1_PLATFORM_OK;
    if (wait_ms > 0xFFFF) wait_ms = 0xFFFF;             // delayMsBlocked nimmt unsigned int

    delayMsBlocked((unsigned int)wait_ms);
    return VL53L1_PLATFORM_OK;
}
