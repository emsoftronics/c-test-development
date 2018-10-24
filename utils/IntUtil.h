/*
 * =====================================================================================
 *
 *       Filename:  IntUtil.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Wednesday 27 December 2017 10:26:40  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#ifndef __INTUTIL_H__
#define __INTUTIL_H__

extern int getUserArray( int *arr);
extern void printArray(int *arr, int size);
extern void printMax( int *arr, int size);
extern void printAvailNum(int *arr, int size);
extern void CopyContentsArray(int *arrSrc, int *arrDst, int size);
extern void compArrayElements(int *arr, int n);


#endif
