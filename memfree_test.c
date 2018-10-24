/*
 * =====================================================================================
 *
 *       Filename:  memfree_test.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Wednesday 24 October 2018 10:48:59  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish22@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>

#define __CHECK_RUNTIME_ERROR reg_sigs(); if (setjmp(env) == 0)

static jmp_buf env;

void sighandler(int sig)
{
    printf("SIGNALED as: %s\n", strsignal(sig));
    longjmp(env, sig);
}

void reg_sigs(void)
{
    static char called = 0;
    if (called) return;
    if (signal(SIGABRT, sighandler) == SIG_ERR) perror("signal");
    if (signal(SIGSEGV, sighandler) == SIG_ERR) perror("signal");
    called = 1;
}


int main(int argc, char **argv)
{
    char *buff = malloc(30);
    perror("malloc");
    strcpy(buff, "hello bro");
    __CHECK_RUNTIME_ERROR free(buff+4);
    printf("hi\n");
    strcpy(buff, "jagdish prajapati");
    perror("strcpy");
    printf("%s\n", buff);
    free(buff);
    perror("free");
    __CHECK_RUNTIME_ERROR free(buff);
    buff = NULL;
    __CHECK_RUNTIME_ERROR *buff = 10;
    return 0;
}
