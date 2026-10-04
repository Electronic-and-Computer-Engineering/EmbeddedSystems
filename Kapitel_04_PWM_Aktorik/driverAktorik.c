
#include <msp430.h>
#include "driverAktorik.h"
#include "haltimerA1.h"

unsigned int setSteerServo, setThrottle;
extern volatile int speedControllerImpulse;

void driverSetSteering(signed char steerVal)
{

}
void driverSetThrottle(signed char throttleVal)
{

}

void driverSetBreak(signed char motorBreak)
{

}

void driverESCinit(void)
{
    createPulses(MaxRPW, 131);
    createPulses(MinRPW, 131);
    createPulses(MinFPW, 131);
    createPulses(MaxFPW, 131);
    createPulses(MaxBreak, 30);
}

void createPulses(int pwm, int pulseDuration)
{
    halTimerA1SetThrottle(pwm);                        // einmal setzen reicht
    halTimerA1ResetPeriodCnt();
    while(halTimerA1GetPeriodCnt() <= pulseDuration);
}
