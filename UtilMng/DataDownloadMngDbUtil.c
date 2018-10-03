/*********  Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS  ******//**
*  @par Language: MISRA C
*  @par Controller: AR7554
*  @par Specification:
*       <a href="https://sqlite.org/capi3ref.html">
*       C-language Interface Specification for SQLite
*       </a>
********************************************************************************
*  @par Project: TCU E4.0
*  @brief   This file contains database or persistent memory related all static
*           and shared function definitions and internally used macros.
*  @author  Jagdish Prajapati(ee.jprajapati@ficosa.com)
*  @version 1.0
*  @par Revision history:
*
*  17/01/17 - Jagdish Prajapati(ee.jprajapati@ficosa.com) Creation of the file.
*
*  @file DataDownloadMngDbUtil.c
*  @copyright Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS
*******************************************************************************/

/* -------------------------------- Includes -------------------------------- */
#include "DataDownloadMngDbUtil.h"

/* -------------------------------- Defines --------------------------------- */
/**
 * DB_LOCK macro is defined to specify for acquiring of mutex for
 * synchronization purpose to avoid race condition.
 */
#define DB_LOCK(utildb)  /*  ({if ((utildb) != NULL) {                 \
                                le_mutex_Lock((utildb)->db_lock);   \
                            }})*/

/**
 * DB_UNLOCK macro is defined to specify for releasing of mutex for
 * synchronization purpose to avoid race condition.
 */
#define DB_UNLOCK(utildb)  /*({if ((utildb) != NULL) {                 \
                                le_mutex_Unlock((utildb)->db_lock);  \
                            }})*/
/**
 * DATADOWNLOADMNG_DB_UTIL_ERROR macro defines database error log method.
 */
/*#define DATADOWNLOADMNG_DB_UTIL_ERROR(database)  SendDebug(DEBUG_INFO,  \
        DEBUG_ID, "@FDDMDBU :  %s\n", sqlite3_errmsg((database)))*/
#define DATADOWNLOADMNG_DB_UTIL_ERROR(database)  printf("@FDDMDBU :  %s\n", sqlite3_errmsg((database)))

/* ------------------------------- Data Types ------------------------------- */

/* ---------------------------- Global Variables ---------------------------- */

/* --------------------------- Routine prototypes --------------------------- */

/* -----------------------------Static Routines ----------------------------- */

#if 0
static SI_32 DataDownloadMngMakeDirectory (const UI_8 *path, mode_t mode)
{
    struct stat st;
    SI_32 status = 0;

    if (stat((char *)path, &st) != 0) {
        /* Directory does not exist. EEXIST for race condition */
        if (mkdir((char *)path, mode) != 0 && errno != EEXIST)
            status = -1;
    }
    else if (!S_ISDIR(st.st_mode))
    {
        errno = ENOTDIR;
    status = -1;
    }

    return (status);
}

SI_32 DataDownloadMngMakePath (const UI_8 *path, mode_t mode)
{
    UI_8 *pp;
    UI_8 *sp;
    SI_32 status;
    UI_8 *copypath = (UI_8 *)strdup((char *)path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = (UI_8 *)strchr((char *)pp, '/')) != 0)
    {
        if (sp != pp)
        {
            /* Neither root nor double slash in path */
            *sp = (UI_8)('\0');
            status = DataDownloadMngMakeDirectory(copypath, mode);
            *sp = (UI_8)('/');
        }
        pp = sp + 1;
    }
    if (status == 0)
        status = DataDownloadMngMakeDirectory(path, mode);

    free(copypath);
    return (status);
}
#endif

static SI_32 DataDownloadMngCloseDb(t_ddm_util_db *db)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    if (db != NULL) {
        DB_LOCK(db);
        if (db->database != NULL) {
            sqlite3_close(db->database);
            db->database = NULL;
        }
        status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
        DB_UNLOCK(db);
    }

    return status;
} /* DataDownloadMngCloseDb */


static SI_32 DataDownloadMngOpenDb(t_ddm_util_db *db)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    if (db != NULL) {
        DB_LOCK(db);
        if (db->database == NULL) {
            status = sqlite3_open_v2((char *)db->db_filename, &(db->database),
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                    | SQLITE_OPEN_FULLMUTEX, NULL);
            if (status != SQLITE_OK) {
                DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
                DataDownloadMngCloseDb(db);
            }
            else {
                status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
            }
        }
        else {
            status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
        }
        DB_UNLOCK(db);
    }

    return status;
} /* DataDownloadMngOpenDb */

/* --------------------------------- Routines ------------------------------- */

/**
 * @brief Initialize database.
 * @par Operations contract:
 *  This function initializes the database to work properly.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_filename   Database file path.
 * @param[in] callback      Database callback if using ExecDbQuery function.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngInitDb(t_ddm_util_db *db, const UI_8 *db_filename, t_ddm_util_db_callback callback)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
    UI_8 mutexname[15] = {0};
    static UI_32 call_count = (UI_32)0;

    if ((db != NULL) && (db_filename != NULL)) {
        sqlite3_snprintf(sizeof(db->db_filename), (char *)db->db_filename,
                "%s", (char *)db_filename);
        sqlite3_snprintf(sizeof(mutexname), (char *)mutexname, "DB_%u_Lock",
                (unsigned int)(++call_count));
        memset(db->sql_query,0,sizeof(db->sql_query));
        db->database = NULL;
        db->sql_statement = NULL;
        /*if (db->db_lock != NULL) {
            le_mutex_Delete(db->db_lock);
            db->db_lock = NULL;
        }
        db->db_lock = le_mutex_CreateRecursive((char *)mutexname);*/
        db->callback = callback;
        status = DataDownloadMngOpenDb(db);
        DataDownloadMngCloseDb(db);
    }

    return status;
} /* DataDownloadMngInitDb */

/**
 * @brief Execute SQL query.
 * @par Operations contract:
 *  This function executes the SQL query provided as input in the string format.
 *
 * @param[in] db        Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] query     SQL query to execute.
 * @param[in] cb_buf    A buffer pointer as input to callback as first argument.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngExecDbQuery(t_ddm_util_db *db, const UI_8 *query, void *cb_buf)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
    UI_8 *errmsg = NULL;

    DB_LOCK(db);
    status = DataDownloadMngOpenDb(db);
    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        status = sqlite3_exec(db->database, (char *)query, db->callback, cb_buf,
                (char **)&errmsg);
        if (status != SQLITE_OK) {
            DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
            status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
        }
        else {
            status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
        }
    }
    db->sql_statement = NULL;
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngExecDbQuery */

/**
 * @brief Create Table.
 * @par Operations contract:
 *  This function creates table of fixed no. of column and types. But these
 *  columns accessed through provided some wrapper functions for getting the
 *  data for a record.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngCreateDbTable (t_ddm_util_db *db, const UI_8 *db_tablename)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "CREATE TABLE IF NOT EXISTS [%s] ("
                "[timestamp] DATETIME UNIQUE DEFAULT "
                "(STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\')), "
                "[id1] INTEGER NOT NULL DEFAULT (-1), "
                "[id2] INTEGER NOT NULL DEFAULT (-1), "
                "[flag] INTEGER CHECK( flag IN (0, 1)) NOT NULL DEFAULT 0, "
                "[time] DATETIME NOT NULL DEFAULT (\'0000-00-00 00:00:00.000\'), "
                "[name] TEXT NOT NULL DEFAULT \'\', [data] BLOB NULL);",
                 (char *)db_tablename);

        status = DataDownloadMngExecDbQuery(db, db->sql_query, NULL);
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
}

/**
 * @brief Put the data in database table.
 * @par Operations contract:
 *  This function inserts or updates the data in database table. If argument
 *  <b>var_name</b> is NULL then always it inserts the data in input table
 *  with current time stamp else it updates the data if a record is existing
 *  corresponding to <b>var_name</b> else it inserts new record corresponding to
 *  <b>var_name</b>.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[in] var_name      A unique name corresponding to data to insert.
 * @param[in] data_block    Data block reference to be stored.
 * @param[in] data_size     Size of data block to be stored.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngPutDataToDb (t_ddm_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name, void *data_block, UI_32 data_size)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
    SI_32 idx = (SI_32)0;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        if (var_name != NULL) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "INSERT INTO [%s] ([name]) SELECT %Q WHERE NOT EXISTS"
                "(SELECT 1 FROM [%s] WHERE [name]=%Q);",
                 (char *)db_tablename, (char *)var_name,
                 (char *)db_tablename, (char *)var_name);

            status = DataDownloadMngExecDbQuery(db, db->sql_query, NULL);
            if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
                status = DataDownloadMngOpenDb(db);
            }
        }

        if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
            if (var_name != NULL) {
                sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                        "UPDATE [%s] SET [data]=@data WHERE [name]=%Q;",
                        (char *)db_tablename, (char *)var_name);
            }
            else {
                sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                        "INSERT INTO [%s] ([name], [data]) VALUES("
                        "STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\'), @data);",
                        (char *)db_tablename);
            }

            status = sqlite3_prepare_v2(db->database, (char *)db->sql_query,
                    (int)(-1), &db->sql_statement, NULL);
            if (status != SQLITE_OK) {
                DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
            }
            else {
                idx  = sqlite3_bind_parameter_index(db->sql_statement, "@data");
                sqlite3_bind_blob(db->sql_statement, (int)idx,
                        data_block, data_size, SQLITE_STATIC);
                status = sqlite3_step(db->sql_statement);
                if ((status != SQLITE_OK) && (status != SQLITE_DONE)) {
                    DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                    status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
                }
                else {
                    status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
                }
            }
            sqlite3_finalize(db->sql_statement);
            db->sql_statement = NULL;
        }
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngPutDataToDb */

/**
 * @brief Get the data in database table.
 * @par Operations contract:
 *  This function retrieves the data from database table. If argument
 *  <b>var_name</b> is NULL then always it retrieves the data from input table
 *  which is inserted first according to time stamp else it retrieves the data
 *  corresponding to <b>var_name</b>.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[in] var_name      A unique name corresponding to data to retrieve.
 * @param[out] out_buf      A output buffer reference to get the data in.
 * @param[in] buf_size      Size of output buffer.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngGetDataFromDb(t_ddm_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name, void *out_buf, UI_32 buf_size)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        if (var_name != NULL) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "SELECT [data] FROM [%s] WHERE [name]=%Q "
                    "ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename, (char *)var_name);
        }
        else {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "SELECT [data] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename);
        }

        status = sqlite3_prepare_v2(db->database, (char *)db->sql_query,
                (int)(-1), &db->sql_statement, NULL);
        if (status != SQLITE_OK) {
                DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
        }
        else {
            status = sqlite3_step(db->sql_statement);
            if ((status == SQLITE_OK) || (status == SQLITE_DONE)
                    || (status == SQLITE_ROW)) {
                if (status == SQLITE_ROW) {
                    status = (SI_32)sqlite3_column_bytes(db->sql_statement, 0);
                    status = (status > (SI_32)buf_size)? (SI_32)buf_size : status;
                    memcpy(out_buf, sqlite3_column_blob (
                            db->sql_statement, 0), status);
                }
                else {
                    status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
                }
            }
            else {
                    DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                    status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;
    }

    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngGetDataFromDb */

/**
 * @brief Delete data form database table.
 * @par Operations contract:
 *  This function deletes the data from database table.If argument
 *  <b>var_name</b> is NULL then always it deletes the data from input table
 *  which is inserted first according to time stamp else it deletes the data
 *  corresponding to <b>var_name</b>.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[in] var_name      A unique name corresponding to data to delete.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngDelDataFromDb(t_ddm_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        if (var_name != NULL) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "DELETE FROM [%s] WHERE [name]=%Q;",
                 (char *)db_tablename, (char *)var_name);
        }
        else {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "DELETE FROM [%s] WHERE [timestamp]="
                "(SELECT [timestamp] FROM [%s] ORDER "
                "BY [timestamp] ASC LIMIT 1);",
                 (char *)db_tablename, (char *)db_tablename);
        }

        status = DataDownloadMngExecDbQuery(db, db->sql_query, NULL);
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngDelDataFromDb */

/**
 * @brief Get count of available data record in database table.
 * @par Operations contract:
 *  This function provides the no. of available data record in database table.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngCountDbRows(t_ddm_util_db *db, const UI_8 *db_tablename)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "SELECT COUNT(*) FROM [%s];",
                (char *)db_tablename);
        status = sqlite3_prepare_v2(db->database, (char *)db->sql_query, -1, &db->sql_statement, NULL);
        if (status != SQLITE_OK) {
                DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
        }
        else {
            status = sqlite3_step(db->sql_statement);
            if ((status == SQLITE_OK) || (status == SQLITE_DONE)
                    || (status == SQLITE_ROW)) {
                if (status == SQLITE_ROW) {
                    status = (SI_32)sqlite3_column_int (db->sql_statement, 0);
                }
                else {
                    status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
                }
            }
            else {
                    DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                    status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngCountDbRows */

/**
 * @brief Delete table from database.
 * @par Operations contract:
 *  This function deletes the table provided in input if it exists.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngDropTableFromDb(t_ddm_util_db *db, const UI_8 *db_tablename)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "DROP TABLE IF EXISTS [%s];",
                (char *)db_tablename);

        status = DataDownloadMngExecDbQuery(db, db->sql_query, NULL);
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngDropTableFromDb */

/**
 * @brief Push data in database table as FIFO.
 * @par Operations contract:
 *  This function pushes the data in database table in a FIFO queue. If no. of
 *  data block reaches to maximum limit specified by <b>max_limit</b> then it
 *  deletes first data block and pushes given data block at the end of queue.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[in] data_block    Data block reference to be pushed.
 * @param[in] data_size     Size of data block to be pushed
 * @param[in] max_limit     Maximum limit of FIFO.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.2yy
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngPushDataToDb(t_ddm_util_db *db, const UI_8 *db_tablename,
        void *data_block, UI_32 data_size, UI_32 max_limit)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
    SI_32 idx = (SI_32)0;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "DELETE FROM [%s] WHERE "
                "(SELECT COUNT(*) FROM [%s])>%u AND [timestamp]="
                "(SELECT [timestamp] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1);",
                 (char *)db_tablename, (char *)db_tablename,
                 (unsigned int)(max_limit - 1), (char *)db_tablename);
        status = DataDownloadMngExecDbQuery(db, db->sql_query, NULL);
        if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
            status = DataDownloadMngOpenDb(db);
        }

        if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                        "INSERT INTO [%s] ([name], [data]) VALUES("
                        "STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\'), @data);",
                        (char *)db_tablename);

            status = sqlite3_prepare_v2(db->database, (char *)db->sql_query,
                    (int)(-1), &db->sql_statement, NULL);
            if (status != SQLITE_OK) {
                DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
            }
            else {
                idx  = sqlite3_bind_parameter_index(db->sql_statement, "@data");
                sqlite3_bind_blob(db->sql_statement, (int)idx,
                        data_block, data_size, SQLITE_STATIC);
                status = sqlite3_step(db->sql_statement);
                if ((status != SQLITE_OK) && (status != SQLITE_DONE)) {
                    DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                    status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
                }
                else {
                    status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
                }
            }
            sqlite3_finalize(db->sql_statement);
            db->sql_statement = NULL;
        }
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngPushDataToDb */

/**
 * @brief Pop the first data block from FIFO queue.
 * @par Operations contract:
 *  This function returns the first data block from database table and deletes
 *  that data block from table.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[out] out_buf      Reference of output buffer for return data.
 * @param[in] buf_size      Size of output buffer.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngPopFrstDataFrmDb(t_ddm_util_db *db, const UI_8 *db_tablename,
        void *out_buf, UI_32 buf_size)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    status = DataDownloadMngGetDataFromDb(db, db_tablename, NULL, out_buf,
            buf_size);

    if (status >= (SI_32)0) {
        if (DataDownloadMngDelDataFromDb(db, db_tablename, NULL)
                == DATADOWNLOADMNG_DB_UTIL_FAILURE) {
            status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
        }
    }
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngPopFrstDataFrmDb */


/**
 * @brief Put record in to database table.
 * @par Operations contract:
 *  This function writes a record of six members as specified by by
 *  <b>t_ddm_util_db_data</b> data type in a database table.
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[in] db_data   A pointer to a record  of type <b>t_ddm_util_db_data</b>.
 * @param[in] max_limit     Maximum no. of record limit can be written.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngPutRecDataToDb (t_ddm_util_db *db, const UI_8 *db_tablename,
        t_ddm_util_db_data *db_data , UI_32 max_limit)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
    SI_32 idx = (SI_32)0;

    DB_LOCK(db);
    if ((db_tablename != NULL) && (db_data != NULL)) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                "DELETE FROM [%s] WHERE "
                "(SELECT COUNT(*) FROM [%s])>%u AND [timestamp]="
                "(SELECT [timestamp] FROM [%s] "
                "ORDER BY [timestamp] ASC LIMIT 1);"
                "UPDATE [%s] SET [id1]=%d, [id2]=%d, [flag]=%d, [name]=%Q, "
                "[time]=%Q WHERE (([name]=%Q) AND ([name]!=\'\')) "
                "OR (([id1]>=0) AND ([id1]=%d)) LIMIT 1;"
                "INSERT INTO [%s] ([id1], [id2], [flag], [name], [time]) "
                "SELECT %d, %d, %d, %Q, %Q WHERE (SELECT CHANGES()=0);",
                 (char *)db_tablename, (char *)db_tablename,
                 (unsigned int)(max_limit -1),
                 (char *)db_tablename, (char *)db_tablename,
                 (int)db_data->id1, (int)db_data->id2,
                 ((db_data->flag == FALSE) ? ((int)0) : ((int)1)),
                 (char *)db_data->name, (char *)db_data->time,
                 (char *)db_data->name, (int)db_data->id1,
                 (char *)db_tablename,
                 (int)db_data->id1, (int)db_data->id2,
                 ((db_data->flag == FALSE) ? ((int)0) : ((int)1)),
                 (char *)db_data->name, (char *)db_data->time
                 );

        status = DataDownloadMngExecDbQuery(db, db->sql_query, NULL);
        if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
            status = DataDownloadMngOpenDb(db);
        }

        if ((status == DATADOWNLOADMNG_DB_UTIL_SUCCESS)
                && (db_data->data_buffer != NULL)) {

            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "UPDATE [%s] SET [data]=@data WHERE "
                    "(([name]=%Q) AND ([name]!=\'\')) "
                    "OR (([id1]>=0) AND ([id1]=%d)) LIMIT 1;",
                    (char *)db_tablename,
                    (char *)db_data->name, (int)db_data->id1
                    );

            status = sqlite3_prepare_v2(db->database, (char *)db->sql_query,
                    (int)(-1), &db->sql_statement, NULL);
            if (status != SQLITE_OK) {
                DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
            }
            else {
                idx  = sqlite3_bind_parameter_index(db->sql_statement, "@data");
                sqlite3_bind_blob(db->sql_statement, (int)idx,
                        db_data->data_buffer, db_data->data_length,
                        SQLITE_STATIC);
                status = sqlite3_step(db->sql_statement);
                if ((status != SQLITE_OK) && (status != SQLITE_DONE)) {
                    DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                    status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
                }
                else {
                    status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
                }
            }
            sqlite3_finalize(db->sql_statement);
            db->sql_statement = NULL;
        }
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngPutRecDataToDb */

/**
 * @brief Get record from database table.
 *  This function provides complete record identified by any one of member of
 *  <b>t_ddm_util_db_data</b> (Only id1, id2, name) with valid value(>=0, >=0,
 *  non empty string respectively).
 *  Searching strategy:\n
 *  - If data searched by id1 then there should be id1 >=0, id2 = -1, name = "",
 *  flag=False, time = "" and data_buffer !=  NULL, and length != 0.
 *  - If data searched by id2 then there should be id2 >=0, id1 = -1, name = "",
 *  flag=False, time = "" and data_buffer !=  NULL, and length != 0.
 *  - If data searched by name then there should be id1 = -1, id2 = -1,
 *  name != "", flag=False, time = "" and data_buffer !=  NULL, and length != 0.
 *  - If data searched by time expired according to time column value then
 *  there should be id1 = -1, id2 = -1, name = "", flag=TRUE, time = "" and
 *  data_buffer !=  NULL, and length != 0.
 *  - If first inserted record is required then there should be id1 = -1 ,
 *  id2 = -1, name = "", flag=False, time = "" and data_buffer !=  NULL,
 *  and length != 0.
 *
 * @par Operations contract:
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[in] db_data   A pointer to a record  of type <b>t_ddm_util_db_data</b>.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngGetRecDataFrmDb (t_ddm_util_db *db, const UI_8 *db_tablename,
        t_ddm_util_db_data *db_data)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    if ((db_tablename != NULL) && (db_data != NULL)) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {
        if (db_data->id1 >= (SI_32)0) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "SELECT [id1], [id2], [flag], [name], [time], [data] "
                    "FROM [%s] WHERE [id1]=%d "
                    "ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename, (int)db_data->id1);
        }
        else if (db_data->id2 >= (SI_32)0) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "SELECT [id1], [id2], [flag], [name], [time], [data] "
                    "FROM [%s] WHERE [id2]=%d "
                    "ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename, (int)db_data->id2);
        }
        else if (db_data->name[0] != (UI_8)'\0') {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "SELECT [id1], [id2], [flag], [name], [time], [data] "
                    "FROM [%s] WHERE [name]=%Q "
                    "ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename, (char *)db_data->name);
        }
        else if (db_data->flag != FALSE) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "SELECT [id1], [id2], [flag], [name], [time], [data] "
                    "FROM [%s] WHERE STRFTIME(\'%%s\', [time])"
                    "<STRFTIME(\'%%s\', \'NOW\') "
                    "ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename, (char *)db_data->name);
        }
        else {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "SELECT [id1], [id2], [flag], [name], [time], [data] "
                    "FROM [%s] ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename);
        }

            printf("Q: %s\n", (char *)db->sql_query);
        status = sqlite3_prepare_v2(db->database, (char *)db->sql_query,
                (int)(-1), &db->sql_statement, NULL);
        if (status != SQLITE_OK) {
                DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
        }
        else {
            status = sqlite3_step(db->sql_statement);
            if ((status == SQLITE_OK) || (status == SQLITE_DONE)
                    || (status == SQLITE_ROW)) {
                if (status == SQLITE_ROW) {
                    db_data->id1 = sqlite3_column_int(db->sql_statement, 0);
                    db_data->id2 = sqlite3_column_int(db->sql_statement, 1);
                    db_data->flag = ((sqlite3_column_int(db->sql_statement, 2)
                            > 0) ? TRUE : FALSE);
                    sqlite3_snprintf(sizeof(db_data->name),
                            (char *)db_data->name, "%s",
                            sqlite3_column_text(db->sql_statement, 3));
                    sqlite3_snprintf(sizeof(db_data->time),
                            (char *)db_data->time, "%s",
                            sqlite3_column_text(db->sql_statement, 4));
                    status = (SI_32)sqlite3_column_bytes(db->sql_statement, 5);
                    status = (status > (SI_32)db_data->data_length)?
                        (SI_32)db_data->data_length : status;
                    if (db_data->data_buffer != NULL) {
                        memcpy(db_data->data_buffer, sqlite3_column_blob (
                            db->sql_statement, 5), status);
                        db_data->data_length = (UI_32)status;
                    }
                }
                status = DATADOWNLOADMNG_DB_UTIL_SUCCESS;
            }
            else {
                    DATADOWNLOADMNG_DB_UTIL_ERROR(db->database);
                    status = DATADOWNLOADMNG_DB_UTIL_FAILURE;
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;
    }

    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngGetRecDataFrmDb */


/**
 * @brief Delete record from database table.
 *  This function deletes complete record identified by any one of member of
 *  <b>t_ddm_util_db_data</b> (Only id1, id2, name) with valid value(>=0, >=0,
 *  non empty string respectively).
 *  Searching strategy:\n
 *  - If data searched by id1 then there should be id1 >= 0, id2 = -1, name = "",
 *  flag=False, time = "" and data_buffer =  NULL, and length = 0.
 *  - If data searched by id2 then there should be id2 >= 0, id1 = -1, name = "",
 *  flag=False, time = "" and data_buffer =  NULL, and length = 0.
 *  - If data searched by name then there should be id1 = -1, id2 = -1, name != "",
 *  flag=False, time = "" and data_buffer =  NULL, and length = 0.
 *  - If data searched by time expired according to time column value then
 *  there should be id1 = -1, id2 = -1, name = "", flag=TRUE, time = "" and
 *  data_buffer =  NULL, and length = 0.
 *  - if <i>db_search_data</i> argument is NULL then it will delete first
 *  inserted data record.
 *
 *
 * @param[in] db            Database utility object of type <b>t_ddm_util_db</b>.
 * @param[in] db_tablename  Database table name.
 * @param[in] db_search_data   A pointer to a record with search key data
 *                             of type <b>t_ddm_util_db_data</b>.
 *
 * @return
 *  - DATADOWNLOADMNG_DB_UTIL_SUCCESS: If database task executed successfully.
 *  - DATADOWNLOADMNG_DB_UTIL_FAILURE: If database task execution failed.
 */
SI_32 DataDownloadMngDelRecDataFromDb(t_ddm_util_db *db, const UI_8 *db_tablename,
        t_ddm_util_db_data *db_search_data)
{
    SI_32 status = DATADOWNLOADMNG_DB_UTIL_FAILURE;

    DB_LOCK(db);
    if (db_tablename != NULL) {
        status = DataDownloadMngOpenDb(db);
    }

    if (status == DATADOWNLOADMNG_DB_UTIL_SUCCESS) {

        if (db_search_data == NULL) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "DELETE FROM [%s] WHERE [timestamp]="
                    "(SELECT [timestamp] FROM [%s] ORDER "
                    "BY [timestamp] ASC LIMIT 1);",
                    (char *)db_tablename, (char *)db_tablename);
        }
        else if (db_search_data->id1 >= (SI_32)0) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "DELETE FROM [%s] WHERE [id1]=%d;",
                    (char *)db_tablename, (int)db_search_data->id1);
        }
        else if (db_search_data->id2 >= (SI_32)0) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "DELETE FROM [%s] WHERE [id2]=%d;",
                    (char *)db_tablename, (int)db_search_data->id2);
        }
        else if (db_search_data->name[0] != (UI_8)'\0') {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "DELETE FROM [%s] WHERE [name]=%Q;",
                    (char *)db_tablename, (char *)db_search_data->name);
        }
        else if (db_search_data->flag != FALSE) {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "DELETE FROM [%s] WHERE "
                    "STRFTIME(%s, [time])<STRFTIME(%s, \'NOW\') "
                    "ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename, (char *)db_search_data->name);
        }
        else {
            sqlite3_snprintf(sizeof(db->sql_query), (char *)db->sql_query,
                    "DELETE FROM [%s] WHERE [timestamp]="
                    "(SELECT [timestamp] FROM [%s] ORDER "
                    "BY [timestamp] ASC LIMIT 1);",
                    (char *)db_tablename, (char *)db_tablename);
        }

            printf("Q: %s\n", (char *)db->sql_query);
        status = DataDownloadMngExecDbQuery(db, db->sql_query, NULL);
    }
    DataDownloadMngCloseDb(db);
    DB_UNLOCK(db);

    return status;
} /* DataDownloadMngDelRecDataFromDb */

