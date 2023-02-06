/*
 * dma_spi.h
 *
 *  Created on: Feb 5, 2023
 *      Author: aj755
 */

#ifndef DISPLAYDRIVER_SPI_DMA_H_
#define DISPLAYDRIVER_SPI_DMA_H_
#include <stdint.h>
//SPI DMA RAM
#define SPIBUFFERSIZE 80*80 //gcd of 240,320

extern uint16_t SPI_DMA_BUFFER_1[SPIBUFFERSIZE];

extern uint16_t SPI_DMA_BUFFER_2[SPIBUFFERSIZE];


typedef struct{
    uint16_t* data_address;
    uint16_t  burst_size;
    uint16_t  transaction_count;
    uint16_t  burst_step_size;
    uint16_t  transaction_step_size;
    uint16_t  transaction_wrap;
    uint16_t  transaction_warp_step;
}dma_transfer;

void dmaResetIli(void);

void startIli9341DMATransaction(dma_transfer transfer);


#endif /* DISPLAYDRIVER_SPI_DMA_H_ */
