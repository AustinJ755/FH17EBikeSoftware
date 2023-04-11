/*
 * stringtools.c
 *
 *  Created on: Apr 10, 2023
 *      Author: aj755
 */
#include "./ryu/ryu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/**
 * Much slower then the simple version but also significantly more accurate. This will print to more decimals and print in either scientific
 * or decimal notation depending on which one takes less space
 * @param f the float to be converted
 * @param str the char array to store the float string in
 */
void float2StringComplex(float f, char* str){
    f2s_buffered(f, str);
}


#define FLOAT_STR_BUFF_SIZE 25
/**
 * Simple conversion of float to string. Make a 26 char array and set the 26th char to zero then pass as p. The returned char pointer will point to the
 * beginning of the of the float string
 * @param x the float to be converted to a string
 * @param p the buffer to store the output string
 * @return pointer to beginning of the float string
 */
char* float2StringSimple(float x, char *p) {
    char *s = p + FLOAT_STR_BUFF_SIZE; // go to end of buffer
    if (x == 0)
    {
        *--s = '0';
        *--s = '.';
        *--s = '0';
        return s;
    }
    uint32_t decimals;  // variable to store the decimals
    uint32_t units; // variable to store the units (part to left of decimal place)
    if (x < 0)
    { // take care of negative numbers
        decimals = (uint32_t) (x * -1000000) % 1000000; // make 1000 for 3 decimals etc.
        units = (uint32_t) (-1 * x);
    }
    else
    { // positive numbers
        decimals = (uint32_t) (x * 1000000) % 1000000;
        units = (uint32_t) x;
    }
    //sprintf();
    while (decimals > 0)
    {
        *--s = (decimals % 10) + '0';
        decimals /= 10; // repeat for as many decimal places as you need
    }
    *--s = '.';

    while (units > 0)
    {
        *--s = (units % 10) + '0';
        units /= 10;
    }
    if (x < 0)
        *--s = '-'; // unary minus sign for negative numbers
    return s;
}

static char* __utoa(unsigned value, char *str, int base) {
    const char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    int i, j;
    unsigned remainder;
    char c;

    /* Check base is supported. */
    if ((base < 2) || (base > 36))
    {
        str[0] = '\0';
        return NULL;
    }

    /* Convert to string. Digits are in reverse order.  */
    i = 0;
    do
    {
        remainder = value % base;
        str[i++] = digits[remainder];
        value = value / base;
    }
    while (value != 0);
    str[i] = '\0';

    /* Reverse string.  */
    for (j = 0, i--; j < i; j++, i--)
    {
        c = str[j];
        str[j] = str[i];
        str[i] = c;
    }

    return str;
}
/**
 * Convert the provided unsigned value into its string representation in the provided base
 * @param value the value to convert
 * @param str the buffer to store the output string
 * @param base the base to use
 * @return beginning of the converted string
 */
char* unsigned2String(unsigned value, char *str, int base) {
    return __utoa(value, str, base);
}

static char* __itoa(int value, char *str, int base) {
    unsigned uvalue;
    int i = 0;

    /* Check base is supported. */
    if ((base < 2) || (base > 36))
    {
        str[0] = '\0';
        return NULL;
    }

    /* Negative numbers are only supported for decimal.
     * Cast to unsigned to avoid overflow for maximum negative value.  */
    if ((base == 10) && (value < 0))
    {
        str[i++] = '-';
        uvalue = (unsigned) -value;
    }
    else
        uvalue = (unsigned) value;

    __utoa(uvalue, &str[i], base);
    return str;
}

/**
 * Convert the provided signed value into its string representation in the provided base
 * @param value the value to convert
 * @param str the buffer to store the output string
 * @param base the base to use
 * @return beginning of the converted string
 */
char* signed2String(int value, char *str, int base) {
    return __itoa(value, str, base);
}

