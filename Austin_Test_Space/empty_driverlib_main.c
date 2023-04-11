//#############################################################################
//
// FILE:   empty_driverlib_main.c
//
// TITLE:  Empty Project
//
// Empty Project Example
//
// This example is an empty project setup for Driverlib development.
//
//#############################################################################
//
//
// $Copyright:
// Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "math.h"
#include "board.h"
#include "./displayDriver/ili9341.h"
#include "datalogger/datalogger.h"
#include "displayDriver/display_driver.h"
#include "./motorcontrol/motor_controller.h"
//
// Main
//
char test_string[] = "hello";
char new_line[] = "\n\r";
uint32_t test = 100;
int32_t test2 = -5;
float test3 = 1.5f;
float ftest1 = 1.5f;
float ftest2 = -1.5f;
float ftest3 = 8.45555e10f;
float ftest4 = 9.5f;
float ftest5 = 1.499999f;
float ftest6 = 1.5111111f;
uint16_t pixels[120];
void main(void)
{

    err_id[0] = 0.0;
    err_id[1] = 0.0;
    err_iq[0] = 0.0;
    err_iq[1] = 0.0;

    yi_id[0] = 0.0;
    yi_id[1] = 0.0;
    yi_iq[0] = 0.0;
    yi_iq[1] = 0.0;

    ki_times_Tsby2 = ki*SampleTime/2.0;
    //
    // Initialize device clock and peripherals
    //

    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    EALLOW;

    PinMux_init();
    SYSCTL_init();
    INPUTXBAR_init();
    SYNC_init();
    ASYSCTL_init();
    ADC_init();
    AIO_init();
    CPUTIMER_init();
    DMA_init();
    ECAP_init();
    GPIO_init();
    SCI_init();
    SPI_init();
    XINT_init();
    INTERRUPT_init();

    EDIS;
    EINT;
    ERTM;
    //EPWM_init();
    initDisplay();
    initMotor();

    cosf(test3);


    const uint16_t text[6] = { 'H', 'E', 'L', 'L', 'O', '\0' };
    SCI_writeCharArray(SCIA_BASE, text, 5);
    logString(test_string);
    logSignedInt(&test2);
    logUInt(&test);
    logString(new_line);
    logFloat(&test3);
    logString(new_line);
    logFloat(&ftest1);
    logString(new_line);
    logFloat(&ftest2);
    logString(new_line);
    logFloat(&ftest3);
    logString(new_line);
    logFloat(&ftest4);
    logString(new_line);
    logFloat(&ftest5);
    logString(new_line);
    logFloat(&ftest6);
    logString(new_line);
    logUIntImmediate(test);
    logSignedIntImmediate(test2);

    checkDebugMessageQueue();
    logString("Completed Initialization");
//    drawFilledColorBox(0, 0, 239, 319, 0xD938);
//    drawFilledColorBox(0, 0, 239, 319, 0x87f0);
//    drawFilledColorBox(0, 0, 239, 319, 0xF800);
//    drawFilledColorBox(10, 10, 10, 10, 0x87f0);
//    drawFilledColorBox(60, 60, 10, 10, 0x87f0);
//    drawFilledColorBox(100, 150, 10, 10, 0x87f0);
//    drawFilledColorBox(300, 200, 10, 10, 0x87f0);
//    checkDisplayCommandFifo();
//    screenDrawText(50, 50, "hello\nworld\0", 0xD938, 0x87f0, 2);
//    checkDisplayCommandFifo();
//    drawOutlineBox(10, 10, 50, 50, 0x87f0, 5);
//    checkDisplayCommandFifo();
//    drawLine(10, 0, 50, 50, 0xD938, 5);
//    checkDisplayCommandFifo();
//    drawLine(0, 0, 30, 50, 0xD938, 2);
//    checkDisplayCommandFifo();
//    drawLine(0, 100, 50, 125, 0xD938, 3);
//    checkDisplayCommandFifo();
    for (;;)
    {
        checkDebugMessageQueue();
        checkDisplayCommandFifo();
    }
//HRCAP
}

//
// End of File
//
