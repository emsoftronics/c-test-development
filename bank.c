/*
 * =====================================================================================
 *
 *       Filename:  bank.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Wednesday 25 April 2018 12:03:06  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>

int main( int argc , char **argv)
{
    float iRate;
    unsigned int time;
    unsigned long amount, interest = 0, total_amount = 0;
    int i;
    printf("\tEnter the amount deposited in each block of time : ");
    scanf(" %lu", &amount);
    printf("\tEnter the interest rate for the block of time : ");
    scanf(" %f", &iRate);
    printf("\tEnter the number of block of time : ");
    scanf(" %u", &time);

    for (total_amount = amount, i = 0; i < time; i++) {
        total_amount += total_amount*(iRate/100) + amount;
    }

    printf("\tTotal Amount Deposited : %lu\n", time*amount);
    printf("\tTotal Interest gain : %lu\n", total_amount - (time*amount));
    printf("\tTotal Amount Recovered : %lu\n", total_amount);

    return 0;
}
