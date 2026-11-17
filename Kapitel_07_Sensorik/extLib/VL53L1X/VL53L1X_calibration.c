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
 * @file  vl53l1x_calibration.c
 * @brief Calibration functions implementation
 *
 * Angepasst fuer MSP430 (FH JOANNEUM):
 *  - CalibrateXtalk: float durch Ganzzahlarithmetik ersetzt
 *  - CalibrateOffset: AverageDistance auf int32_t (Ueberlauf bei 50 Messwerten)
 */
#include <extLib/VL53L1X/VL53L1X_api.h>
#include <extLib/VL53L1X/VL53L1X_calibration.h>

#define ALGO__PART_TO_PART_RANGE_OFFSET_MM  0x001E
#define MM_CONFIG__INNER_OFFSET_MM          0x0020
#define MM_CONFIG__OUTER_OFFSET_MM          0x0022


int8_t VL53L1X_CalibrateOffset(uint16_t dev, uint16_t TargetDistInMm, int16_t *offset)
{
    uint8_t i, tmp=0;
    int32_t AverageDistance = 0;        /* war int16_t: 50 Messwerte laufen sonst ueber */
    uint16_t distance;
    VL53L1X_ERROR status = 0;
    uint16_t timeout_counter =0;

    status |= VL53L1_WrWord(dev, ALGO__PART_TO_PART_RANGE_OFFSET_MM, 0x0);
    status |= VL53L1_WrWord(dev, MM_CONFIG__INNER_OFFSET_MM, 0x0);
    status |= VL53L1_WrWord(dev, MM_CONFIG__OUTER_OFFSET_MM, 0x0);
    status |= VL53L1X_StartRanging(dev);    /* Enable VL53L1X sensor */
    for (i = 0; i < 50; i++) {
        while (tmp == 0)
        {
            status = VL53L1X_CheckForDataReady(dev, &tmp);
            timeout_counter++;
            if (timeout_counter >= 1000)
            {
                status = (uint8_t)VL53L1X_ERROR_TIMEOUT;
                return status;
            }
            status = VL53L1_WaitMs(dev, 1);
        }
        tmp = 0;
        timeout_counter=0;

        status |= VL53L1X_GetDistance(dev, &distance);
        status |= VL53L1X_ClearInterrupt(dev);
        AverageDistance = AverageDistance + distance;
    }
    status |= VL53L1X_StopRanging(dev);
    AverageDistance = AverageDistance / 50;
    *offset = (int16_t)((int32_t)TargetDistInMm - AverageDistance);
    status |= VL53L1_WrWord(dev, ALGO__PART_TO_PART_RANGE_OFFSET_MM, *offset*4);
    return status;
}


int8_t VL53L1X_CalibrateXtalk(uint16_t dev, uint16_t TargetDistInMm, uint16_t *xtalk)
{
    uint8_t i, tmp=0;
    uint32_t SumSignalRate = 0;         /* waren float */
    uint32_t SumDistance = 0;
    uint32_t SumSpadNb = 0;
    uint32_t AverageSignalRate, AverageDistance, AverageSpadNb;
    uint16_t distance = 0, spadNum;
    uint16_t sr;
    VL53L1X_ERROR status = 0;
    int32_t calXtalk;
    uint16_t timeout_counter =0;

    status |= VL53L1_WrWord(dev, 0x0016,0);
    status |= VL53L1X_StartRanging(dev);
    for (i = 0; i < 50; i++) {
        while (tmp == 0)
        {
            status = VL53L1X_CheckForDataReady(dev, &tmp);
            timeout_counter++;
            if (timeout_counter >= 1000)
            {
                status = (uint8_t)VL53L1X_ERROR_TIMEOUT;
                return status;
            }
            status = VL53L1_WaitMs(dev, 1);
        }
        tmp = 0;
        timeout_counter=0;

        status |= VL53L1X_GetSignalRate(dev, &sr);
        status |= VL53L1X_GetDistance(dev, &distance);
        status |= VL53L1X_ClearInterrupt(dev);
        SumDistance = SumDistance + distance;
        status = VL53L1X_GetSpadNb(dev, &spadNum);
        SumSpadNb = SumSpadNb + spadNum;
        SumSignalRate = SumSignalRate + sr;
    }
    status |= VL53L1X_StopRanging(dev);
    AverageDistance = SumDistance / 50;
    AverageSpadNb = SumSpadNb / 50;
    AverageSignalRate = SumSignalRate / 50;
    if (AverageSpadNb == 0 || TargetDistInMm == 0)
        return (int8_t)VL53L1X_ERROR_XTALK_FAILED;
    /* Calculate Xtalk value: 512 * SR * (Target - D) / (Target * SpadNb) */
    calXtalk = (int32_t)(((int64_t)512 * (int64_t)AverageSignalRate *
            ((int64_t)TargetDistInMm - (int64_t)AverageDistance)) /
            ((int64_t)TargetDistInMm * (int64_t)AverageSpadNb));
    if (calXtalk < 0)
        calXtalk = 0;
    if (calXtalk > 0xffff)
        calXtalk = 0xffff;
    *xtalk = (uint16_t)(((uint32_t)calXtalk*1000)>>9);
    status |= VL53L1_WrWord(dev, 0x0016, (uint16_t)calXtalk);
    return status;
}
