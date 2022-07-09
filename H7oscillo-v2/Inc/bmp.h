/**
  ******************************************************************************
  * @file    bmp.h
  * @author  anirudhr
  * @brief   Header file for bmp.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BMP_H
#define __BMP_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


#define BITMAP_FILEHEADER_LEN				14
#define DIB_HEADER_LEN						40
#define BIT_MASK_LEN						12

#define BMP_FILE_SZ							261186				/* size of the full screen BMP image file */

/* External function declarations */
extern uint32_t raw2bmp(const uint8_t* raw, uint32_t npixels, uint8_t* bmp);
extern void bmp2raw(const uint8_t* bmp, uint32_t npixels, uint8_t* raw);

#endif /* __BMP_H */
