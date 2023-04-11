/*
 * datalogger.c
 *
 *  Created on: Feb 5, 2023
 *      Author: aj755
 */
//typedef uint16_t uint8_t;
#include "datalogger.h"

#include "board.h"
#include "driverlib.h"
#include "device.h"
#include "customtools/stringtools.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DECIMAL 10

//always use this when referencing the DEBUG_UART_BASE
#define DEBUG_UART_BASE DATA_LOG_UART_BASE
#define CHAR_BUFF_SIZE 50
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
    int charCount = strlen(message_to_send);

    SCI_writeCharArray(DEBUG_UART_BASE, (uint16_t*) message_to_send, charCount);
}

void sendUARTUInt(uint32_t int_to_send)
{
    called++;
    char buffer[20];
    unsigned2String(int_to_send, buffer, DECIMAL);
    int charCount = strlen(buffer);
    SCI_writeCharArray(DEBUG_UART_BASE, (uint16_t*) buffer, charCount);
}

void sendUARTSInt(int32_t int_to_send)
{
    called++;
    char buffer[20];
    signed2String(int_to_send, buffer, DECIMAL);
    int charCount = strlen(buffer);
    SCI_writeCharArray(DEBUG_UART_BASE, (uint16_t*) buffer, charCount);

}

void sendUARTFloat(float float_to_send)
{
    called++;
    char buffer[CHAR_BUFF_SIZE+1]={0,};
    char* buf = buffer;
    if(float_to_send<1e5&&float_to_send>1e-5){
        buf =  float2StringSimple(float_to_send,buffer);
    }else{
    float2StringComplex(float_to_send, buffer);//_float_to_char(float_to_send, buffer);
    }
    int charCount = strlen(buf);
    SCI_writeCharArray(DEBUG_UART_BASE, (uint16_t*) buf, charCount);
}


//TODO make all these functions thread safe
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
