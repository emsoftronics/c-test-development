
#include "UtilMngDb.h"

const UI_8 *util_col_type[] = {
    "INTEGER",
    "TEXT",
    "REAL",
    "BLOB",
    "DATETIME",
    NULL
};

const UI_8 *util_col_constraint[] = {
    "NULL",
    "PRIMARY KEY",
    "PRIMARY KEY AUTOINCREMENT",
    "NOT NULL",
    "UNIQUE",
    "DEFAULT 0",
    "DEFAULT \'\'",
    "DEFAULT CURRENT_TIMESTAMP",
    "DEFAULT \'0000-00-00 00:00:00\'",
    NULL
};

static void UtilMngLogDbError(SI_32 err_code)
{
    switch (err_code)                                                            
    {                                                                            
    case SQLITE_OK:                                                                                                           
        break;                                                                   
    case SQLITE_ERROR:                                                           
        printf("Error: SQL error or missing database.\n");                       
        break;                                                                   
    case SQLITE_INTERNAL:                                                        
        printf("Error: Internal logic error in SQLite.\n");                      
        break;                                                                   
    case SQLITE_PERM:                                                            
        printf("Error: Access permission denied.\n");                            
        break;                                                                   
    case SQLITE_ABORT:                                                           
        printf("Error: Callback routine requested an abort.\n");                 
        break;                                                                   
    case SQLITE_BUSY:                                                            
        printf("Error: The database file is locked.\n");                         
        break;                                                                   
    case SQLITE_LOCKED:                                                          
        printf("Error: A table in the database is locked.\n");                   
        break;                                                                   
    case SQLITE_NOMEM:                                                           
        printf("Error: A malloc() failed.\n");                                   
        break;                                                                   
    case SQLITE_READONLY:                                                        
        printf("Error: Attempt to write a readonly database.\n");                
        break;                                                                   
    case SQLITE_INTERRUPT:                                                       
        printf("Error: Operation terminated by sqlite3_interrupt().\n");         
        break;                                                                   
    case SQLITE_IOERR:                                                           
        printf("Error: Some kind of disk I/O error occurred.\n");                
        break;                                                                   
    case SQLITE_CORRUPT:                                                         
        printf("Error: The database disk image is malformed.\n");                
        break;                                                                   
    case SQLITE_NOTFOUND:                                                        
        printf("Error: Unknown opcode in sqlite3_file_control().\n");            
        break;                                                                   
    case SQLITE_FULL:                                                            
        printf("Error: Insertion failed because database is full.\n");           
        break;                                                                   
    case SQLITE_CANTOPEN:                                                        
        printf("Error: Unable to open the database file.\n");                    
        break;                                                                   
    case SQLITE_PROTOCOL:                                                        
        printf("Error: Database lock protocol error.\n");                        
        break;
    case SQLITE_EMPTY:                                                           
        printf("Error: Database is empty.\n");                                   
        break;                                                                   
    case SQLITE_SCHEMA:                                                          
        printf("Error: The database schema changed.\n");                         
        break;                                                                   
    case SQLITE_TOOBIG:                                                          
        printf("Error: String or BLOB exceeds size limit.\n");                   
        break;                                                                   
    case SQLITE_CONSTRAINT:                                                      
        printf("Error: Abort due to constraint violation.\n");                   
        break;                                                                   
    case SQLITE_MISMATCH:                                                        
        printf("Error: Data type mismatch.\n");                                  
        break;                                                                   
    case SQLITE_MISUSE:                                                          
        printf("Error: Library used incorrectly.\n");                            
        break;                                                                   
    case SQLITE_NOLFS:                                                           
        printf("Error: Uses OS features not supported on host.\n");              
        break;                                                                   
    case SQLITE_AUTH:                                                            
        printf("Error: Authorization denied.\n");                                
        break;                                                                   
    case SQLITE_FORMAT:                                                          
        printf("Error: Auxiliary database format error.\n");                     
        break;                                                                   
    case SQLITE_RANGE:                                                           
        printf("Error: 2nd parameter to sqlite3_bind out of range.\n");          
        break;                                                                   
    case SQLITE_NOTADB:                                                          
        printf("Error: File opened that is not a database file.\n");             
        break;                                                                   
    case SQLITE_ROW:                                                             
        printf("INFO: sqlite3_step() has another row ready.\n");                
        break;                                                                   
    case SQLITE_DONE:                                                            
        printf("INFO: sqlite3_step() has finished executing.\n");               
        break;                                                                   
    default:                                                                     
        printf("Error: Sqlite3 library Unknown Error!!.\n");                     
        break;                                                                   
    } 
    return;
} /* UtilMngLogSqlite3Error */

static SI_32 UtilMngCloseDb(t_util_db *db)
{
    SI_32 status = UM_DB_SUCCESS;

    if ((db != NULL) && (db->initiated))
    {
        if (db->database != NULL)
        {
            sqlite3_close(db->database);
            db->database = NULL;
            status = UM_DB_SUCCESS;
        }
        else
        {
            /* Nothing to do */
        }
    } 
    else
    {
            status = UM_DB_FAILURE;
    }

    return status;
} /* UtilMngCloseDb */

static SI_32 UtilMngOpenDb(t_util_db *db)
{
    SI_32 status = UM_DB_SUCCESS;

    if ((db != NULL) && (db->initiated))
    {
        if (db->database != NULL)
        {
            status = UM_DB_SUCCESS;
        }
        else
        {
            status = sqlite3_open((char *)db->db_filename, &db->database);
            db->ret_status = status;
            if (status != SQLITE_OK)
            {
                UtilMngLogDbError(status);
                printf("%s: \n", __func__);
                status = UM_DB_FAILURE;
                UtilMngCloseDb(db);
            }
            else
            {
                status = UM_DB_SUCCESS;
            }
        }
    } 
    else
    {
            status = UM_DB_FAILURE;
    }

    return status;
} /* UtilMngOpenDb */


SI_32 UtilMngInitDb(t_util_db *db, UI_8 *db_filename, t_util_db_callback callback)
{
    SI_32 status = UM_DB_SUCCESS;

    if ((db != NULL) && (db_filename != NULL))
    {
        db->initiated = FALSE;
        snprintf(db->db_filename, sizeof(db->db_filename), "%s", db_filename);
        db->database = NULL;
        db->sql_statement = NULL;
        db->callback = callback;
        db->initiated = TRUE;
        db->ret_status = SQLITE_OK;
        db->record_counter = 0;
        status = UtilMngOpenDb(db);
        if (status != UM_DB_SUCCESS)
        {
            status = UM_DB_FAILURE;
            db->initiated = FALSE;
        }
        else
        {
            UtilMngCloseDb(db);
            db->initiated = TRUE;
        }
    }
    else
    {
        status = UM_DB_FAILURE;
    }

    return status;
} /* UtilMngInitDb */

void UtilMngDbEndSession(t_util_db *db, UI_32 max_row)
{
    if ((db->record_counter >= max_row) 
        || (db->ret_status != UM_DB_ROW_AVAILABLE)) 
    {
            db->ret_status = SQLITE_OK;
            db->record_counter = 0;
            if (db->sql_statement != NULL)
            {
                sqlite3_finalize(db->sql_statement);
                db->sql_statement = NULL;
            }
            UtilMngCloseDb(db);
    }
    else
    {
        /* Nothing to do */;
    }

    return;
} /* UtilMngDbNextRow */

SI_32 UtilMngCreateDbTable(t_util_db *db, UI_8 *db_tablename, UI_8 * unique_col_name,
    t_db_col_type column_type)
{
    SI_32 status = UM_DB_SUCCESS;
    UI_8 query[150] = {0};
    
    UtilMngDbEndSession(db, 0);  
    status = UtilMngOpenDb(db);
    if (status != UM_DB_SUCCESS)
    {
        status = UM_DB_FAILURE;
    }
    else
    {
        if (unique_col_name != NULL)
        {
            snprintf((char *)query, sizeof(query),
                    "CREATE TABLE IF NOT EXISTS [%s] ("                      
                    "[id] INTEGER PRIMARY KEY, "                             
                    "[timestamp] DATETIME DEFAULT CURRENT_TIMESTAMP, "
                    "[%s] %s UNIQUE);",       
                    (char *)db_tablename, (char *)unique_col_name, 
                    (char *)util_col_type[column_type]);
        }
        else
        {
            snprintf((char *)query, sizeof(query),
                    "CREATE TABLE IF NOT EXISTS [%s] ("                      
                    "[id] INTEGER PRIMARY KEY, "                             
                    "[timestamp] DATETIME DEFAULT CURRENT_TIMESTAMP);",       
                    (char *)db_tablename);
        }
        
        status = sqlite3_prepare_v2(db->database, query, -1, &db->sql_statement, NULL);
        db->ret_status = status;
        if (status != SQLITE_OK)
        {
            UtilMngLogDbError(status);
            printf("%s: %s\n", __func__,query);
            status = UM_DB_FAILURE;
        }
        else
        {
            status = sqlite3_step(db->sql_statement);
            db->ret_status = status;
            if ((status != SQLITE_OK) && (status != SQLITE_DONE))
            {
                UtilMngLogDbError(status);
                printf("%s: %s\n", __func__,query);
                status = UM_DB_FAILURE;
            }
            else
            {
                status = UM_DB_SUCCESS;
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;

        UtilMngCloseDb(db);
    }

    return status;
} /* UtilMngCreateDbTable */

SI_32 UtilMngAddDbColumn(t_util_db *db, UI_8 *db_tablename, UI_8 *column_name,
        t_db_col_type column_type)
{
    SI_32 status = UM_DB_SUCCESS;
    UI_8 query[150] = {0};
    t_db_col_constraint column_constraint = UM_DB_NULL;

    UtilMngDbEndSession(db, 0);  
    status = UtilMngOpenDb(db);
    if (status != UM_DB_SUCCESS)
    {
        status = UM_DB_FAILURE;
    }
    else
    {
        switch(column_type)
        {
        case UM_DB_INTEGER:
        case UM_DB_FLOAT:
            column_constraint = UM_DB_DEFAULT_0;
            break;
        case UM_DB_TEXT:
            column_constraint = UM_DB_DEFAULT_EMPTY;
            break;
        case UM_DB_DATETIME:
            column_constraint = UM_DB_DEFAULT_EMPTY_DATETIME;
            break;
        case UM_DB_BINARY:
            column_constraint = UM_DB_NULL;
            break;
        default:
            break;
        }

        snprintf((char *)query, sizeof(query), "ALTER TABLE [%s] ADD COLUMN [%s] %s %s;",
                (char *)db_tablename, (char *)column_name, 
                (char *)util_col_type[column_type],
                (char *)util_col_constraint[column_constraint]);
        
        status = sqlite3_prepare_v2(db->database, query, -1, &db->sql_statement, NULL);
        db->ret_status = status;
        if (status != SQLITE_OK)
        {
            UtilMngLogDbError(status);
            printf("%s: %s\n", __func__,query);
            status = UM_DB_FAILURE;
        }
        else
        {
            status = sqlite3_step(db->sql_statement);
            db->ret_status = status;
            if ((status != SQLITE_OK) && (status != SQLITE_DONE))
            {
                UtilMngLogDbError(status);
                printf("%s: %s\n", __func__,query);
                status = UM_DB_FAILURE;
            }
            else
            {
                status = UM_DB_SUCCESS;
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;

        UtilMngCloseDb(db);
    }

    return status;
} /* UtilMngAddDbColumn */



SI_32 UtilMngUpdateDataToDb(t_util_db *db, UI_8 *db_tablename, 
        t_Util_recrd_col_val *process_db_block)
{
    SI_32 status = UM_DB_SUCCESS;
    UI_8 query[250] = {0};
    SI_32 idx = (SI_32)0;
      

    UtilMngDbEndSession(db, 0);  
    status = UtilMngOpenDb(db);
    if (status != UM_DB_SUCCESS)
    {
        status = UM_DB_FAILURE;
    }
    else
    {
        snprintf((char *)query, sizeof(query),
            "UPDATE [%s] SET [%s]=@val WHERE [%s]=@sid;",
            (char *)db_tablename, (char *)process_db_block->process_col_name,
            (char *)process_db_block->search_col_name
            );

        status = sqlite3_prepare_v2(db->database, query, -1, &db->sql_statement, NULL);
        db->ret_status = status;
        if (status != SQLITE_OK)
        {
            UtilMngLogDbError(status);
            printf("%s: %s\n", __func__,query);
            status = UM_DB_FAILURE;
        }
        else
        {
            idx = sqlite3_bind_parameter_index(db->sql_statement, "@val");
            switch(process_db_block->process_col_type)
            {
            case UM_DB_INTEGER:
                sqlite3_bind_int(db->sql_statement, (int)idx,
                        (sqlite3_int64) *((SI_64 *) process_db_block->process_col_val_ptr));
                break;
            case UM_DB_FLOAT:
                sqlite3_bind_double(db->sql_statement, (int)idx,
                        (double) *((FL_64 *) process_db_block->process_col_val_ptr));
                break;
            case UM_DB_TEXT:
            case UM_DB_DATETIME:
                sqlite3_bind_text(db->sql_statement, (int)idx,
                        (const char*)process_db_block->process_col_val_ptr, -1,
                        SQLITE_STATIC);
                break;
            case UM_DB_BINARY:
                sqlite3_bind_blob(db->sql_statement, (int)idx,
                        process_db_block->process_col_val_ptr, 
                        process_db_block->process_col_data_len, SQLITE_STATIC);
                break;
            default:
                status = UM_DB_FAILURE;
                break;
            }

            idx = sqlite3_bind_parameter_index(db->sql_statement, "@sid");
            switch(process_db_block->search_col_type)
            {
            case UM_DB_INTEGER:
                sqlite3_bind_int(db->sql_statement, (int)idx,
                        (sqlite3_int64) *((SI_64 *) process_db_block->search_col_val_ptr));
                break;
            case UM_DB_FLOAT:
                sqlite3_bind_double(db->sql_statement, (int)idx,
                        (double) *((FL_64 *) process_db_block->search_col_val_ptr));
                break;
            case UM_DB_TEXT:
            case UM_DB_DATETIME:
                sqlite3_bind_text(db->sql_statement, (int)idx,
                        (const char*)process_db_block->search_col_val_ptr, -1, 
                        SQLITE_STATIC);
                break;
            default:
                status = UM_DB_FAILURE;
                break;
            }
            
            if (status == UM_DB_SUCCESS)
            {
                status = sqlite3_step(db->sql_statement);
                db->ret_status = status;
                if ((status != SQLITE_OK) && (status != SQLITE_DONE))
                {
                    UtilMngLogDbError(status);
                    printf("%s: %s\n", __func__,query);
                    status = UM_DB_FAILURE;
                }
                else
                {
                    status = UM_DB_SUCCESS;
                }
            }
            else
            {
                /* Nothing to do */
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;

        UtilMngCloseDb(db);
    }

    return status;
} /* UtilMngUpdateDataToDb */



SI_32 UtilMngPutDataToDb(t_util_db *db, UI_8 *db_tablename, 
        t_Util_recrd_col_val *process_db_block)
{
    SI_32 status = UM_DB_SUCCESS;
    UI_8 query[250] = {0};
    SI_32 idx = (SI_32)0;
    SI_32 idx2 = (SI_32)0;
    sqlite3_stmt *tstmt = NULL;
      
    UtilMngDbEndSession(db, 0);  
    status = UtilMngOpenDb(db);
    if (status != UM_DB_SUCCESS)
    {
        status = UM_DB_FAILURE;
    }
    else
    {
        memset(query, 0, sizeof(query));
        snprintf((char *)query, sizeof(query),
            "UPDATE [%s] SET [%s]=@val WHERE [%s]=@sid;",
            (char *)db_tablename, (char *)process_db_block->process_col_name,
            (char *)process_db_block->search_col_name
            );

        status = sqlite3_prepare_v2(db->database, query, -1, &db->sql_statement, NULL);
        db->ret_status = status;
        if (status != SQLITE_OK) 
        {
            UtilMngLogDbError(status);
            printf("%s: %s\n", __func__,query);
            status = UM_DB_FAILURE; 
        }
        else
        {
            snprintf((char *)query, sizeof(query),
                "INSERT INTO [%s] ([%s],[%s]) SELECT @sid2, @val2 "
                "WHERE (Select Changes() = 0);",
                (char *)db_tablename, (char *)process_db_block->search_col_name,
                (char *)process_db_block->process_col_name
                );

            status = sqlite3_prepare_v2(db->database, query, -1, &tstmt, NULL);
            db->ret_status = status;
        }

        if (status != SQLITE_OK)
        {
            UtilMngLogDbError(status);
            printf("%s: %s\n", __func__,query);
            status = UM_DB_FAILURE;
        }
        else
        {
            idx = sqlite3_bind_parameter_index(db->sql_statement, "@val");
            idx2 = sqlite3_bind_parameter_index(tstmt, "@val2");
            switch(process_db_block->process_col_type)
            {
            case UM_DB_INTEGER:
                sqlite3_bind_int(db->sql_statement, (int)idx,
                        (sqlite3_int64) *((SI_64 *) process_db_block->process_col_val_ptr));
                sqlite3_bind_int(tstmt, (int)idx2,
                        (sqlite3_int64) *((SI_64 *) process_db_block->process_col_val_ptr));
                break;
            case UM_DB_FLOAT:
                sqlite3_bind_double(db->sql_statement, (int)idx,
                        (double) *((FL_64 *) process_db_block->process_col_val_ptr));
                sqlite3_bind_double(tstmt, (int)idx2,
                        (double) *((FL_64 *) process_db_block->process_col_val_ptr));
                break;
            case UM_DB_TEXT:
            case UM_DB_DATETIME:
                sqlite3_bind_text(db->sql_statement, (int)idx,
                        (const char*)process_db_block->process_col_val_ptr, -1,
                        SQLITE_STATIC);
                sqlite3_bind_text(tstmt, (int)idx2,
                        (const char*)process_db_block->process_col_val_ptr, -1,
                        SQLITE_STATIC);
                break;
            case UM_DB_BINARY:
                sqlite3_bind_blob(db->sql_statement, (int)idx,
                        process_db_block->process_col_val_ptr, 
                        process_db_block->process_col_data_len, SQLITE_STATIC);
                sqlite3_bind_blob(tstmt, (int)idx2,
                        process_db_block->process_col_val_ptr, 
                        process_db_block->process_col_data_len, SQLITE_STATIC);
                break;
            default:
                status = UM_DB_FAILURE;
                break;
            }

            idx = sqlite3_bind_parameter_index(db->sql_statement, "@sid");
            idx2 = sqlite3_bind_parameter_index(tstmt, "@sid2");
            switch(process_db_block->search_col_type)
            {
            case UM_DB_INTEGER:
                sqlite3_bind_int(db->sql_statement, (int)idx,
                        (sqlite3_int64) *((SI_64 *) process_db_block->search_col_val_ptr));
                sqlite3_bind_int(tstmt, (int)idx2,
                        (sqlite3_int64) *((SI_64 *) process_db_block->search_col_val_ptr));
                break;
            case UM_DB_FLOAT:
                sqlite3_bind_double(db->sql_statement, (int)idx,
                        (double) *((FL_64 *) process_db_block->search_col_val_ptr));
                sqlite3_bind_double(tstmt, (int)idx2,
                        (double) *((FL_64 *) process_db_block->search_col_val_ptr));
                break;
            case UM_DB_TEXT:
            case UM_DB_DATETIME:
                sqlite3_bind_text(db->sql_statement, (int)idx,
                        (const char*)process_db_block->search_col_val_ptr, -1, 
                        SQLITE_STATIC);
                sqlite3_bind_text(tstmt, (int)idx2,
                        (const char*)process_db_block->search_col_val_ptr, -1, 
                        SQLITE_STATIC);
                break;
            default:
                status = UM_DB_FAILURE;
                break;
            }
            
            if (status == UM_DB_SUCCESS)
            {
                status = sqlite3_step(db->sql_statement);
                db->ret_status = status;
                if ((status != SQLITE_OK) && (status != SQLITE_DONE))
                {
                    UtilMngLogDbError(status);
                    printf("%s: %s\n", __func__,query);
                    status = UM_DB_FAILURE;
                }
                else
                {
                    status = sqlite3_step(tstmt);
                    db->ret_status = status;
                    if ((status != SQLITE_OK) && (status != SQLITE_DONE))
                    {
                        UtilMngLogDbError(status);
                        printf("%s: %s\n", __func__,query);
                        status = UM_DB_FAILURE;
                    }
                    else
                    {
                        status = UM_DB_SUCCESS;
                    }
                }
            }
            else
            {
                /* Nothing to do */
            }
        }
        sqlite3_finalize(db->sql_statement);
        sqlite3_finalize(tstmt);
        db->sql_statement = NULL;
        tstmt = NULL;
        UtilMngCloseDb(db);
    }

    return status;
} /* UtilMngPutDataToDb */



SI_32 UtilMngExecQuery(t_util_db *db, UI_8 *query, void *cb_buf)
{
    SI_32 status = UM_DB_SUCCESS;
    UI_8 *errmsg = NULL;

    UtilMngDbEndSession(db,0);  
    status = UtilMngOpenDb(db);
    if (status != UM_DB_SUCCESS)
    {
        status = UM_DB_FAILURE;
    }
    else
    {
        status = sqlite3_exec(db->database, (char *)query, db->callback, cb_buf, 
                (char **)&errmsg);;        
        db->ret_status = status;
        if (status != SQLITE_OK)
        {
            UtilMngLogDbError(status);
            printf("%s: %s\n", __func__,query);
            status = UM_DB_FAILURE;
        }
        else
        {
            status = UM_DB_SUCCESS;
        }
        db->sql_statement = NULL;

        UtilMngCloseDb(db);
    }

    return status;
} /* UtilMngExecQuery */


SI_32 UtilMngStartDbSession(t_util_db *db, UI_8 *db_tablename, UI_8 *db_query)
{
    SI_32 status = UM_DB_SUCCESS;
    UI_8 query[256] = {0};
      
    UtilMngDbEndSession(db,0);  
    status = UtilMngOpenDb(db);
    if (status != UM_DB_SUCCESS)
    {
        status = UM_DB_FAILURE;
    }
    else
    {
        if (db_query != NULL)
        {
            memcpy(query, db_query,256);
        }
        else
        {
            snprintf((char *)query, sizeof(query), "SELECT * FROM [%s] ORDER BY [timestamp] ASC;",
                (char *)db_tablename);
        }
        status = sqlite3_prepare_v2(db->database, query, -1, &db->sql_statement, NULL);
        db->ret_status = status;
        if (status != SQLITE_OK)
        {
            UtilMngLogDbError(status);
            printf("%s: %s\n", __func__,query);
            status = UM_DB_FAILURE;
            sqlite3_finalize(db->sql_statement);
            db->sql_statement = NULL;
            UtilMngCloseDb(db);
        }
        else
        {
            db->ret_status = UM_DB_SESSION_STARTED;
            db->record_counter = 0;
            status = UM_DB_SESSION_STARTED;
        }

    }

    return status;
} /* UtilMngStartDbSession */

SI_32 UtilMngDbNextRow(t_util_db *db)
{
    SI_32 status = UM_DB_FAILURE;
    
    if ((db->ret_status == UM_DB_SESSION_STARTED) 
            || (db->ret_status == UM_DB_ROW_AVAILABLE))
    {
        status = sqlite3_step(db->sql_statement);
        if (status == SQLITE_ROW)
        {
            db->ret_status = UM_DB_ROW_AVAILABLE;
            db->record_counter++;
            status = UM_DB_ROW_AVAILABLE;
        }
        else if (status == SQLITE_DONE)
        {
            db->ret_status = UM_DB_NO_MORE_ROW;
            status = UM_DB_NO_MORE_ROW;
        }
        else
        {
            db->ret_status = UM_DB_FAILURE;
            UtilMngLogDbError(status);
            status = UM_DB_FAILURE;
            sqlite3_finalize(db->sql_statement);
            db->sql_statement = NULL;
            UtilMngCloseDb(db);
        }
    
    }
    else
    {
        status = db->ret_status;
    }

    return status;
} /* UtilMngDbNextRow */


static SI_32 UtilGetColumnIndex(t_util_db *db , UI_8 *col_name)
{

    SI_32 ret_index = UM_DB_FAILURE;
    UI_32 index = (UI_32)0;
    UI_32 col_count = (UI_32) 0;
    BOOL matched = FALSE;

    if ((db != NULL) && (db->ret_status == UM_DB_ROW_AVAILABLE) && 
            (col_name != NULL))
    {
        col_count = sqlite3_column_count(db->sql_statement);
        for (index = 0; (index < col_count) && (matched == FALSE); index++)
        {
            if (strcmp(col_name, sqlite3_column_name(db->sql_statement, index)) == 0)
            {
                matched = TRUE;
            }
        }    
        if (matched) {
            ret_index = index - 1;
        }
        else
        {
            ret_index = UM_DB_FAILURE;
        }
    }
}



SI_32 UtilMngGetDataByColName(t_util_db *db , UI_8 *col_name, void *storage, 
        UI_32 storage_size)
{
    SI_32 ret_length = UM_DB_FAILURE;
    SI_32 col_index = (SI_32)0;
    SI_32 col_type = (SI_32)0;
    SI_8 *v1 =NULL;
    SI_16 *v2 =NULL; 
    SI_32 *v4 =NULL; 
    SI_64 *v8 =NULL; 
    FL_32 *f4 =NULL; 
    FL_64 *f8 =NULL; 

    if ((db != NULL) && (db->ret_status == UM_DB_ROW_AVAILABLE) && 
            (col_name != NULL) && (storage != NULL))
    {
        col_index = UtilGetColumnIndex(db, col_name);
        if (col_index >= 0)
        {
            ret_length = storage_size;
            col_type = sqlite3_column_type(db->sql_statement, col_index);
            switch(col_type)
            {
            case SQLITE_INTEGER:
                if (storage_size == 1)
                {
                    v1 = storage;
                    *v1 = (SI_8)sqlite3_column_int64(db->sql_statement, col_index);
                }
                else if (storage_size == 2)
                {
                    v2 = storage;
                    *v2 = (SI_16)sqlite3_column_int64(db->sql_statement, col_index);
                }
                else if (storage_size == 4)
                {
                    v4 = storage;
                    *v4 = (SI_32)sqlite3_column_int64(db->sql_statement, col_index);
                }
                else 
                {
                    v8 = storage;
                    *v8 = (SI_64)sqlite3_column_int64(db->sql_statement, col_index);
                    ret_length = 8;
                }
                break;
            case SQLITE_FLOAT:
                if (storage_size == 4)
                {
                    f4 = storage;
                    *f4 = (FL_32)sqlite3_column_double(db->sql_statement, col_index);
                }
                else
                {
                    f8 = storage;
                    *f8 = (FL_64)sqlite3_column_double(db->sql_statement, col_index);
                    ret_length = 8;
                }
                break;
            case SQLITE_TEXT:
                ret_length = (SI_32)sqlite3_column_bytes(db->sql_statement, col_index);
                ret_length = (ret_length >= (SI_32)storage_size)? (SI_32)(storage_size - 1) : ret_length;
                memcpy((char *)storage, (const char *)sqlite3_column_text(
                            db->sql_statement, col_index), ret_length);
                memset((char *)storage + ret_length,0,1);
                break;
            case SQLITE_BLOB:
                ret_length = (SI_32)sqlite3_column_bytes(db->sql_statement, col_index);
                ret_length = (ret_length > (SI_32)storage_size)? (SI_32)storage_size : ret_length;
                memcpy(storage, sqlite3_column_blob (
                            db->sql_statement, col_index), ret_length);
                break;
            default:
                ret_length = UM_DB_FAILURE;
                break;
            }
            }
        else
        {
            ret_length = UM_DB_FAILURE;
        }        
    }
    else
    {
        ret_length = UM_DB_FAILURE;
    }

    return ret_length;
} /* UtilMngGetDataByColName */




