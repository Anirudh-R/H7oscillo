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


#define WINDOW1				 1
#define WINDOW2				 2
#define WINDOW3				 3
#define WINDOW4				 4
#define WINDOW5				 5
#define WINDOW6				 6

/* Colors and dimensions of GUI elements */
#define CH1_COLOR			 	 	C_GREEN				/* colors of CH1 waveform and related parameter displays */
#define CH2_COLOR			 	 	C_AQUA				/* colors of CH2 waveform and related parameter displays */
#define TBASE_ICON_COLOR	 	 	C_YELLOW			/* color of the time base icon */
#define MODE_ICON_COLOR		 	 	C_MEDIUM_ORCHID		/* color of the Mode icon */
#define RUNSTOP_ICON_COLOR_RUN	 	C_YELLOW_GREEN		/* color of the Run/Stop icon for Run state */
#define RUNSTOP_ICON_COLOR_STP	 	C_TOMATO			/* color of the Run/Stop icon for Stop state */
#define RUNSTOP_ICON_COLOR_TRGWT	C_DARK_GRAY			/* color of the Run/Stop icon for Trigger wait state */
#define TOFFSET_ICON_COLOR	 	 	C_YELLOW			/* color of the time offset icon */
#define INACTIVE_ICON_COLOR	 	 	C_SLATE_GRAY		/* color of an inactive icon */
#define MENUBAR_HEIGHT		 	 	15					/* heights of the top and bottom menu bars */
#define WIND1_ICON_WIDTH	 	 	50					/* window 1 & 2 icons width */
#define WIND1_ICON_SPACING	 	 	10					/* window 1 & 2 icons horizontal spacing */
#define MEASURE_ICON_WIDTH	 	 	65					/* measurement icons width */
#define GRID_HORZ_DIVS		 	 	10					/* no. of horizontal divisions in grid */
#define GRID_VERT_DIVS	     	 	5					/* no. of vertical divisions in grid */
#define GRID_DOT_SPACING	 	 	10					/* grid dotted line spacing */
#define GRID_COLOR			 	 	C_DARK_GRAY			/* grid color */
#define CH_SEPARATOR_COLOR		 	C_WHITE				/* channel separator color */
#define WIND3_WIDTH	 		 	 	80					/* window 3 & 4 width */
#define WIND3_BTN_SPACING	 	 	10					/* window 3 & 4 vertical spacing between buttons */
#define WIND3_BTN_HEIGHT	 	 	30					/* window 3 & 4 button heights */
#define WIND5_WIDTH	 		 	 	140					/* window 5 width */
#define WIND5_HEIGHT	 	 	 	60					/* window 5 height */
#define WIND5_BTN_SPACING	 	 	5					/* window 5 vertical spacing between buttons */
#define WIND5_BTN_WIDTH	 	 		60					/* window 5 button widths */
#define WIND5_BTN_HEIGHT	 	 	20					/* window 5 button heights */
#define WIND5_X_START	 	 	 	250					/* window 5 X start position */
#define WIND5_Y_START	 	 	 	90					/* window 5 Y start position */
#define WIND6_WIDTH	 		 	 	140					/* window 6 width */
#define WIND6_HEIGHT	 	 	 	30					/* window 6 height */
#define WIND6_BTN_WIDTH	 	 		60					/* window 6 button widths */
#define WIND6_BTN_HEIGHT	 	 	20					/* window 6 button heights */
#define WIND6_X_START	 	 	 	250					/* window 6 X start position */
#define WIND6_Y_START	 	 	 	90					/* window 6 Y start position */
#define CURSOR_LENGTH	 	 	 	13					/* length of trigger and vertical offset cursors */
#define CURSOR_WIDTH	 	 	 	10					/* width of trigger and vertical offset cursors */
#define TOFF_CURSOR_LENGTH	 	 	10					/* length of horizontal offset cursors */
#define TOFF_CURSOR_WIDTH	 	 	10					/* width of horizontal offset cursors */
#define TOFF_CURSOR_COLOR	 	 	C_PLUM				/* color of horizontal offset cursors */

/* Fields in the top and bottom menubars */
#define FLD_NONE			0
#define FLD_CH1_VSCALE		1
#define FLD_CH2_VSCALE		2
#define FLD_TSCALE			3
#define FLD_TRIGSRC			4
#define FLD_TRIGTYPE		5
#define FLD_TRIGLVL			6
#define FLD_TRIGMODE		7
#define FLD_RUNSTOP			8
#define FLD_CH1_VOFF		9
#define FLD_CH2_VOFF		10
#define FLD_TOFF			11
#define FLD_MAXVALS			12		/* number of fields */

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
extern UG_OBJECT obj_buff_wnd_5[4];
extern UG_TEXTBOX txtb5_0;
extern UG_TEXTBOX txtb5_1;
extern UG_BUTTON button5_0;
extern UG_BUTTON button5_1;
extern UG_WINDOW window_6;
extern UG_OBJECT obj_buff_wnd_6[2];
extern UG_TEXTBOX txtb6_0;
extern UG_BUTTON button6_0;

char* gcvt(double value, int ndigit, char* buf);
void initUI(void);
void drawGrid(void);
void voltsToStr(uint8_t val, char* buf);
void hertzToStr(float32_t freq, char* buf);
void secToStr(float32_t t, char* buf);
void switchNextWindow(void);
void DisplayMeasurements(void);
uint8_t getCurrField(void);
void goToField(uint8_t field);
void switchNextField(void);
void changeFieldValue(uint8_t dir);
void dispToff(void);
void initFields(void);
void drawRedBorder(void);
void clearRedBorder(void);

#endif /* __UI_H */
