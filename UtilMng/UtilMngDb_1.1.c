
#include "UtilMngDb.h"


static SI_32 UtilMngCloseDb(t_util_db *db)
{
    SI_32 status = UM_DB_FAILURE;

    if (db != NULL)
    {
        if (db->database != NULL)
        {
            sqlite3_close(db->database);
            db->database = NULL;
        }
            status = UM_DB_SUCCESS;
    }

    return status;
} /* UtilMngCloseDb */


static SI_32 UtilMngOpenDb(t_util_db *db)
{
    SI_32 status = UM_DB_FAILURE;

    if (db != NULL)
    {
        if (db->database == NULL)
        {
            status = sqlite3_open_v2((char *)db->db_filename, &(db->database),
                    SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE
                    | SQLITE_OPEN_FULLMUTEX, NULL);
            if (status != SQLITE_OK)
            {
                UM_DB_ERROR(db->database);
                status = UM_DB_FAILURE;
                UtilMngCloseDb(db);
            }
            else
            {
                status = UM_DB_SUCCESS;
            }
        }
        else
        {
            status = UM_DB_SUCCESS;
        }
    }

    return status;
} /* UtilMngOpenDb */


SI_32 UtilMngInitDb(t_util_db *db, const UI_8 *db_filename, t_util_db_callback callback)
{
    SI_32 status = UM_DB_FAILURE;

    if ((db != NULL) && (db_filename != NULL))
    {
        snprintf(db->db_filename, sizeof(db->db_filename), "%s", db_filename);
        memset(db->sql_query,0,sizeof(db->sql_query));
        db->database = NULL;
        db->sql_statement = NULL;
        db->callback = callback;
        status = UtilMngOpenDb(db);
        UtilMngCloseDb(db);
    }

    return status;
} /* UtilMngInitDb */


SI_32 UtilMngExecDbQuery(t_util_db *db, const UI_8 *query, void *cb_buf)
{
    SI_32 status = UM_DB_FAILURE;
    UI_8 *errmsg = NULL;

    status = UtilMngOpenDb(db);
    if (status == UM_DB_SUCCESS)
    {
        status = sqlite3_exec(db->database, (char *)query, db->callback, cb_buf,
                (char **)&errmsg);
        if (status != SQLITE_OK)
        {
            UM_DB_ERROR(db->database);
            status = UM_DB_FAILURE;
        }
        else
        {
            status = UM_DB_SUCCESS;
        }
    }
    db->sql_statement = NULL;
    UtilMngCloseDb(db);

    return status;
} /* UtilMngExecDbQuery */

SI_32 UtilMngPutDataToDb(t_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name, void *data_block, UI_32 data_size)
{
    SI_32 status = UM_DB_FAILURE;
    SI_32 idx = (SI_32)0;

    if (db_tablename != NULL)
    {
        status = UtilMngOpenDb(db);
    }

    if (status == UM_DB_SUCCESS)
    {
        if (var_name != NULL)
        {
            snprintf((char *)db->sql_query, sizeof(db->sql_query),
                "CREATE TABLE IF NOT EXISTS [%s] ("
                "[timestamp] DATETIME DEFAULT (STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\')), "
                "[name] TEXT UNIQUE, [data] BLOB NULL); "
                "INSERT INTO [%s] ([name]) SELECT \'%s\' WHERE NOT EXISTS"
                "(SELECT 1 FROM [%s] WHERE [name]=\'%s\');",
                 (char *)db_tablename, (char *)db_tablename, (char *)var_name,
                 (char *)db_tablename, (char *)var_name);
        }
        else
        {
            snprintf((char *)db->sql_query, sizeof(db->sql_query),
                "CREATE TABLE IF NOT EXISTS [%s] ("
                "[timestamp] DATETIME DEFAULT (STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\')), "
                "[name] TEXT UNIQUE, [data] BLOB NULL);",
                 (char *)db_tablename);
        }
        status = UtilMngExecDbQuery(db, db->sql_query, NULL);
        if (status == UM_DB_SUCCESS)
        {
            status = UtilMngOpenDb(db);
        }

        if (status == UM_DB_SUCCESS)
        {
            if (var_name != NULL)
            {
                snprintf((char *)db->sql_query, sizeof(db->sql_query),
                        "UPDATE [%s] SET [data]=@data WHERE [name]=\'%s\';",
                        (char *)db_tablename, (char *)var_name);
            }
            else
            {
                snprintf((char *)db->sql_query, sizeof(db->sql_query),
                        "INSERT INTO [%s] ([name], [data]) VALUES("
                        "STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\'), @data);",
                        (char *)db_tablename);
            }

            status = sqlite3_prepare_v2(db->database, db->sql_query, -1, &db->sql_statement, NULL);
            if (status != SQLITE_OK)
            {
                UM_DB_ERROR(db->database);
                status = UM_DB_FAILURE;
            }
            else
            {
                idx  = sqlite3_bind_parameter_index(db->sql_statement, "@data");
                sqlite3_bind_blob(db->sql_statement, (int)idx,
                        data_block, data_size, SQLITE_STATIC);
                status = sqlite3_step(db->sql_statement);
                if ((status != SQLITE_OK) && (status != SQLITE_DONE))
                {
                    UM_DB_ERROR(db->database);
                    status = UM_DB_FAILURE;
                }
                else
                {
                    status = UM_DB_SUCCESS;
                }
            }
            sqlite3_finalize(db->sql_statement);
            db->sql_statement = NULL;
        }
    }
    UtilMngCloseDb(db);

    return status;
}

#if 0
static SI_32 UtilGetColumnIndex(t_util_db *db , UI_8 *col_name)
{
    SI_32 ret_index = UM_DB_FAILURE;
    UI_32 index = (UI_32)0;
    UI_32 col_count = (UI_32) 0;
    BOOL matched = FALSE;

    if ((db != NULL) && (col_name != NULL))
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
#endif

SI_32 UtilMngGetDataFromDb(t_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name, void *out_buf, UI_32 buf_size)
{
    SI_32 status = UM_DB_FAILURE;

    if (db_tablename != NULL)
    {
        status = UtilMngOpenDb(db);
    }

    if (status == UM_DB_SUCCESS)
    {
        if (var_name != NULL)
        {
            snprintf((char *)db->sql_query, sizeof(db->sql_query),
                    "SELECT [data] FROM [%s] WHERE [name]=\'%s\';",
                    (char *)db_tablename, (char *)var_name);
        }
        else
        {
            snprintf((char *)db->sql_query, sizeof(db->sql_query),
                    "SELECT [data] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1;",
                    (char *)db_tablename);
        }

        status = sqlite3_prepare_v2(db->database, db->sql_query, -1, &db->sql_statement, NULL);
        if (status != SQLITE_OK)
        {
                UM_DB_ERROR(db->database);
                status = UM_DB_FAILURE;
        }
        else
        {
            status = sqlite3_step(db->sql_statement);
            if ((status == SQLITE_OK) || (status == SQLITE_DONE)
                    || (status == SQLITE_ROW))
            {
                if (status == SQLITE_ROW)
                {
                    status = (SI_32)sqlite3_column_bytes(db->sql_statement, 0);
                    status = (status > (SI_32)buf_size)? (SI_32)buf_size : status;
                    memcpy(out_buf, sqlite3_column_blob (
                            db->sql_statement, 0), status);
                }
                else
                {
                    status = UM_DB_SUCCESS;
                }
            }
            else
            {
                    UM_DB_ERROR(db->database);
                    status = UM_DB_FAILURE;
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;
    }

    UtilMngCloseDb(db);

    return status;
}


SI_32 UtilMngDelDataFromDb(t_util_db *db, const UI_8 *db_tablename,
        UI_8 *var_name)
{
    SI_32 status = UM_DB_FAILURE;
    SI_32 idx = (SI_32)0;

    if (db_tablename != NULL)
    {
        status = UtilMngOpenDb(db);
    }

    if (status == UM_DB_SUCCESS)
    {
        if (var_name != NULL)
        {
            snprintf((char *)db->sql_query, sizeof(db->sql_query),
                "DELETE FROM [%s] WHERE [name]=\'%s\';",
                 (char *)db_tablename, (char *)var_name);
        }
        else
        {
            snprintf((char *)db->sql_query, sizeof(db->sql_query),
                "DELETE FROM [%s] WHERE [timestamp]="
                "(SELECT [timestamp] FROM [%s] ORDER "
                "BY [timestamp] ASC LIMIT 1) LIMIT 1;",
                 (char *)db_tablename, (char *)db_tablename);
        }

        status = UtilMngExecDbQuery(db, db->sql_query, NULL);
    }
    UtilMngCloseDb(db);

    return status;
}


SI_32 UtilMngCountDbRows(t_util_db *db, const UI_8 *db_tablename)
{
    SI_32 status = UM_DB_FAILURE;
    SI_32 idx = (SI_32)0;

    if (db_tablename != NULL)
    {
        status = UtilMngOpenDb(db);
    }

    if (status == UM_DB_SUCCESS)
    {
        snprintf((char *)db->sql_query, sizeof(db->sql_query),
                "SELECT COUNT(*) FROM [%s];",
                (char *)db_tablename);
        status = sqlite3_prepare_v2(db->database, db->sql_query, -1, &db->sql_statement, NULL);
        if (status != SQLITE_OK)
        {
                UM_DB_ERROR(db->database);
                status = UM_DB_FAILURE;
        }
        else
        {
            status = sqlite3_step(db->sql_statement);
            if ((status == SQLITE_OK) || (status == SQLITE_DONE)
                    || (status == SQLITE_ROW))
            {
                if (status == SQLITE_ROW)
                {
                    status = (SI_32)sqlite3_column_int (db->sql_statement, 0);
                }
                else
                {
                    status = UM_DB_SUCCESS;
                }
            }
            else
            {
                    UM_DB_ERROR(db->database);
                    status = UM_DB_FAILURE;
            }
        }
        sqlite3_finalize(db->sql_statement);
        db->sql_statement = NULL;
    }
    UtilMngCloseDb(db);

    return status;
}


SI_32 UtilMngDropTableFromDb(t_util_db *db, const UI_8 *db_tablename)
{
    SI_32 status = UM_DB_FAILURE;
    SI_32 idx = (SI_32)0;

    if (db_tablename != NULL)
    {
        status = UtilMngOpenDb(db);
    }

    if (status == UM_DB_SUCCESS)
    {
        snprintf((char *)db->sql_query, sizeof(db->sql_query),
                "DROP TABLE IF EXISTS [%s];",
                (char *)db_tablename);

        status = UtilMngExecDbQuery(db, db->sql_query, NULL);
    }
    UtilMngCloseDb(db);

    return status;
}


SI_32 UtilMngPushDataToDb(t_util_db *db, const UI_8 *db_tablename,
        void *data_block, UI_32 data_size, UI_32 max_limit)
{
    SI_32 status = UM_DB_FAILURE;
    SI_32 idx = (SI_32)0;

    if (db_tablename != NULL)
    {
        status = UtilMngOpenDb(db);
    }

    if (status == UM_DB_SUCCESS)
    {
        snprintf((char *)db->sql_query, sizeof(db->sql_query),
                "CREATE TABLE IF NOT EXISTS [%s] ("
                "[timestamp] DATETIME DEFAULT (STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\')), "
                "[name] TEXT UNIQUE, [data] BLOB NULL); "
                "DELETE FROM \'%s\' WHERE "
                "(SELECT COUNT(*) FROM [%s])>%u AND [timestamp]="
                "(SELECT [timestamp] FROM [%s] ORDER BY [timestamp] ASC LIMIT 1);",
                 (char *)db_tablename, (char *)db_tablename, (char *)db_tablename,
                 (unsigned int)(max_limit - 1), (char *)db_tablename);
        status = UtilMngExecDbQuery(db, db->sql_query, NULL);
        if (status == UM_DB_SUCCESS)
        {
            status = UtilMngOpenDb(db);
        }

        if (status == UM_DB_SUCCESS)
        {
            snprintf((char *)db->sql_query, sizeof(db->sql_query),
                        "INSERT INTO [%s] ([name], [data]) VALUES("
                        "STRFTIME(\'%%Y-%%m-%%d %%H:%%M:%%f\', \'NOW\'), @data);",
                        (char *)db_tablename);

            status = sqlite3_prepare_v2(db->database, db->sql_query, -1, &db->sql_statement, NULL);
            if (status != SQLITE_OK)
            {
                UM_DB_ERROR(db->database);
                status = UM_DB_FAILURE;
            }
            else
            {
                idx  = sqlite3_bind_parameter_index(db->sql_statement, "@data");
                sqlite3_bind_blob(db->sql_statement, (int)idx,
                        data_block, data_size, SQLITE_STATIC);
                status = sqlite3_step(db->sql_statement);
                if ((status != SQLITE_OK) && (status != SQLITE_DONE))
                {
                    UM_DB_ERROR(db->database);
                    status = UM_DB_FAILURE;
                }
                else
                {
                    status = UM_DB_SUCCESS;
                }
            }
            sqlite3_finalize(db->sql_statement);
            db->sql_statement = NULL;
        }
    }
    UtilMngCloseDb(db);

    return status;
}

