/*
 * =====================================================================================
 *
 *       Filename:  StringUtil.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Wednesday 27 December 2017 10:38:57  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#ifndef __STRINGUTIL_H__
#define __STRINGUTIL_H__

#define MAX(a, b) (((a) > (b)) ? (a) :(b))
extern int isLower(char ch);
extern int isUpper(char ch);
extern char toUpper(char ch);
extern char toLower(char ch);
extern void toUpperStr(char *str);
extern void toLowerStr(char *str);
extern void toProperStr(char *str);
extern void getUsrStr(char *buf);


#endif
