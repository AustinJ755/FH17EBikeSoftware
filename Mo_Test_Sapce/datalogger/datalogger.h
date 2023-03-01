/*
 * datalogger.h
 *
 *  Created on: Feb 4, 2023
 *      Author: aj755, zmi69
 */

#ifndef DATALOGGER_DATALOGGER_H_
#define DATALOGGER_DATALOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib.h"
#include "device.h"
//
#define MESSAGE_MASK                0xF0000000

#define MESSAGE_MASK_CLEAR          0x0FFFFFFF
#define MESSAGE_STRING_PTR_MASK     0x00000000
#define MESSAGE_FLOAT_PTR_MASK      0x80000000
#define MESSAGE_UINT_PTR_MASK       0x40000000
#define MESSAGE_SINT_PTR_MASK       0x20000000
#define MESSAGE_UINT_IMM_MASK       0x10000000
#define MESSAGE_SINT_IMM_MASK       0x30000000

#define EMPTY_MESSAGE               0xF0000000

#define QUEUE_SIZE                   1000

const char MESSAGE_INT_OUT_OF_RANGE[] = "the int was out of range";
typedef uintptr_t LOG_MESSAGE;

uint16_t pushMessage(LOG_MESSAGE message);

//We only want this section to ever be poping messages
//LOG_MESSAGE popMessage(void);


//Log functions
void checkDebugMessageQueue(void);


/**
 * Send a pointer to a string that will be written to the UART later. Must be a constant value in memory.
 * @param log_string The string to be printed to the UART
 * @return
 */
static inline uint16_t logString(const char* log_string) {
    return pushMessage((LOG_MESSAGE) log_string);
}


static inline uint16_t logUInt(uint32_t* log_int) {
    return pushMessage((((LOG_MESSAGE) log_int) & MESSAGE_MASK_CLEAR) | MESSAGE_UINT_PTR_MASK);
}


static inline uint16_t logSignedInt(int32_t* log_int) {
    return pushMessage((((LOG_MESSAGE) log_int) & MESSAGE_MASK_CLEAR) | MESSAGE_SINT_PTR_MASK);
}


//These should maybe not be inlined
static inline uint16_t logUIntImmediate(uint32_t log_int) {
   if(log_int < 268435455) {
       //TODO: Send a message to the stack saying that the int was out of range
       //Return if putting that message up was successful
       uint16_t success = logString(MESSAGE_INT_OUT_OF_RANGE);

       if(success == 0) return success;
   }

   return pushMessage((((LOG_MESSAGE) log_int) & MESSAGE_MASK_CLEAR) | MESSAGE_UINT_IMM_MASK);
}


//This should maybe not be inlined
static inline uint16_t logSignedIntImmediate(int32_t log_int) {
   if(log_int < -134217728) {
       //TODO:Send a message to the stack saying that the int was out of range
       //Return if putting that message up was successful
       uint16_t success = logString(MESSAGE_INT_OUT_OF_RANGE);

       if(success == 0) return success;
   }

   if(log_int > 134217727) {
       //TODO:Send a message to the stack saying that the int was out of range
       //Return if putting that message up was successful

       uint16_t success = logString(MESSAGE_INT_OUT_OF_RANGE);

       if(success == 0) return success;
   }

   return pushMessage((((LOG_MESSAGE) log_int) & MESSAGE_MASK_CLEAR) | MESSAGE_SINT_IMM_MASK);
}


static inline uint16_t logFloat(float* log_float) {
    return pushMessage((((LOG_MESSAGE)log_float) & MESSAGE_MASK_CLEAR) | MESSAGE_FLOAT_PTR_MASK);
}

#endif /* DATALOGGER_DATALOGGER_H_ */
