/*
 * motor_driver.h
 *
 *  Created on: Apr 21, 2023
 *      Author: aj755
 */

#ifndef MOTORCONTROL_MOTOR_DRIVER_H_
#define MOTORCONTROL_MOTOR_DRIVER_H_

/**
 * Prepare the bike to use the motor setup all the pins according to the control scheme
 */
void initializeMotor(void);

/**
 * disables all pwm and puts all pins in a low state
 */
void stopMotors(void);



#endif /* MOTORCONTROL_MOTOR_DRIVER_H_ */
