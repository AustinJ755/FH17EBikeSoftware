/*
 * motor_driver.c
 *
 *  Created on: Apr 21, 2023
 *      Author: Austin James
 */

#include "motor_driver.h"
#include "board.h"
#include "math.h"
uint32_t system_time_large = 0;
uint32_t last_hall_time = 0;
uint32_t last_hall_time_large = 0;

uint32_t hall_trigger=HALLCUTOFF+1;
uint32_t ecap_ready=0;
uint16_t rotation = 0; //indicates measured direction of wheel rotation

/**
 * Disables PWM pins puts them in standard GPIO Mode and sets them all to low
 */
void stopPWM (void){
    //
    // EPWM1 disable
    //
    GPIO_setPinConfig(GPIO_1_GPIO1);
    GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);
    GPIO_setPadConfig(PHASE_A_EPWM_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_A_EPWM_EPWMB_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(PHASE_A_EPWM_EPWMB_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(PHASE_A_EPWM_EPWMB_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(GPIO_0_GPIO0);
    GPIO_writePin(PHASE_A_EPWM_EPWMA_GPIO, 0);
    GPIO_setPadConfig(PHASE_A_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_A_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(PHASE_A_EPWM_EPWMA_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(PHASE_A_EPWM_EPWMA_GPIO, GPIO_CORE_CPU1);

    //
    // EPWM2 -disable
    //
    GPIO_setPinConfig(GPIO_3_GPIO3);
    GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);
    GPIO_setPadConfig(PHASE_B_EPWM_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_B_EPWM_EPWMB_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(PHASE_B_EPWM_EPWMB_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(PHASE_B_EPWM_EPWMB_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(GPIO_2_GPIO2);
    GPIO_writePin(PHASE_B_EPWM_EPWMA_GPIO, 0);
    GPIO_setPadConfig(PHASE_B_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_B_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(PHASE_B_EPWM_EPWMA_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(PHASE_B_EPWM_EPWMA_GPIO, GPIO_CORE_CPU1);

    //
    // EPWMC disable
    //
    GPIO_setPinConfig(GPIO_5_GPIO5);
    GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 0);
    GPIO_setPadConfig(PHASE_C_EPWM_EPWMB_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_C_EPWM_EPWMB_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(PHASE_C_EPWM_EPWMB_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(PHASE_C_EPWM_EPWMB_GPIO, GPIO_CORE_CPU1);

    GPIO_setPinConfig(GPIO_4_GPIO4);
    GPIO_writePin(PHASE_C_EPWM_EPWMA_GPIO, 0);
    GPIO_setPadConfig(PHASE_C_EPWM_EPWMA_GPIO, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(PHASE_C_EPWM_EPWMA_GPIO, GPIO_QUAL_SYNC);
    GPIO_setDirectionMode(PHASE_C_EPWM_EPWMA_GPIO, GPIO_DIR_MODE_OUT);
    GPIO_setMasterCore(PHASE_C_EPWM_EPWMA_GPIO, GPIO_CORE_CPU1);
}

void initializeMotor(void){
#ifdef USE_SINUSOIDAL_CONTROL
    sinusoidalMotorInit();
#else
#ifdef USE_TRAPEZOIDAL_CONTROL
    trapezoidalMotorInit();
#endif
#endif
}

/**
 * This interrupt is called after the pwm count reaches zero or its period and after the ADC finishes reading the value from the current
 * sensors
 */
__interrupt void pwmCountTrigger(void){
    EPWM_clearADCTriggerFlag(PHASE_A_EPWM_BASE, EPWM_SOC_A);
#ifdef USE_SINUSOIDAL_CONTROL
    sinusoidalDriver();
#endif
    //EPWM_clearEventTriggerInterruptFlag(base)

    ADC_clearInterruptStatus(ADC_A_BASE, ADC_INT_NUMBER1);
    //ADC_INT_SOC_TRIGGER_ADCINT1
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

/**
 * Gets the time difference from now and the last time a hall effect sensor was triggered
 * @return float representing the time difference in seconds
 */
float getDiffTime(void){
    float time_diff;
    uint32_t curr_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    if(curr_time<last_hall_time){
        uint32_t diff = last_hall_time-curr_time;
        uint64_t largediff = system_time_large - last_hall_time_large;
        time_diff = largediff*((uint32_t)(0xFFFFFFFF))+diff;
        time_diff = time_diff*(1.0f/25000000.0f); // get the time difference in terms of seconds remeber if CLKFREQ/4
    }else{
        uint32_t diff = last_hall_time+((0xFFFFFFFF)-curr_time);
        uint64_t largediff = system_time_large - last_hall_time_large-1;
        time_diff = largediff*((uint32_t)(0xFFFFFFFF))+diff;
        time_diff = time_diff*(1.0f/25000000.0f); // get the time difference in terms of seconds remeber if CLKFREQ/4
    }
    return time_diff;
}
float getDiffTimeV(uint32_t small, uint32_t large){
    float time_diff;
    uint32_t curr_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    if(curr_time<small){
        uint32_t diff =small-curr_time;
        uint64_t largediff = system_time_large - large;
        time_diff = largediff*((uint32_t)(0xFFFFFFFF))+diff;
        time_diff = time_diff*(1.0f/25000000.0f); // get the time difference in terms of seconds remeber if CLKFREQ/4
    }else{
        uint32_t diff = small+((0xFFFFFFFF)-curr_time);
        uint64_t largediff = system_time_large - large-1;
        time_diff = largediff*((uint32_t)(0xFFFFFFFF))+diff;
        time_diff = time_diff*(1.0f/25000000.0f); // get the time difference in terms of seconds remeber if CLKFREQ/4
    }
    return time_diff;
}

/**
 * Gets the electrical speed of the motor in radians per second
 * @return speed in radians per second
 */
float getElectricalSpeed(void) {
    float timed = ECAP_getEventTimeStamp(ECAP_PHASEU_BASE, ECAP_EVENT_1); // get count per cycle



    timed = 100000000.00f / timed; // get electrical frequency

    //set speed to zero if not moving
    if(getDiffTime()>ZERO_CUTOFF_TIME){
        timed=0;
    }

    //calculate the electrical angular speed
    return M_PI * 2.0f * timed;
}

/**
 * Gets the mechanical speed of the motor in MPH
 * @return mechanical speed in MPH
 */
float getMotorSpeed(void) {
    //get the electrical speed
    float electrical_speed = getElectricalSpeed();

    //gets the mechanical speed of the motor in radians per second
    float motorspeed = electrical_speed / POLE_PAIRS;

    //gets the speed of the motor in inches per second
    motorspeed *= WHEEL_RADIUS;

    motorspeed *= 3600.0f; // convert to inches per hour

    motorspeed = motorspeed/63360.0f; // convert to miles per hour

    return motorspeed;
}

/**
 * This interrupt triggers everytime the system timer rolls over. This allows us to keep track of time with 64 bits instead of 32
 */
__interrupt void timer0ISR(void) {
    system_time_large++;
    CPUTimer_clearOverflowFlag(CPUTIMER0_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

/**
 * This interrupt is triggered by ECAP 1, it is called on the rising edge of hall effect sensor U. It keeps track of the direction that the motor is spinning
 */
static phaseUTime;
static phaseUTimeLarge;
__interrupt void directionCheck(void) {
    //update the last time that a hall effect was triggered
    last_hall_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_hall_time_large = system_time_large;

    //ensure that the motor is actually spinning
#ifndef PIDEBUG
    if (hall_trigger > HALLCUTOFF) {
        //bike wheel stopped wait for next valid rotation

        ecap_ready = 0;
    } else {
        //bike wheel has started after stopping
        ecap_ready = 1;
    }
    hall_trigger = 0;
#endif

    //set the rotation direction for the motor
    //check 101 if so then forward spin
    if (GPIO_readPin(HALL_EFFECT_W_GPIO) == 1) {
        rotation = 0;
    } else {
        rotation = 1;
    }

    //with trapezoidal control we update everytime we enter a new sector
#ifdef  TRAPEZOID_MOTOR
    trapezoidalDriver();
#endif
    //clear the interrupt
    ECAP_clearInterrupt(ECAP_PHASEU_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_1);
    ECAP_clearGlobalInterrupt(ECAP_PHASEU_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);
}

/*
 * The following interrupts are caused by the rising/falling edges of the hall effect sensors excluding Phase U Rising which handles rotation direction detection
 * The interrupts update the most recent hall effect time and call the trapezoidal driver if enabled
 */
__interrupt void logTime(void) {
    last_hall_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_hall_time_large = system_time_large;
    //GPIO_Inter
#ifdef  TRAPEZOID_MOTOR
    trapezoidalDriver();
#endif
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
__interrupt void logTime1(void) {
    static phaseVTime;
    static phaseVTimeLarge;
    last_hall_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_hall_time_large = system_time_large;
#ifdef  TRAPEZOID_MOTOR
    trapezoidalDriver();
#endif
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
__interrupt void logTime2(void) {
    static phaseWTime;
    static phaseWTimeLarge;
    last_hall_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_hall_time_large = system_time_large;
#ifdef  TRAPEZOID_MOTOR
    trapezoidalDriver();
#endif
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);
}
