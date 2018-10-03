/*
 * =====================================================================================
 *
 *       Filename:  signal_test.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Saturday 13 January 2018 10:15:47  IST
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
#include <unistd.h>

void sig_handler(int sig)
{
    if(sig == SIGINT)
    printf("\nHello!! I will never terminated by CTRL+C\n");
    else if (sig == SIGQUIT)
    printf("\nHello!! I will never terminated by CTRL+|\n");
    else if (sig == SIGKILL)
    printf("\nHello!! I am unable to stop from termination.\n");

}

int main()
{
    if(signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\nUnable to capture signal SIGINT\n");
    if(signal(SIGQUIT, sig_handler) == SIG_ERR)
        printf("\nUnable to capture signal SIGQUIT\n");
    if(signal(SIGKILL, sig_handler) == SIG_ERR)
        printf("\nUnable to capture signal SIGKILL\n");

    printf("Enter a no.: ");
    int a;
    scanf(" %d", &a);
    printf("Finally, You did whatever I said.\n");
    return ;
}
