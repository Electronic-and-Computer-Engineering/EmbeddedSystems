/*
 * testHalCleanUp.c
 *
 *  Created on: 02.10.2026
 *      Author: mayerflo
 */

#include "testHalCleanUp.h"
#include "halGPIO.h"

extern volatile ButtonCom CCButton;

void testHalCleanupAll(void)
{
    // halGPIO
    CCButton.active = 0;
    CCButton.button = 0;
    // weitere Module kommen hier dazu
}


