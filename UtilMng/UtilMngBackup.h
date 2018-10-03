/*
 * =====================================================================================
 *
 *       Filename:  UtilMngBackup.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  Sunday 08 January 2017 04:44:58  IST
 *       Revision:  1.1
 *       Compiler:  gcc
 *
 *         Author:  Jagdish Prajapati (JP), prajapatijagdish@gmail.com
 *        Company:  Emsoftronic
 *
 * =====================================================================================
 */

#ifndef __UTILMNGBINSTORE_H__
#define __UTILMNGBINSTORE_H__

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

/**
 * This macro is defined for return status success of a function.
 */
#define UM_BS_SUCCESS       ((SI_32)0)

/**
 * This macro is defined for return status failure of a function.
 */
#define UM_BS_FAILURE       ((SI_32)(-1))

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
LE_SHARED SI_32 UtilMngAppendBackupData(UI_8 *filename, const void *datblock, UI_32 datablock_size);

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
LE_SHARED SI_32 UtilMngReadFirstBackupBlock(UI_8 *filename, void *datblock, UI_32 datablock_size);

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
LE_SHARED SI_32 UtilMngDelFirstBackupBlock(UI_8 *filename, UI_32 datablock_size);


#endif /* __UTILMNGBINSTORE_H__ */
