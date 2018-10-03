/*
 * ==============================================================================
 *
 *       Filename:  UtilMngBackup.c
 *
 *    Description:  This file contains the utility for saving binary blocks in a
 *                  file.
 *
 *        Version:  1.0
 *        Created:  Sunday 08 January 2017 04:20:40  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  L&T
 *
 * ==============================================================================
 */

#include "UtilMngBackup.h"

/**
 * @brief This function writes the backup data blocks in a file
 *
 * @param filename[in]          Backup data filename.
 * @param datblock[in]          Data block to be written.
 * @param datablock_size[in]    Size of data block to be written.
 *
 * @return
 *   - Length of datablock which is written successfully.
 *   - UI_BKUP_FAILURE on error.
 */
SI_32 UtilMngAppendBackupData(UI_8 *filename, const void *datblock, UI_32 datablock_size)
{
    SI_32 fd = (SI_32)(-1);
    SI_32 status = UM_BS_SUCCESS;

    fd = open((char *)filename, O_APPEND|O_CREAT, 0666);
    if (fd < 0)
    {
        status = UM_BS_FAILURE;
    }
    else
    {
        status = write(fd, datablock, datablock_size);
        if(status < 0)
        {
            status = UM_BS_FAILURE;
        }
        else
        {
            /* Nothing to do */
        }
        close(fd);
    }
    return status;
}


/**
 * @brief This function returns first backup data blocks in a file.
 *
 * @param filename[in]          Backup data filename.
 * @param datblock[out]          Data block to be written.
 * @param datablock_size[in]    Size of data block to be written.
 *
 * @return
 *   - Length of datablock which is written successfully.
 *   - UI_BKUP_FAILURE on error.
 */
SI_32 UtilMngReadFirstBackupBlock(UI_8 *filename, void *datblock, UI_32 datablock_size)
{
    SI_32 fd = (SI_32)(-1);
    SI_32 status = UM_BS_SUCCESS;

    fd = open((char *)filename, O_RDONLY);
    if (fd < 0)
    {
        status = UM_BS_FAILURE;
    }
    else
    {
        status = read(fd, datablock, datablock_size);
        if(status < 0)
        {
            status = UM_BS_FAILURE;
        }
        else
        {
            /* Nothing to do */
        }
        close(fd);
    }
    return status;
}

/**
 * @brief This function removes first backup data blocks in a file.
 *
 * @param filename[in]          Backup data filename.
 * @param datablock_size[in]    Size of data block to be written.
 *
 * @return
 *   - UI_BKUP_SUCCESS on success.
 *   - UI_BKUP_FAILURE on error.
 */
SI_32 UtilMngDelFirstBackupBlock(UI_8 *filename, UI_32 datablock_size)
{
    SI_32 fd = (SI_32)(-1);
    SI_32 tfd = (SI_32)(-1);
    SI_32 status = UM_BS_SUCCESS;
    UI_8 tmpfile[100];
    UI_8 datablock[datablock_size];

    snprintf(tmpfile, sizeof(tmpfile),"%s.swp", filename);
    fd = open((char *)filename, O_RDONLY);
    if (fd < 0)
    {
        status = UM_BS_FAILURE;
    }
    else
    {

        tfd = open((char *)tmpfile, O_WRONLY| O_CREAT, 0666);
        if (tfd < 0)
        {
            status = UM_BS_FAILURE;
        }
        else
        {
            status = read(fd, datablock, datablock_size);
            if(status < 0)
            {
                status = UM_BS_FAILURE;
            }
            else
            {
                if (status > 0)
                {
                    while((status = read(fd, datablock, datablock_size)) > 0)
                    {
                        status = write(tfd, datablock, status);
                        if (status < 0)
                        {
                            status = UM_BS_FAILURE;
                        }
                        else
                        {
                            status = UM_BS_SUCCESS;
                        }
                    }
                }
                else
                {
                    status = UM_BS_SUCCESS;
                }
            }
            close(tfd);
        }
        close(fd);
        remove(filename);
        rename(tmpfile,filename);
    }
    return status;
}



