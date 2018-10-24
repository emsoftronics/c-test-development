/*
 * =====================================================================================
 *
 *       Filename:  DateUtil.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Monday 08 January 2018 06:31:52  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#ifndef __DATEUTIL_H__
#define __DATEUTIL_H__

struct DU_Date {
    unsigned int day:3;
    unsigned int date:5;
    unsigned int month:4;
    unsigned int year:7;
    unsigned int hour:5;
    unsigned int minut:6;
};

typedef struct DU_Date DU_Date_t;
extern const char *WeekDay[];
extern const char *Month[];


void getUserDate(DU_Date_t *user_date_storage);
void printDate(DU_Date_t *user_date);


#endif /* __DATEUTIL_H__ */


