#include "UtilMngDb.h"
#include <string.h>
#define DEMO_DB (UI_8 *)"demo.db"

t_util_db my_db = UM_DB_INIT;

typedef struct 
{
    int a;
    char b;
    char name[20];
    unsigned long int l;
} t_test;

t_test td = {1880,'A',"jagdish Prajapti", 7898491071};


BOOL flag = FALSE;

/**
 * @brief This is callback which will be called automatically each time for 
 * every row return by query execution.
 * @param ex_buf external buffer
 * @param argc no of colums return
 * @param argv values corresponding column
 * @param colname name of corresponding column
 *
 */

typedef struct {
    int sid;
    int cid;
    char name[32];
    int length;
    char type[15];
    void *value;
} t_my_table;

void CreateTable(t_util_db *db, UI_8 *tablename)
{
    UtilMngCreateDbTable(&my_db, tablename, NULL,0);    
    UtilMngAddDbColumn(&my_db, tablename, "sid",UM_DB_INTEGER);
    UtilMngAddDbColumn(&my_db, tablename, "cid",UM_DB_INTEGER);
    UtilMngAddDbColumn(&my_db, tablename, "name",UM_DB_TEXT);
    UtilMngAddDbColumn(&my_db, tablename, "length",UM_DB_INTEGER);
    UtilMngAddDbColumn(&my_db, tablename, "type",UM_DB_TEXT);
    UtilMngAddDbColumn(&my_db, tablename, "value",UM_DB_BINARY);
}

void UpdateData(t_util_db *db, UI_8 *tablename, t_my_table *data, UI_32 bin_bytes)
{
    char query[256];
    SI_32 ret = -1;
    SI_32 id = 0;

    t_Util_recrd_col_val record = {"id", &id, UM_DB_INTEGER, "value", data->value, UM_DB_BINARY, bin_bytes};

    sprintf(query, "UPDATE [%s] SET [name]=\'%s\', [length]=%ld, [type]=\'%s\' WHERE [sid]=%ld AND [cid]=%ld; "
            "INSERT INTO [%s] ([sid], [cid],[name],[length], [type]) "
            "SELECT %ld, %ld, \'%s\',%ld,\'%s\' WHERE (Select Changes() = 0);",
            tablename, data->name, data->length, data->type, data->sid, data->cid,
            tablename, data->sid, data->cid,data->name, data->length, data->type);
    
    UtilMngExecQuery(db, query, NULL);

     
    sprintf(query, "SELECT [id] FROM [%s] WHERE [sid]=%ld AND [cid]=%ld;",
            tablename, data->sid, data->cid);
     
    UM_DB_FOREACH_ROW(db,tablename,query, 1)
    {
        ret = UtilMngGetDataByColName(db , "id", &id, sizeof(id));
    }
    if (ret >= 0)
    {
        UtilMngPutDataToDb(db, tablename, &record);
    }
}

void GetData(t_util_db *db, UI_8 *tablename, t_my_table *data, UI_32 bin_bytes)
{
    char query[100];
    SI_32 ret = -1;

    sprintf(query, "SELECT * FROM [%s] WHERE [sid]=%ld AND [cid]=%ld;",
            tablename, data->sid, data->cid);
    UM_DB_FOREACH_ROW(db,tablename,query, 1)
    {
        ret = UtilMngGetDataByColName(db , "name", data->name, sizeof(data->name));
        ret = UtilMngGetDataByColName(db , "length", &(data->length), sizeof(data->length));
        ret = UtilMngGetDataByColName(db , "type", data->type, sizeof(data->type));
        ret = UtilMngGetDataByColName(db , "value", data->value, bin_bytes);
    }
    if (ret >= 0)
    {
        printf("sid=%d, cid=%d, name=%s, length=%d, type=%s, value=",
                data->sid, data->cid,data->name, data->length, data->type);
    }
    
}


static int db_callback(void * ex_buf, int argc, char **argv, char **colname)
{
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

int main(int argc, char **argv )
{
    int id = 2;
    t_test tmp ={0,0,{0},0};
 //   char dt[] = "2016-09-24 00:20:55"; 
    t_Util_recrd_col_val rcv ={"id", &id, UM_DB_INTEGER, "data", &td, UM_DB_BINARY,sizeof(td)};
     if (UtilMngInitDb(&my_db, DEMO_DB, db_callback) != 0)
         printf("Database initialization failed!!\n");
     if (argc == 2)
     {
         
        flag =TRUE;
        if (UtilMngExecQuery(&my_db, argv[1], NULL) != 0)
            printf("Database query failed!!\n");
        flag =FALSE;
     }
     else if (argc > 2)
     {
        if (strcmp(argv[1], "createtbl") == 0)
        {
            //UtilMngCreateDbTable(&my_db,argv[2], NULL,0);
            CreateTable(&my_db,argv[2]);
        }
        else if (argc > 5 && strcmp(argv[1], "addcol") == 0)
        {
            UtilMngAddDbColumn(&my_db,argv[2],argv[3],atoi(argv[4]));
        }
        else if (strcmp(argv[1], "insertdata") == 0)
        {
                id = 2;
                char a[20]="jagdish prajapati";
                t_my_table t = {1,2,"test_name",sizeof(id),"int",a};
                //dt[12] = 0x30+id;
                td.a += id;
                td.b += id;
                td.l += id;
                sprintf(td.name,"data%d",id);
                //UtilMngPutDataToDb(&my_db, "my_data", &rcv);
                UpdateData(&my_db, argv[2], &t, sizeof(a));
                id=5;
                strcpy(a,"hello");
                printf("a=%s\n", a);
                GetData(&my_db, argv[2], &t, sizeof(a));
                printf("%s\n", a);
        }
        else {
            UM_DB_FOREACH_ROW(&my_db,"my_data",NULL, 1000U)
            {
                UtilMngGetDataByColName(&my_db,"data",&tmp,sizeof(tmp));
                printf(" %u.  a=%d,  b=%c,  name=\'%s\', l=%ld\n",
                (unsigned int)UM_DB_ROW_NUMBER(&my_db) ,tmp.a,tmp.b,tmp.name,tmp.l);
            }
        }
     }
     else
     {
        printf("Query argument is required!!\n");
     }
    return 0;
}
