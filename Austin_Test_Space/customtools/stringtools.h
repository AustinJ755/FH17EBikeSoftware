/*
 * stringtools.h
 *
 *  Created on: Apr 10, 2023
 *      Author: aj755
 */

#ifndef CUSTOMTOOLS_STRINGTOOLS_H_
#define CUSTOMTOOLS_STRINGTOOLS_H_

char* signed2String(int value, char *str, int base);

char* unsigned2String(unsigned value, char *str, int base);

char* float2StringSimple(float x, char *p);

void float2StringComplex(float f, char* str);

#endif /* CUSTOMTOOLS_STRINGTOOLS_H_ */
