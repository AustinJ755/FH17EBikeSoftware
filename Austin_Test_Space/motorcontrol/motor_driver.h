/*
 * motor_driver.h
 *
 *  Created on: Apr 21, 2023
 *      Author: aj755
 */

#ifndef MOTORCONTROL_MOTOR_DRIVER_H_
#define MOTORCONTROL_MOTOR_DRIVER_H_
#include "stdint.h"

//#########################################################
//DEFINE STATEMENTS
//#########################################################

//motor driver configuration
//#define USE_SINUSOIDAL_CONTROL
#define USE_TRAPEZOIDAL_CONTROL
#define TRAPEZOID_MOTOR
//ENSURE VALID CONFIG OF MOTOR DRIVER
#if  !defined(USE_SINUSOIDAL_CONTROL) && !defined(USE_TRAPEZOIDAL_CONTROL)
#error "MUST SELECT A MOTOR CONTROL SCHEME"
#endif
#if  defined(USE_SINUSOIDAL_CONTROL) && defined(USE_TRAPEZOIDAL_CONTROL)
#error "MUST ONLY SELECT ONE CONTROL SCHEME"
#endif

//BIKE MOTOR CHARACTERISTICS

#define POLE_PAIRS 24
#define WHEEL_RADIUS 13.0f //radius in inches
#define MOTOR_INDUCTANCE 0.000430f //Motor winding inductance measured
#define MOTOR_RESISTANCE 0.250f    //Motor winding resistance measured

#define FLUX_LINKAGE .024f         //This value was used from previous class and is likely incorrect
#define MOTOR_KEMF (flux_linkage*pole_pairs)

//Control Scheme Config
#define SWITCHING_FREQUENCY 10000.0f
#define PWMPERIOD 5000.0f //pwm period in cycles
#define PWMPERIOD_2 2500 //half of the pwm period in cycles

//Other stuff
#define CHARGE_TIME 2000 //The amount of time to charge the gates for the motor driver in microseconds
#define ZERO_CUTOFF_TIME 1.0f //sets the speed to zero if no hall effects are triggered in 1 second
#define HALLCUTOFF 4000 //sets the number of iterations that can run without a halleffect signal being detected before the wheel is determined to be stationary
//#########################################################
//GLOBAL VARIABLES
//#########################################################

//This represents everytime 2^32 cycles has executed at 100MHZ
extern uint32_t system_time_large;

//The last timer time that a hall sensor was triggered
extern uint32_t last_hall_time;

//The last large time that a hall was triggered
extern uint32_t last_hall_time_large;


//used by FOC to determine if wheel is spinning
extern uint32_t hall_trigger;
extern uint32_t ecap_ready;

extern uint16_t rotation; //indicates measured direction of wheel rotation
//#########################################################
//COMMON FUNCTIONS
//#########################################################

/**
 * Prepare the bike to use the motor setup all the pins according to the control scheme
 */
void initializeMotor(void);

/**
 * disables all pwm and puts all pins in a low state
 */
void stopPWM(void);


float getMotorSpeed(void);

float getElectricalSpeed(void);

float getDiffTime(void);
float getDiffTimeV(uint32_t small, uint32_t large);
//#########################################################
//EXTERNAL FUNCTIONS
//#########################################################

//The following functions will be defined in sinusoidal_drive.c
void sinusoidalMotorInit(void);
void sinusoidalDriver(void);


//the following functions will be define in trapezoidal_motor_control.c
void trapezoidalMotorInit(void);
void trapezoidalDriver(void);



#endif /* MOTORCONTROL_MOTOR_DRIVER_H_ */
