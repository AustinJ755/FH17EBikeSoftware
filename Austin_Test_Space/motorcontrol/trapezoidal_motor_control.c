/*
 * trapezoidal_motor_control.c
 *
 *  Created on: Apr 11, 2023
 *      Author: Austin James
 */
//#include "trapezoidal_motor_control.h"
#include "motor_driver.h"
#include "driverlib.h"
#include "device.h"
#include "math.h"
#include "board.h"

typedef enum
{
    GPIOMODE, PWMMODE
} phaseMode;

static void setTopA(phaseMode mode) {
    if (mode == PWMMODE) {
        GPIO_setPinConfig(GPIO_0_EPWM1_A);
        GPIO_setPadConfig(PHASE_A_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(PHASE_A_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
    } else {
        GPIO_setPinConfig(GPIO_0_GPIO0);
        GPIO_writePin(PHASE_A_EPWM_EPWMA_GPIO, 0);
        GPIO_setPadConfig(PHASE_A_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(PHASE_A_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
        GPIO_setDirectionMode(PHASE_A_EPWM_EPWMA_GPIO, GPIO_DIR_MODE_OUT);
        GPIO_setMasterCore(PHASE_A_EPWM_EPWMA_GPIO, GPIO_CORE_CPU1);
    }
}

static void setTopB(phaseMode mode) {
    if (mode == PWMMODE) {
        GPIO_setPinConfig(GPIO_2_EPWM2_A);
        GPIO_setPadConfig(PHASE_B_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(PHASE_B_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
    } else {
        GPIO_setPinConfig(GPIO_2_GPIO2);
        GPIO_writePin(PHASE_B_EPWM_EPWMA_GPIO, 0);
        GPIO_setPadConfig(PHASE_B_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(PHASE_B_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
        GPIO_setDirectionMode(PHASE_B_EPWM_EPWMA_GPIO, GPIO_DIR_MODE_OUT);
        GPIO_setMasterCore(PHASE_B_EPWM_EPWMA_GPIO, GPIO_CORE_CPU1);
    }
}
static void setTopC(phaseMode mode) {
    if (mode == PWMMODE) {
        GPIO_setPinConfig(GPIO_4_EPWM3_A);
        GPIO_setPadConfig(PHASE_C_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(PHASE_C_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
    } else {
        GPIO_setPinConfig(GPIO_4_GPIO4);
        GPIO_writePin(PHASE_C_EPWM_EPWMA_GPIO, 0);
        GPIO_setPadConfig(PHASE_C_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
        GPIO_setQualificationMode(PHASE_C_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
        GPIO_setDirectionMode(PHASE_C_EPWM_EPWMA_GPIO, GPIO_DIR_MODE_OUT);
        GPIO_setMasterCore(PHASE_C_EPWM_EPWMA_GPIO, GPIO_CORE_CPU1);
    }
}

void trapezoidalMotorInit(void) {
    //put all pwm pins into GPIO mode
    stopPWM();

    //go through the required startup
    GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(CHARGE_TIME);
    GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);

    GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(CHARGE_TIME);
    GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);

    GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(CHARGE_TIME);
    GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);

    //we are finished all we have to do is enable the pwm module
    EPWM_init();
    EPWM_setCounterCompareValue(PHASE_A_EPWM_BASE, EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(PHASE_B_EPWM_BASE, EPWM_COUNTER_COMPARE_A, 0);
    EPWM_setCounterCompareValue(PHASE_C_EPWM_BASE, EPWM_COUNTER_COMPARE_A, 0);
}
static uint16_t HallLookup[8] = { 0, // bad reading //0
        2, // 1 Sector //2
        4, // 2 //
        3, // 3 //
        6, // 4 //1//
        1, // 5 //
        5, // 6
        0  // bad reading //7
        };

//slow moving wheel
float pwm_duty = 1000;
uint32_t manual = 7;
uint32_t newhall = 0;
uint32_t hallread = 0;
float old_duty = 500;
#define DELAY_CHANGE 2.5
#define SAFECHANGE 20
uint32_t last_run = 0;
uint32_t last_run_large = 0;
float time_diff;
void trapezoidalDriver(void) {
    //we will have a Watchdog timer which allows for pwmduty to change even if the motor isn't moving
    //write watchdog value

    if (GPIO_readPin(MotorEnableINV) == 1) {
        //if the motor enable pin is high then the motor should NOT run
        stopPWM();
        return;
    }
    time_diff= getDiffTimeV(last_run,last_run_large);
    if(time_diff<.004f){
        return;
    }
    last_run = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_run_large = system_time_large;
    //get the value of the pwm_duty from the the throttle if it is enabled
    float throttle_read = ADC_readResult(ADC_A_RESULT_BASE,
                                         ADC_A_THROTTLE_SAMPLE);

    //clamp low side of throttle value to prevent drift from causing movement
    if (throttle_read < 600) {
        throttle_read = 0;
    }

    //calculate the pwm_duty
    pwm_duty = (throttle_read / 2300.0f);
    if (pwm_duty > 1.0f) {
        pwm_duty = 1.0f;
    }
    pwm_duty = pwm_duty * .94;
    pwm_duty *= PWMPERIOD;
    if (pwm_duty < 0) {
        pwm_duty = 0;
    }
    if (pwm_duty > 4800) {
        pwm_duty = 4800;
    }
    //don't let the duty change to quickly
    if (pwm_duty > old_duty + SAFECHANGE) {
        old_duty = old_duty + SAFECHANGE;
        pwm_duty = old_duty;
    } else if (pwm_duty < old_duty - SAFECHANGE) {
        old_duty = old_duty - SAFECHANGE;
        pwm_duty = old_duty;
    } else {
        old_duty = pwm_duty;
    }

    //set the pwm rate of the phases
    EPWM_setCounterCompareValue(PHASE_A_EPWM_BASE, EPWM_COUNTER_COMPARE_A,
                                pwm_duty);
    EPWM_setCounterCompareValue(PHASE_B_EPWM_BASE, EPWM_COUNTER_COMPARE_A,
                                pwm_duty);
    EPWM_setCounterCompareValue(PHASE_C_EPWM_BASE, EPWM_COUNTER_COMPARE_A,
                                pwm_duty);

    //start with a invalid value
    static uint32_t last_hall_val = 8;
    uint32_t hall_u = GPIO_readPin(HALL_EFFECT_U_GPIO);
    uint32_t hall_v = GPIO_readPin(HALL_EFFECT_V_GPIO);
    uint32_t hall_w = GPIO_readPin(HALL_EFFECT_W_GPIO);
    uint32_t hall_val = ((hall_u << 2) + (hall_v << 1) + hall_w) & 0x7;
    newhall = HallLookup[hall_val];
    if (last_hall_val == hall_val) {
        //we don't need to change anything
        return;
    }
    //lookup the sector we need
    uint16_t sector = 0;
    if (manual != 7) {
        sector = manual;    //HallLookup[hall_val];
    } else {
        sector = HallLookup[hall_val];
    }
    hallread = hall_val;
    //toggle the pwm to properly drive
    //##############################################
    //FORWARDS DRIVING CODE
    //##############################################

    switch (sector)
    {
        case 1:
            //swapped
            setTopB(GPIOMODE);
            setTopC(GPIOMODE);
            DEVICE_DELAY_US(DELAY_CHANGE);
            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 1);
            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
            DEVICE_DELAY_US(DELAY_CHANGE);
            setTopA(PWMMODE);
            break;

        case 2:
            //swapped
            setTopB(GPIOMODE);
            setTopC(GPIOMODE);
            DEVICE_DELAY_US(DELAY_CHANGE);
            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 1);
            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
            DEVICE_DELAY_US(DELAY_CHANGE);
            setTopA(PWMMODE);
            break;

        case 3:
            //Swapped
            setTopA(GPIOMODE);
            setTopC(GPIOMODE);
            DEVICE_DELAY_US(DELAY_CHANGE);
            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 1);
            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
            DEVICE_DELAY_US(DELAY_CHANGE);
            setTopB(PWMMODE);
            break;

        case 4:
            //swapped
            setTopA(GPIOMODE);
            setTopC(GPIOMODE);
            DEVICE_DELAY_US(DELAY_CHANGE);
            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 1);
            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
            DEVICE_DELAY_US(DELAY_CHANGE);
            setTopB(PWMMODE);
            break;
        case 5:
            //swapped
            setTopA(GPIOMODE);
            setTopB(GPIOMODE);
            DEVICE_DELAY_US(DELAY_CHANGE);
            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 1);
            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
            DEVICE_DELAY_US(DELAY_CHANGE);
            setTopC(PWMMODE);
            break;
        case 6:
            //swapped
            setTopA(GPIOMODE);
            setTopB(GPIOMODE);
            DEVICE_DELAY_US(DELAY_CHANGE);
            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 1);
            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
            DEVICE_DELAY_US(DELAY_CHANGE);
            setTopC(PWMMODE);
            break;

        default:
            //TODO NEED TO IMPLEMENT SOME SORT OF SAFE SHUTDOWN BEHAVIOR
            //an invalid state occurred shutdown the motor
            stopPWM();
            break;
    }
    //##############################################
    //BACKWARDS DRIVING CODE
    //##############################################
//    float delay =1;
//    switch(sector){
//        case 1:
//            setTopA(GPIOMODE);
//            setTopC(GPIOMODE);
//            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
//            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
//            DEVICE_DELAY_US(delay);
//            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 1);
//            DEVICE_DELAY_US(delay);
//            setTopB(PWMMODE);
//            break;
//        case 2:
//            setTopA(GPIOMODE);
//            setTopC(GPIOMODE);
//            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
//            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
//            DEVICE_DELAY_US(delay);
//            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 1);
//            DEVICE_DELAY_US(delay);
//            setTopB(PWMMODE);
//            break;
//        case 3:
//            setTopA(GPIOMODE);
//            setTopB(GPIOMODE);
//            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
//            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
//            DEVICE_DELAY_US(delay);
//            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 1);
//            DEVICE_DELAY_US(delay);
//            setTopC(PWMMODE);
//            break;
//        case 4:
//            setTopA(GPIOMODE);
//            setTopB(GPIOMODE);
//            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
//            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
//            DEVICE_DELAY_US(delay);
//            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 1);
//            DEVICE_DELAY_US(delay);
//            setTopC(PWMMODE);
//            break;
//        case 5:
//            setTopB(GPIOMODE);
//            setTopC(GPIOMODE);
//            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
//            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
//            DEVICE_DELAY_US(delay);
//            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 1);
//            DEVICE_DELAY_US(delay);
//            setTopA(PWMMODE);
//            break;
//        case 6:
//            setTopB(GPIOMODE);
//            setTopC(GPIOMODE);
//            GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
//            GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
//            DEVICE_DELAY_US(delay);
//            GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 1);
//            DEVICE_DELAY_US(delay);
//            setTopA(PWMMODE);
//            break;
//        default:
//            //TODO NEED TO IMPLEMENT SOME SORT OF SAFE SHUTDOWN BEHAVIOR
//            //an invalid state occurred shutdown the motor
//            disablePWMPinsT();
//            break;
//    }

}

