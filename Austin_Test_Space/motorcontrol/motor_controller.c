/*
 * motor_controller.c
 *
 *  Created on: Apr 2, 2023
 *      Author: aj755
 */

#include <board.h>
#include "motor_controller.h"
#include "trapezoidal_motor_control.h"
#include "board.h"
#include "customtools/stringtools.h"
#include "math.h"
//#define PIDEBUG
//#define OPENLOOPTEST
//#define CURRENTCUTOFF
static int failure = 0;
uint32_t time_large = 0;
static uint32_t last_time = 0;
static uint32_t last_time_large = 0;

double err_id[2];
double err_iq[2];
double yi_id[2];
double yi_iq[2];
double ki_times_Tsby2;
float angle_offset;
/**
 * This variable is reset everytime the HALL_U is triggered. If hall U is not triggered within the required number of cycles then values from the reading are invalid
 */
uint32_t hall_trigger=HALLCUTOFF+1;
uint32_t ecap_ready=0;



#define tau_i (10/(2*M_PI*fsw))
//const conversion_factor1
float iq_prev = 0;
float id_prev = 0;
float kp = motor_resistance / tau_i;
float ki = motor_inductance / tau_i;
float LGain = motor_inductance;
float phi_m = flux_linkage;
float SampleTime = 1.0f / (2.0f * fsw); //we are sampling at 20kHZ since both PWM zero and period trigger ADC
float e_angle = 0.0f;    //electrical angle
float e_speed = 0.0f;
float idref = 2.0f;
float iqref = 0.0f;
static float supply_voltage = 48.0f;

//half the value of the battery supply voltage this value could be updated in real time due to our voltage sensor which could allow for more accurate PI control
float supply_voltage2 = 24.0f;
static float time_diff;
uint16_t rotation = 0;
const float offset_120 = M_PI * 2 / 3;
#define TRAPEZOID_MOTOR
const float offset_240 = -M_PI * 2 / 3;
static uint16_t HallLookup[8] = {
        0, // bad reading //0
        2, // 1 Sector //2
        4, // 2 //
        3, // 3 //
        6, // 4 //1//
        1, // 5 //
        5, // 6
        0  // bad reading //7
};


/**
 * Disables PWM pins puts them in standard GPIO Mode and sets them all to low
 */
void disablePWMPins(void){
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
/**
 * Setup the motor by running through the initialization process
 *
 * The motor should be pulled online by going individually through each phase and pulling ONLY its low side driver higher for the required amount of time
 * The required time can be calculated from the AN-9088.pdf data sheet
 *
 * The motor should boot with all pwm off and each signal controlled individually. In this section we will run through all low phases and initialize.
 * Next we will change the PWM so that the phases are differentially driven with all phases set to a starting duty cycle of 0
 */
void initMotor(void)
{
#ifdef  TRAPEZOID_MOTOR
    initTMotor();
    updateDriver();
    return;
#endif
    EALLOW;
    //reset all pwm pins to generic io pulled down
    EPWM_setTimeBaseCounterMode(PHASE_A_EPWM_BASE,
                                EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBaseCounterMode(PHASE_B_EPWM_BASE,
                                EPWM_COUNTER_MODE_STOP_FREEZE);
    EPWM_setTimeBaseCounterMode(PHASE_C_EPWM_BASE,
                                EPWM_COUNTER_MODE_STOP_FREEZE);

    disablePWMPins();
    GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(2000);
    GPIO_writePin(PHASE_A_EPWM_EPWMB_GPIO, 0);

    GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(2000);
    GPIO_writePin(PHASE_B_EPWM_EPWMB_GPIO, 0);

    GPIO_writePin(PHASE_C_EPWM_EPWMB_GPIO, 1);
    DEVICE_DELAY_US(2000);
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
    time_large = 0;
    last_time=0;
    last_time_large=0;
    EDIS;
}





__interrupt void timer0ISR(void){
    time_large++;
    CPUTimer_clearOverflowFlag(CPUTIMER0_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
__interrupt void directionCheck(void){
    last_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_time_large = time_large;
    //Acknowledge the interrupt
#ifndef PIDEBUG
    if(hall_trigger>HALLCUTOFF){
        //bike wheel stopped wait for next valid rotation

       ecap_ready=0;
    }else{
        //bike wheel has started after stopping
        ecap_ready=1;
    }
    hall_trigger=0;
#endif
    if(GPIO_readPin(HALL_EFFECT_V_GPIO)==1){
        rotation = 0;
    }else{
        rotation = 1;
    }
#ifdef  TRAPEZOID_MOTOR
    updateDriver();
#endif
    ECAP_clearInterrupt(ECAP_PHASEU_BASE,ECAP_ISR_SOURCE_CAPTURE_EVENT_1);
    ECAP_clearGlobalInterrupt(ECAP_PHASEU_BASE);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);
}

/*
 * The following interrupts are caused by the rising falling edges of the hall effect sensors excluding Phase U Rising which handles rotation direction detection
 */
__interrupt void logTime(void){
    last_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_time_large = time_large;
    //GPIO_Inter
#ifdef  TRAPEZOID_MOTOR
    updateDriver();
#endif
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
__interrupt void logTime1(void){
    last_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_time_large = time_large;
#ifdef  TRAPEZOID_MOTOR
    updateDriver();
#endif
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
__interrupt void logTime2(void){
    last_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);
    last_time_large = time_large;
#ifdef  TRAPEZOID_MOTOR
    updateDriver();
#endif
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP12);
}

void estimateAngleandSpeed(void){
    uint32_t hall_u = GPIO_readPin(HALL_EFFECT_U_GPIO);
    uint32_t hall_v = GPIO_readPin(HALL_EFFECT_V_GPIO);
    uint32_t hall_w = GPIO_readPin(HALL_EFFECT_W_GPIO);
    //find the speed that the wheel is moving at
    uint32_t time1 = ECAP_getEventTimeStamp(ECAP_PHASEU_BASE, ECAP_EVENT_1);
    uint32_t time2 = ECAP_getEventTimeStamp(ECAP_PHASEV_BASE, ECAP_EVENT_1);
    uint32_t time3 = ECAP_getEventTimeStamp(ECAP_PHASEW_BASE, ECAP_EVENT_1);
    //save the current stats
    uint32_t curr_time = CPUTimer_getTimerCount(myCPUTIMER0_BASE);;
    uint32_t curr_time_large = time_large;


    uint32_t hall_val = (hall_u<<2)+(hall_v<<1)+hall_w;
    uint32_t hall_search = HallLookup[hall_val];

    if(hall_search==0){
        int i=0;
        for(i=0;i<5;i++){
            hall_u = GPIO_readPin(HALL_EFFECT_U_GPIO);
            hall_v = GPIO_readPin(HALL_EFFECT_U_GPIO);
            hall_w = GPIO_readPin(HALL_EFFECT_U_GPIO);
            hall_val = hall_u<<2+hall_v<<1+hall_w;
            hall_search = HallLookup[hall_val];
            if(hall_search!=0){
                break;
            }else if(i==4){
                failure = -1; //failed to get quadrant
                e_angle = 0;
                return;
            }
        }
    }
    //get the base angle
    if(hall_search!=1){
    e_angle = (hall_search-1)*M_PI/3.0f; // hall loc *2pi/6
    }else{
        e_angle = 0;
    }
#ifndef PIDEBUG
    //check to make sure the wheel is actually moving
    //technically this is a write after read CRIT with the direction check but it probably wont be a problem since we don't currently
    //allow nested interrupts and this is called by the pwm interrupt
    if(hall_trigger>HALLCUTOFF){
        e_speed=0.0f;
        return;
    }
    hall_trigger++;
    if(ecap_ready==0){
        e_speed=0.0f;
        return;
    }
#endif
    //get the time to complete a full electrical rotation
    float timed = time1;//+time2+time3;
    //timed/=3.0f; // get cycles to complete electrical rotation
    timed=100000000.0f/timed; // get electrical frequency

    //calculate the electrical angular speed
    float SpdEst=M_PI*2.0f*timed; ///(pole_pairs);


    //figure out which cap we should be looking at



    if(curr_time<last_time){
        uint32_t diff = last_time-curr_time;
        uint64_t largediff = curr_time_large - last_time_large;
        time_diff = largediff*((uint32_t)(0xFFFFFFFF))+diff;
        time_diff = time_diff*(1.0f/25000000.0f); // get the time difference in terms of seconds remeber if CLKFREQ/4
    }else{
        uint32_t diff = last_time+((0xFFFFFFFF)-curr_time);
        uint64_t largediff = curr_time_large - last_time_large-1;
        time_diff = largediff*((uint32_t)(0xFFFFFFFF))+diff;
        time_diff = time_diff*(1.0f/25000000.0f); // get the time difference in terms of seconds remeber if CLKFREQ/4
    }

    //calculate the actual angle
    if(rotation==0){
        e_speed = SpdEst;
    }else{
        e_speed = -SpdEst;
    }
    //check if we are out of range and if so go to midway
    if(time_diff*SpdEst>M_PI/3.0f){
        if(rotation==0){
            e_angle+=M_PI/6.0f;
        }else{
            e_angle-=M_PI/6.0f;
        }
    }else{
        e_angle+=time_diff*e_speed;
    }

    //update prev values
//    last_time=curr_time;
//    last_time_large=curr_time_large;
}

static float ia = 0;
static float ib = 0;
static float ic = 0;
float v_a;
float v_b;
float v_c;
float id_e;
float iq_e;
static float angle = 0.0;
int setup = 0;
void sinusoidalControl(void){
    if(GPIO_readPin(MotorEnableINV)==1){
        //if the motor enable pin is high then the motor should NOT run
        setup=0;
        disablePWMPinsT();
        return;
    }else if(setup==0){
        initMotor();
        setup=1;
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
    //TODO Reenable
//    if(failure!=0){
//        EPWM_setTimeBaseCounterMode(PHASE_A_EPWM_BASE,
//                                    EPWM_COUNTER_MODE_STOP_FREEZE);
//        EPWM_setTimeBaseCounterMode(PHASE_B_EPWM_BASE,
//                                    EPWM_COUNTER_MODE_STOP_FREEZE);
//        EPWM_setTimeBaseCounterMode(PHASE_C_EPWM_BASE,
//                                    EPWM_COUNTER_MODE_STOP_FREEZE);
//        disablePWMPins();
//        on_run=-1;
//        return;
//    }

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
    yi_id[1] = yi_id[0] + (ki_times_Tsby2)*(err_id[0]+err_id[1]);
    yi_iq[1] = yi_iq[0] + (ki_times_Tsby2)*(err_iq[0]+err_iq[1]);

    float vd = kp*err_id[1] + yi_id[1];
    float vq = kp*err_iq[1] + yi_iq[1];

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
    float v_a1 = fminf(PWMLIMIT, v_a);
    v_a1 = fmaxf(-PWMLIMIT, v_a);
    float v_b1 = fminf(PWMLIMIT, v_b);
    v_b1 = fmaxf(-PWMLIMIT, v_b);
    float v_c1 = fminf(PWMLIMIT, v_c);
    v_c1 = fmaxf(-PWMLIMIT, v_c);

    //5000 is the range of the PWM so scale our output values
    v_a1 *= (2500);
    v_b1 *= (2500);
    v_c1 *= (2500);
    v_a1 += 2500.0f;
    v_b1 += 2500.0f;
    v_c1 += 2500.0f;

    //###############################
    //Update PWM values
    //###############################
    EPWM_setCounterCompareValue(PHASE_A_EPWM_BASE, EPWM_COUNTER_COMPARE_A, v_a1);
    EPWM_setCounterCompareValue(PHASE_B_EPWM_BASE, EPWM_COUNTER_COMPARE_A, v_b1);
    EPWM_setCounterCompareValue(PHASE_C_EPWM_BASE, EPWM_COUNTER_COMPARE_A, v_c1);

}
void trapezoidalControl(void){
    //start with a invalid value
    static uint32_t last_hall_val = 8;
    uint32_t hall_u = GPIO_readPin(HALL_EFFECT_U_GPIO);
    uint32_t hall_v = GPIO_readPin(HALL_EFFECT_V_GPIO);
    uint32_t hall_w = GPIO_readPin(HALL_EFFECT_W_GPIO);
    uint32_t hall_val = ((hall_u<<2)+(hall_v<<1)+hall_w)&0x7;
    if(last_hall_val==hall_val){
        //we don't need to change anything
        return;
    }
    switch(hall_val){

    }
}

__interrupt void executePIControl(void)
{
#ifdef  TRAPEZOID_MOTOR
    //we are in trapezoid form no PI PWM
    ADC_clearInterruptStatus(ADC_A_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
    return;
#endif
#ifdef OPENLOOPTEST

    if(angle>1000){
        angle =0;
    }else{
        angle+=1.0f/6000.0f;
    }
    float va = .5f+.35f*cosf(2.0f*M_PI*angle);
    float vb = .5f+.35f*cosf(2.0f*M_PI*angle-2.0f*M_PI/3.0f);
    float vc = .5f+.35f*cosf(2.0f*M_PI*angle+2.0f*M_PI/3.0f);
    uint32_t v_a = va*5000;//
    uint32_t v_b = vb*5000;
    uint32_t v_c = vc*5000;
    EPWM_setCounterCompareValue(PHASE_A_EPWM_BASE, EPWM_COUNTER_COMPARE_A,v_a );
    EPWM_setCounterCompareValue(PHASE_B_EPWM_BASE, EPWM_COUNTER_COMPARE_A, v_b);
    EPWM_setCounterCompareValue(PHASE_C_EPWM_BASE, EPWM_COUNTER_COMPARE_A, v_c);
    estimateAngleandSpeed();
    //just drive the load with base values
#else

    sinusoidalControl();
    //acknowledge the interrupt so this can continue to run

#endif
    ADC_clearInterruptStatus(ADC_A_BASE, ADC_INT_NUMBER1);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}


//TODO create a file for handling this parsing
#include "displayDriver/display_driver.h"

char* ia_text = "Ia: ";
char* ib_text = "Ib: ";
char* ic_text = "Ic: ";
char* a_est_text = "Angle: ";
char* w_est_text = "Speed: ";
char* id_text = "Id_e: ";
char* iq_text = "Iq_e: ";
char* va_text = "va: ";
char* vb_text = "vb: ";
char* vc_text = "vc: ";
char* time_text  = "Time: ";
char buf [27] = {' ',};
#include "./ryu/ryu.h"

/**
 * This timer ISR is used for debug of the module
 */
__interrupt void timer1ISR(void){
    //profile Current Conversion and Hall Effect
    updateDriver();
#ifdef 0
    static uint16_t ran = 0;
    if(!ran){
        ran = 1;
        //clear the screen
        drawFilledColorBox(0, 0, 239, 319, 0);
        //Log the three currents we read
        screenDrawText(0, 0, ia_text, 0xFFFF, 0, 2);
        screenDrawText(0, FONT_HEIGHT*2, ib_text, 0xFFFF, 0, 2);
        screenDrawText(0, FONT_HEIGHT*2*2, ic_text, 0xFFFF, 0, 2);
        //Log the angle and speed
        screenDrawText(0, FONT_HEIGHT*2*3, a_est_text, 0xFFFF, 0, 2);
        screenDrawText(0, FONT_HEIGHT*2*4, w_est_text, 0xFFFF, 0, 2);
        //LOG the id/iq
        screenDrawText(0, FONT_HEIGHT*2*5, id_text, 0xFFFF, 0, 2);
        screenDrawText(0, FONT_HEIGHT*2*6, iq_text, 0xFFFF, 0, 2);

        //log the 3 output set points
        screenDrawText(0, FONT_HEIGHT*2*7, va_text, 0xFFFF, 0, 2);
        screenDrawText(0, FONT_HEIGHT*2*8, vb_text, 0xFFFF, 0, 2);
        screenDrawText(0, FONT_HEIGHT*2*9, vc_text, 0xFFFF, 0, 2);
    }

    buf[25]=' ';

#ifdef 0//OPENLOOPTEST
    screenDrawText(7*10,FONT_HEIGHT*2*3,_float_to_char(angle,buf),0xFFFF, 0, 2);
#else
    //log the 3 currents we read
    screenDrawText(4*10,0,float2StringSimple(ia,buf),0xFFFF, 0, 2);
    screenDrawText(4*10,FONT_HEIGHT*2,float2StringSimple(ib,buf),0xFFFF, 0, 2);
    screenDrawText(4*10,FONT_HEIGHT*2*2,float2StringSimple(ic,buf),0xFFFF, 0, 2);

    screenDrawText(8*10,FONT_HEIGHT*2*3,float2StringSimple(e_angle,buf),0xFFFF, 0, 2);
    screenDrawText(8*10,FONT_HEIGHT*2*4,float2StringSimple(e_speed,buf),0xFFFF, 0, 2);

    //id and iq
    screenDrawText(6*10,FONT_HEIGHT*2*5,float2StringSimple(id_e,buf),0xFFFF, 0, 2);
    screenDrawText(6*10,FONT_HEIGHT*2*6,float2StringSimple(iq_e,buf),0xFFFF, 0, 2);

    //log all 3 output voltages
    screenDrawText(4*10,FONT_HEIGHT*2*7,float2StringSimple(v_a,buf),0xFFFF, 0, 2);
    screenDrawText(4*10,FONT_HEIGHT*2*8,float2StringSimple(v_b,buf),0xFFFF, 0, 2);
    screenDrawText(4*10,FONT_HEIGHT*2*9,float2StringSimple(v_c,buf),0xFFFF, 0, 2);

//    float2StringComplex((float)v_c,buf);
//    screenDrawText(6*10, FONT_HEIGHT*2*5, buf, 0xFFFF, 0, 2);
#endif
#endif
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);
}

/**
 * Disables all motor action... Hopefully
 */
void disableMotor(void)
{
    //set all epwm pins to be low
    EPWM_forceActionQualifierSWAction(PHASE_A_EPWM_BASE, EPWM_AQ_OUTPUT_A);
    EPWM_forceActionQualifierSWAction(PHASE_A_EPWM_BASE, EPWM_AQ_OUTPUT_B);
    EPWM_forceActionQualifierSWAction(PHASE_B_EPWM_BASE, EPWM_AQ_OUTPUT_A);
    EPWM_forceActionQualifierSWAction(PHASE_B_EPWM_BASE, EPWM_AQ_OUTPUT_B);
    EPWM_forceActionQualifierSWAction(PHASE_C_EPWM_BASE, EPWM_AQ_OUTPUT_A);
    EPWM_forceActionQualifierSWAction(PHASE_C_EPWM_BASE, EPWM_AQ_OUTPUT_B);
    disablePWMPins();
}

