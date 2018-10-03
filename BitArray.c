/*
 * =====================================================================================
 *
 *       Filename:  BitArray.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Tuesday 10 July 2018 08:54:41  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jitendra Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>

typedef unsigned char bitarr;

#define BIT_ARRAY(name, size) bitarr name[(((size) + 7) & ~7) >> 3] = {0}

#define GET_BIT(name, n)   ((name[(n)>>3] & (1<<((n)&7))) > 0)
#define UPDATE_BIT(name, n, val) ({ name[(n)>>3] &= (~(1<<((n)&7))); \
                            name[(n)>>3] |= (((val) > 0)<<((n)&7)); })

inline void printBitArray(bitarr *barray, int size)
{
    int i = 0, n = 0;
    while(i < size) {
        for (n = 0; (n < 8) && (i < size); n++, i++)
            putchar(((*(barray)>>n) & 0x1) + 0x30);
        barray++;
    }
    putchar('\n');
}


int main(void)
{
    #define bits  1000
    BIT_ARRAY(my_bit_array, bits);
    printBitArray(my_bit_array, bits);
    UPDATE_BIT(my_bit_array, 9, 1);
    UPDATE_BIT(my_bit_array, 5, 1);
    UPDATE_BIT(my_bit_array, bits - 2, 1);
    printBitArray(my_bit_array, bits);
    return 0;
}

