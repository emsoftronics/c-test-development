#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

#define DB_FILE "demo.db"


static int DbCallback(void *out_buf, int argc, char **argv, char **Colname)
{
    int i = (int)0;
    if (out_buf != NULL)
    {
        for (i = 0; i < argc; i++)
        {
           // printf("Callback:%d\n",i);
            if (strcmp(Colname[i], "data") == 0)
            {
           //     printf("{ %s:%s }\n",Colname[i], argv[i] );
                strcpy(out_buf,argv[i]);
            }
        }
    }
    return 0;
}


int PushToDb( char *listname, char *data, unsigned int max_limit)
{
    sqlite3 *db = NULL;
    char query[500] = {0};
    int ret_val = (int)(-1);
    char *errmsg = NULL;

    ret_val = sqlite3_open_v2(DB_FILE, &db,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                    | SQLITE_OPEN_FULLMUTEX, NULL);
    if (ret_val == SQLITE_OK)
    {
        snprintf((char *)query, sizeof(query),
                "CREATE TABLE IF NOT EXISTS [%s] ("
                "[timestamp] DATETIME DEFAULT CURRENT_TIMESTAMP, "
                "[data] TEXT DEFAULT \'\');"
                "DELETE FROM \'%s\' WHERE "
                "(SELECT COUNT(*) FROM [%s])>%u AND [timestamp]="
                "(SELECT [timestamp] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1); "
                "INSERT INTO [%s] ([data]) VALUES(\'%s\');"
                ,
                 listname, listname, listname, (max_limit - 1), listname,
                 listname, data);
        ret_val = sqlite3_exec(db, query, NULL, NULL, &errmsg);
        if (ret_val != SQLITE_OK)
        {
            printf("Error: %s\n", errmsg);
            ret_val = (int)(-1);
        }
        else
        {
            ret_val == (int)0;
        }
    }
    else
    {
        printf("Error: %s\n", sqlite3_errmsg(db));
        ret_val = (int)(-1);
    }
    sqlite3_close(db);
    return ret_val;
}


int PeekFromDb(char *listname, char *ret_data)
{
    sqlite3 *db = NULL;
    char query[100] = {0};
    int ret_val = (int)(-1);
    char *errmsg = NULL;

    ret_val = sqlite3_open_v2(DB_FILE, &db,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                    | SQLITE_OPEN_FULLMUTEX, NULL);
    if (ret_val == SQLITE_OK)
    {
        snprintf((char *)query, sizeof(query),
                 "SELECT [data] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1;",
                 listname);
        ret_val = sqlite3_exec(db, query, DbCallback, ret_data, &errmsg);
        if (ret_val != SQLITE_OK)
        {
            printf("Error: %s\n", errmsg);
            ret_val = (int)(-1);
        }
        else
        {
            ret_val == (int)0;
        }
    }
    else
    {
        printf("Error: %s\n", sqlite3_errmsg(db));
        ret_val = (int)(-1);
    }
    sqlite3_close(db);
    return ret_val;
}

int PopFromDb(char *listname, char *ret_data)
{
    sqlite3 *db = NULL;
    char query[200] = {0};
    int ret_val = (int)(-1);
    char *errmsg = NULL;

    ret_val = sqlite3_open_v2(DB_FILE, &db,
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                    | SQLITE_OPEN_FULLMUTEX, NULL);
    if (ret_val == SQLITE_OK)
    {
        snprintf((char *)query, sizeof(query),
                 "SELECT [data] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1;"
                 "DELETE FROM [%s] WHERE [timestamp]="
                "(SELECT [timestamp] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1); ",
                 listname, listname, listname);
        ret_val = sqlite3_exec(db, query, DbCallback, ret_data, &errmsg);
        if (ret_val != SQLITE_OK)
        {
            printf("Error: %s\n", errmsg);
            ret_val = (int)(-1);
        }
        else
        {
            ret_val == (int)0;
        }
    }
    else
    {
        printf("Error: %s\n", sqlite3_errmsg(db));
        ret_val = (int)(-1);
    }
    sqlite3_close(db);
    return ret_val;
}

char data_buf[1024];

void main(int argc, char **argv)
{

    if (argc > 1)
    {
        if (strcmp(argv[1],"push") == 0)
        {
            if (PushToDb("data_table", argv[2], atoi(argv[3])) < 0)
                printf("Error: Push function failed!!\n");
        }
        else if (strcmp(argv[1],"peek") == 0)
        {
             memset(data_buf,0, sizeof(data_buf));
            if (PeekFromDb("data_table", data_buf) < 0)
            {
                printf("Error: Peek function failed!!\n");
            }
            else
            {
                printf("Return Data: \n%s\n",data_buf);
            }
        }
        else if (strcmp(argv[1],"pop") == 0)
        {
            memset(data_buf,0, sizeof(data_buf));
            if (PopFromDb("data_table", data_buf) < 0)
            {
                printf("Error: Pop function failed!!\n");
            }
            else
            {
                printf("Return Data: \n%s\n",data_buf);
            }
        }
        else
        {
            /* Nothing*/
        }
    }
}
