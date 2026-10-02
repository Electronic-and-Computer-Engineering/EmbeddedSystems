/*
 * testGeneral.c
 *
 *  Created on: 02.10.2026
 *      Author: mayerflo
 */

#include <testHalCleanUp.h>
#include "unity.h"
#include "testHalGeneral.h"
#include "testHalGPIO.h"

void setUp(void)    { }

void tearDown(void)
{
    testHalCleanupAll();
}


void halInitTest(void)
{
    UNITY_BEGIN();

    testHalGPIORun();

    UNITY_END();
}

