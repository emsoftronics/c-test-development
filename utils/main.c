/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Wednesday 27 December 2017 10:48:59  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include "StringUtil.h"
//#include "IntUtil.h"
#include <stdio.h>
#include <string.h>
//#include "StructUtil.h"
#include "DateUtil.h"
#include "RegUtil.h"
#if 0
//struct information my_info[10];

int main(int argc, char *argv[])
{
    int i;
    int len;
    int last_id = 0;
    //int fd = open("mydata.bin", O_RDWR | O_CREAT, 0666);
    //if (fd < 0) {
    //    printf("Unable to open the file.\n");
    //    return;
    //}
/*
    len = read(fd, (char *)my_info, sizeof(my_info));
    if ((len < 0) || (len % sizeof(struct information) !=0)) {
        printf("Read failed!\n");
    }


    for ( i = 0; (i < 10) && (my_info[i].id != 0); i++)
         last_id = my_info[i].id;

    last_id = getMultiRecord(&my_info[last_id], last_id);

    for(i=0; i<last_id; i++)
    {
        printRecord(&my_info[i]);
    }
*/
    /*
    last_id = getMultiRecord(&my_info[last_id], last_id);
    for(i=0; i<last_id; i++)
    {
        printRecord(&my_info[i]);
    }
    */
char buffer[256];
FILE *myfile = fopen(argv[1], "r");
//getUsrStr(buffer);
while(fgets(buffer, 256, myfile) != NULL) {
    printf("%s\n", buffer);
}
fclose(myfile);
//memset(buffer,0,sizeof(buffer));
}
#endif

enum Days {
    Sun = 0,
    Mon,
    Tue,
    Wed,
    Thu,
    Fri,
    Sat
};

typedef enum Days Days_t;

enum Months {
    Jan = 1,
    Feb,
    Mar,
    Apr,
    May,
    Jun,
    Jul,
    Aug,
    Sep,
    Oct,
    Nov,
    Dec
};

typedef enum Months Months_t;

void printFullNameOfWeekday(Days_t day)
{
    printf("\n%s\n", WeekDay[day]);
}

int main()
{
    /*DU_Date_t mydate = {Tue, 12, Aug, 90, 3,20};
    //getUserDate(&mydate);
    printDate(&mydate);
    printFullNameOfWeekday(Dec);*/

    dword_t mydata = {0};
    printf("Enter any unsigned integer: ");
    scanf(" %u", &(mydata.reg32));

    printf("byte0 = 0x%08x\n",mydata.reg32);

    printf("Binary of byte0: %d%d%d%d%d%d%d%d\n",mydata.dword.byte0.bits.b7,
            mydata.dword.byte0.bits.b6,
            mydata.dword.byte0.bits.b5,
            mydata.dword.byte0.bits.b4,
            mydata.dword.byte0.bits.b3,
            mydata.dword.byte0.bits.b2,
            mydata.dword.byte0.bits.b1,
            mydata.dword.byte0.bits.b0);

    return 0;
}
