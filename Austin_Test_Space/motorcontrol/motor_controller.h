/*
 * motor_controller.h
 *
 *  Created on: Apr 2, 2023
 *      Author: aj755
 */

#ifndef MOTORCONTROL_MOTOR_CONTROLLER_H_
#define MOTORCONTROL_MOTOR_CONTROLLER_H_


#define poles 48
#define pole_pairs 24
#define motor_inductance .000430f
#define motor_resistance 0.250f
#define flux_linkage .024f
#define motor_kemf (flux_linkage*pole_pairs)
#define fsw 10000.0f

#define PWMLIMIT .95f

//How many PI cycles can run before tthe wheel is considered not spining
//at 5000 cycles cutoff if 1 electrical rotation is not completed every 250ms then the wheel is considered to not be spinning
#define HALLCUTOFF 4000
extern float iq_prev;
extern float id_prev;
extern float kp;
extern float ki;
extern float LGain;
extern float phi_m;
extern float SampleTime;
extern float e_angle;//electrical angle
extern float e_speed;
extern float idref;
extern float  iqref;

extern float angle_offset;
extern double err_id[2];
extern double err_iq[2];
extern double yi_id[2];
extern double yi_iq[2];
extern double vd;
extern double vq;
extern double ki_times_Tsby2;

void initMotor(void);
void disableMotor(void);

#endif /* MOTORCONTROL_MOTOR_CONTROLLER_H_ */
