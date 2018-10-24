/*
 * =====================================================================================
 *
 *       Filename:  stringUtil.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Sunday 24 December 2017 10:41:01  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */
#include<stdio.h>

int getUserArray( int *arr)
{
    int j, size;
    printf("Enter the size of array:");
    scanf("%d",&size);
    printf("Enter the values of %d elements: ", size);
    for(j=0; j<size; j++)
    {
    scanf(" %d",&arr[j]);
    }
    return size;
}

void printArray(int *arr, int size)
{
    int i;
    printf("\n\t[");
    for (i = 0; i < size; i++)
        printf(" %d,", arr[i]);

    printf(" ]\n");
}

void printMax( int *arr, int size)
{
    int i,max = arr[0];
    for(i = 1; i<size; i++)
    {
        if(max < arr[i])
        {
            max = arr[i];
        }
    }
    printf("The maximum value in the array is = %d\n",max);
}

void printAvailNum(int *arr, int size)
{
    int i,n;
    printf("Enter the number to be searched: ");
    scanf("%d",&n);
    for(i=0; i<size; i++)
    {
        if(n == *(arr+i))
        {
          printf("The searched number position is = %d\n",i);
          break;
        }
    }
    if(i == size)
        printf("The number is not available");
}
void CopyContentsArray(int *arrSrc, int *arrDst, int size)
{
    int i;
    for(i=0; i< size; i++)
    {
        arrDst[size-i-1] = arrSrc[i];
    }
}
void compArrayElements(int *arr, int n)
{
    int i;
    for(i=0; i<n; i++)
    {
        if(arr[i] == arr[n-i-1])
        {
            printf("The elements are Palindrom");
            break;
        }
        if(arr[i] != arr[n-i-1])
        {
            printf("The elements are not Palindrom");
            break;
        }

    }
}

