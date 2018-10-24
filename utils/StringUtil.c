/*
 * =====================================================================================
 *
 *       Filename:  stringtest.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Tuesday 26 December 2017 03:40:13  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */
#include<stdio.h>

int isLower(char ch)
{
    if((ch>='a') && (ch<='z') )
        return 1;
    else
        return 0;
}

int isUpper(char ch)
{
    if((ch>='A') && (ch<='Z'))
        return 1;
    else
        return 0;
}
char toUpper(char ch)
{
    if(isLower(ch))
        return ch - 'a' + 'A';
    else
        return ch;
}

char toLower(char ch)
{

   if(isUpper(ch))
       return ch + 'a' - 'A';
   else
       return ch;
}

void toUpperStr(char *str)
{
    char *ptr;
    ptr = str;
    while(*ptr != '\0')
    {
        *ptr = toUpper(*ptr);
        ptr++;
    }

}
void toLowerStr(char *str)
{
     char *ptr;                                                                                                                                      ptr = str;
    while(*ptr != '\0')
    {
        *ptr = toLower(*ptr);
        ptr++;
    }
}

void toProperStr(char *str)
{
    char *ptr;
    ptr = str;
    int isSpace = 0;
    while(*ptr != '\0')
    {
        if((ptr == str) || isSpace)
        {
            *ptr = toUpper(*ptr);
        }
        else {
            *ptr = toLower(*ptr);
        }

        if(*ptr == ' ')
        {
            isSpace = 1;
        }
        else {
            isSpace = 0;
        }
        ptr++;

    }
}

void getUsrStr(char *buf)
{
    printf("Enter any string: \n");
    scanf(" %[^\n]", buf);
}

