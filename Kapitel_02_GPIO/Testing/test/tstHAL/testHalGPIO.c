/*
 * testHalGPIO.c
 *
 *  Created on: 02.10.2026
 *      Author: mayerflo
 */

#include <msp430.h>
#include "unity.h"
#include "testHalGPIO.h"
#include "halGPIO.h"

extern volatile ButtonCom CCButton;

static void triggerButton(unsigned char button)
{
    unsigned int timeout = 60000;

    CCButton.active = 0;
    CCButton.button = 0;

    P1IFG |= button;                            // trigger interrupt by software

    while (CCButton.active == 0 && timeout > 0) timeout--;
}

static void startButtonSetsEventFlag(void)
{
    triggerButton(START_BUTTON);
    TEST_ASSERT_EQUAL_UINT8(1, CCButton.active);
}

static void startButtonReportsCorrectKey(void)
{
    triggerButton(START_BUTTON);
    TEST_ASSERT_EQUAL_UINT8(START_BUTTON, CCButton.button);
}

static void stopButtonSetsEventFlag(void)
{
    triggerButton(STOP_BUTTON);
    TEST_ASSERT_EQUAL_UINT8(1, CCButton.active);
}

static void stopButtonReportsCorrectKey(void)
{
    triggerButton(STOP_BUTTON);
    TEST_ASSERT_EQUAL_UINT8(STOP_BUTTON, CCButton.button);
}

static void interruptFlagIsCleared(void)
{
    triggerButton(START_BUTTON);
    TEST_ASSERT_BITS_LOW(START_BUTTON, P1IFG);
}

void testHalGPIORun(void)
{
    RUN_TEST(startButtonSetsEventFlag);
    RUN_TEST(startButtonReportsCorrectKey);
    RUN_TEST(stopButtonSetsEventFlag);
    RUN_TEST(stopButtonReportsCorrectKey);
    RUN_TEST(interruptFlagIsCleared);
}
