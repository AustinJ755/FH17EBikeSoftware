/*
 * display_driver.c
 *
 *  Created on: Jan 20, 2023
 *      Author: aj755
 */
#include "display_driver.h"
#include "spi_dma.h"
#include "ili9341.h"
int initDisplay(void)
{
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
} display_function;

typedef struct
{
    display_function function;
    uint16_t params[8];
} display_command;

#define COMMANDFIFOSIZE 100

static display_command commands_to_run[COMMANDFIFOSIZE];

static uint16_t place_command = 0;
static uint16_t next_command = 0;

#pragma DATA_SECTION(commands_to_run, "ramgs2");
static uint16_t check_display_locked = 0;
//continue_transaction curr_transaction;
void displayDMAComplete(void){
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
static void __ColorBox(uint16_t burst,uint16_t transactions, uint16_t color){
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
        uint16_t *command_params=command_to_run.params;
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
                __ColorBox(command_params[0], command_params[1],command_params[2]);
                //we return because check_display_locked should be cleared by dma
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
    command.function=SETROWADDRESS;
    command.params[0]=x;
    command.params[1]=x+width;
    putCommand(command);
    command.function=SETCOLADDRESS;
    command.params[0]=y;
    command.params[1]=y+height;
    putCommand(command);
    command.function=STARTFRAMEWRITE;
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
        command.function=DRAWCFILLEDCOLORBOX;
        command.params[0]=pixels;
        command.params[1]=0;
        command.params[2]=color;
        putCommand(command);
    }
    else if (pixels % 16 != 0)
    {
        command.function=DRAWCFILLEDCOLORBOX;
        command.params[0]=16;
        command.params[1]=pixels / 16;
        command.params[2]=color;
        putCommand(command);
        command.function=DRAWCFILLEDCOLORBOX;
        command.params[0]=pixels % 16;
        command.params[1]=0;
        command.params[2]=color;
        putCommand(command);
    }
    else
    {
        command.function=DRAWCFILLEDCOLORBOX;
        command.params[0]=16;
        command.params[1]=pixels / 16;
        command.params[2]=color;
        putCommand(command);
    }
    return 0;
}

