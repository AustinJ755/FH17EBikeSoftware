/*
 * ili9341.c
 *
 *  Created on: Jan 23, 2023
 *      Author: aj755
 */
#include "ili9341.h"
#include "device.h"
#include "spi.h"
#include "board.h"
uint8_t initialized = 0;






//void *tx_loc =(void*)SPI_DMA_BUFFER_1;


#define checkInit if(initialized==0){return ILI9341_ERROR_NOT_INITIALIZED;}

#define REVERSE_HOLD32              (((uint32_t)hold[0])<<16)|(((uint32_t)hold[1])<<8)|hold[2]|(hold[3]>>8)

uint16_t hold[4];
void init_ili9341(void)
{
    SPI_disableFIFO(Ili9341_SPI_BASE);
    initialized = 1;
}

void sendReceiveBytes(uint8_t initial_command, uint16_t *received_data,
                      uint8_t receive_data_length)
{
    //We use a 9 bit length because the display request 1 extra cycle
    CS_LOW;
    DCX_COMMAND;
    SPI_pollingNonFIFOTransaction(Ili9341_SPI_BASE, 9U, initial_command << 1);
    DCX_DATA;
    SPI_receiveNBytes(Ili9341_SPI_BASE, received_data, receive_data_length, 0);
    CS_HIGH;
}

void sendReceiveByte(uint8_t initial_command, uint16_t *received_data)
{
    CS_LOW;
    DCX_COMMAND;
    SPI_transmitByte(Ili9341_SPI_BASE, initial_command);
    DCX_DATA;
    *received_data = SPI_receiveByte(Ili9341_SPI_BASE, 0x00);
    CS_HIGH;
}

void sendMultibyteData(uint8_t initial_command, uint16_t *data_packet, uint8_t datalength)
{
    CS_LOW;
    DCX_COMMAND;
    SPI_transmitByte(Ili9341_SPI_BASE, initial_command);
    DCX_DATA;
    SPI_transmitNBytes(Ili9341_SPI_BASE, data_packet, datalength, 0);
    CS_HIGH;
}
void sendByte(uint8_t command)
{
    CS_LOW;
    DCX_COMMAND;
    SPI_transmitByte(Ili9341_SPI_BASE, command);
    CS_HIGH;
}
void ili9341_sendPixel(uint16_t pixel){
    CS_LOW;
    DCX_DATA;
    SPI_pollingNonFIFOTransaction(Ili9341_SPI_BASE, 16, pixel);
    CS_HIGH;
}
/**
 * This is what actually draws pixels to the display
 * @param pixel_data
 * @return
 */
int ili9341_startWriteFrameMemory(){//uint16_t* pixel_data, uint16_t data_length){
    checkInit;
    CS_LOW;
    DCX_COMMAND;
    SPI_transmitByte(Ili9341_SPI_BASE, ILI9341_CMD_MEMORY_WRITE);
    //sendDataFromDMA(0, pixel_data, data_length);
//    DCX_DATA;
//    SPI_pollingFIFOTransaction(Ili9341_SPI_BASE, 16U,  pixel_data, NULL, data_length, 0);
//    CS_HIGH;
    return ILI9341_SUCCESS;
}


/**
 * This can read pixels from the display
 * @param storage_location
 * @return
 */
int ili9341_startReadFrameMemory(void){//uint16_t* storage_location, uint16_t data_length){
    return ILI9341_ERROR;
}

int ili9341_writeMemoryContinue(){//uint16_t* pixel_data, uint16_t data_length){
    checkInit;
    CS_LOW;
    DCX_COMMAND;
    SPI_transmitByte(Ili9341_SPI_BASE, ILI9341_CMD_WRITE_MEMORY_CONTINUE);
    //sendDataFromDMA(0, pixel_data, data_length);
//    DCX_DATA;
//    SPI_pollingFIFOTransaction(Ili9341_SPI_BASE, 16U,  pixel_data, NULL, data_length, 0);
//    CS_HIGH;
    return ILI9341_SUCCESS;
}

int ili9341_readMemoryContinue(void){//uint16_t* storage_location){
    checkInit;
    return ILI9341_ERROR;
}












int ili9341_NOP(void)
{
    checkInit;
    sendByte(ILI9341_CMD_NOP);
    return ILI9341_SUCCESS;
}

int ili9341_softwareReset(void)
{
    checkInit;
    sendByte(ILI9341_CMD_SOFTWARE_RESET);
    return ILI9341_SUCCESS;
}

int ili9341_readDisplayIDInfo(ili9341_display_id_info *device_id_info)
{
    checkInit;

    sendReceiveBytes(ILI9341_CMD_READ_DISPLAY_ID, hold, 3);
    return ILI9341_SUCCESS;
}

int ili9341_readDisplayStatus(uint32_t *display_status)
{
    checkInit;
    sendReceiveBytes(ILI9341_CMD_READ_DISPLAY_STATUS, hold, 4);
    *display_status = REVERSE_HOLD32;
    return ILI9341_SUCCESS;
}

int ili9341_readDisplayPowerMode(uint8_t *power_mode)
{
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_DISPLAY_POWER_MODE, hold);
    *power_mode = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

int ili9341_readDisplayMADCTL(uint8_t *madctl)
{
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_DISPLAY_MADCTL, hold);
    *madctl = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

int ili9341_readDisplayPixelFormat(uint8_t *pixel_format)
{
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_DISPLAY_PIXEL_FORMAT, hold);
    *pixel_format = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

int ili9341_readImageFormat(uint8_t *image_format)
{
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_DISPLAY_IMAGE_FORMAT, hold);
    *image_format = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

int ili9341_readDisplaySignalMode(uint8_t *signal_mode)
{
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_DISPLAY_SIGNAL_MODE, hold);
    *signal_mode = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

int ili9341_readDisplaySelfDiagnosticReport(uint8_t *diagnostic_report)
{
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_DISPLAY_SELF_TEST_RESULT, hold);
    *diagnostic_report = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

/**
 * Will make the display sleep
 * CAN'T BE CALLED WITHIN 120MS after a sleep exit
 * @return SPI_STATUS_CODE
 */
int ili9341_enterSleep(void)
{
    checkInit;
    sendByte(ILI9341_CMD_ENTER_SLEEP_MODE);
    return ILI9341_SUCCESS;
}

/**
 * Will force the display to exit sleep mode
 * MUST WAIT 5MS BEFORE SENDING ANYMORE COMMANDS TO THE DISPLAY
 * CAN'T ENTER SLEEP FOR ANOTHER 120MS
 * @return SPI_STATUS_CODE
 */
int ili9341_exitSleep(void)
{
    checkInit;
    sendByte(ILI9341_CMD_SLEEP_OUT);
    return ILI9341_SUCCESS;
}

int ili9341_partialModeOn(void)
{
    checkInit;
    sendByte(ILI9341_CMD_PARTIAL_MODE_ON);
    return ILI9341_SUCCESS;
}

int ili9341_normalDisplayModeOn(void)
{
    checkInit;
    sendByte(ILI9341_CMD_NORMAL_DISPLAY_MODE_ON);
    return ILI9341_SUCCESS;
}

int ili9341_setDisplayInversionOn(void)
{
    checkInit;
    sendByte(ILI9341_CMD_DISPLAY_INVERSION_ON);
    return ILI9341_SUCCESS;
}

int ili9341_setDisplayInversionOff(void)
{
    checkInit;
    sendByte(ILI9341_CMD_DISPLAY_INVERSION_OFF);
    return ILI9341_SUCCESS;
}

int ili9341_setGammaCurve(uint8_t gamma_curve)
{
    checkInit;
    hold[0]=gamma_curve;
    sendMultibyteData(ILI9341_CMD_GAMMA_SET, hold,1);
    return ILI9341_ERROR;
}

int ili9341_displayOn(void)
{
    checkInit;
    sendByte(ILI9341_CMD_DISPLAY_ON);
    return ILI9341_SUCCESS;
}

int ili9341_displayOff(void)
{
    checkInit;
    sendByte(ILI9341_CMD_DISPLAY_OFF);
    return ILI9341_SUCCESS;
}

int ili9341_setColumnAddress(uint16_t start_column, uint16_t end_column)
{
    checkInit;
    hold[0]=start_column>>8;
    hold[1]=start_column;
    hold[2]=end_column>>8;
    hold[3]=end_column;
    sendMultibyteData(ILI9341_CMD_COLUMN_ADDRESS_SET, hold, 4);
    return ILI9341_SUCCESS;
}

int ili9341_setRowAddress(uint16_t start_row, uint16_t end_row)
{
    checkInit;
    hold[0]=start_row>>8;
    hold[1]=start_row;
    hold[2]=end_row>>8;
    hold[3]=end_row;
    sendMultibyteData(ILI9341_CMD_PAGE_ADDRESS_SET, hold, 4);
    return ILI9341_SUCCESS;
}

/**
 *
 * @param color_set must provide 128 bytes. These bytes describe the color mapping.
 * @return
 */
int ili9341_colorSet(uint8_t* color_set){
    return ILI9341_ERROR;
}

int ili9341_partialArea(uint16_t start_row, uint16_t end_row){
    checkInit;
    hold[0]=start_row>>8;
    hold[1]=start_row;
    hold[2]=end_row>>8;
    hold[3]=end_row;
    sendMultibyteData(ILI9341_CMD_PARTIAL_AREA, hold, 4);
    return ILI9341_SUCCESS;
}

int ili9341_vertialScrollingDefinition(uint16_t* top_fixed_area, uint16_t* vertical_scrolling_area, uint16_t* bottom_fixed_area){
    return ILI9341_ERROR;
}

int ili9341_tearingEffectLineOff(void){
    checkInit;
    sendByte(ILI9341_CMD_TEARING_EFFECT_LINE_OFF);
    return ILI9341_SUCCESS;
}

int ili9341_tearingEffectLineOn(void){
    checkInit;
    sendByte(ILI9341_CMD_TEARING_EFFECT_LINE_ON);
    return ILI9341_SUCCESS;
}

int ili9341_memoryAccessControl(uint8_t mem_access_control){
    checkInit;
    hold[0]=mem_access_control;
    sendMultibyteData(ILI9341_CMD_MEMORY_ACCESS_CONTROL, hold,1);
    return ILI9341_SUCCESS;
}

int ili9341_verticalScrollingAreaStartAddress(uint16_t* vertical_scrolling_line_pointer){
    checkInit;
    return ILI9341_ERROR;
}

int ili9341_IdleModeOff(void){
    checkInit;
    sendByte(ILI9341_CMD_IDLE_MODE_OFF);
    return ILI9341_SUCCESS;
}

int ili9341_IdleModeOn(void){
    checkInit;
    sendByte(ILI9341_CMD_IDLE_MODE_ON);
    return ILI9341_SUCCESS;
}

int ili9341_COLMODPixelFormatSet(uint8_t pxl_frmt){
    checkInit;
    hold[0]=pxl_frmt;
    sendMultibyteData(ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET, hold,1);
    return ILI9341_SUCCESS;
}



int ili9341_writeDisplayBrightness(uint8_t brightness_value){
    checkInit;
    hold[0]=brightness_value;
    sendMultibyteData(ILI9341_CMD_WRITE_DISPLAY_BRIGHTNESS, hold,1);
    return ILI9341_SUCCESS;
}

int ili9341_readDisplayBrightness(uint8_t* brightness_value){
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_DISPLAY_BRIGHTNESS, hold);
    *brightness_value = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

int ili9341_writeCTRLDisplay(uint8_t ctrl_display){
    checkInit;
    hold[0]=ctrl_display;
    sendMultibyteData(ILI9341_CMD_WRITE_CTRL_DISPLAY, hold,1);
    return ILI9341_SUCCESS;
}

int ili9341_readCTRLDisplay(uint8_t* ctrl_display){
    checkInit;
    sendReceiveByte(ILI9341_CMD_READ_CTRL_DISPLAY, hold);
    *ctrl_display = hold[0] >> 8;
    return ILI9341_SUCCESS;
}

int ili9341_writeCABC(uint8_t cabc){
    checkInit;
    return ILI9341_ERROR;
}

int ili9341_readCABC(uint8_t* cabc){
    checkInit;
    return ILI9341_ERROR;
}

int ili9341_writeCABCMinimumBrightness(uint8_t min_brightness){
    checkInit;
    return ILI9341_ERROR;
}

int ili9341_readCABCMinimumBrightness(uint8_t* min_brightness){
    checkInit;
    return ILI9341_ERROR;
}

