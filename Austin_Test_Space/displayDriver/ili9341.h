/*
 * ili9341.h
 *
 *  Created on: Jan 23, 2023
 *      Author: aj755
 *
 *      for more help understanding the commands refer to this sheet https://cdn-shop.adafruit.com/datasheets/ILI9341.pdf
 */

#ifndef DISPLAYDRIVER_ILI9341_H_
#define DISPLAYDRIVER_ILI9341_H_

//include
#include "driverlib.h"

//ILI9341 Level 1 Commands
#define ILI9341_CMD_NOP 0x00
#define ILI9341_CMD_SOFTWARE_RESET 0x01
#define ILI9341_CMD_READ_DISPLAY_ID 0x04
#define ILI9341_CMD_READ_DISPLAY_STATUS 0x09
#define ILI9341_CMD_READ_DISPLAY_POWER_MODE 0x0A
#define ILI9341_CMD_READ_DISPLAY_MADCTL 0x0B
#define ILI9341_CMD_READ_DISPLAY_PIXEL_FORMAT 0x0C
#define ILI9341_CMD_READ_DISPLAY_IMAGE_FORMAT 0x0D
#define ILI9341_CMD_READ_DISPLAY_SIGNAL_MODE 0x0E
#define ILI9341_CMD_READ_DISPLAY_SELF_TEST_RESULT 0x0F
#define ILI9341_CMD_ENTER_SLEEP_MODE 0x10
#define ILI9341_CMD_SLEEP_OUT 0x11
#define ILI9341_CMD_PARTIAL_MODE_ON 0x12
#define ILI9341_CMD_NORMAL_DISPLAY_MODE_ON 0x13
#define ILI9341_CMD_DISPLAY_INVERSION_OFF 0x20
#define ILI9341_CMD_DISPLAY_INVERSION_ON 0x21
#define ILI9341_CMD_GAMMA_SET 0x26
#define ILI9341_CMD_DISPLAY_OFF 0x28
#define ILI9341_CMD_DISPLAY_ON 0x29
#define ILI9341_CMD_COLUMN_ADDRESS_SET 0x2A
#define ILI9341_CMD_PAGE_ADDRESS_SET 0x2B
#define ILI9341_CMD_MEMORY_WRITE 0x2C
#define ILI9341_CMD_COLOR_SET 0x2D
#define ILI9341_CMD_MEMORY_READ 0x2E
#define ILI9341_CMD_PARTIAL_AREA 0x30
#define ILI9341_CMD_VERTICAL_SCROLLING_DEF 0x33
#define ILI9341_CMD_TEARING_EFFECT_LINE_OFF 0x34
#define ILI9341_CMD_TEARING_EFFECT_LINE_ON 0x35
#define ILI9341_CMD_MEMORY_ACCESS_CONTROL 0x36
#define ILI9341_CMD_VERTICAL_SCROLLING_START_ADDRESS 0x37
#define ILI9341_CMD_IDLE_MODE_OFF 0x38
#define ILI9341_CMD_IDLE_MODE_ON 0x39
#define ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET 0x3A
#define ILI9341_CMD_WRITE_MEMORY_CONTINUE 0x3C
#define ILI9341_CMD_READ_MEMORY_CONTINUE 0x3E
#define ILI9341_CMD_SET_TEAR_SCANLINE 0x44
#define ILI9341_CMD_GET_SCANLINE 0x45
#define ILI9341_CMD_WRITE_DISPLAY_BRIGHTNESS 0x51
#define ILI9341_CMD_READ_DISPLAY_BRIGHTNESS 0x52
#define ILI9341_CMD_WRITE_CTRL_DISPLAY 0x53
#define ILI9341_CMD_READ_CTRL_DISPLAY 0x54
#define ILI9341_CMD_WR_CONTENT_ADAPTIVE_BRIGHTNESS_CTRL 0x55
#define ILI9341_CMD_RD_CONTENT_ADAPTIVE_BRIGHTNESS_CTRL 0x56
#define ILI9341_CMD_WR_CABC_MIN_BRIGHTNESS 0x5E
#define ILI9341_CMD_RD_CABC_MIN_BRIGHTNESS 0x5F
#define ILI9341_CMD_RD_ID1 0xDA
#define ILI9341_CMD_RD_ID2 0xDB
#define ILI9341_CMD_RD_ID3 0xDC

//ILI9341 Level 2 Commands (These will be added if needed)



#define SPI_CS_GPIO ILI9341_GPIO_CS
#define SPI_DX_GPIO ILI9341_GPIO_DCX
#define CS_LOW                      GPIO_writePin(SPI_CS_GPIO, 0)
#define CS_HIGH                     GPIO_writePin(SPI_CS_GPIO, 1)
#define DCX_COMMAND                 GPIO_writePin(SPI_DX_GPIO, 0)
#define DCX_DATA                    GPIO_writePin(SPI_DX_GPIO, 1)





//We will have two buffers this will allow us to read from the sd card and write to the display


//ILI9341 data_structs and predefined values


//0x04 Read display ID
typedef struct {
    uint8_t dummy_data;
    uint8_t manufacturer_id;
    uint8_t driver_version_id;
    uint8_t driver_id;
} ili9341_display_id_info;


//display inversion
#define ILI9341_DISPLAY_INVERSION_ON 1
#define ILI9341_DISPLAY_INVERSION_OFF 0


//Content Adaptive Brightness Control

#define ILI9341_CABC_OFF 0x00
#define ILI9341_CABC_USER_INTERFACE_IMAGE 0x01
#define ILI9341_CABC_STILL_PICTURE 0x02
#define ILI9341_CABC_MOVING_IMAGE 0x03


//INTERFACE CODES

#define ILI9341_SUCCESS 0
#define ILI9341_ERROR 1
#define ILI9341_ERROR_NOT_INITIALIZED 2

//ILI9341 interface and setup

void init_ili9341(void);

//ILI9341 display commands
int ili9341_NOP(void);

int ili9341_softwareReset(void);

int ili9341_readDisplayIDInfo(ili9341_display_id_info* device_id_info);

int ili9341_readDisplayStatus(uint32_t* display_status);

int ili9341_readDisplayPowerMode(uint8_t* power_mode);

int ili9341_readDisplayMADCTL(uint8_t* madctl);

int ili9341_readDisplayPixelFormat(uint8_t* pixel_format);

int ili9341_readImageFormat(uint8_t* image_format);

int ili9341_readDisplaySignalMode(uint8_t* signal_mode);

int ili9341_readDisplaySelfDiagnosticReport(uint8_t* diagnostic_report);

/**
 * Will make the display sleep
 * CAN'T BE CALLED WITHIN 120MS after a sleep exit
 * @return SPI_STATUS_CODE
 */
int ili9341_enterSleep(void);
/**
 * Will force the display to exit sleep mode
 * MUST WAIT 5MS BEFORE SENDING ANYMORE COMMANDS TO THE DISPLAY
 * CAN'T ENTER SLEEP FOR ANOTHER 120MS
 * @return SPI_STATUS_CODE
 */
int ili9341_exitSleep(void);

int ili9341_partialModeOn(void);

int ili9341_normalDisplayModeOn(void);

int ili9341_setDisplayInversionOn(void);

int ili9341_setDisplayInversionOff(void);

int ili9341_setGammaCurve(uint8_t gamma_curve);

int ili9341_displayOn(void);

int ili9341_displayOff(void);

int ili9341_setColumnAddress(uint16_t start_column, uint16_t end_column);

int ili9341_setRowAddress(uint16_t start_row, uint16_t end_row);

/**
 * This is what actually draws pixels to the display
 * @param pixel_data
 * @return
 */
int ili9341_startWriteFrameMemory(void);//uint16_t* pixel_data, uint16_t data_length);

/**
 *
 * @param color_set must provide 128 bytes. These bytes describe the color mapping.
 * @return
 */
int ili9341_colorSet(uint8_t* color_set);


/**
 * This can read pixels from the display
 * @param storage_location
 * @return
 */
int ili9341_startReadFrameMemory(void);//uint16_t* storage_location , uint16_t data_length);

int ili9341_partialArea(uint16_t start_row, uint16_t end_row);

int ili9341_vertialScrollingDefinition(uint16_t* top_fixed_area, uint16_t* vertical_scrolling_area, uint16_t* bottom_fixed_area);

int ili9341_tearingEffectLineOff(void);

int ili9341_tearingEffectLineOn(void);

int ili9341_memoryAccessControl(uint8_t mem_access_control);

int ili9341_verticalScrollingAreaStartAddress(uint16_t* vertical_scrolling_line_pointer);

int ili9341_IdleModeOff(void);

int ili9341_IdleModeOn(void);

int ili9341_COLMODPixelFormatSet(uint8_t pxl_frmt);

int ili9341_writeMemoryContinue(void);//uint16_t* pixel_data, uint16_t data_length);

int ili9341_readMemoryContinue(void);//uint16_t* storage_location);

int ili9341_writeDisplayBrightness(uint8_t brightness_value);

int ili9341_readDisplayBrightness(uint8_t* brightness_value);

int ili9341_writeCTRLDisplay(uint8_t ctrl_display);

int ili9341_readCTRLDisplay(uint8_t* ctrl_display);

int ili9341_writeCABC(uint8_t cabc);

int ili9341_readCABC(uint8_t* cabc);

int ili9341_writeCABCMinimumBrightness(uint8_t min_brightness);

int ili9341_readCABCMinimumBrightness(uint8_t* min_brightness);

void ili9341_sendPixel(uint16_t pixel);




#endif /* DISPLAYDRIVER_ILI9341_H_ */
