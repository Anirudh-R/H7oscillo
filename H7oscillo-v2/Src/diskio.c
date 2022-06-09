/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "ff.h"			/* Obtains integer types */
#include "diskio.h"		/* Declarations of disk functions */


/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive number to identify the drive */
)
{
	return 0;		/* always initialized, ready and not write protected */
}


/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive number to identify the drive */
)
{
	QSPI_flash_init();

	return 0;				/* success */
}


/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive number to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	LBA_t sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	UINT i;

#if QSPI_USE_NONBLK_WRITES
	while(QSPI_is_busy());		/* finish pending writes */
#endif

	for(i = 0; i < count; i++){
		if(QSPI_flash_read(buff + i*SECTOR_SIZE, (sector + i)*SECTOR_SIZE, SECTOR_SIZE)){
			return RES_ERROR;
		}
	}

	return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive number to identify the drive */
	const BYTE *buff,	/* Data to be written */
	LBA_t sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	UINT i;

#if QSPI_USE_NONBLK_WRITES
	while(QSPI_is_busy());		/* finish pending writes */

	/* copy data to scratch buffer, as buff becomes invalid once function returns */
	uint8_t* pScrBuf = (uint8_t *)DISKIO_BUFFER;
	for(i = 0; i < count*SECTOR_SIZE; i++){
		pScrBuf[i] = buff[i];
	}

	if(QSPI_flash_write_nb(pScrBuf, sector*SECTOR_SIZE, count)){
		return RES_ERROR;
	}
#else
	for(i = 0; i < count; i++){
		if(QSPI_flash_write(buff + i*SECTOR_SIZE, (sector + i)*SECTOR_SIZE, SECTOR_SIZE)){
			return RES_ERROR;
		}
	}
#endif

	return RES_OK;
}

#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive number (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	switch(cmd)
	{
		case CTRL_SYNC:						/* complete pending writes */
#if QSPI_USE_NONBLK_WRITES
			while(QSPI_is_busy());
#endif
			return RES_OK;

		case GET_SECTOR_COUNT:				/* number of sectors in flash */
			*(LBA_t *)buff = (LBA_t)NUM_SECTORS;
			return RES_OK;

		case GET_BLOCK_SIZE:				/* erase block size of flash (in units of sector) */
			*(DWORD *)buff = (DWORD)1;
			return RES_OK;
	}

	return RES_PARERR;
}

