/**
  ******************************************************************************
  * @file    display.h
  * @author  anirudhr
  * @brief   Header file for display.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DISPLAY_H
#define __DISPLAY_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"


void pset(UG_S16 x, UG_S16 y, UG_COLOR color);
void clearScreen(void);
void fillScreenWave(UG_COLOR color);
UG_RESULT fillFrameUGUI(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR color);
UG_RESULT drawLineUGUI(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR color);
void updateToScreen(void);

#endif /* __DISPLAY_H */
