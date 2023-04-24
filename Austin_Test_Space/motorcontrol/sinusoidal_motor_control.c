/*
 * sinusoidal_motor_control.c
 *
 *  Created on: Apr 21, 2023
 *      Author: Austin James
 */

#include "board.h"
#include "math.h"
#include "motor_driver.h"
#include "sinusoidal_motor_control.h"
#include "datalogger/datalogger.h"

const float offset_120 = M_PI * 2 / 3;
const float offset_240 = -M_PI * 2 / 3;
const float PI_OVER2 = M_PI / 2.0f;
const float PI_OVER3 = M_PI / 3.0f;
const float PI_OVER6 = M_PI / 6.0f;

//###################################################
//CALCULATION DEBUG VIEW
//###################################################
//offset angle value to account for code delay
float angle_offset = 0.0f;

//measured current values
float ia = 0;
float ib = 0;
float ic = 0;

//error values
float err_id[2];
float err_iq[2];

//output calculations
float yi_id[2];
float yi_iq[2];

//calculated dq voltage
float vd;
float vq;

//calculated abc voltage
float v_a;
float v_b;
float v_c;

//calculated abc pwm
uint32_t foc_pwm_a;
uint32_t foc_pwm_b;
uint32_t foc_pwm_c;

//###################################################
//STUFF FOR FOC CALC
//###################################################

//half the value of the battery supply voltage this value could be updated in real time due to our voltage sensor which could allow for more accurate PI control
float supply_voltage2 = 24.0f;

#define tau_i (10 / (2 * M_PI * SWITCHING_FREQUENCY))
//const conversion_factor1
float iq_prev = 0;
float id_prev = 0;
float kp = MOTOR_RESISTANCE / tau_i;
float ki = MOTOR_INDUCTANCE / tau_i;
float LGain = MOTOR_INDUCTANCE;
float phi_m = FLUX_LINKAGE;
float SampleTime = 1.0f / (2.0f * SWITCHING_FREQUENCY); //we are sampling at 20kHZ since both PWM zero and period trigger ADC
float e_angle = 0.0f;    //electrical angle
float e_speed = 0.0f;
float idref = 2.0f;
float iqref = 0.0f;

//Z^-1 value
float ki_times_Tsby2;

static int failure = 0; //indicates a failure in reading the hall sensor

int setup = 0; //indicates if the FOC pwm is initialized

//static uint16_t HallLookup[8] = { 0, // bad reading //0
//        2, // 1 Sector //2
//        4, // 2 //
//        3, // 3 //
//        6, // 4 //1//
//        1, // 5 //
//        5, // 6
//        0  // bad reading //7
//        };
static uint16_t HallLookup[8] = { 0, // bad reading //0
        5, // 1 Sector //2
        3, // 2 //
        4, // 3 //
        1, // 4 //1//
        6, // 5 //
        2, // 6
        0  // bad reading //7
};
#if EXTRADEBUG
uint16_t last_sector = 0;
#endif

void sinusoidalMotorInit(void) {
    EALLOW;
    //reset all pwm pins to generic io pulled down
    EPWM_setTimeBaseCounterMode(PHASE_A_EPWM_BASE,
                                EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBaseCounterMode(PHASE_B_EPWM_BASE,
                                EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBaseCounterMode(PHASE_C_EPWM_BASE,
                                EPWM_COUNTER_MODE_STOP_FREEZE);

    stopPWM();

    //reset all calculation values
    err_id[0] = 0;
    err_iq[0] = 0;

    yi_id[0] = 0;
    yi_iq[0] = 0;
    ki_times_Tsby2 = ki*SampleTime/2.0;
#if EXTRADEBUG
    last_sector = 0;
#endif
    GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(CHARGE_TIME);
    GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);

    GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(CHARGE_TIME);
    GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);

    GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(CHARGE_TIME);
    GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);

    //
    // EPWM1 -> PHASE_A_EPWM Pinmux
    //
    GPIO_setPinConfig(GPIO_0_EPWM1_A);
    GPIO_setPadConfig(PHASE_A_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_A_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(GPIO_1_EPWM1_B);
    GPIO_setPadConfig(PHASE_A_EPWM_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_A_EPWM_EPWMB_GPIO, GPIO_QUAL_SYNC);

    //
    // EPWM2 -> PHASE_B_EPWM Pinmux
    //
    GPIO_setPinConfig(GPIO_2_EPWM2_A);
    GPIO_setPadConfig(PHASE_B_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_B_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(GPIO_3_EPWM2_B);
    GPIO_setPadConfig(PHASE_B_EPWM_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_B_EPWM_EPWMB_GPIO, GPIO_QUAL_SYNC);

    //
    // EPWM3 -> PHASE_C_EPWM Pinmux
    //
    GPIO_setPinConfig(GPIO_4_EPWM3_A);
    GPIO_setPadConfig(PHASE_C_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_C_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);

    GPIO_setPinConfig(GPIO_5_EPWM3_B);
    GPIO_setPadConfig(PHASE_C_EPWM_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_C_EPWM_EPWMB_GPIO, GPIO_QUAL_SYNC);
    failure = 0;
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
    EPWM_init();
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //reset the speed estimator
    //time_large = 0;
    //last_time = 0;
    //last_time_large = 0;
    EDIS;
}

void estimateAngleandSpeed(void) {
    //read the hall effect values
    uint32_t hall_u = GPIO_readPin(HALL_EFFECT_U_GPIO);
    uint32_t hall_v = GPIO_readPin(HALL_EFFECT_V_GPIO);
    uint32_t hall_w = GPIO_readPin(HALL_EFFECT_W_GPIO);

    //calculate the current sector
    uint32_t hall_val = (hall_u << 2) + (hall_v << 1) + hall_w;
    uint32_t hall_search = HallLookup[hall_val];

#if EXTRADEBUG
    if (last_sector != 0) {
        if (hall_search == 0) {
            //logString("\n\rINVALID HALL READ OF 0");
        } else {
            if (hall_search != last_sector) {
                if ((hall_search != last_sector + 1 //check if next
                        && (hall_search == 1 && last_sector != 6))//check no wrap
                        && (hall_search != last_sector - 1 //check if backwards
                                && (hall_search == 6 && last_sector != 1))) { //check backwards wrap
                    logString("\n\rINVALID HALL ORDER:");
                    logUIntImmediate(last_sector);
                    logUIntImmediate(hall_search);
                }
            }
        }
    }
    last_sector = hall_search;
#endif

    //check if valid sector
    if (hall_search == 0) {
        int i = 0;
        //if invalid sector attempt to gain correct sector
        for (i = 0; i < 5; i++) {
            hall_u = GPIO_readPin(HALL_EFFECT_U_GPIO);
            hall_v = GPIO_readPin(HALL_EFFECT_V_GPIO);
            hall_w = GPIO_readPin(HALL_EFFECT_W_GPIO);
            hall_val = hall_u << 2 + hall_v << 1 + hall_w;
            hall_search = HallLookup[hall_val];
            if (hall_search != 0) {
                break;
            } else if (i == 4) {
#if EXTRADEBUG
    if (last_sector != 0) {
        if (hall_search == 0) {
            logString("\n\rINVALID HALL READ OF 0");
        }}
#endif
                failure = -1; //failed to get quadrant
                e_angle = 0;
                e_speed = 0;
                return;
            }
        }
    }
    //get the base angle
#warning "angle estimate is different remeber this"
    e_angle = (hall_search-1)*PI_OVER3-PI_OVER6;
//    if (hall_search != 1) {
//
//        //e_angle = (hall_search - 1) * PI_OVER3; // hall loc *2pi/6
//
//    } else {
//        e_angle = 0;
//    }
#ifndef PIDEBUG
    //check to make sure the wheel is actually moving
    //technically this is a write after read CRIT with the direction check but it probably wont be a problem since we don't currently
    //allow nested interrupts and this is called by the pwm interrupt
    if (hall_trigger > HALLCUTOFF) {
        e_speed = 0.0f;
        return;
    }
    hall_trigger++;
    if (ecap_ready == 0) {
        e_speed = 0.0f;
        return;
    }
#endif

    //calculate the electrical angular speed
    float SpdEst = getElectricalSpeed(); ///(pole_pairs);

    //get the time difference since the last hall effect was triggered
    float time_diff = getDiffTime();

    //calculate the actual angle
    if (rotation == 0) {
        e_speed = SpdEst;
    } else {
        e_speed = -SpdEst;
    }

    //check if we are out of range and if so go to midway between poles
    if (time_diff * SpdEst > PI_OVER3) {
        if (rotation == 0) {
            e_angle += PI_OVER6;
        } else {
            e_angle -= PI_OVER6;
        }
    } else {
        e_angle += time_diff * e_speed;
    }

}

/**
 * Runs the PI based Field oriented control algorithm
 */
void sinusoidalDriver(void) {
    if (GPIO_readPin(MotorEnableINV) == 1) {
        //if the motor enable pin is high then the motor should NOT run
        setup = 0;
        stopPWM();
        return;
    } else if (setup == 0) {
        initializeMotor();
        setup = 1;
    }
    //###############################
    //Read current measurements from the board sensors convert from adc values
    //###############################
    //i=(((adc/4097)*3.3-1.5)*3/2/.625)*5

    //pull the current sensor a and c and calculate b
    ia = ADC_readResult(ADC_A_RESULT_BASE, ADC_A_PHASE_U_CURR_SAMPLE);
    //clamp around 0
#ifdef CURRENTCUTTOFF
    if (ia > 2042 && ia < 2054)
    {
        ia = 0;
    }else{
        ia = ia * .0096703297f - 19.8f;
    }
#else
    ia = ia * .0096703297f - 19.8f;
#endif

    //negate this one since the current sensor will be backwards
    ic = ADC_readResult(ADC_C_RESULT_BASE, ADC_C_PHASE_W_CURR_SAMPLE);

    //clamp around 0
#ifdef CURRENTCUTTOFF
    if (ic > 2042 && ic < 2054)
    {
        ic = 0;
    }else{
        ic = -(ic * .0096703297f - 19.8f);
    }
#else
    ic = -(ic * .0096703297f - 19.8f);
#endif

    //we can calculate the third current from the inputs
    ib = -(ia + ic);

    //###############################
    //Run the angle and speed estimate and ensure a valid calculation is reached
    //###############################
    estimateAngleandSpeed();

#ifndef PIDEBUG
    if (failure != 0) {
        EPWM_setTimeBaseCounterMode(PHASE_A_EPWM_BASE,
                                    EPWM_COUNTER_MODE_STOP_FREEZE);
        EPWM_setTimeBaseCounterMode(PHASE_B_EPWM_BASE,
                                    EPWM_COUNTER_MODE_STOP_FREEZE);
        EPWM_setTimeBaseCounterMode(PHASE_C_EPWM_BASE,
                                    EPWM_COUNTER_MODE_STOP_FREEZE);
        stopPWM();
        //on_run = -1;
        return;
    }
#endif

    //###############################
    //Convert our 3 phase ac to the dc dq
    //###############################
    e_angle = e_angle + angle_offset;
    float dq00 = cosf(e_angle);
    float dq01 = cosf(e_angle + offset_240);
    float dq02 = cosf(e_angle + offset_120);
    float dq10 = -sinf(e_angle);
    float dq11 = -sinf(e_angle - offset_240);
    float dq12 = -sinf(e_angle + offset_120);

    //3phase -> dq
    float id = ia * dq00 + ib * dq01 + ic * dq02;
    float iq = ia * dq10 + ib * dq11 + ic * dq12;

    //###############################
    //Calculate the error values of the dc currents
    //###############################

    //get our error signals out
    err_id[1] = idref - id;
    err_iq[1] = iqref - iq;

    //###############################
    //Run the PI on the error currents
    //###############################
    //run the id pid
    yi_id[1] = yi_id[0] + (ki_times_Tsby2) * (err_id[0] + err_id[1]);
    yi_iq[1] = yi_iq[0] + (ki_times_Tsby2) * (err_iq[0] + err_iq[1]);

    float vd = kp * err_id[1] + yi_id[1];
    float vq = kp * err_iq[1] + yi_iq[1];

    yi_id[0] = yi_id[1];
    yi_iq[0] = yi_iq[1];
    err_id[0] = err_id[1];
    err_iq[0] = err_iq[1];

//    //run the iq pid
//    specialStuff = 2.0f / SampleTime * ((1.0f - iq_prev) / (1.0f + iq_prev));
//    iq_prev = iq_e;
//    specialStuff = 1.0f / specialStuff;
//    float u_iq = kp * iq_e + ki * iq_e * specialStuff;
//
//    //compute the cross talk signals
//    float id_c = 0;//iq * -LGain * e_speed;
//    float iq_c = 0;/id * LGain * e_speed;
//
//    //compute vd
//    float vd = id_c + u_id;
//
//    //compute vq
//    float vq = iq_c + u_iq + (-phi_m) * e_speed;

    //###############################
    //Convert from the DQ to 3 PHASE AC
    //###############################
    //dq to 3 phase
    v_a = dq00 * vd + dq10 * vq;
    v_b = dq01 * vd + dq11 * vq;
    v_c = dq02 * vd + dq12 * vq;

    //###############################
    //Prepare to update PWM values
    //###############################
    //scale the values according to the battery voltage
    v_a /= supply_voltage2;
    v_b /= supply_voltage2;
    v_c /= supply_voltage2;

    //ensure the pwm values are not out of range and don't allow for 100% or 0% duty cycle
    float pwm_a_duty = fminf(PWMLIMIT, v_a);
    pwm_a_duty = fmaxf(-PWMLIMIT, v_a);
    float pwm_b_duty = fminf(PWMLIMIT, v_b);
    pwm_b_duty = fmaxf(-PWMLIMIT, v_b);
    float pwm_c_duty = fminf(PWMLIMIT, v_c);
    pwm_c_duty = fmaxf(-PWMLIMIT, v_c);

    //5000 is the range of the PWM so scale our output values pwm_duty*pwm_cycles/2+pwm_cycles/2
    foc_pwm_a = ((uint32_t) (pwm_a_duty * (float) PWMPERIOD_2)) + PWMPERIOD_2;
    foc_pwm_b = ((uint32_t) (pwm_b_duty * (float) PWMPERIOD_2)) + PWMPERIOD_2;
    foc_pwm_c = ((uint32_t) (pwm_c_duty * (float) PWMPERIOD_2)) + PWMPERIOD_2;

    //###############################
    //Update PWM values
    //###############################
    EPWM_setCounterCompareValue(PHASE_A_EPWM_BASE, EPWM_COUNTER_COMPARE_A,
                                foc_pwm_a);
    EPWM_setCounterCompareValue(PHASE_B_EPWM_BASE, EPWM_COUNTER_COMPARE_A,
                                foc_pwm_b);
    EPWM_setCounterCompareValue(PHASE_C_EPWM_BASE, EPWM_COUNTER_COMPARE_A,
                                foc_pwm_c);

}
