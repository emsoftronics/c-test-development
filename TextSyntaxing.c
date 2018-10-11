/*
 * =====================================================================================
 *
 *       Filename:  TextSyntaxing.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Saturday 30 September 2017 10:53:29  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void draw_hline(FILE *stream, unsigned int count, char linechar, unsigned int mdelayperchar)
{
   while (count--) {
       putc(linechar, stream);
       fflush(stream);
       usleep(1000*mdelayperchar);
   }
}

void draw_vline(FILE *stream,unsigned int len, unsigned int spacecount, unsigned int mdelayperchar)
{
    while (len--) {
       draw_hline(stream,spacecount,' ', 0);
       putc('|', stream);
       putc('\n', stream);
       fflush(stream);
       usleep(1000*mdelayperchar);
   }
}

void test_hline(int argc, char **argv)
{
    int c = 0;
    char lc = '-';
    unsigned int delay = 0;
    if (argc > 1) c = atoi(argv[1]);
    if (argc > 2) lc = argv[2][0];
    if (argc > 3) delay = atoi(argv[3]);
    draw_hline(stdout,c,lc,delay);
    putchar('\n');
}

void test_vline(int argc, char **argv)
{
    int c = 0;
    unsigned int sc = 8;
    unsigned int delay = 0;
    if (argc > 1) c = atoi(argv[1]);
    if (argc > 2) sc = atoi(argv[2]);
    if (argc > 3) delay = atoi(argv[3]);
    draw_vline(stdout,c,sc,delay);
    putchar('\n');
}

int main(int argc, char **argv)
{
    test_vline(argc, argv);
    test_hline(argc, argv);
    return 0;
}

