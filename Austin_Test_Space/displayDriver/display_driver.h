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
#define DISPLAYWIDTH 319
#define DISPLAYHEIGHT 239
void checkDisplayCommandFifo(void);
void displayDMAComplete(void);
int initDisplay(void);
uint16_t drawFilledColorBox(uint16_t x, uint16_t y, uint16_t height, uint16_t width, uint16_t color);
uint16_t screenDrawText(uint16_t x, uint16_t y, char *string,
                        uint16_t text_color, uint16_t background_color,
                        uint16_t font_size);
uint16_t drawOutlineBox(uint16_t x, uint16_t y, uint16_t height,
                        uint16_t width, uint16_t color, uint16_t thickness);
uint16_t drawHLine(uint16_t x, uint16_t y, uint16_t length,uint16_t thickness, uint16_t color);
uint16_t drawVLine(uint16_t x, uint16_t y, uint16_t length,uint16_t thickness, uint16_t color);
uint16_t drawLine(uint16_t startx,uint16_t starty, uint16_t endx, uint16_t endy, uint16_t color,uint16_t thickness);
#endif /* DISPLAYDRIVER_DISPLAY_DRIVER_H_ */
