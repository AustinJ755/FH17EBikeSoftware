/*
 * sinusoidal_motor_control.h
 *
 *  Created on: Apr 21, 2023
 *      Author: Austin James
 */

#ifndef MOTORCONTROL_SINUSOIDAL_MOTOR_CONTROL_H_
#define MOTORCONTROL_SINUSOIDAL_MOTOR_CONTROL_H_
#include "stdint.h"
//###############################################
//Field Oriented Control Params
//###############################################
#define PWMLIMIT .95f //limits the maximum and minimum PWM value FOC is allowed to produce


#define EXTRADEBUG 1


//###############################################
//Field Oriented Control Result Access
//"These variables allow for the calculation results of the FOC code to be accessed by the display debug code"
//###############################################

//offset angle value to account for code delay
extern float angle_offset;

//measured current values
extern float ia;
extern float ib;
extern float ic;

//error values
extern float err_id[2];
extern float err_iq[2];

//output calculations
extern float yi_id[2];
extern float yi_iq[2];

//calculated dq voltage
extern float vd;
extern float vq;

//calculated abc voltage
extern float v_a;
extern float v_b;
extern float v_c;

//calculated abc pwm
extern uint32_t foc_pwm_a;
extern uint32_t foc_pwm_b;
extern uint32_t foc_pwm_c;

//Z^-1 value
extern float ki_times_Tsby2;

extern float e_angle;// = 0.0f;    //electrical angle
extern float e_speed;// = 0.0f;
#endif /* MOTORCONTROL_SINUSOIDAL_MOTOR_CONTROL_H_ */
