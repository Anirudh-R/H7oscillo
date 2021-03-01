/**
  ******************************************************************************
  * @file    ui.h
  * @author  anirudhr
  * @brief   Header file for ui.c.
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UI_H
#define __UI_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdlib.h>
#include <string.h>


#define WINDOW1				 1
#define WINDOW2				 2
#define WINDOW3				 3
#define WINDOW4				 4
#define WINDOW5				 5

#define CH1_COLOR			 C_GREEN			/* colors of CH1 waveform and related parameter displays */
#define CH2_COLOR			 C_AQUA				/* colors of CH2 waveform and related parameter displays */
#define TBASE_ICON_COLOR	 C_YELLOW			/* color of the time base icon */
#define MODE_ICON_COLOR		 C_MEDIUM_ORCHID	/* color of the Mode icon */
#define RUNSTOP_ICON_COLOR	 C_YELLOW_GREEN		/* color of the Run/Stop icon */
#define TOFFSET_ICON_COLOR	 C_YELLOW			/* color of the time offset icon */
#define INACTIVE_ICON_COLOR	 C_SLATE_GRAY		/* color of an inactive icon */
#define MENUBAR_HEIGHT		 15					/* heights of the top and bottom menu bars */
#define WIND1_ICON_WIDTH	 50					/* window 1 & 2 icons width */
#define WIND1_ICON_SPACING	 10					/* window 1 & 2 icons horizontal spacing */
#define MEASURE_ICON_WIDTH	 65					/* measurement icons width */
#define GRID_HORZ_DIVS		 10					/* no. of horizontal divisions in grid */
#define GRID_VERT_DIVS	     5					/* no. of vertical divisions in grid */
#define GRID_DOT_SPACING	 10					/* grid dotted line spacing */
#define GRID_COLOR			 C_DARK_GRAY		/* grid color */
#define WIND3_WIDTH	 		 80					/* window 3 & 4 width */
#define WIND3_BTN_SPACING	 10					/* window 3 & 4 vertical spacing between buttons */
#define WIND3_BTN_HEIGHT	 30					/* window 3 & 4 button heights */
#define WIND5_WIDTH	 		 140				/* window 5 width */
#define WIND5_HEIGHT	 	 60					/* window 5 height */
#define WIND5_BTN_SPACING	 5					/* window 5 vertical spacing between buttons */
#define WIND5_BTN_HEIGHT	 20					/* window 5 button heights */
#define WIND5_X_START	 	 250				/* window 5 X start position */
#define WIND5_Y_START	 	 90					/* window 5 Y start position */

extern UG_GUI gui;
extern UG_WINDOW window_1;
extern UG_OBJECT obj_buff_wnd_1[UGUI_MAX_OBJECTS];
extern UG_BUTTON button1_0;
extern UG_TEXTBOX txtb1_0;
extern UG_TEXTBOX txtb1_1;
extern UG_TEXTBOX txtb1_2;
extern UG_TEXTBOX txtb1_3;
extern UG_TEXTBOX txtb1_4;
extern UG_TEXTBOX txtb1_5;
extern UG_TEXTBOX txtb1_6;
extern UG_WINDOW window_2;
extern UG_OBJECT obj_buff_wnd_2[UGUI_MAX_OBJECTS];
extern UG_TEXTBOX txtb2_0;
extern UG_TEXTBOX txtb2_1;
extern UG_TEXTBOX txtb2_2;
extern UG_TEXTBOX txtb2_3;
extern UG_TEXTBOX txtb2_4;
extern UG_TEXTBOX txtb2_5;
extern UG_TEXTBOX txtb2_6;
extern UG_WINDOW window_3;
extern UG_OBJECT obj_buff_wnd_3[UGUI_MAX_OBJECTS];
extern UG_TEXTBOX txtb3_0;
extern UG_BUTTON button3_0;
extern UG_BUTTON button3_1;
extern UG_BUTTON button3_2;
extern UG_BUTTON button3_3;
extern UG_BUTTON button3_4;
extern UG_WINDOW window_4;
extern UG_OBJECT obj_buff_wnd_4[UGUI_MAX_OBJECTS];
extern UG_TEXTBOX txtb4_0;
extern UG_BUTTON button4_0;
extern UG_BUTTON button4_1;
extern UG_BUTTON button4_2;
extern UG_BUTTON button4_3;
extern UG_BUTTON button4_4;
extern UG_WINDOW window_5;
extern UG_OBJECT obj_buff_wnd_5[UGUI_MAX_OBJECTS];
extern UG_TEXTBOX txtb5_0;
extern UG_TEXTBOX txtb5_1;
extern UG_BUTTON button5_0;
extern UG_BUTTON button5_1;

extern uint8_t showWindow3;
extern uint8_t showWindow4;
extern uint8_t showWindow5;

extern uint8_t wind5OpenedBy;

char* gcvt(double value, int ndigit, char* buf);
void initUI(void);
void drawGrid(void);
void voltsToStr(uint8_t val, char* buf);
void hertzToStr(uint32_t freq, char* buf);
void window_1_callback(UG_MESSAGE* msg);
void window_2_callback(UG_MESSAGE* msg);
void window_3_callback(UG_MESSAGE* msg);
void window_4_callback(UG_MESSAGE* msg);
void window_5_callback(UG_MESSAGE* msg);

#endif /* __UI_H */
