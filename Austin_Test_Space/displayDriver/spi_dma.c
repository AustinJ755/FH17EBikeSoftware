/*
 * spi_dma.c
 *
 *  Created on: Jan 30, 2023
 *      Author: aj755
 */
#include "spi_dma.h"
#include "board.h"
#include "ili9341.h"
#include "display_driver.h"

//Array for DMA
uint16_t SPI_DMA_BUFFER_1[SPIBUFFERSIZE];

uint16_t SPI_DMA_BUFFER_2[SPIBUFFERSIZE];

//We will have two buffers this will allow us to read from the sd card and write to the display
uint16_t transferComplete=1;
#pragma DATA_SECTION(SPI_DMA_BUFFER_1, "ramgs0");
#pragma DATA_SECTION(SPI_DMA_BUFFER_2, "ramgs1");

const void *ili9341_dma_src_address = (const void *) SPI_DMA_BUFFER_1;

void startIli9341DMATransaction(dma_transfer transfer){
    SPI_setcharLength(Ili9341_SPI_BASE,16);
    SPI_enableFIFO(Ili9341_SPI_BASE);
    SPI_setTxFifoTransmitDelay(Ili9341_SPI_BASE,0);
    CS_LOW;
    DCX_DATA;
    transferComplete = 0;
    DMA_configAddresses(Ili9341_SPI_TX_DMA_BASE, Ili9341_SPI_TX_DMA_ADDRESS, ili9341_dma_src_address);
    DMA_configBurst(Ili9341_SPI_TX_DMA_BASE, transfer.burst_size, transfer.burst_step_size, 0);
    DMA_configTransfer(Ili9341_SPI_TX_DMA_BASE, transfer.transaction_count, transfer.transaction_step_size, 0);
    DMA_configWrap(Ili9341_SPI_TX_DMA_BASE, transfer.transaction_wrap, 0, 65535U, 0);
    DMA_startChannel(Ili9341_SPI_TX_DMA_BASE);
}


__interrupt void INT_Ili9341_SPI_TX_DMA_ISR(void){
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP7);
    while(SPI_getTxFIFOStatus(Ili9341_SPI_BASE)!=SPI_FIFO_TX0||SPI_isBusy(Ili9341_SPI_BASE)){
    }
    //Channel auto stops I believe so I don't think we need this
    //DMA_stopChannel(Ili9341_SPI_TX_DMA_BASE);
    CS_HIGH;
    SPI_disableFIFO(Ili9341_SPI_BASE);
    displayDMAComplete();
//    if(ili9341_current_transaction.transaction_complete_listener!=0){
//        //execute the next transaction block
//        ili9341_current_transaction.transaction_complete_listener();
//    }
//    return;
}
