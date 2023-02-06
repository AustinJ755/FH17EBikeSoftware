/*
 * display_driver.h
 *
 *  Created on: Jan 20, 2023
 *      Author: aj755
 */

#ifndef DISPLAYDRIVER_DISPLAY_DRIVER_H_
#define DISPLAYDRIVER_DISPLAY_DRIVER_H_

#include <stdint.h>
//Use the default SPI line for SPI_A



#define DISPLAYINTIALIZED 0

void checkDisplayCommandFifo(void);
void displayDMAComplete(void);
int initDisplay(void);
uint16_t drawFilledColorBox(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint16_t color);


#endif /* DISPLAYDRIVER_DISPLAY_DRIVER_H_ */
