/*
 * =====================================================================================
 *
 *       Filename:  sqlite-demo.c
 *
 *    Description:  demo program.
 *
 *        Version:  1.0
 *        Created:  Sunday 01 January 2017 12:50:55  IST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <sqlite3.h>

#define MY_DB "mydb.db"

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
  }

typedef int (*t_db_callback)(void *, int, char **, char **);

typedef struct {
	char filename[100];
	t_db_callback callback;
	sqlite3 *db;
	sqlite3_stmt *stmt;
} t_util_db;


int dbopen(t_util_db *db)
{
    if (db->db == NULL)
    {
     rc = sqlite3_open(db->filename, &db->db);
     if( rc ){
       fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db->db));
       sqlite3_close(db->db);
       return(1);
     }
    }
    return 0;
}

void dbclose(t_util_db *db)
{
    if (db->db != NULL)
    {
    	sqlite3_close(db->db);
	    db->db = NULL;
    }
}

void dbinit(t_util_db *db, char *filename, t_db_callback callback)
{
	db->db = NULL;
	sprintf(db->filename,"%s",filename);
	db->callback = callback;
	db->stmt = NULL;
}

int dbcreatetable(t_util_db *db, char *tablename)
{
	char* q[250];
	int rc = 0;

	if(dbopen(db) == 0)
	{
		sprintf(q,"CREATE TABLE IF NOT EXISTS [%s] (id INTEGER PRIMARY KEY, BLOB data);", tablename);
		sqlite3_prepare_v2(db->db,q,-1,&db->stmt, 0);
		if (rc == SQLITE_OK)
		{
			if(sqlite3_step(db->stmt) != SQLITE_OK)
				printf("sql error!!\n");
		}
		dbclose(db);
		sqlite3_finalize(db->stmt);
		db->stmt = NULL;
	}
	else dbclose(db);
}

int dbputdata(t_util_db *db, char *tablename, int id, void *data, int len)
{
	char* q[250];
	int rc = 0;

	if(dbopen(db) == 0)
	{
		sprintf(q,"CREATE TABLE IF NOT EXISTS [%s] (id INTEGER PRIMARY KEY, BLOB data);", tablename);
		sqlite3_prepare_v2(db->db,q,-1,&db->stmt, 0);
		if (rc == SQLITE_OK)
		{
			if(sqlite3_step(db->stmt) != SQLITE_OK)
				printf("sql error!!\n");
		}
		dbclose(db);
		sqlite3_finalize(db->stmt);
		db->stmt = NULL;
	}
	else dbclose(db);
}


t_util_db my_db;

  int main(int argc, char **argv){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    if( argc!=3 ){
      fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
      return(1);
    }
    rc = sqlite3_open(argv[1], &db);
    if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      sqlite3_close(db);
      return(1);
    }
    rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
    if( rc!=SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
    }
    sqlite3_close(db);
     return 0;
  }
