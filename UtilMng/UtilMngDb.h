
#ifndef __UTILMNGDB_H__
#define __UTILMNGDB_H__
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include "Global.h"

/**
 * UM_DB_FAILURE macro defines failure status.
 */
#define UM_DB_FAILURE           (SI_32)(-1)

/**
 * UM_DB_SUCCESS macro defines success status.
 */
#define UM_DB_SUCCESS           (SI_32)(0)

#define UM_DB_ERROR(database)  printf("%s: %s\n", __func__, sqlite3_errmsg((database)))

#define UM_DB_INIT              {{0},{0}, NULL, NULL, NULL}


typedef int (*t_util_db_callback)(void *, int, char**, char**);

typedef struct {
    UI_8 db_filename[100];
    UI_8 sql_query[512];
    sqlite3 *database;
    sqlite3_stmt *sql_statement;
    t_util_db_callback callback;
} t_util_db;


SI_32 UtilMngInitDb(t_util_db *db, const UI_8 *db_filename, t_util_db_callback callback);

SI_32 UtilMngExecDbQuery(t_util_db *db, const UI_8 *query, void *cb_buf);

SI_32 UtilMngPutDataToDb(t_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name, void *data_block, UI_32 data_size);

SI_32 UtilMngGetDataFromDb(t_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name, void *out_buf, UI_32 buf_size);

SI_32 UtilMngDelDataFromDb(t_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name);

SI_32 UtilMngCountDbRows(t_util_db *db, const UI_8 *db_tablename);

SI_32 UtilMngDropTableFromDb(t_util_db *db, const UI_8 *db_tablename);

SI_32 UtilMngPushDataToDb(t_util_db *db, const UI_8 *db_tablename,
        void *data_block, UI_32 data_size, UI_32 max_limit);

SI_32 UtilMngPopFrstDataFromDb(t_util_db *db, const UI_8 *db_tablename,
        void *out_buf, UI_32 buf_size);
#endif/*  __UTILMNGDB_H__ */
