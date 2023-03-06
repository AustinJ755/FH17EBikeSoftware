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
//
// Main
//
char test_string[] = "hello";
uint32_t test = 100;
int32_t test2 = -5;
float test3 = 1.5f;
uint16_t pixels[120];
void main(void)
{

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


    Board_init();
    cos(test3);


    EINT;
    ERTM;

    logString(test_string);
    logSignedInt(&test2);
    logUInt(&test);
    logFloat(&test3);
    logUIntImmediate(test);
    logSignedIntImmediate(test2);

    checkDebugMessageQueue();


    initDisplay();
    drawFilledColorBox(0, 0, 239, 319, 0xD938);
    drawFilledColorBox(0, 0, 239, 319, 0x87f0);
    drawFilledColorBox(0, 0, 239, 319, 0xF800);
    drawFilledColorBox(10, 10, 10,10, 0x87f0);
    drawFilledColorBox(60, 60, 10,10, 0x87f0);
    drawFilledColorBox(100, 150, 10,10, 0x87f0);
    drawFilledColorBox(300, 200, 10,10, 0x87f0);
    checkDisplayCommandFifo();
    screenDrawText(50, 50, "gello\njorld\0", 0xD938, 0x87f0, 2);
    checkDisplayCommandFifo();
    drawOutlineBox(10, 10, 50, 50, 0x87f0, 5);
    checkDisplayCommandFifo();

    for(;;){
    }
//HRCAP
}

//
// End of File
//
