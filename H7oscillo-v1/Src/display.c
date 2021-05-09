/**
  ******************************************************************************
  * @file    display.c
  * @author  anirudhr
  * @brief   Implements common display/drawing functions using hardware peripherals.
  ******************************************************************************
  */

/* Includes */
#include "display.h"


/**
  * @brief  Set color of the given pixel (in the uGUI draw buffer).
  * @param  x: x coordinate of the pixel
  * @param  y: y coordinate of the pixel
  * @param  color: color of the pixel
  * @retval None
  */
void pset(UG_S16 x, UG_S16 y, UG_COLOR color)
{
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	pFrame[y][x] = color;
}

/**
  * @brief  Clears the screen (directly draws to frame buffer).
  * @param  None
  * @retval None
  */
void clearScreen(void)
{
	/* wait for any ongoing transfers to complete */
	while(DMA2D->CR & 0x01);

	DMA2D->CR = 0x00030100;					/* reg-memory, transfer error intr enable */
	DMA2D->OCOLR = C_BLACK;					/* color */

	DMA2D->OMAR	= LCD_FRAME_BUFFER;			/* output addr */
	DMA2D->OOR = 0; 						/* output offset */
	DMA2D->OPFCCR = 0x02;					/* output format RGB565 */
	DMA2D->NLR = (LCD_WIDTH)<<16 |			/* frame dimensions */
					(LCD_HEIGHT)<<0;

	DMA2D->IFCR |= 0x3F;					/* clear all flags */
	DMA2D->CR |= 0x01;						/* start transfer */
}

/**
  * @brief  Fills the entire Wave draw buffer with the given color.
  * @param  color: color to fill the screen with
  * @retval None
  */
void fillScreenWave(UG_COLOR color)
{
	/* wait for any ongoing transfers to complete */
	while(DMA2D->CR & 0x01);

	DMA2D->CR = 0x00030100;					/* reg-memory, transfer error intr enable */
	DMA2D->OCOLR = color;					/* color */

	DMA2D->OMAR	= LCD_DRAW_BUFFER_WAVE;		/* output addr */
	DMA2D->OOR = 0; 						/* output offset */
	DMA2D->OPFCCR = 0x02;					/* output format RGB565 */
	DMA2D->NLR = (LCD_WIDTH)<<16 |			/* frame dimensions */
					(LCD_HEIGHT)<<0;

	DMA2D->IFCR |= 0x3F;					/* clear all flags */
	DMA2D->CR |= 0x01;						/* start transfer */
}

/**
  * @brief  Fills a portion of the uGUI draw buffer with the given color.
  * @param  x1: x start
  * @param  y1: y start
  * @param  x2: x end
  * @param  y2: y end
  * @param  color: color to fill the frame with
  * @retval None
  */
UG_RESULT fillFrameUGUI(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR color)
{
	/* wait for any ongoing transfers to complete */
	while(DMA2D->CR & 0x01);

	DMA2D_error_flag = 0;					/* clear any existing error */

	DMA2D->CR = 0x00030100;					/* reg-memory, transfer error intr enable */
	DMA2D->OCOLR = color;					/* color */

	DMA2D->OMAR	= LCD_DRAW_BUFFER_UGUI +
					2*(y1*LCD_WIDTH + x1);	/* output addr */
	DMA2D->OOR = LCD_WIDTH - (x2 - x1 + 1); /* output offset */
	DMA2D->OPFCCR = 0x02;					/* output format RGB565 */
	DMA2D->NLR = (x2 - x1 + 1)<<16 |		/* frame dimensions */
					(y2 - y1 + 1)<<0;

	DMA2D->IFCR |= 0x3F;					/* clear all flags */
	DMA2D->CR |= 0x01;						/* start transfer */

	/* wait for the transfer to complete */
	while(DMA2D->CR & 0x01);

	return (DMA2D_error_flag ? UG_RESULT_FAIL : UG_RESULT_OK);
}

/**
  * @brief  Draws a line of the given color in the uGUI draw buffer.
  * @param  x1: x start
  * @param  y1: y start
  * @param  x2: x end
  * @param  y2: y end
  * @param  color: color of the line
  * @retval None
  */
UG_RESULT drawLineUGUI(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR color)
{
	/* wait for any ongoing transfers to complete */
	while(DMA2D->CR & 0x01);

	DMA2D_error_flag = 0;					/* clear any existing error */

	DMA2D->CR = 0x00030100;					/* reg-memory, transfer error intr enable */
	DMA2D->OPFCCR = 0x02;					/* output format RGB565 */
	DMA2D->OCOLR = color;					/* color */
	DMA2D->OMAR	= LCD_DRAW_BUFFER_UGUI +
					2*(y1*LCD_WIDTH + x1);	/* output addr */

	if(y1 == y2){
		/* horizontal line */
		DMA2D->OOR = 0; 					/* output offset */
		DMA2D->NLR = (x2 - x1 + 1)<<16 |	/* frame dimensions */
						(1)<<0;
	}
	else if(x1 == x2){
		/* vertical line */
		DMA2D->OOR = LCD_WIDTH - 1; 		/* output offset */
		DMA2D->NLR = (1)<<16 |				/* frame dimensions */
						(y2 - y1 + 1)<<0;
	}
	else{
		return UG_RESULT_FAIL;
	}

	DMA2D->IFCR |= 0x3F;					/* clear all flags */
	DMA2D->CR |= 0x01;						/* start transfer */

	/* wait for the transfer to complete */
	while(DMA2D->CR & 0x01);

	return (DMA2D_error_flag ? UG_RESULT_FAIL : UG_RESULT_OK);
}

/**
  * @brief  Blends the uGUI and Wave draw buffers and outputs to the frame buffer (screen).
  * @param  None
  * @retval None
  */
void updateToScreen(void)
{
	/* wait for any ongoing transfers to complete */
	while(DMA2D->CR & 0x01);

	DMA2D->CR = 0x00020100;						/* memory-memory with blending, transfer error intr enable */

	DMA2D->FGMAR = LCD_DRAW_BUFFER_UGUI;		/* foreground addr */
	DMA2D->FGPFCCR = (0x02<<0) | 				/* foreground format RGB565 */
					 (0x1<<16) |				/* replace original alpha */
					 (UGUI_FRAME_ALPHA<<24);	/* blending alpha */

	DMA2D->BGMAR = LCD_DRAW_BUFFER_WAVE;		/* background addr */
	DMA2D->BGPFCCR = (0x02<<0) | 				/* background format RGB565 */
					 (0x1<<16) |				/* replace original alpha */
					 (WAVE_FRAME_ALPHA<<24);	/* blending alpha */

	DMA2D->OMAR	= LCD_FRAME_BUFFER;				/* output addr */
	DMA2D->OOR = 0; 							/* output offset */
	DMA2D->OPFCCR = 0x02;						/* output format RGB565 */
	DMA2D->NLR = (LCD_WIDTH)<<16 |				/* frame dimensions */
					(LCD_HEIGHT)<<0;

	DMA2D->IFCR |= 0x3F;						/* clear all flags */
	DMA2D->CR |= 0x01;							/* start transfer */
}
