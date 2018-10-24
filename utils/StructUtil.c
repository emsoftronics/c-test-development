/*
 * =====================================================================================
 *
 *       Filename:  StructUtil.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Thursday 28 December 2017 02:21:30  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include "StructUtil.h"
#include <stdio.h>
#include "StringUtil.h"

void getRecord(struct information *info)
{
    info->id = 0;
    printf("Enter Name: ");
    scanf(" %[^\n]",info->name);
    printf("Enter Address: ");
    scanf(" %[^\n]",info->address);
    printf("Enter Contact Number: ");
    scanf(" %lu",&(info->contact_no));
    printf("Enter Remark: ");
    scanf(" %[^\n]",info->remark);

}

int getMultiRecord(struct information *info, int lastId)
{
    char c = 0;
    do {
        getRecord(info);
        info->id = lastId+1;
        lastId++;
        toProperStr(info->name);
        info++;
        printf("\nDo you want enter any more record? (y/n) : ");
        scanf(" %c", &c);
    }while((c=='Y')|| (c=='y'));

    return lastId;
}

void printRecord(struct information *info)
{

    printf("\n\t\tUSER RECORD\n");
    printf("\t\t===========\n");
    printf("--------------------------------------------------------------------\n");
    printf("\tUser ID    : %u\n",info->id);
    printf("\tName       : %s\n",info->name);
    printf("\tAddress    : %s\n",info->address);
    printf("\tContact No.: %lu\n",info->contact_no);
    printf("\tRemark     : %s\n",info->remark);
    printf("--------------------------------------------------------------------\n");

}
