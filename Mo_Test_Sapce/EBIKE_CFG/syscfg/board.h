/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef BOARD_H
#define BOARD_H

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//
// Included Files
//

#include "driverlib.h"
#include "device.h"

//*****************************************************************************
//
// PinMux Configurations
//
//*****************************************************************************
//
// GPIO11 - GPIO Settings
//
#define ILI9341_GPIO_CS_GPIO_PIN_CONFIG GPIO_11_GPIO11
//
// GPIO39 - GPIO Settings
//
#define ILI9341_GPIO_DCX_GPIO_PIN_CONFIG GPIO_39_GPIO39

//
// SCIA -> DATA_LOG_UART Pinmux
//
//
// SCIA_RX - GPIO Settings
//
#define GPIO_PIN_SCIA_RX 28
#define DATA_LOG_UART_SCIRX_GPIO 28
#define DATA_LOG_UART_SCIRX_PIN_CONFIG GPIO_28_SCIA_RX
//
// SCIA_TX - GPIO Settings
//
#define GPIO_PIN_SCIA_TX 29
#define DATA_LOG_UART_SCITX_GPIO 29
#define DATA_LOG_UART_SCITX_PIN_CONFIG GPIO_29_SCIA_TX

//*****************************************************************************
//
// GPIO Configurations
//
//*****************************************************************************
#define ILI9341_GPIO_CS 11
void ILI9341_GPIO_CS_init();
#define ILI9341_GPIO_DCX 39
void ILI9341_GPIO_DCX_init();

//*****************************************************************************
//
// SCI Configurations
//
//*****************************************************************************
#define DATA_LOG_UART_BASE SCIA_BASE
#define DATA_LOG_UART_BAUDRATE 115200
#define DATA_LOG_UART_CONFIG_WLEN SCI_CONFIG_WLEN_8
#define DATA_LOG_UART_CONFIG_STOP SCI_CONFIG_STOP_ONE
#define DATA_LOG_UART_CONFIG_PAR SCI_CONFIG_PAR_NONE
#define DATA_LOG_UART_FIFO_TX_LVL SCI_FIFO_TX0
#define DATA_LOG_UART_FIFO_RX_LVL SCI_FIFO_RX1
void DATA_LOG_UART_init();

//*****************************************************************************
//
// Board Configurations
//
//*****************************************************************************
void	Board_init();
void	GPIO_init();
void	SCI_init();
void	PinMux_init();


//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif  // end of BOARD_H definition
