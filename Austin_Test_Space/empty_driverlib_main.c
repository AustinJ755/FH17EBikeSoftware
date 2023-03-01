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



    init_ili9341();
    uint32_t test=0;
    for(test=0;test<800000;test++){

    }
    ili9341_exitSleep();
    for(test=0;test<8000000;test++){

    }
    ili9341_NOP();
    ili9341_readDisplayIDInfo(0);
    uint32_t display_info;
    ili9341_readDisplayStatus(&display_info);
//    uint8_t ctrl_val = 0x24;
//    ili9341_writeCTRLDisplay(ctrl_val);
//    ili9341_readCTRLDisplay(&ctrl_val);
//    uint8_t brightness = 0x50;
//    ili9341_writeDisplayBrightness(0x45);
//    ili9341_readDisplayBrightness(&brightness);
    for(test=0;test<800000;test++){

    }
    //ili9341_memoryAccessControl(0x20);
    ili9341_COLMODPixelFormatSet(0x55);
    ili9341_memoryAccessControl(0x08);
    ili9341_displayOn();
    for(test=0;test<800000;test++){

    }
    drawFilledColorBox(0, 0, 239, 319, 0xD938);
    drawFilledColorBox(0, 0, 239, 319, 0x87f0);
    drawFilledColorBox(0, 0, 239, 319, 0xF800);
    drawFilledColorBox(10, 10, 10,10, 0x87f0);
    drawFilledColorBox(60, 60, 10,10, 0x87f0);
    drawFilledColorBox(100, 150, 10,10, 0x87f0);
    drawFilledColorBox(300, 200, 10,10, 0x87f0);
    checkDisplayCommandFifo();
//    for(test=0;test<120;test++){
//        pixels[test]=0x87f0;
//    }
//    ili9341_writeFrameMemory(pixels, 120);
//    while(!DMAComplete());
//    for(test=0;test<29;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
//
//    for(test=0;test<120;test++){
//        pixels[test]=0xf410;
//    }
//    for(test=0;test<30;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
//
//    for(test=0;test<120;test++){
//        pixels[test]=0xD938;
//    }
//    for(test=0;test<30;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
//
//    for(test=0;test<120;test++){
//        pixels[test]=0xF800;
//    }
//    for(test=0;test<90;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
//    for(test=0;test<120;test++){
//        pixels[test]=0x07E0;
//    }
//    for(test=0;test<90;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
//    for(test=0;test<120;test++){
//        pixels[test]=0x0760;
//    }
//    for(test=0;test<90;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
//    for(test=0;test<120;test++){
//        pixels[test]=0x03E0;
//    }
//    for(test=0;test<90;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
//    for(test=0;test<120;test++){
//        pixels[test]=0x001F;
//    }
//    for(test=0;test<90;test++){
//        ili9341_writeMemoryContinue(pixels, 120);
//        while(0==DMAComplete());
//    }
    for(;;){
    }

}

//
// End of File
//
