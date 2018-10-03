/*
 * =====================================================================================
 *
 *       Filename:  testsemaphore.c
 *
 *    Description:  Test Semaphore
 *
 *        Version:  1.0
 *        Created:  Wednesday 25 January 2017 01:06:38  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <pthread.h>    /* required for pthreads */
#include <semaphore.h>  /* required for semaphores */


static sem_t sem;
static unsigned char stop = 0;

static void threadsignal(void)
{
   if (stop) {
        sem_wait(&sem);
   }
}

static threadsupend(void)
{
    int v = 0;
    sem_getvalue(&sem,&v);
    if (v>0) {
        sem_wait(&sem);
    }
    stop = 1;
}
static threadresume(void)
{
    int v = 0;
    stop = 0;
    sem_getvalue(&sem,&v);
    if (v==0) {
        sem_post(&sem);
    }
}

void *thread_func(void *arg)
{
    while(1) {
        printf("hello\n");
        fflush(stdout);
        sleep(1);
        threadsignal();
    }
}

void main()
{
    int c= 0;
    pthread_t thread = {0};
    pthread_attr_t attr ={0};

    sem_init(&sem, 0, 1);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&thread, &attr, thread_func, NULL);
    do {
        fflush(stdout);
        c = getchar();
        if (c == 'y'){
            threadresume();
            printf("thread resumed!!\n");
        }
        else if (c=='n') {
            threadsupend();
            printf("thread suspended!!\n");
        }
    }while(c!='x');
}
