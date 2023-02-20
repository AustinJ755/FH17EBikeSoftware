/*
 * datalogger.c
 *
 *  Created on: Feb 5, 2023
 *      Author: aj755
 */

#include "datalogger.h"
#include "board.h"

//always use this when referencing the DEBUG_UART_BASE
#define DEBUG_UART_BASE DATA_LOG_UART_BASE;

LOG_MESSAGE MESSAGEQUEUE[QUEUE_SIZE];
uint16_t curr_message = 0;
uint16_t next_message = 0;

#pragma DATA_SECTION(MESSAGEQUEUE, "ramls6");

uint16_t pushMessage(LOG_MESSAGE message)
{
    //Only one push or pop event may be occurring at a time.
    DINT;

    //Check if the queue has space
    if ((curr_message == QUEUE_SIZE - 1 && next_message == 0)
            || curr_message == next_message - 1)
    {
        //Failed to add message indicate that
        EINT;
        return 1;
    }

    MESSAGEQUEUE[curr_message] = message;
    curr_message++;
    if (curr_message == QUEUE_SIZE)
    {
        curr_message = 0;
    }
    EINT;
    //Queue Success
    return 0;

}

//This method should never be called by any function outside this file
static LOG_MESSAGE popMessage(void)
{
    //Only one push or pop event may be occurring at a time.
    DINT;

    if (curr_message == next_message)
    {
        EINT;
        return (LOG_MESSAGE) EMPTY_MESSAGE;
    }

    LOG_MESSAGE message = MESSAGEQUEUE[next_message];
    next_message++;
    if (next_message == QUEUE_SIZE)
    {
        next_message = 0;
    }
    EINT;
    return message;
}

uint16_t called = 0;
void sendUARTString(char *message_to_send)
{
    called++;
}

void sendUARTUInt(uint32_t int_to_send)
{
    called++;
}

void sendUARTSInt(int32_t int_to_send)
{
    called++;
}

void sendUARTFloat(int32_t float_to_send)
{
    called++;
}

void checkDebugMessageQueue(void)
{
    //Either dequeue messages until empty or dequeue 20 messages and pass control
    uint8_t count = 0;
    while (curr_message != next_message && count != 20)
    {
        count++;
        LOG_MESSAGE message_send = popMessage();

        LOG_MESSAGE message_type = message_send & MESSAGE_MASK;

        message_send &= MESSAGE_MASK_CLEAR;
        switch (message_type)
        {
            case MESSAGE_STRING_PTR_MASK:
                sendUARTString((char*) message_send);
                break;
            case MESSAGE_UINT_PTR_MASK:
                sendUARTUInt(*((uint32_t*) message_send));
                break;
            case MESSAGE_SINT_PTR_MASK:
                sendUARTSInt(*((int32_t*) message_send));
                break;
            case MESSAGE_FLOAT_PTR_MASK:
                sendUARTFloat(*((float*) message_send));
                break;
            case MESSAGE_UINT_IMM_MASK:
                sendUARTUInt((uint32_t) message_send);
                break;
            case MESSAGE_SINT_IMM_MASK:
                if(message_send&0x8000000==0x8000000){
                    message_send|=MESSAGE_MASK;
                }
                sendUARTSInt((int32_t) message_send);
                break;

        }
    }
}
