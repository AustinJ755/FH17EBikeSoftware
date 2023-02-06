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

#include "board.h"

//*****************************************************************************
//
// Board Configurations
// Initializes the rest of the modules. 
// Call this function in your application if you wish to do all module 
// initialization.
// If you wish to not use some of the initializations, instead of the 
// Board_init use the individual Module_inits
//
//*****************************************************************************
void Board_init()
{
	EALLOW;

	PinMux_init();
	GPIO_init();
	SCI_init();

	EDIS;
}

//*****************************************************************************
//
// PINMUX Configurations
//
//*****************************************************************************
void PinMux_init()
{
	//
	// PinMux for modules assigned to CPU1
	//
	
	// GPIO11 -> ILI9341_GPIO_CS Pinmux
	GPIO_setPinConfig(GPIO_11_GPIO11);
	// GPIO39 -> ILI9341_GPIO_DCX Pinmux
	GPIO_setPinConfig(GPIO_39_GPIO39);
	//
	// SCIA -> DATA_LOG_UART Pinmux
	//
	GPIO_setPinConfig(GPIO_28_SCIA_RX);
	GPIO_setPadConfig(DATA_LOG_UART_SCIRX_GPIO, GPIO_PIN_TYPE_STD | GPIO_PIN_TYPE_PULLUP);
	GPIO_setQualificationMode(DATA_LOG_UART_SCIRX_GPIO, GPIO_QUAL_ASYNC);

	GPIO_setPinConfig(GPIO_29_SCIA_TX);
	GPIO_setPadConfig(DATA_LOG_UART_SCITX_GPIO, GPIO_PIN_TYPE_STD | GPIO_PIN_TYPE_PULLUP);
	GPIO_setQualificationMode(DATA_LOG_UART_SCITX_GPIO, GPIO_QUAL_ASYNC);


}

//*****************************************************************************
//
// GPIO Configurations
//
//*****************************************************************************
void GPIO_init(){
	ILI9341_GPIO_CS_init();
	ILI9341_GPIO_DCX_init();
}

void ILI9341_GPIO_CS_init(){
	GPIO_writePin(ILI9341_GPIO_CS, 1);
	GPIO_setPadConfig(ILI9341_GPIO_CS, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(ILI9341_GPIO_CS, GPIO_QUAL_SYNC);
	GPIO_setDirectionMode(ILI9341_GPIO_CS, GPIO_DIR_MODE_OUT);
	GPIO_setMasterCore(ILI9341_GPIO_CS, GPIO_CORE_CPU1);
}
void ILI9341_GPIO_DCX_init(){
	GPIO_writePin(ILI9341_GPIO_DCX, 0);
	GPIO_setPadConfig(ILI9341_GPIO_DCX, GPIO_PIN_TYPE_STD);
	GPIO_setQualificationMode(ILI9341_GPIO_DCX, GPIO_QUAL_SYNC);
	GPIO_setDirectionMode(ILI9341_GPIO_DCX, GPIO_DIR_MODE_OUT);
	GPIO_setMasterCore(ILI9341_GPIO_DCX, GPIO_CORE_CPU1);
}

//*****************************************************************************
//
// SCI Configurations
//
//*****************************************************************************
void SCI_init(){
	DATA_LOG_UART_init();
}

void DATA_LOG_UART_init(){
	SCI_clearInterruptStatus(DATA_LOG_UART_BASE, SCI_INT_RXFF | SCI_INT_TXFF | SCI_INT_FE | SCI_INT_OE | SCI_INT_PE | SCI_INT_RXERR | SCI_INT_RXRDY_BRKDT | SCI_INT_TXRDY);
	SCI_clearOverflowStatus(DATA_LOG_UART_BASE);
	SCI_resetTxFIFO(DATA_LOG_UART_BASE);
	SCI_resetRxFIFO(DATA_LOG_UART_BASE);
	SCI_resetChannels(DATA_LOG_UART_BASE);
	SCI_setConfig(DATA_LOG_UART_BASE, DEVICE_LSPCLK_FREQ, DATA_LOG_UART_BAUDRATE, (SCI_CONFIG_WLEN_8|SCI_CONFIG_STOP_ONE|SCI_CONFIG_PAR_NONE));
	SCI_disableLoopback(DATA_LOG_UART_BASE);
	SCI_performSoftwareReset(DATA_LOG_UART_BASE);
	SCI_enableInterrupt(DATA_LOG_UART_BASE, SCI_INT_RXFF);
	SCI_setFIFOInterruptLevel(DATA_LOG_UART_BASE, SCI_FIFO_TX0, SCI_FIFO_RX1);
	SCI_enableFIFO(DATA_LOG_UART_BASE);
	SCI_enableModule(DATA_LOG_UART_BASE);
}

