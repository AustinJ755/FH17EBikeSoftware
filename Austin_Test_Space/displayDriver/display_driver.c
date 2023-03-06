/*
 * display_driver.c
 *
 *  Created on: Jan 20, 2023
 *      Author: aj755
 */
#include "display_driver.h"
#include "spi_dma.h"
#include "ili9341.h"
/**
 * Call to initialize the display module setups everything that you need must be called first
 * @return
 */
int initDisplay(void)
{
    //TODO actually confirm that the display initializes correctly
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
    ili9341_COLMODPixelFormatSet(0x55);
    ili9341_memoryAccessControl(0x08);
    ili9341_displayOn();
    for(test=0;test<800000;test++){

    }
    return 0;
}

typedef enum
{
    NOCOMMAND,
    SETCOLADDRESS,
    SETROWADDRESS,
    STARTFRAMEWRITE,
    CONTINUEFRAMEWRITE,
    STARTFRAMEREAD,
    CONTINUEFRAMEREAD,
    DRAWCFILLEDCOLORBOX,
    DRAWFILLEDCOLORBOX2,
    DRAWCHAR
} display_function;

typedef struct
{
    display_function function;
    uint16_t params[8];
} display_command;

#define FONT_X 5
#define FONT_WIDTH (FONT_X+1)
#define FONT_Y 8
#define FONT_HEIGHT FONT_Y

// standard ascii 5x8 font
// originally from glcdfont.c from Adafruit project
// standard ascii 5x7 font
// originally from glcdfont.c from Adafruit project
static const uint8_t Font[] = {
  0x00, 0x00, 0x00, 0x00, 0x00,
  0x3E, 0x5B, 0x4F, 0x5B, 0x3E,
  0x3E, 0x6B, 0x4F, 0x6B, 0x3E,
  0x1C, 0x3E, 0x7C, 0x3E, 0x1C,
  0x18, 0x3C, 0x7E, 0x3C, 0x18,
  0x1C, 0x57, 0x7D, 0x57, 0x1C,
  0x1C, 0x5E, 0x7F, 0x5E, 0x1C,
  0x00, 0x18, 0x3C, 0x18, 0x00,
  0xFF, 0xE7, 0xC3, 0xE7, 0xFF,
  0x00, 0x18, 0x24, 0x18, 0x00,
  0xFF, 0xE7, 0xDB, 0xE7, 0xFF,
  0x30, 0x48, 0x3A, 0x06, 0x0E,
  0x26, 0x29, 0x79, 0x29, 0x26,
  0x40, 0x7F, 0x05, 0x05, 0x07,
  0x40, 0x7F, 0x05, 0x25, 0x3F,
  0x5A, 0x3C, 0xE7, 0x3C, 0x5A,
  0x7F, 0x3E, 0x1C, 0x1C, 0x08,
  0x08, 0x1C, 0x1C, 0x3E, 0x7F,
  0x14, 0x22, 0x7F, 0x22, 0x14,
  0x5F, 0x5F, 0x00, 0x5F, 0x5F,
  0x06, 0x09, 0x7F, 0x01, 0x7F,
  0x00, 0x66, 0x89, 0x95, 0x6A,
  0x60, 0x60, 0x60, 0x60, 0x60,
  0x94, 0xA2, 0xFF, 0xA2, 0x94,
  0x08, 0x04, 0x7E, 0x04, 0x08,
  0x10, 0x20, 0x7E, 0x20, 0x10,
  0x08, 0x08, 0x2A, 0x1C, 0x08,
  0x08, 0x1C, 0x2A, 0x08, 0x08,
  0x1E, 0x10, 0x10, 0x10, 0x10,
  0x0C, 0x1E, 0x0C, 0x1E, 0x0C,
  0x30, 0x38, 0x3E, 0x38, 0x30,
  0x06, 0x0E, 0x3E, 0x0E, 0x06,
  0x00, 0x00, 0x00, 0x00, 0x00, //SPACE
  0x00, 0x00, 0x5F, 0x00, 0x00,
  0x00, 0x07, 0x00, 0x07, 0x00,
  0x14, 0x7F, 0x14, 0x7F, 0x14,
  0x24, 0x2A, 0x7F, 0x2A, 0x12,
  0x23, 0x13, 0x08, 0x64, 0x62,
  0x36, 0x49, 0x56, 0x20, 0x50,
  0x00, 0x08, 0x07, 0x03, 0x00,
  0x00, 0x1C, 0x22, 0x41, 0x00,
  0x00, 0x41, 0x22, 0x1C, 0x00,
  0x2A, 0x1C, 0x7F, 0x1C, 0x2A,
  0x08, 0x08, 0x3E, 0x08, 0x08,
  0x00, 0x80, 0x70, 0x30, 0x00,
  0x08, 0x08, 0x08, 0x08, 0x08,
  0x00, 0x00, 0x60, 0x60, 0x00,
  0x20, 0x10, 0x08, 0x04, 0x02,
  0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
  0x00, 0x42, 0x7F, 0x40, 0x00, // 1
  0x72, 0x49, 0x49, 0x49, 0x46, // 2
  0x21, 0x41, 0x49, 0x4D, 0x33, // 3
  0x18, 0x14, 0x12, 0x7F, 0x10, // 4
  0x27, 0x45, 0x45, 0x45, 0x39, // 5
  0x3C, 0x4A, 0x49, 0x49, 0x31, // 6
  0x41, 0x21, 0x11, 0x09, 0x07, // 7
  0x36, 0x49, 0x49, 0x49, 0x36, // 8
  0x46, 0x49, 0x49, 0x29, 0x1E, // 9
  0x00, 0x00, 0x14, 0x00, 0x00,
  0x00, 0x40, 0x34, 0x00, 0x00,
  0x00, 0x08, 0x14, 0x22, 0x41,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x00, 0x41, 0x22, 0x14, 0x08,
  0x02, 0x01, 0x59, 0x09, 0x06,
  0x3E, 0x41, 0x5D, 0x59, 0x4E,
  0x7C, 0x12, 0x11, 0x12, 0x7C, // A
  0x7F, 0x49, 0x49, 0x49, 0x36, // B
  0x3E, 0x41, 0x41, 0x41, 0x22, // C
  0x7F, 0x41, 0x41, 0x41, 0x3E, // D
  0x7F, 0x49, 0x49, 0x49, 0x41, // E
  0x7F, 0x09, 0x09, 0x09, 0x01, // F
  0x3E, 0x41, 0x41, 0x51, 0x73, // G
  0x7F, 0x08, 0x08, 0x08, 0x7F, // H
  0x00, 0x41, 0x7F, 0x41, 0x00, // I
  0x20, 0x40, 0x41, 0x3F, 0x01, // J
  0x7F, 0x08, 0x14, 0x22, 0x41, // K
  0x7F, 0x40, 0x40, 0x40, 0x40, // L
  0x7F, 0x02, 0x1C, 0x02, 0x7F, // M
  0x7F, 0x04, 0x08, 0x10, 0x7F, // N
  0x3E, 0x41, 0x41, 0x41, 0x3E, // O
  0x7F, 0x09, 0x09, 0x09, 0x06, // P
  0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
  0x7F, 0x09, 0x19, 0x29, 0x46, // R
  0x26, 0x49, 0x49, 0x49, 0x32, // S
  0x03, 0x01, 0x7F, 0x01, 0x03, // T
  0x3F, 0x40, 0x40, 0x40, 0x3F, // U
  0x1F, 0x20, 0x40, 0x20, 0x1F, // V
  0x3F, 0x40, 0x38, 0x40, 0x3F, // W
  0x63, 0x14, 0x08, 0x14, 0x63, // X
  0x03, 0x04, 0x78, 0x04, 0x03, // Y
  0x61, 0x59, 0x49, 0x4D, 0x43, // Z
  0x00, 0x7F, 0x41, 0x41, 0x41,
  0x02, 0x04, 0x08, 0x10, 0x20,
  0x00, 0x41, 0x41, 0x41, 0x7F,
  0x04, 0x02, 0x01, 0x02, 0x04,
  0x40, 0x40, 0x40, 0x40, 0x40,
  0x00, 0x03, 0x07, 0x08, 0x00,
  0x20, 0x54, 0x54, 0x78, 0x40, // a
  0x7F, 0x28, 0x44, 0x44, 0x38, // b
  0x38, 0x44, 0x44, 0x44, 0x28, // c
  0x38, 0x44, 0x44, 0x28, 0x7F, // d
  0x38, 0x54, 0x54, 0x54, 0x18, // e
  0x00, 0x08, 0x7E, 0x09, 0x02, // f
  0x18, 0xA4, 0xA4, 0x9C, 0x78, // g
  0x7F, 0x08, 0x04, 0x04, 0x78, // h
  0x00, 0x44, 0x7D, 0x40, 0x00, // i
  0x20, 0x40, 0x40, 0x3D, 0x00, // j
  0x7F, 0x10, 0x28, 0x44, 0x00, // k
  0x00, 0x41, 0x7F, 0x40, 0x00, // l
  0x7C, 0x04, 0x78, 0x04, 0x78, // m
  0x7C, 0x08, 0x04, 0x04, 0x78, // n
  0x38, 0x44, 0x44, 0x44, 0x38, // o
  0xFC, 0x18, 0x24, 0x24, 0x18, // p
  0x18, 0x24, 0x24, 0x18, 0xFC, // q
  0x7C, 0x08, 0x04, 0x04, 0x08, // r
  0x48, 0x54, 0x54, 0x54, 0x24, // s
  0x04, 0x04, 0x3F, 0x44, 0x24, // t
  0x3C, 0x40, 0x40, 0x20, 0x7C, // u
  0x1C, 0x20, 0x40, 0x20, 0x1C, // v
  0x3C, 0x40, 0x30, 0x40, 0x3C, // w
  0x44, 0x28, 0x10, 0x28, 0x44, // x
  0x4C, 0x90, 0x90, 0x90, 0x7C, // y
  0x44, 0x64, 0x54, 0x4C, 0x44, // z
  0x00, 0x08, 0x36, 0x41, 0x00, // {
  0x00, 0x00, 0x77, 0x00, 0x00, // |
  0x00, 0x41, 0x36, 0x08, 0x00, // }
  0x02, 0x01, 0x02, 0x04, 0x02,
  0x3C, 0x26, 0x23, 0x26, 0x3C,
  0x1E, 0xA1, 0xA1, 0x61, 0x12,
  0x3A, 0x40, 0x40, 0x20, 0x7A,
  0x38, 0x54, 0x54, 0x55, 0x59,
  0x21, 0x55, 0x55, 0x79, 0x41,
  0x21, 0x54, 0x54, 0x78, 0x41,
  0x21, 0x55, 0x54, 0x78, 0x40,
  0x20, 0x54, 0x55, 0x79, 0x40,
  0x0C, 0x1E, 0x52, 0x72, 0x12,
  0x39, 0x55, 0x55, 0x55, 0x59,
  0x39, 0x54, 0x54, 0x54, 0x59,
  0x39, 0x55, 0x54, 0x54, 0x58,
  0x00, 0x00, 0x45, 0x7C, 0x41,
  0x00, 0x02, 0x45, 0x7D, 0x42,
  0x00, 0x01, 0x45, 0x7C, 0x40,
  0xF0, 0x29, 0x24, 0x29, 0xF0,
  0xF0, 0x28, 0x25, 0x28, 0xF0,
  0x7C, 0x54, 0x55, 0x45, 0x00,
  0x20, 0x54, 0x54, 0x7C, 0x54,
  0x7C, 0x0A, 0x09, 0x7F, 0x49,
  0x32, 0x49, 0x49, 0x49, 0x32,
  0x32, 0x48, 0x48, 0x48, 0x32,
  0x32, 0x4A, 0x48, 0x48, 0x30,
  0x3A, 0x41, 0x41, 0x21, 0x7A,
  0x3A, 0x42, 0x40, 0x20, 0x78,
  0x00, 0x9D, 0xA0, 0xA0, 0x7D,
  0x39, 0x44, 0x44, 0x44, 0x39,
  0x3D, 0x40, 0x40, 0x40, 0x3D,
  0x3C, 0x24, 0xFF, 0x24, 0x24,
  0x48, 0x7E, 0x49, 0x43, 0x66,
  0x2B, 0x2F, 0xFC, 0x2F, 0x2B,
  0xFF, 0x09, 0x29, 0xF6, 0x20,
  0xC0, 0x88, 0x7E, 0x09, 0x03,
  0x20, 0x54, 0x54, 0x79, 0x41,
  0x00, 0x00, 0x44, 0x7D, 0x41,
  0x30, 0x48, 0x48, 0x4A, 0x32,
  0x38, 0x40, 0x40, 0x22, 0x7A,
  0x00, 0x7A, 0x0A, 0x0A, 0x72,
  0x7D, 0x0D, 0x19, 0x31, 0x7D,
  0x26, 0x29, 0x29, 0x2F, 0x28,
  0x26, 0x29, 0x29, 0x29, 0x26,
  0x30, 0x48, 0x4D, 0x40, 0x20,
  0x38, 0x08, 0x08, 0x08, 0x08,
  0x08, 0x08, 0x08, 0x08, 0x38,
  0x2F, 0x10, 0xC8, 0xAC, 0xBA,
  0x2F, 0x10, 0x28, 0x34, 0xFA,
  0x00, 0x00, 0x7B, 0x00, 0x00,
  0x08, 0x14, 0x2A, 0x14, 0x22,
  0x22, 0x14, 0x2A, 0x14, 0x08,
  0xAA, 0x00, 0x55, 0x00, 0xAA,
  0xAA, 0x55, 0xAA, 0x55, 0xAA,
  0x00, 0x00, 0x00, 0xFF, 0x00,
  0x10, 0x10, 0x10, 0xFF, 0x00,
  0x14, 0x14, 0x14, 0xFF, 0x00,
  0x10, 0x10, 0xFF, 0x00, 0xFF,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x14, 0x14, 0x14, 0xFC, 0x00,
  0x14, 0x14, 0xF7, 0x00, 0xFF,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x14, 0x14, 0xF4, 0x04, 0xFC,
  0x14, 0x14, 0x17, 0x10, 0x1F,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0x1F, 0x00,
  0x10, 0x10, 0x10, 0xF0, 0x00,
  0x00, 0x00, 0x00, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0x1F, 0x10,
  0x10, 0x10, 0x10, 0xF0, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x10,
  0x10, 0x10, 0x10, 0x10, 0x10,
  0x10, 0x10, 0x10, 0xFF, 0x10,
  0x00, 0x00, 0x00, 0xFF, 0x14,
  0x00, 0x00, 0xFF, 0x00, 0xFF,
  0x00, 0x00, 0x1F, 0x10, 0x17,
  0x00, 0x00, 0xFC, 0x04, 0xF4,
  0x14, 0x14, 0x17, 0x10, 0x17,
  0x14, 0x14, 0xF4, 0x04, 0xF4,
  0x00, 0x00, 0xFF, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x14, 0x14,
  0x14, 0x14, 0xF7, 0x00, 0xF7,
  0x14, 0x14, 0x14, 0x17, 0x14,
  0x10, 0x10, 0x1F, 0x10, 0x1F,
  0x14, 0x14, 0x14, 0xF4, 0x14,
  0x10, 0x10, 0xF0, 0x10, 0xF0,
  0x00, 0x00, 0x1F, 0x10, 0x1F,
  0x00, 0x00, 0x00, 0x1F, 0x14,
  0x00, 0x00, 0x00, 0xFC, 0x14,
  0x00, 0x00, 0xF0, 0x10, 0xF0,
  0x10, 0x10, 0xFF, 0x10, 0xFF,
  0x14, 0x14, 0x14, 0xFF, 0x14,
  0x10, 0x10, 0x10, 0x1F, 0x00,
  0x00, 0x00, 0x00, 0xF0, 0x10,
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
  0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
  0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xFF, 0xFF,
  0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
  0x38, 0x44, 0x44, 0x38, 0x44,
  0x7C, 0x2A, 0x2A, 0x3E, 0x14,
  0x7E, 0x02, 0x02, 0x06, 0x06,
  0x02, 0x7E, 0x02, 0x7E, 0x02,
  0x63, 0x55, 0x49, 0x41, 0x63,
  0x38, 0x44, 0x44, 0x3C, 0x04,
  0x40, 0x7E, 0x20, 0x1E, 0x20,
  0x06, 0x02, 0x7E, 0x02, 0x02,
  0x99, 0xA5, 0xE7, 0xA5, 0x99,
  0x1C, 0x2A, 0x49, 0x2A, 0x1C,
  0x4C, 0x72, 0x01, 0x72, 0x4C,
  0x30, 0x4A, 0x4D, 0x4D, 0x30,
  0x30, 0x48, 0x78, 0x48, 0x30,
  0xBC, 0x62, 0x5A, 0x46, 0x3D,
  0x3E, 0x49, 0x49, 0x49, 0x00,
  0x7E, 0x01, 0x01, 0x01, 0x7E,
  0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
  0x44, 0x44, 0x5F, 0x44, 0x44,
  0x40, 0x51, 0x4A, 0x44, 0x40,
  0x40, 0x44, 0x4A, 0x51, 0x40,
  0x00, 0x00, 0xFF, 0x01, 0x03,
  0xE0, 0x80, 0xFF, 0x00, 0x00,
  0x08, 0x08, 0x6B, 0x6B, 0x08,
  0x36, 0x12, 0x36, 0x24, 0x36,
  0x06, 0x0F, 0x09, 0x0F, 0x06,
  0x00, 0x00, 0x18, 0x18, 0x00,
  0x00, 0x00, 0x10, 0x10, 0x00,
  0x30, 0x40, 0xFF, 0x01, 0x01,
  0x00, 0x1F, 0x01, 0x01, 0x1E,
  0x00, 0x19, 0x1D, 0x17, 0x12,
  0x00, 0x3C, 0x3C, 0x3C, 0x3C,
  0x00, 0x00, 0x00, 0x00, 0x00,
};

#define COMMANDFIFOSIZE 100

static display_command commands_to_run[COMMANDFIFOSIZE];

static uint16_t place_command = 0;
static uint16_t next_command = 0;

#pragma DATA_SECTION(commands_to_run, "ramgs2");
static uint16_t check_display_locked = 0;
//continue_transaction curr_transaction;
void displayDMAComplete(void)
{
    check_display_locked = 0;
    checkDisplayCommandFifo();
}
static uint8_t putCommand(display_command add_command)
{
    DINT;
    if (place_command == next_command - 1
            || (place_command == COMMANDFIFOSIZE - 1 && next_command == 0))
    {
        EINT;
        return 1;
    }
    commands_to_run[place_command] = add_command;
    place_command++;
    if (place_command == COMMANDFIFOSIZE)
    {
        place_command = 0;
    }
    EINT;
    return 0;
}

static display_command getCommand(void)
{
    DINT;
    if (next_command == place_command)
    {
        EINT;
        display_command a;
        a.function = NOCOMMAND;
        return a;
    }
    display_command n_command = commands_to_run[next_command];
    next_command++;
    if (next_command == COMMANDFIFOSIZE)
    {
        next_command = 0;
    }
    EINT;
    return n_command;
}
static void __ColorBox(uint16_t burst, uint16_t transactions, uint16_t color)
{
    SPI_DMA_BUFFER_1[0] = color;
    dma_transfer transfer;
    transfer.burst_step_size = 0;
    transfer.transaction_step_size = 0;
    transfer.transaction_warp_step = 0;
    transfer.transaction_wrap = 65535;
    transfer.data_address = SPI_DMA_BUFFER_1;
    transfer.transaction_count = transactions;
    transfer.burst_size = burst;
    startIli9341DMATransaction(transfer);

}

static void __ColorBox2(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t burst, uint16_t transactions, uint16_t color)
{
    ili9341_setRowAddress(x,x+width);
    ili9341_setColumnAddress(y, y+height);
    ili9341_startWriteFrameMemory();
    SPI_DMA_BUFFER_1[0] = color;
    dma_transfer transfer;
    transfer.burst_step_size = 0;
    transfer.transaction_step_size = 0;
    transfer.transaction_warp_step = 0;
    transfer.transaction_wrap = 65535;
    transfer.data_address = SPI_DMA_BUFFER_1;
    transfer.transaction_count = transactions;
    transfer.burst_size = burst;
    startIli9341DMATransaction(transfer);

}

static void __DrawChar(uint16_t x, uint16_t y, uint16_t text_color,
                      uint16_t background_color,
                      uint16_t font_size,uint16_t let)
{
    //setup memory
    int i=0;
    int on_x,on_y,scaley,scalex;
    for (on_x = 0; on_x < FONT_WIDTH; on_x++)
    {
        for (scalex = 0; scalex < font_size; scalex++)
        {
            for (on_y = 0; on_y < FONT_HEIGHT; on_y++)
            {
                for (scaley = 0; scaley < font_size; scaley++)
                {
                    if(on_x>FONT_X-1){
                        SPI_DMA_BUFFER_1[i]=background_color;
                    }else{
                        SPI_DMA_BUFFER_1[i]=(Font[let*5+on_x]&(0x1<<on_y))!=0?text_color:background_color;
                    }
                    i++;
                }
            }
        }
    }
    ili9341_setRowAddress(x, x + FONT_WIDTH * font_size - 1);
    ili9341_setColumnAddress(y, y + FONT_HEIGHT * font_size - 1);
    ili9341_startWriteFrameMemory();
    dma_transfer transfer;
    transfer.burst_step_size = 1;
    transfer.transaction_step_size = 1;
    transfer.transaction_warp_step = 1;
    transfer.transaction_wrap = 65535;
    transfer.data_address = SPI_DMA_BUFFER_1;
    transfer.transaction_count = i/FONT_HEIGHT;
    transfer.burst_size = FONT_HEIGHT;
    startIli9341DMATransaction(transfer);
}
void checkDisplayCommandFifo(void)
{
    //See if another process is already using this resource if so return
    DINT;
    if (check_display_locked != 0)
    {
        EINT;
        return;
    }

    //Resource available so claim it
    check_display_locked = 1;
    EINT;

    if (next_command == place_command)
    {
        //No commands in fifo so return and free resource
        check_display_locked = 0;
        return;
    }

    while (1)
    {
        display_command command_to_run = getCommand();
        uint16_t *command_params = command_to_run.params;
        switch (command_to_run.function)
        {
            case NOCOMMAND:
                check_display_locked = 0;
                return;
            case SETCOLADDRESS:
                ili9341_setColumnAddress(command_params[0], command_params[1]);
                break;
            case SETROWADDRESS:
                ili9341_setRowAddress(command_params[0], command_params[1]);
                break;
            case STARTFRAMEWRITE:
                ili9341_startWriteFrameMemory();
                break;
            case CONTINUEFRAMEWRITE:
                ili9341_writeMemoryContinue();
                break;
            case STARTFRAMEREAD:
                ili9341_startReadFrameMemory();
                break;
            case CONTINUEFRAMEREAD:
                ili9341_readMemoryContinue();
                break;
            case DRAWCFILLEDCOLORBOX:
                __ColorBox(command_params[0], command_params[1],
                           command_params[2]);
                //we return because check_display_locked should be cleared by dma
                return;
            case DRAWFILLEDCOLORBOX2:
                __ColorBox2(command_params[0], command_params[1],
                            command_params[2], command_params[3],
                            command_params[4], command_params[5],
                            command_params[6]);
                return;
            case DRAWCHAR:
                __DrawChar(command_params[0],command_params[1],command_params[2],command_params[3],command_params[4],command_params[5]);
                return;
        }
    }
}

//void finishFCB(void)
//{
//    dma_transfer transfer;
//    transfer.burst_step_size = 0;
//    transfer.transaction_step_size = 0;
//    transfer.transaction_warp_step = 0;
//    transfer.transaction_wrap = 65535;
//    transfer.data_address = SPI_DMA_BUFFER_1;
//    transfer.transaction_count = 0;
//    transfer.burst_size = curr_transaction.a;
//    startIli9341DMATransaction(transfer);
//
//}

uint16_t drawFilledColorBox2(uint16_t x, uint16_t y, uint16_t height,
                            uint16_t width, uint16_t color)
{
    //Do dimension checks
    if (x > 319 || y > 239)
    {
        return 1;
    }
    if (x + width > 319 || y + height > 239)
    {
        return 1;
    }
    display_command command;
    command.function = SETROWADDRESS;
    command.params[0] = x;
    command.params[1] = x + width;
    putCommand(command);
    command.function = SETCOLADDRESS;
    command.params[0] = y;
    command.params[1] = y + height;
    putCommand(command);
    command.function = STARTFRAMEWRITE;
    putCommand(command);

    uint32_t pixels = ((uint32_t) (width + 1)) * ((uint32_t) (height + 1));
//
//    dma_transfer transfer;
//    transfer.burst_step_size = 0;
//    transfer.transaction_step_size = 0;
//    transfer.transaction_warp_step = 0;
//    transfer.transaction_wrap = 65535;
//    transfer.data_address = SPI_DMA_BUFFER_1;
//    transfer.transaction_count = pixels / 16;

    if (pixels < 16)
    {
        command.function = DRAWCFILLEDCOLORBOX;
        command.params[0] = pixels;
        command.params[1] = 0;
        command.params[2] = color;
        putCommand(command);
    }
    else if (pixels % 16 != 0)
    {
        command.function = DRAWCFILLEDCOLORBOX;
        command.params[0] = 16;
        command.params[1] = pixels / 16;
        command.params[2] = color;
        putCommand(command);
        command.function = DRAWCFILLEDCOLORBOX;
        command.params[0] = pixels % 16;
        command.params[1] = 0;
        command.params[2] = color;
        putCommand(command);
    }
    else
    {
        command.function = DRAWCFILLEDCOLORBOX;
        command.params[0] = 16;
        command.params[1] = pixels / 16;
        command.params[2] = color;
        putCommand(command);
    }
    return 0;
}
uint16_t drawFilledColorBox(uint16_t x, uint16_t y, uint16_t height,
                            uint16_t width, uint16_t color)
{
    //Do dimension checks
    if (x > 319 || y > 239)
    {
        return 1;
    }
    if (x + width > 319 || y + height > 239)
    {
        return 1;
    }
    display_command command;
    command.function = DRAWFILLEDCOLORBOX2;
    command.params[0] = x;
    command.params[2] = width;
    command.params[1] = y;
    command.params[3] = height;

    uint32_t pixels = ((uint32_t) (width + 1)) * ((uint32_t) (height + 1));
//
//    dma_transfer transfer;
//    transfer.burst_step_size = 0;
//    transfer.transaction_step_size = 0;
//    transfer.transaction_warp_step = 0;
//    transfer.transaction_wrap = 65535;
//    transfer.data_address = SPI_DMA_BUFFER_1;
//    transfer.transaction_count = pixels / 16;

    if (pixels < 16)
    {
        command.params[4] = pixels;
        command.params[5] = 0;
        command.params[6] = color;
        putCommand(command);
    }
    else if (pixels % 16 != 0)
    {
        command.params[4] = 16;
        command.params[5] = pixels / 16;
        command.params[6] = color;
        putCommand(command);
        command.params[4] = pixels % 16;
        command.params[5] = 0;
        command.params[6] = color;
        putCommand(command);
    }
    else
    {
        command.params[4] = 16;
        command.params[5] = pixels / 16;
        command.params[6] = color;
        putCommand(command);
    }
    return 0;
}
uint16_t drawOutlineBox(uint16_t x, uint16_t y, uint16_t height,
                        uint16_t width, uint16_t color, uint16_t thickness)
{
    //Do dimension checks
    if (x > DISPLAYWIDTH || y > DISPLAYHEIGHT)
    {
        return 1;
    }
    if (x + width > DISPLAYWIDTH || y + height > DISPLAYHEIGHT)
    {
        return 1;
    }
    if(thickness>width||thickness>height){
        return 2;
    }
    //Draw top line
    drawFilledColorBox(x, y, thickness-1, width, color);
    //Draw bottom line
    drawFilledColorBox(x, y+height-(thickness-1), thickness-1, width, color);
    //Draw left line
    drawFilledColorBox(x, y, height, thickness-1, color);
    //Draw right line
    drawFilledColorBox(x+width-(thickness-1), y, height, thickness-1, color);
    return 0;
}

uint16_t screenDrawText(uint16_t x, uint16_t y, char *string,
                        uint16_t text_color, uint16_t background_color,
                        uint16_t font_size)
{
    //check if too long
    if (x + FONT_WIDTH * font_size > DISPLAYWIDTH)
    {
        return 1;
    }
    //check if to low
    if (y + FONT_HEIGHT * font_size > DISPLAYHEIGHT)
    {
        return 1;
    }
    if (font_size > 8)
    {
        return 1;
    }
    int i = 0;
    //have a cutoff for 100 letters to prevent non terminated strings from hanging system
    display_command command;
    command.function = DRAWCHAR;
    uint16_t curr_x = x;
    uint16_t curr_y = y;
    while (string[i] != 0 && i < 100)
    {
        if (string[i] != '\n')
        {
            command.params[0] = curr_x;
            command.params[1] = curr_y;
            command.params[2] = text_color;
            command.params[3] = background_color;
            command.params[4] = font_size;
            command.params[5] = string[i];
            putCommand(command);
            curr_x += FONT_WIDTH * font_size;
            if (curr_x > DISPLAYWIDTH)
            {
                curr_x = x;
                curr_y += FONT_HEIGHT * font_size;
            }
        }
        else
        {
            curr_x = x;
            curr_y += FONT_HEIGHT * font_size;
        }
        if (curr_y > DISPLAYHEIGHT)
        {
            //no more charcters can fit indicate there was a partial write
            return 3;
        }
        i++;

    }
    return 0;

}
