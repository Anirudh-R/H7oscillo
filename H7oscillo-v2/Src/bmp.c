/**
  ******************************************************************************
  * @file    bmp.c
  * @author  anirudhr
  * @brief   Implements raw pixel data to BMP format conversion.
  ******************************************************************************
  */

/* Includes */
#include "bmp.h"


static const uint8_t bitmapFileHeader[BITMAP_FILEHEADER_LEN] = {	/* all BMP fields are in little-endian */
						0x42, 0x4D, 						/* FileType: "BM" */
						0x42, 0xFC, 0x03, 0x00, 			/* FileSize: 261,186 bytes */
						0x00, 0x00, 						/* Reserved */
						0x00, 0x00,							/* Reserved */
						0x42, 0x00, 0x00, 0x00,				/* PixelDataOffset: 14 + 40 + 12 bytes */
					};

static const uint8_t DIBHeader[DIB_HEADER_LEN] = {
						0x28, 0x00, 0x00, 0x00,				/* HeaderSize: 40 bytes */
						0xE0, 0x01, 0x00, 0x00, 			/* ImageWidth: 480 pixels */
						0xF0, 0xFE, 0xFF, 0xFF,				/* ImageHeight: -272 pixels (negative to indicate image scanned from upper left corner) */
						0x01, 0x00,							/* Planes: 1 */
						0x10, 0x00,							/* BitsPerPixel: 16bpp */
						0x03, 0x00, 0x00, 0x00,				/* Compression: BI_BITFIELDS */
						0x00, 0x00, 0x00, 0x00,				/* ImageSize: same as raw (no compression used) */
						0x00, 0x00, 0x00, 0x00,				/* XpixelsPerMeter: no preference */
						0x00, 0x00, 0x00, 0x00,				/* YpixelsPerMeter: no preference */
						0x00, 0x00, 0x00, 0x00,				/* TotalColors: 2^BitsPerPixel colors are used */
						0x00, 0x00, 0x00, 0x00				/* ImportantColors: None */
					};

static const uint8_t bitMasks[BIT_MASK_LEN] = {
						0x00, 0xF8, 0x00, 0x00,				/* Red mask */
						0xE0, 0x07, 0x00, 0x00,				/* Green mask */
						0x1F, 0x00, 0x00, 0x00				/* Blue mask */
					};

/**
  * @brief  Convert raw pixel data to BMP format.
  * @param  raw: pointer to input image in RGB565 format
  * @param  npixels: no. of pixels in the image
  * @param  bmp: pointer to store the BMP image
  * @retval size of the returned BMP image (in bytes)
  */
uint32_t raw2bmp(const uint8_t* raw, uint32_t npixels, uint8_t* bmp)
{
	uint32_t i, j;

	/* add the BMP file header */
	for(i = 0, j = 0; j < BITMAP_FILEHEADER_LEN; i++, j++){
		bmp[i] = bitmapFileHeader[j];
	}

	/* append the DIB header */
	for(j = 0; j < DIB_HEADER_LEN; i++, j++){
		bmp[i] = DIBHeader[j];
	}

	/* append the RGB bit masks */
	for(j = 0; j < BIT_MASK_LEN; i++, j++){
		bmp[i] = bitMasks[j];
	}

	/* add image pixel data */
	for(j = 0; j < 2*npixels; i++, j++){
		bmp[i] = raw[j];
	}

	return i;
}
