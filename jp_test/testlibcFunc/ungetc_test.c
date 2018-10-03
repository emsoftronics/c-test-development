/* ungetc.c -- pushes a character back on the stream */
/* int ungetc(int c, FILE *fp) */
 
#include <stdio.h>
int main(void)
{
    int ch;
 
    /* reads characters from the stdin and show them on stdout */
    /* until encounters '1' */
 
    while ((ch = getchar()) != '1')
        putchar(ch);
 
    /* ungetc() returns '1' previously read back to stdin */
    ungetc(ch, stdin);
 
    /* getchar() attempts to read next character from stdin */
    /* and reads character '1' returned back to the stdin by ungetc() */
    ch = getchar();
 
//    ungetc(ch, stdin);
//    ch = getchar();
    /* putchar() displays character */
    putchar(ch);
    puts("");
 
    printf("Thank you!\n");
    return 0;
}
