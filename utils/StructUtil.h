/*
 * =====================================================================================
 *
 *       Filename:  StructUtil.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Thursday 28 December 2017 01:54:55  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#ifndef __STRUCTUTIL_H__
#define __STRUCTUTIL_H__
struct information {
    unsigned int id;
    char name[32];
    char address[64];
    long unsigned int contact_no;
    char remark[128];
};

extern void getRecord(struct information *info);
extern void printRecord(struct information *info);
extern int getMultiRecord(struct information *info, int lastId);
#endif /* __STRUCTUTIL_H__ */
