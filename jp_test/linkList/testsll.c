/*
 * =====================================================================================
 *
 *       Filename:  testsll.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Friday 28 September 2018 10:52:32  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include "linklist.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define UNUSED(x)   (void)(x);

static slist_t list = SLLIST_INITIALIZER;

static int mycomp(sliteminfo_t *x, sliteminfo_t *y)
{
    return strcmp(x->item,y->item);
}

int main(int argc, char **argv)
{
    char buffer[20];
    int length = 0;
    int choice, i;

    UNUSED(argc);
    UNUSED(argv);
    while(1) {
        printf("\nChoices :-\n\t 1. add string to head in list.\n"
        "\t 2. add string to tail in list.\n\t 3. remove string from head in list\n"
        "\t 4. remove string from tail in list\n\t 5. print linklist\n"
        "\t 6. sort list in ascending order.\n\t 7. sort list in descending order.\n\t 8. exit.\n");
        printf("Enter the choice (1-6): ");
        fflush(stdout);
        scanf(" %d", &choice);
        switch(choice)
        {
            case 1:
                printf("Enter string to add at head in list: ");
                scanf(" %[^\n]", buffer);
                sll_addAtHead(&list, buffer, sizeof(buffer), 0);
                break;
            case 2:
                printf("Enter string to add at tail in list: ");
                scanf(" %[^\n]", buffer);
                sll_addAtTail(&list, buffer, sizeof(buffer), 0);
                break;
            case 3:
                length = sll_removeFromHead(&list, buffer, 0);
                printf("removed string from head with buffer size %d: %s\n", length, buffer);
                break;
            case 4:
                length = sll_removeFromTail(&list, buffer, 0);
                printf("removed string from tail with buffer size %d: %s\n", length, buffer);
                break;
            case 5:
                i = 0;
                for (length = sll_getListItem(ITEM_BEGIN, &list, buffer, 0); length > 0;
                        length = sll_getListItem(ITEM_NEXT, &list, buffer, 0)) printf(" %d. %s\n", ++i, buffer);
                fflush(stdout);
                break;
            case 6:
                sll_sortList(&list, mycomp, 0);
                printf("sorting done in ascending order!!\n");
                break;
            case 7:
                sll_sortList(&list, mycomp, 1);
                printf("sorting done in descending order!!\n");
                break;
            case 8:
                exit(0);
                break;
        }
    }
}

