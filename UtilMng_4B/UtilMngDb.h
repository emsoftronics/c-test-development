
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

/**
 * UM_DB_SESSION_STARTED macro defines the status of started session for
 * database access.
 */
#define UM_DB_SESSION_STARTED   (SI_32)(0x0ff1)

/**
 * UM_DB_ROW_AVAILABLE macro defines the status of availability of queried
 * record in database.
 */
#define UM_DB_ROW_AVAILABLE     (SI_32)(0x0ff2)

/**
 * UM_DB_NO_MORE_ROW macro defines the status of no any queried record is
 * available.
 */
#define UM_DB_NO_MORE_ROW       (SI_32)(0x0ff3)

/**
 * UM_DB_SESSION_ENDED macro defines the status of database access termination.
 */
#define UM_DB_SESSION_ENDED     (SI_32)(0x0ff4)

/**
 * UM_DB_INIT macro defines t_util_db instance initializing value.
 */
#define UM_DB_INIT              {{0}, NULL, NULL, FALSE, SQLITE_OK, 0, NULL}

/**
 * @brief UM_DB_FOREACH_ROW macro is similar to for loop, it traverses all
 * possible records selected by query <i>query_str</i> in table <i>tablename</i>
 * and records traversal can be limited by <i>max_row</i>. This macro will be
 * followed by {} block for internal loop statements.
 *
 * @par Usage Syntax:
 *  @code
 *  #include "UtilMngDb.h"
 *
 *  SI_32 id = 0;
 *  t_Util_db my_db = UM_DB_INIT;
 *
 *  function()
 *  {
 *      UtilMngInitDb(&my_db, "mydbfile.db", NULL);
 *
 *      UM_DB_FOREACH_ROW(&db,"mytable",NULL, 1)
 *      {
 *        ret = UtilMngGetDataByColName(&db , "id", &id, sizeof(id));
 *      }
 *  }
 *  @endcode
 *
 * @param util_db_ref    Reference to database file context.
 * @param tablename      Database table name.
 * @param query_str      SQL Query in string format.
 * @param max_row        Maximum no. of rows to be traversed.
 *
 */
#define UM_DB_FOREACH_ROW(util_db_ref, tablename, query_str, max_row)       \
                    for (UtilMngStartDbSession((t_util_db *)(util_db_ref),  \
                        (UI_8 *)(tablename),(UI_8 *)(query_str));           \
                        UtilMngDbNextRow((t_util_db *)(util_db_ref))        \
                        == UM_DB_ROW_AVAILABLE; UtilMngDbEndSession(        \
                        (t_util_db *)(util_db_ref), (UI_32)(max_row)))

#define UM_DB_ROW_NUMBER(util_db_ref) ((UI_32)((t_util_db *)(util_db_ref))->record_counter)

typedef int (*t_util_db_callback)(void *, int, char**, char**);

typedef enum {
    UM_DB_INTEGER = 0,
    UM_DB_TEXT,
    UM_DB_FLOAT,
    UM_DB_BINARY,
    UM_DB_DATETIME
} t_db_col_type;


typedef enum {
    UM_DB_NULL = 0,
    UM_DB_PRIMARY_KEY,
    UM_DB_PRIMARY_KEY_AUTOINCREMENT,
    UM_DB_NOT_NULL,
    UM_DB_UNIQUE,
    UM_DB_DEFAULT_0,
    UM_DB_DEFAULT_EMPTY,
    UM_DB_DEFAULT_CURRENT_TIMESTAMP,
    UM_DB_DEFAULT_EMPTY_DATETIME,
} t_db_col_constraint;

typedef struct {
    UI_8 db_filename[100];
    sqlite3 *database;
    sqlite3_stmt *sql_statement;
    BOOL initiated;
    SI_32 ret_status;
    UI_32 record_counter;
    t_util_db_callback callback;
} t_util_db;

typedef struct {
    UI_8 search_col_name[20];
    void *search_col_val_ptr;
    t_db_col_type search_col_type;
    UI_8 process_col_name[20];
    void *process_col_val_ptr;
    t_db_col_type process_col_type;
    UI_32 process_col_data_len;
} t_Util_recrd_col_val;



LE_SHARED SI_32 UtilMngInitDb(t_util_db *db, UI_8 *db_filename,
        t_util_db_callback callback);

LE_SHARED void UtilMngDbEndSession(t_util_db *db, UI_32 max_row);

LE_SHARED SI_32 UtilMngCreateDbTable(t_util_db *db, UI_8 *db_tablename,
        UI_8 *unique_column_name, t_db_col_type column_type);

LE_SHARED SI_32 UtilMngAddDbColumn(t_util_db *db, UI_8 *db_tablename,
        UI_8 *column_name, t_db_col_type column_type);

LE_SHARED SI_32 UtilMngExecQuery(t_util_db *db, UI_8 *query, void *cb_buf);

LE_SHARED SI_32 UtilMngPutDataToDb(t_util_db *db, UI_8 *db_tablename,
        t_Util_recrd_col_val *process_db_block);

LE_SHARED SI_32 UtilMngStartDbSession(t_util_db *db, UI_8 *db_tablename, UI_8 *db_query);

LE_SHARED SI_32 UtilMngDbNextRow(t_util_db *db);

LE_SHARED SI_32 UtilMngGetDataByColName(t_util_db *db , UI_8 *col_name, void *storage,
        UI_32 storage_size);

#endif /* end of __UTILMNGDB_H__ */

