/*********  Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS  ******//**
*  @par Language: MISRA C
*  @par Controller: AR7554
*  @par Specification:
*       <a href="https://sqlite.org/capi3ref.html">
*       C-language Interface Specification for SQLite
*       </a>
********************************************************************************
*  @par Project: TCU E4.0
*  @brief   This file contains database or persistent memory related shared
*           function prototypes, data types and macros which can be used by any
*           component for persistent memory related implementation.
*  @author  Jagdish Prajapati(ee.jprajapati@ficosa.com)
*  @version 1.0
*  @par Revision history:
*
*  17/01/17 - Jagdish Prajapati(ee.jprajapati@ficosa.com) Creation of the file.
*
*  @file DataDownloadMngDbUtil.h
*  @copyright Copyright 2004-2014, 2016 FICOSA CORPORATIVE ELECTRONICS
*******************************************************************************/

#ifndef __DATADOWNLOADMNGDBUTIL_H__
#define __DATADOWNLOADMNGDBUTIL_H__

/* -------------------------------- Includes -------------------------------- */
//#include "legato.h"
#include "sqlite3.h"
#include "Global.h"
#include <stdio.h>
//#include "Debug.h"

/* -------------------------------- Defines --------------------------------- */

/**
 * DATADOWNLOADMNG_DB_UTIL_FAILURE macro defines failure status.
 */
#define DATADOWNLOADMNG_DB_UTIL_FAILURE           ((SI_32)(-1))

/**
 * DATADOWNLOADMNG_DB_UTIL_SUCCESS macro defines success status.
 */
#define DATADOWNLOADMNG_DB_UTIL_SUCCESS           ((SI_32)(0))

/**
 * DATADOWNLOADMNG_DB_UTIL_INIT macro defines default initialization value for
 * <b>t_ddm_util_db</b> data type.
 */
#define DATADOWNLOADMNG_DB_UTIL_INIT           {{0},{0}, NULL, NULL, NULL}//, NULL}

/**
 * DATADOWNLOADMNG_DB_DATA_INIT macro defines default initialization value for
 * <b>t_ddm_util_db_data</b> data type.
 */
#define DATADOWNLOADMNG_DB_DATA_INIT  {(SI_32)(-1), (SI_32)(-1), FALSE,     \
                                      {0}, {0}, (UI_32)0, NULL}
/* ------------------------------- Data Types ------------------------------- */
/**
 * This is function pointer declaration data type for sqlite3 callback which
 * will be called N number of times when "SELECT" query returns N number of rows
 * from table.
 */
typedef int (*t_ddm_util_db_callback)(
        void *,     ///< External buffer pointer to assign data inside callback.
        int,        ///< No. of column return by query.
        char**,     ///< Return data strings according to column index.
        char**      ///< Return column name according to column index.
);

/**
 * This is user defined data type which defines essential member parameters
 * which will be used by database functions to use sqlite3 library.
 */
typedef struct {
    UI_8 db_filename[100];          ///< Database file path.
    UI_8 sql_query[512];            ///< Query buffer (<b>for internal use</b>).
    sqlite3 *database;              ///< Sqlite3 opened database pointer.
    sqlite3_stmt *sql_statement;    ///< SQL statement (<b>for internal use</b>).
//    le_mutex_Ref_t db_lock;         ///< Mutex lock (<b>for internal use</b>).
    t_ddm_util_db_callback callback; ///< Callback function pointer.
} t_ddm_util_db;

/**
 * This is user defined data type which defines column members in a record to
 * get and put the data record in a table.
 */
typedef struct {
    SI_32 id1;        ///< Primary id of integer type column in a table.
    SI_32 id2;        ///< Secondary id of integer type column in a table.
    BOOL flag;        ///< Boolean flag of integer type column in a table.
    UI_8 name[30];    ///< Name of data block text type column in a table.
    UI_8 time[20];    ///< Expire time of data block text type column in a table.
    UI_32 data_length; ///< Length of binary data block.
    void *data_buffer; ///< Binary data block reference to store in a table.
} t_ddm_util_db_data;

/* ---------------------------- Global Variables ---------------------------- */
/* --------------------------- Routine prototypes --------------------------- */

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
LE_SHARED SI_32 DataDownloadMngInitDb(t_ddm_util_db *db, const UI_8 *db_filename, t_ddm_util_db_callback callback);

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
LE_SHARED SI_32 DataDownloadMngExecDbQuery(t_ddm_util_db *db, const UI_8 *query, void *cb_buf);


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
LE_SHARED SI_32 DataDownloadMngCreateDbTable (t_ddm_util_db *db, const UI_8 *db_tablename);

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
LE_SHARED SI_32 DataDownloadMngPutDataToDb(t_ddm_util_db *db,
        const UI_8 *db_tablename, UI_8 *var_name, void *data_block, UI_32 data_size);

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
LE_SHARED SI_32 DataDownloadMngGetDataFromDb(t_ddm_util_db *db,
        const UI_8 *db_tablename, UI_8 *var_name, void *out_buf, UI_32 buf_size);

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
LE_SHARED SI_32 DataDownloadMngDelDataFromDb(t_ddm_util_db *db, const UI_8 *db_tablename, UI_8 *var_name);

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
LE_SHARED SI_32 DataDownloadMngCountDbRows(t_ddm_util_db *db, const UI_8 *db_tablename);

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
LE_SHARED SI_32 DataDownloadMngDropTableFromDb(t_ddm_util_db *db, const UI_8 *db_tablename);

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
LE_SHARED SI_32 DataDownloadMngPushDataToDb(t_ddm_util_db *db,
        const UI_8 *db_tablename, void *data_block, UI_32 data_size, UI_32 max_limit);

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
LE_SHARED SI_32 DataDownloadMngPopFrstDataFrmDb(t_ddm_util_db *db,
        const UI_8 *db_tablename, void *out_buf, UI_32 buf_size);

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
LE_SHARED SI_32 DataDownloadMngPutRecDataToDb (t_ddm_util_db *db,
        const UI_8 *db_tablename, t_ddm_util_db_data *db_data , UI_32 max_limit);

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
LE_SHARED SI_32 DataDownloadMngGetRecDataFrmDb (t_ddm_util_db *db,
        const UI_8 *db_tablename, t_ddm_util_db_data *db_data);

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
LE_SHARED SI_32 DataDownloadMngDelRecDataFromDb(t_ddm_util_db *db, const UI_8 *db_tablename,
        t_ddm_util_db_data *db_search_data);

#endif/*  __DATADOWNLOADMNGDBUTIL_H__ */
