#ifndef DL_DRIVER_AKTORIK_H_
#define DL_DRIVER_AKTORIK_H_

#define ST_MIDDLE 000       // TBD

#define ST_maxLeft 000      // TBD entspr. 1,1ms
#define ST_maxRight 000     //  TBD entspr. 1,9ms

#define MaxRPW 2500         //1000 muS
#define MinRPW 5000         //2000 muS
#define MinFPW 7500         //3000 muS
#define MaxFPW 10000        //4000 muS

#define MaxBreak 6250

// Functions
void driverSetSteering(signed char steerVal);
void driverSetBreak(signed char motorBreak);
void driverSetThrottle(signed char throttleVal);

void driverESCinit(void);
void createPulses(int pwm, int pulseDuration);

#endif /* DL_DRIVER_AKTORIK_H_ */
