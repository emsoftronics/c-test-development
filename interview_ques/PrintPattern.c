/*
 * =====================================================================================
 *
 *       Filename:  PrintPattern.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Saturday 31 March 2018 02:25:34  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>

void PrintPattern(int n)
{
    if ((n<1) || (n>9)) {
        printf("Invalid value of N: No pattern output.");
        return;
    }
    int i, j, k;
    int max = n + (n-1);
    for (i = 0; i < max; i++) {
        k = n;
        for (j = 0; j < max; j++) {
            printf(" %d",k);
            if ( i <= (max>>1)) {
                if (j < i) k--;
                else if ((j >= i) && (j < max-i-1)) ;
                else k++;
            }
            else {
                if (j < max-i-1) k--;
                else if ((j >= max-i-1) && (j < i)) ;
                else k++;
            }
        }
        printf("\n");
    }
}

void PatternTests(void)
{
    int T;
    int arr[10];
    int i;

    scanf(" %d", &T);
    for (i = 0; (i < T) && (i < 10); i++) scanf(" %d", &arr[i]);
    for (i = 0; (i < T) && (i < 10); i++) PrintPattern( arr[i]);
}


int StrRotatedVersion(char *s1, char *s2)
{
    if ((s1 == NULL) || (s2 == NULL)) return -1;
    //int matchChar = 0;
    int i1 = 0, i2 = 0;
    int lcount = 0;
    int l1 = strlen(s1);
    int l2 = strlen(s2);

    if (l1 != l2) return 0;
    while(lcount < 2) {
      //  if(!matchChar) i2 = 0;
        if (s2[i2] == '\0') return 1;

        if (s1[i1] == s2[i2]) i2++;
        else i2 = 0;

        i1++;
        if (s1[i1] == '\0') {i1 = 0; lcount++;}
    }

    return 0;
}

void TestStrRotatedVersion(void)
{
    int T = 0;
    int i = 0;
    char **str = NULL;

    scanf(" %d", &T);
    T *= 2;
    str = (char **)malloc(sizeof (char *)*T);
    for (i = 0; i < T; i++) {
        str[i] = (char *)malloc(sizeof(char)*101);
        scanf(" %[^\n]", str[i]);
    }

    for (i = 0; i < T; i+=2) {
        printf("%d\n", StrRotatedVersion(str[i], str[i+1]));
        free(str[i]);
        free(str[i+1]);
    }
    free(str);
}

int main()
{
    TestStrRotatedVersion();
}
