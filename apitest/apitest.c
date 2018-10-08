/*
 * =====================================================================================
 *
 *       Filename:  apitest.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Saturday 06 October 2018 10:19:58  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>

static jmp_buf g_resume_here;

static void signal_handler(int sig)
{
    psignal(sig, "Runtime ERROR");
    longjmp(g_resume_here, sig);
}

static int register_signal(int sig)
{
    if (singal(sig, signal_handler) == SIG_ERR) {
        printf("%s:%s: ERROR: Unable to register signal (%d)!!\n",
                __FILE__, __func__, sig);
        return -1;
    }
    return 0;
}

static int register_error_signals(void)
{
    int ret = 0;

    ret |= register_signal(SIGILL);
    ret |= register_signal(SIGABRT);
    ret |= register_signal(SIGBUS);
    ret |= register_signal(SIGFPE);
    ret |= register_signal(SIGSEGV);
    ret |= register_signal(SIGPIPE);
    ret |= register_signal(SIGSTKFLT);
    ret |= register_signal(SIGCHLD);
    ret |= register_signal(SIGXCPU);
    ret |= register_signal(SIGXFSZ);
    ret |= register_signal(SIGSYS);

    return ret;
}

static int check_api_call(void)
{
    return setjmp(g_resume_here);
}



