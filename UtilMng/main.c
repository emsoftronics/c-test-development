/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Sunday 15 January 2017 12:03:19  IST
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
#include "DataDownloadMngDbUtil.h"
#define DBASE "mytest.db"

t_ddm_util_db udb = DATADOWNLOADMNG_DB_UTIL_INIT;


static int db_callback(void * ex_buf, int argc, char **argv, char **colname)
{
    static BOOL flag = TRUE;
    int i;

    if (flag)
    {
        printf("---------------------------------------------------------------------------------------\n");
        for(i=0; i<argc; i++){
            printf(" %12s |", colname[i]);
        }
        printf("\n---------------------------------------------------------------------------------------\n");
        flag = FALSE;
    }
    for(i=0; i<argc; i++){
        printf(" %12s |",  argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

char buf[200] = {0};
void main(int argc, char **argv)
{
    t_ddm_util_db_data db_data = DATADOWNLOADMNG_DB_DATA_INIT;
    t_ddm_util_db_data *dbdptr = NULL;
    if (DataDownloadMngInitDb(&udb,DBASE,db_callback))
    {
        printf("Error: Db init failed!!\n");
        return;
    }

    DataDownloadMngCreateDbTable(&udb,"testdata");

    if (argc == 1)
    {
        DataDownloadMngExecDbQuery(&udb, "select * from testdata;", NULL);
    }
    else if (argc == 2 )
    {
        printf("Total Rec = %d\n", (int)DataDownloadMngCountDbRows(&udb,"testdata"));
    }
    else if (argc ==7)
    {
        db_data.id1 = (SI_32)atoi(argv[2]);
        db_data.id2 = (SI_32)atoi(argv[3]);
        db_data.flag = ((SI_32)atoi(argv[4]) > 0)? TRUE :FALSE;
        strcpy(db_data.name,argv[5]);
        strcpy(db_data.time,argv[6]);
        db_data.data_buffer = &db_data;
        db_data.data_length = sizeof(db_data);
        if (argv[1][0] == 'p')
        {
             DataDownloadMngPutRecDataToDb(&udb,"testdata", &db_data, 5);
        }
        else if (argv[1][0] == 'g')
        {
            db_data.data_buffer = &buf;
             DataDownloadMngGetRecDataFrmDb(&udb,"testdata", &db_data);

            dbdptr = db_data.data_buffer;
            printf("id1 = %d\nid2 = %d\n flag = %s\nname = %s\ntime = %s",
                    dbdptr->id1, dbdptr->id2, ((dbdptr->flag)? "TRUE": "FALSE"), dbdptr->name, dbdptr->time);
        }
        else if (argv[1][0] == 'd')
        {
            db_data.data_buffer = NULL;
             DataDownloadMngDelRecDataFromDb(&udb,"testdata", &db_data);

        }
    }


}
