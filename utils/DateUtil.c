/*
 * =====================================================================================
 *
 *       Filename:  DateUtil.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Monday 08 January 2018 06:26:50  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include "DateUtil.h"

const char *WeekDay[] = {"Sunday",
                       "Monday",
                       "Tuesday",
                       "Wednesday",
                       "Thursday",
                       "Friday",
                       "Saturday"
                       };


const char *Month[] = {
                        "Unknown",
                        "Jan",
                        "Feb",
                        "Mar",
                        "Apr",
                        "May",
                        "Jun",
                        "Jul",
                        "Aug",
                        "Sep",
                        "Oct",
                        "Nov",
                        "Dec"
                      };



void getUserDate(DU_Date_t *user_date_storage)
{
    unsigned int tmp;
    printf("Enter the weekday no.(0-6) :" );
    scanf(" %u", &tmp);
    user_date_storage->day = tmp;
    printf("Enter the date no.(1-31) :" );
    scanf(" %u", &tmp);
    user_date_storage->date = tmp;
    printf("Enter the month no.(1-12) :" );
    scanf(" %u", &tmp);
    user_date_storage->month = tmp;
    printf("Enter the year no.(0-99) :" );
    scanf(" %u", &tmp);
    user_date_storage->year = tmp;
    printf("Enter the hour no.(0-23) :" );
    scanf(" %u", &tmp);
    user_date_storage->hour = tmp;
    printf("Enter the minute no.(0-59) :" );
    scanf(" %u", &tmp);
    user_date_storage->minut = tmp;
}

void printDate(DU_Date_t *user_date)
{
    printf("%10s, %02d %3s, %02d; %02d:%02d\n",
            WeekDay[user_date->day], user_date->date,
            Month[user_date->month], user_date->year,
            user_date->hour, user_date->minut);
}
