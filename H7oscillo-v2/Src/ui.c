/**
  ******************************************************************************
  * @file    ui.c
  * @author  anirudhr
  * @brief   Implements functions to create and manage the graphical user interface.
  ******************************************************************************
  */

/* Includes */
#include "ui.h"


static void selectField(uint8_t field, uint8_t sel);

/* uGUI related globals */
UG_GUI gui;
/* window 1 */
UG_WINDOW window_1;
UG_OBJECT obj_buff_wnd_1[UGUI_MAX_OBJECTS];
UG_BUTTON button1_0;
UG_TEXTBOX txtb1_0;
UG_TEXTBOX txtb1_1;
UG_TEXTBOX txtb1_2;
UG_TEXTBOX txtb1_3;
UG_TEXTBOX txtb1_4;
UG_TEXTBOX txtb1_5;
UG_TEXTBOX txtb1_6;
/* window 2 */
UG_WINDOW window_2;
UG_OBJECT obj_buff_wnd_2[UGUI_MAX_OBJECTS];
UG_TEXTBOX txtb2_0;
UG_TEXTBOX txtb2_1;
UG_TEXTBOX txtb2_2;
UG_TEXTBOX txtb2_3;
UG_TEXTBOX txtb2_4;
UG_TEXTBOX txtb2_5;
UG_TEXTBOX txtb2_6;
/* window 3 */
UG_WINDOW window_3;
UG_OBJECT obj_buff_wnd_3[UGUI_MAX_OBJECTS];
UG_TEXTBOX txtb3_0;
UG_BUTTON button3_0;
UG_BUTTON button3_1;
UG_BUTTON button3_2;
UG_BUTTON button3_3;
UG_BUTTON button3_4;
/* window 4 */
UG_WINDOW window_4;
UG_OBJECT obj_buff_wnd_4[UGUI_MAX_OBJECTS];
UG_TEXTBOX txtb4_0;
UG_BUTTON button4_0;
UG_BUTTON button4_1;
UG_BUTTON button4_2;
UG_BUTTON button4_3;
UG_BUTTON button4_4;
/* window 5 */
UG_WINDOW window_5;
UG_OBJECT obj_buff_wnd_5[UGUI_MAX_OBJECTS];
UG_TEXTBOX txtb5_0;
UG_TEXTBOX txtb5_1;
UG_BUTTON button5_0;
UG_BUTTON button5_1;

/* Menu page display selector flags */
static uint8_t showWindow3 = 0;
static uint8_t showWindow4 = 0;
static uint8_t showWindow5 = 0;

static uint8_t wind5OpenedBy = MEASURE_NONE;
static uint8_t currField = FLD_NONE;					/* currently selected field in the top and bottom menubar */

/**
  * @brief  Create windows and other basic ui elements.
  * @param  None
  * @retval None
  */
void initUI(void)
{
	/*** Create Window 1 (Top menubar) ***/
	UG_WindowCreate(&window_1, obj_buff_wnd_1, UGUI_MAX_OBJECTS, window_1_callback);
	UG_WindowSetStyle(&window_1, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_1, 0, 0, LCD_WIDTH - 1, MENUBAR_HEIGHT - 1);
	UG_WindowSetBackColor(&window_1, C_KHAKI);

	/* Create MENU Button */
	UG_ButtonCreate(&window_1, &button1_0, BTN_ID_0, 427, 0, 474, MENUBAR_HEIGHT - 1);
	UG_ButtonSetFont(&window_1, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_1, BTN_ID_0, C_ORANGE);
	UG_ButtonSetText(&window_1, BTN_ID_0, "MENU");

	/* Draw "icons" to display various parameters */
	UG_TextboxCreate(&window_1, &txtb1_0, TXB_ID_0, WIND1_ICON_SPACING, 1, WIND1_ICON_SPACING + WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* CH1 vertical scale */
	UG_TextboxSetFont(&window_1, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetBackColor(&window_1, TXB_ID_0, CH1_COLOR);
	UG_TextboxSetText(&window_1, TXB_ID_0, "1V");

	UG_TextboxCreate(&window_1, &txtb1_1, TXB_ID_1, 2*WIND1_ICON_SPACING + WIND1_ICON_WIDTH, 1, 2*WIND1_ICON_SPACING + 2*WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* CH2 vertical scale */
	UG_TextboxSetFont(&window_1, TXB_ID_1, &FONT_6X8);
	UG_TextboxSetBackColor(&window_1, TXB_ID_1, CH2_COLOR);
	UG_TextboxSetText(&window_1, TXB_ID_1, "1V");

	UG_TextboxCreate(&window_1, &txtb1_2, TXB_ID_2, 3*WIND1_ICON_SPACING + 2*WIND1_ICON_WIDTH, 1, 3*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Time scale */
	UG_TextboxSetFont(&window_1, TXB_ID_2, &FONT_6X8);
	UG_TextboxSetBackColor(&window_1, TXB_ID_2, TBASE_ICON_COLOR);
	UG_TextboxSetText(&window_1, TXB_ID_2, "1ms");

	UG_TextboxCreate(&window_1, &txtb1_3, TXB_ID_3, 4*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH, 1, 4*WIND1_ICON_SPACING + 4*WIND1_ICON_WIDTH + WIND1_ICON_SPACING/2 - 1, MENUBAR_HEIGHT - 2);	/* Trigger type */
	UG_TextboxSetFont(&window_1, TXB_ID_3, &FONT_6X8);
	UG_TextboxSetBackColor(&window_1, TXB_ID_3, CH1_COLOR);
	UG_TextboxSetText(&window_1, TXB_ID_3, "Trg:R");

	UG_TextboxCreate(&window_1, &txtb1_4, TXB_ID_4, 5*WIND1_ICON_SPACING + 4*WIND1_ICON_WIDTH - WIND1_ICON_SPACING/2, 1, 5*WIND1_ICON_SPACING + 5*WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Trigger level */
	UG_TextboxSetFont(&window_1, TXB_ID_4, &FONT_6X8);
	UG_TextboxSetBackColor(&window_1, TXB_ID_4, CH1_COLOR);
	UG_TextboxSetText(&window_1, TXB_ID_4, "1.63V");

	UG_TextboxCreate(&window_1, &txtb1_5, TXB_ID_5, 6*WIND1_ICON_SPACING + 5*WIND1_ICON_WIDTH, 1, 6*WIND1_ICON_SPACING + 6*WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Trigger Mode */
	UG_TextboxSetFont(&window_1, TXB_ID_5, &FONT_6X8);
	UG_TextboxSetBackColor(&window_1, TXB_ID_5, MODE_ICON_COLOR);
	UG_TextboxSetText(&window_1, TXB_ID_5, "AUTO");

	UG_TextboxCreate(&window_1, &txtb1_6, TXB_ID_6, 7*WIND1_ICON_SPACING + 6*WIND1_ICON_WIDTH, 1, 7*WIND1_ICON_SPACING + 7*WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Run/Stop icon */
	UG_TextboxSetFont(&window_1, TXB_ID_6, &FONT_6X8);
	UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_RUN);
	UG_TextboxSetText(&window_1, TXB_ID_6, "RUN");

	/*** Create Window 2 (Bottom menubar) ***/
	UG_WindowCreate(&window_2, obj_buff_wnd_2, UGUI_MAX_OBJECTS, window_2_callback);
	UG_WindowSetStyle(&window_2, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_2, 0, LCD_HEIGHT - MENUBAR_HEIGHT, LCD_WIDTH - 1, LCD_HEIGHT - 1);
	UG_WindowSetBackColor(&window_2, C_KHAKI);

	/* Draw "icons" to display various parameters */
	UG_TextboxCreate(&window_2, &txtb2_0, TXB_ID_0, WIND1_ICON_SPACING, 1, WIND1_ICON_SPACING + WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* CH1 vertical offset */
	UG_TextboxSetFont(&window_2, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetBackColor(&window_2, TXB_ID_0, CH1_COLOR);
	UG_TextboxSetText(&window_2, TXB_ID_0, "0V");

	UG_TextboxCreate(&window_2, &txtb2_1, TXB_ID_1, 2*WIND1_ICON_SPACING + WIND1_ICON_WIDTH, 1, 2*WIND1_ICON_SPACING + 2*WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* CH2 vertical offset */
	UG_TextboxSetFont(&window_2, TXB_ID_1, &FONT_6X8);
	UG_TextboxSetBackColor(&window_2, TXB_ID_1, CH2_COLOR);
	UG_TextboxSetText(&window_2, TXB_ID_1, "0V");

	UG_TextboxCreate(&window_2, &txtb2_2, TXB_ID_2, 3*WIND1_ICON_SPACING + 2*WIND1_ICON_WIDTH, 1, 3*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Horizontal offset */
	UG_TextboxSetFont(&window_2, TXB_ID_2, &FONT_6X8);
	UG_TextboxSetBackColor(&window_2, TXB_ID_2, TOFFSET_ICON_COLOR);
	UG_TextboxSetText(&window_2, TXB_ID_2, "0us");

	UG_TextboxCreate(&window_2, &txtb2_3, TXB_ID_3, 4*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH, 1, 4*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH + MEASURE_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Measurement 1 */
	UG_TextboxSetFont(&window_2, TXB_ID_3, &FONT_6X8);
	UG_TextboxSetBackColor(&window_2, TXB_ID_3, INACTIVE_ICON_COLOR);
	UG_TextboxSetText(&window_2, TXB_ID_3, "--");

	UG_TextboxCreate(&window_2, &txtb2_4, TXB_ID_4, 5*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH + MEASURE_ICON_WIDTH, 1, 5*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH + 2*MEASURE_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Measurement 2 */
	UG_TextboxSetFont(&window_2, TXB_ID_4, &FONT_6X8);
	UG_TextboxSetBackColor(&window_2, TXB_ID_4, INACTIVE_ICON_COLOR);
	UG_TextboxSetText(&window_2, TXB_ID_4, "--");

	UG_TextboxCreate(&window_2, &txtb2_5, TXB_ID_5, 6*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH + 2*MEASURE_ICON_WIDTH, 1, 6*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH + 3*MEASURE_ICON_WIDTH - 1, MENUBAR_HEIGHT - 2);	/* Measurement 3 */
	UG_TextboxSetFont(&window_2, TXB_ID_5, &FONT_6X8);
	UG_TextboxSetBackColor(&window_2, TXB_ID_5, INACTIVE_ICON_COLOR);
	UG_TextboxSetText(&window_2, TXB_ID_5, "--");

	UG_TextboxCreate(&window_2, &txtb2_6, TXB_ID_6, 7*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH + 3*MEASURE_ICON_WIDTH - 1, 1, 7*WIND1_ICON_SPACING + 3*WIND1_ICON_WIDTH + 4*MEASURE_ICON_WIDTH - 2, MENUBAR_HEIGHT - 2);	/* Measurement 4 */
	UG_TextboxSetFont(&window_2, TXB_ID_6, &FONT_6X8);
	UG_TextboxSetBackColor(&window_2, TXB_ID_6, INACTIVE_ICON_COLOR);
	UG_TextboxSetText(&window_2, TXB_ID_6, "--");

	/* draw grid */
	drawGrid();

	/*** Create Window 3 (Measure window) ***/
	UG_WindowCreate(&window_3, obj_buff_wnd_3, UGUI_MAX_OBJECTS, window_3_callback);
	UG_WindowSetStyle(&window_3, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_3, LCD_WIDTH - WIND3_WIDTH, MENUBAR_HEIGHT + 10, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 11);
	UG_WindowSetBackColor(&window_3, C_WHITE);

	UG_TextboxCreate(&window_3, &txtb3_0, TXB_ID_0, 1, 1, WIND3_WIDTH - 8, MENUBAR_HEIGHT);		/* Menu heading */
	UG_TextboxSetFont(&window_3, TXB_ID_0, &FONT_8X12);
	UG_TextboxSetBackColor(&window_3, TXB_ID_0, C_MEDIUM_VIOLET_RED);
	UG_TextboxSetText(&window_3, TXB_ID_0, "MEASURE");

	UG_ButtonCreate(&window_3, &button3_0, BTN_ID_0, 6, MENUBAR_HEIGHT + WIND3_BTN_SPACING + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + WIND3_BTN_SPACING + WIND3_BTN_HEIGHT);	/* Measure 1 */
	UG_ButtonSetFont(&window_3, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_3, BTN_ID_0, INACTIVE_ICON_COLOR);
	UG_ButtonSetText(&window_3, BTN_ID_0, "None");

	UG_ButtonCreate(&window_3, &button3_1, BTN_ID_1, 6, MENUBAR_HEIGHT + 2*WIND3_BTN_SPACING + WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 2*WIND3_BTN_SPACING + 2*WIND3_BTN_HEIGHT);	/* Measure 2 */
	UG_ButtonSetFont(&window_3, BTN_ID_1, &FONT_6X8);
	UG_ButtonSetBackColor(&window_3, BTN_ID_1, INACTIVE_ICON_COLOR);
	UG_ButtonSetText(&window_3, BTN_ID_1, "None");

	UG_ButtonCreate(&window_3, &button3_2, BTN_ID_2, 6, MENUBAR_HEIGHT + 3*WIND3_BTN_SPACING + 2*WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 3*WIND3_BTN_SPACING + 3*WIND3_BTN_HEIGHT);	/* Measure 3 */
	UG_ButtonSetFont(&window_3, BTN_ID_2, &FONT_6X8);
	UG_ButtonSetBackColor(&window_3, BTN_ID_2, INACTIVE_ICON_COLOR);
	UG_ButtonSetText(&window_3, BTN_ID_2, "None");

	UG_ButtonCreate(&window_3, &button3_3, BTN_ID_3, 6, MENUBAR_HEIGHT + 4*WIND3_BTN_SPACING + 3*WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 4*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT);	/* Measure 4 */
	UG_ButtonSetFont(&window_3, BTN_ID_3, &FONT_6X8);
	UG_ButtonSetBackColor(&window_3, BTN_ID_3, INACTIVE_ICON_COLOR);
	UG_ButtonSetText(&window_3, BTN_ID_3, "None");

	UG_ButtonCreate(&window_3, &button3_4, BTN_ID_4, 38, MENUBAR_HEIGHT + 5*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 5*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT + 25);	/* Next menu */
	UG_ButtonSetFont(&window_3, BTN_ID_4, &FONT_6X8);
	UG_ButtonSetBackColor(&window_3, BTN_ID_4, C_MEDIUM_VIOLET_RED);
	UG_ButtonSetText(&window_3, BTN_ID_4, ">");

	/*** Create Window 4 (Cursors / Math / Memory depth / Sample rate) ***/
	UG_WindowCreate(&window_4, obj_buff_wnd_4, UGUI_MAX_OBJECTS, window_4_callback);
	UG_WindowSetStyle(&window_4, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_4, LCD_WIDTH - WIND3_WIDTH, MENUBAR_HEIGHT + 10, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 11);
	UG_WindowSetBackColor(&window_4, C_WHITE);

	UG_TextboxCreate(&window_4, &txtb4_0, TXB_ID_0, 1, 1, WIND3_WIDTH - 8, MENUBAR_HEIGHT);		/* Menu heading */
	UG_TextboxSetFont(&window_4, TXB_ID_0, &FONT_8X12);
	UG_TextboxSetBackColor(&window_4, TXB_ID_0, C_MEDIUM_VIOLET_RED);
	UG_TextboxSetText(&window_4, TXB_ID_0, "PAGE 2");

	UG_ButtonCreate(&window_4, &button4_0, BTN_ID_0, 6, MENUBAR_HEIGHT + WIND3_BTN_SPACING + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + WIND3_BTN_SPACING + WIND3_BTN_HEIGHT);	/* Cursors */
	UG_ButtonSetFont(&window_4, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_0, C_ORANGE);
	UG_ButtonSetText(&window_4, BTN_ID_0, "Cursors");

	UG_ButtonCreate(&window_4, &button4_1, BTN_ID_1, 6, MENUBAR_HEIGHT + 2*WIND3_BTN_SPACING + WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 2*WIND3_BTN_SPACING + 2*WIND3_BTN_HEIGHT);	/* Math */
	UG_ButtonSetFont(&window_4, BTN_ID_1, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_1, C_ORANGE);
	UG_ButtonSetText(&window_4, BTN_ID_1, "Math");

	UG_ButtonCreate(&window_4, &button4_2, BTN_ID_2, 6, MENUBAR_HEIGHT + 3*WIND3_BTN_SPACING + 2*WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 3*WIND3_BTN_SPACING + 3*WIND3_BTN_HEIGHT);	/* Memory depth */
	UG_ButtonSetFont(&window_4, BTN_ID_2, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_2, C_ORANGE);
	UG_ButtonSetText(&window_4, BTN_ID_2, "Memory");

	UG_ButtonCreate(&window_4, &button4_3, BTN_ID_3, 6, MENUBAR_HEIGHT + 4*WIND3_BTN_SPACING + 3*WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 4*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT);	/* Sample rate */
	UG_ButtonSetFont(&window_4, BTN_ID_3, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_3, C_ORANGE);
	UG_ButtonSetText(&window_4, BTN_ID_3, "Sample");

	UG_ButtonCreate(&window_4, &button4_4, BTN_ID_4, 6, MENUBAR_HEIGHT + 5*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT + 1, 35, MENUBAR_HEIGHT + 5*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT + 25);		/* Prev menu */
	UG_ButtonSetFont(&window_4, BTN_ID_4, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_4, C_MEDIUM_VIOLET_RED);
	UG_ButtonSetText(&window_4, BTN_ID_4, "<");

	/*** Create Window 5 (Measurement sub-menu) ***/
	UG_WindowCreate(&window_5, obj_buff_wnd_5, UGUI_MAX_OBJECTS, window_5_callback);
	UG_WindowSetStyle(&window_5, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_5, WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1);
	UG_WindowSetBackColor(&window_5, C_WHITE);

	UG_TextboxCreate(&window_5, &txtb5_0, TXB_ID_0, 1, 1, 60, WIND5_BTN_HEIGHT);		/* label */
	UG_TextboxSetFont(&window_5, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetAlignment(&window_5, TXB_ID_0, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_5, TXB_ID_0, "source:");

	UG_TextboxCreate(&window_5, &txtb5_1, TXB_ID_1, 1, WIND5_BTN_HEIGHT + WIND5_BTN_SPACING + 1, 60, 2*WIND5_BTN_HEIGHT + WIND5_BTN_SPACING);		/* label */
	UG_TextboxSetFont(&window_5, TXB_ID_1, &FONT_6X8);
	UG_TextboxSetAlignment(&window_5, TXB_ID_1, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_5, TXB_ID_1, "param:");

	UG_ButtonCreate(&window_5, &button5_0, BTN_ID_0, 71, 1, 130, WIND5_BTN_HEIGHT);		/* source select */
	UG_ButtonSetFont(&window_5, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_5, BTN_ID_0, C_OLIVE);
	UG_ButtonSetText(&window_5, BTN_ID_0, "CH1");

	UG_ButtonCreate(&window_5, &button5_1, BTN_ID_1, 71, WIND5_BTN_HEIGHT + WIND5_BTN_SPACING + 1, 130, 2*WIND5_BTN_HEIGHT + WIND5_BTN_SPACING);	/* measure parameter select */
	UG_ButtonSetFont(&window_5, BTN_ID_1, &FONT_6X8);
	UG_ButtonSetBackColor(&window_5, BTN_ID_1, C_OLIVE);
	UG_ButtonSetText(&window_5, BTN_ID_1, "Freq");
}

/**
  * @brief  Draw horizontal and vertical grid lines.
  * @param  None
  * @retval None
  */
void drawGrid(void)
{
	__IO uint16_t (*pFrame)[LCD_WIDTH];
	int i, j;

	pFrame = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	/* Draw vertical grid lines */
	for(j = LCD_WIDTH/GRID_HORZ_DIVS - 1; j < LCD_WIDTH - 1; j += LCD_WIDTH/GRID_HORZ_DIVS){
		for(i = MENUBAR_HEIGHT; i < LCD_HEIGHT - MENUBAR_HEIGHT; i += GRID_DOT_SPACING){
			pFrame[i][j] = GRID_COLOR;
		}
	}

	/* Draw horizontal grid lines */
	for(i = MENUBAR_HEIGHT; i < LCD_HEIGHT - MENUBAR_HEIGHT; i += (LCD_HEIGHT - 2*MENUBAR_HEIGHT)/GRID_VERT_DIVS){
		for(j = 0; j < LCD_WIDTH; j += GRID_DOT_SPACING){
				pFrame[i][j] = GRID_COLOR;
			}
	}

	/* Draw separator between the two channels */
	i = 135;
	for(j = 0; j < LCD_WIDTH; j += 1){
			pFrame[i][j] = CH_SEPARATOR_COLOR;
	}
}

/**
  * @brief  Convert a 8-bit ADC value to mV/V voltage constant-length string.
  * @param  val: 8-bit ADC value
  * @param  buf: output string of length 5
  * @retval None
  */
void voltsToStr(uint8_t val, char* buf)
{
	float32_t volts;
	char buf2[5];
	uint8_t len;

	volts = (val * 3.3) / 255;

	if(volts < 1){
		itoa(volts*1000, buf2, 10);

		/* append spaces to make it constant-length string (good for displaying) */
		len = strlen(buf2);
		if(len == 1)
			strcat(buf, "  ");
		else if(len == 2)
			strcat(buf, " ");

		strcat(buf, buf2);
		strcat(buf, "mV");
	}
	else{
		gcvt(volts, 3, buf2);

		/* append spaces to make it constant-length string */
		len = strlen(buf2);
		if(len == 3)
			strcat(buf, " ");
		else if(len == 1)
			strcat(buf, "   ");

		strcat(buf, buf2);
		strcat(buf, "V");
	}
}

/**
  * @brief  Convert a frequency value to Hz/KHz constant-length string.
  * @param  freq: frequency to display
  * @param  buf: output string of length 8
  * @retval None
  */
void hertzToStr(float32_t freq, char* buf)
{
	float32_t Khz;
	char buf2[5];
	uint8_t len;

	if(freq < 1000){
		itoa(freq, buf2, 10);

		/* append spaces to make it constant-length string (good for displaying) */
		len = strlen(buf2);
		if(len == 1)
			strcat(buf, "     ");
		else if(len == 2)
			strcat(buf, "    ");
		else if(len == 3)
			strcat(buf, "   ");

		strcat(buf, buf2);
		strcat(buf, "Hz");
	}
	else{
		Khz = freq/1000.0;
		gcvt(Khz, 4, buf2);

		/* append spaces to make it constant-length string */
		len = strlen(buf2);
		if(len == 1)
			strcat(buf, "    ");
		else if(len == 3)
			strcat(buf, "  ");
		else if(len == 4)
			strcat(buf, " ");

		strcat(buf, buf2);
		strcat(buf, "KHz");
	}
}

/**
  * @brief  Convert a time value to us/ms string.
  * @param  t: time to display (in seconds)
  * @param  buf: output string of length 6
  * @retval None
  */
void secToStr(float32_t t, char* buf)
{
	t = t * 1000000;	/* convert to usec */

	if(abs(t) < 100){
		gcvt((int32_t)(t*100)/100.0f, 3 + (t < -10), buf);
		strcat(buf, "us");
	}
	else if(abs(t) < 1000){
		itoa(t, buf, 10);
		strcat(buf, "us");
	}
	else{
		t = t/1000;		/* convert to msec */
		gcvt((int32_t)(t*100)/100.0f, 3 + (t < -10), buf);
		strcat(buf, "ms");
	}
}

/* Callback function for window 1 (top menubar) */
void window_1_callback(UG_MESSAGE* msg)
{
	static uint8_t menu_button_state = 0;

	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
			 	 /* MENU button */
			 	 case BTN_ID_0:
					if(menu_button_state == 0){
						showWindow3 = 1;			/* show measurement menu */
						menu_button_state = 1;
					}
					else{
						showWindow3 = 0;			/* close menus */
						showWindow4 = 0;
						showWindow5 = 0;
						wind5OpenedBy = MEASURE_NONE;
						menu_button_state = 0;
						fillFrameUGUI(0, MENUBAR_HEIGHT, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_BLACK);
						drawGrid();
					}
					break;
			 }
		  }
	  }
	}
}

/* Callback function for window 2 (button menubar) */
void window_2_callback(UG_MESSAGE* msg)
{
	/* Nothing to do */
}

/* Callback function for window 3 (Measurement menu) */
void window_3_callback(UG_MESSAGE* msg)
{
	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
				 /* Measure 1 button */
				 case BTN_ID_0:
					if((showWindow5 == 0) || (wind5OpenedBy != MEASURE1)){
						showWindow5 = 1;			/* show submenu */
						wind5OpenedBy = MEASURE1;
						UG_ButtonSetText(&window_5, BTN_ID_0, (measure1.src == CHANNEL1) ? "CH1" : "CH2");
						UG_ButtonSetText(&window_5, BTN_ID_1, measParamTexts[measure1.param]);
					}
					else{
						showWindow5 = 0;			/* close submenu */
						wind5OpenedBy = MEASURE_NONE;
						fillFrameUGUI(WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1, C_BLACK);
						drawGrid();
					}
					break;

				 /* Measure 2 button */
				 case BTN_ID_1:
					if((showWindow5 == 0) || (wind5OpenedBy != MEASURE2)){
						showWindow5 = 1;			/* show submenu */
						wind5OpenedBy = MEASURE2;
						UG_ButtonSetText(&window_5, BTN_ID_0, (measure2.src == CHANNEL1) ? "CH1" : "CH2");
						UG_ButtonSetText(&window_5, BTN_ID_1, measParamTexts[measure2.param]);
					}
					else{
						showWindow5 = 0;			/* close submenu */
						wind5OpenedBy = MEASURE_NONE;
						fillFrameUGUI(WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1, C_BLACK);
						drawGrid();
					}
					break;

				 /* Measure 3 button */
				 case BTN_ID_2:
					if((showWindow5 == 0) || (wind5OpenedBy != MEASURE3)){
						showWindow5 = 1;			/* show submenu */
						wind5OpenedBy = MEASURE3;
						UG_ButtonSetText(&window_5, BTN_ID_0, (measure3.src == CHANNEL1) ? "CH1" : "CH2");
						UG_ButtonSetText(&window_5, BTN_ID_1, measParamTexts[measure3.param]);
					}
					else{
						showWindow5 = 0;			/* close submenu */
						wind5OpenedBy = MEASURE_NONE;
						fillFrameUGUI(WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1, C_BLACK);
						drawGrid();
					}
					break;

				 /* Measure 4 button */
				 case BTN_ID_3:
					if((showWindow5 == 0) || (wind5OpenedBy != MEASURE4)){
						showWindow5 = 1;			/* show submenu */
						wind5OpenedBy = MEASURE4;
						UG_ButtonSetText(&window_5, BTN_ID_0, (measure4.src == CHANNEL1) ? "CH1" : "CH2");
						UG_ButtonSetText(&window_5, BTN_ID_1, measParamTexts[measure4.param]);
					}
					else{
						showWindow5 = 0;			/* close submenu */
						wind5OpenedBy = MEASURE_NONE;
						fillFrameUGUI(WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1, C_BLACK);
						drawGrid();
					}
					break;

			 	 /* Next button */
			 	 case BTN_ID_4:
					showWindow3 = 0;			/* Go to Page 2 */
					showWindow5 = 0;			/* close any window 3 sub-menus */
					wind5OpenedBy = MEASURE_NONE;
					showWindow4 = 1;
					fillFrameUGUI(WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1, C_BLACK);
					break;
			 }
		  }
	  }
	}
}

/* Callback function for window 4 (Menu page 2) */
void window_4_callback(UG_MESSAGE* msg)
{
	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
			 	 /* Previous button */
			 	 case BTN_ID_4:
					showWindow3 = 1;			/* Go to Page 1 (Measurements) */
					showWindow4 = 0;
					break;
			 }
		  }
	  }
	}
}

/* Callback function for window 5 (Measurement sub-menu) */
void window_5_callback(UG_MESSAGE* msg)
{
	Measure_TypeDef* measPtr;
	uint8_t wind3BtnID;

	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 /* change which measure's settings */
			 if(wind5OpenedBy == MEASURE1){
				 measPtr = &measure1;
				 wind3BtnID = BTN_ID_0;
			 }
			 else if(wind5OpenedBy == MEASURE2){
				 measPtr = &measure2;
				 wind3BtnID = BTN_ID_1;
			 }
			 else if(wind5OpenedBy == MEASURE3){
				 measPtr = &measure3;
				 wind3BtnID = BTN_ID_2;
			 }
			 else{
				 measPtr = &measure4;
				 wind3BtnID = BTN_ID_3;
			 }

			 switch(msg->sub_id)
			 {
				 /* measure source button */
			 	 case BTN_ID_0:							/* toggle channel */
			 		 if(measPtr->src == CHANNEL1)
			 			measPtr->src = CHANNEL2;
			 		 else
						measPtr->src = CHANNEL1;
			 		 break;

				 /* measure param button */
				 case BTN_ID_1:							/* select next measure parameter */
					 if(measPtr->param == MEAS_VAVG)
						 measPtr->param = MEAS_NONE;
					 else
						 measPtr->param = measPtr->param + 1;
					 break;
			 }

			 /* update button text and colors according to chosen setting */
			 UG_ButtonSetText(&window_5, BTN_ID_0, (measPtr->src == CHANNEL1) ? "CH1" : "CH2");
			 UG_ButtonSetText(&window_5, BTN_ID_1, measParamTexts[measPtr->param]);

			 UG_ButtonSetBackColor(&window_3, wind3BtnID, (measPtr->param == MEAS_NONE) ? INACTIVE_ICON_COLOR : ((measPtr->src == CHANNEL1) ? CH1_COLOR : CH2_COLOR));
			 UG_ButtonSetText(&window_3, wind3BtnID, measParamTexts[measPtr->param]);
		  }
	  }
	}
}

/**
  * @brief  Cycles through all the visible windows, displaying them one at a time.
  * @param  None
  * @retval None
  */
void switchNextWindow(void)
{
	static uint8_t currWind = WINDOW1;

	if(currWind == WINDOW1){
		UG_WindowShow(&window_2);	/* bottom menubar */
		currWind = WINDOW2;
	}
	else if(currWind == WINDOW2){
		if(showWindow3){
			UG_WindowShow(&window_3);	/* menu page 1 */
			currWind = WINDOW3;
		}
		else if(showWindow4){			/* menu page 2 */
			UG_WindowShow(&window_4);
			currWind = WINDOW4;
		}
		else{
			UG_WindowShow(&window_1);
			currWind = WINDOW1;
		}
	}
	else if((currWind == WINDOW3) && showWindow5){
		UG_WindowShow(&window_5);
		currWind = WINDOW5;
	}
	else{
		UG_WindowShow(&window_1);	/* top menubar */
		currWind = WINDOW1;
	}

	return;
}

/**
  * @brief  Computes and displays the active measurements.
  * @param  None
  * @retval None
  */
void DisplayMeasurements(void)
{
	static char buf1[11], buf2[11], buf3[11], buf4[11];

	/* measurement 1 */
	if(measure1.param != MEAS_NONE){
		if(measure1.param == MEAS_FREQ){
			strcpy(buf1, "F:");
			hertzToStr(calcMeasure(measure1.src, MEAS_FREQ) * samprateVals[tscale], buf1 + 2);
		}
		else if(measure1.param == MEAS_DUTY){
			strcpy(buf1, measParamTexts[measure1.param]);
			strcat(buf1, ":");
			itoa(calcMeasure(measure1.src, measure1.param), buf1 + 5, 10);
			strcat(buf1, "%");
		}
		else{
			strcpy(buf1, measParamTexts[measure1.param]);
			strcat(buf1, ":");
			voltsToStr(calcMeasure(measure1.src, measure1.param), buf1 + 5);
		}
		UG_TextboxSetBackColor(&window_2, TXB_ID_3, (measure1.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_3, buf1);
	}
	else{
		UG_TextboxSetBackColor(&window_2, TXB_ID_3, INACTIVE_ICON_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_3, "--");
	}

	/* measurement 2 */
	if(measure2.param != MEAS_NONE){
		if(measure2.param == MEAS_FREQ){
			strcpy(buf2, "F:");
			hertzToStr(calcMeasure(measure2.src, MEAS_FREQ) * samprateVals[tscale], buf2 + 2);
		}
		else if(measure2.param == MEAS_DUTY){
			strcpy(buf2, measParamTexts[measure2.param]);
			strcat(buf2, ":");
			itoa(calcMeasure(measure2.src, measure2.param), buf2 + 5, 10);
			strcat(buf2, "%");
		}
		else{
			strcpy(buf2, measParamTexts[measure2.param]);
			strcat(buf2, ":");
			voltsToStr(calcMeasure(measure2.src, measure2.param), buf2 + 5);
		}
		UG_TextboxSetBackColor(&window_2, TXB_ID_4, (measure2.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_4, buf2);
	}
	else{
		UG_TextboxSetBackColor(&window_2, TXB_ID_4, INACTIVE_ICON_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_4, "--");
	}

	/* measurement 3 */
	if(measure3.param != MEAS_NONE){
		if(measure3.param == MEAS_FREQ){
			strcpy(buf3, "F:");
			hertzToStr(calcMeasure(measure3.src, MEAS_FREQ) * samprateVals[tscale], buf3 + 2);
		}
		else if(measure3.param == MEAS_DUTY){
			strcpy(buf3, measParamTexts[measure3.param]);
			strcat(buf3, ":");
			itoa(calcMeasure(measure3.src, measure3.param), buf3 + 5, 10);
			strcat(buf3, "%");
		}
		else{
			strcpy(buf3, measParamTexts[measure3.param]);
			strcat(buf3, ":");
			voltsToStr(calcMeasure(measure3.src, measure3.param), buf3 + 5);
		}
		UG_TextboxSetBackColor(&window_2, TXB_ID_5, (measure3.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_5, buf3);
	}
	else{
		UG_TextboxSetBackColor(&window_2, TXB_ID_5, INACTIVE_ICON_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_5, "--");
	}

	/* measurement 4 */
	if(measure4.param != MEAS_NONE){
		if(measure4.param == MEAS_FREQ){
			strcpy(buf4, "F:");
			hertzToStr(calcMeasure(measure4.src, MEAS_FREQ) * samprateVals[tscale], buf4 + 2);
		}
		else if(measure4.param == MEAS_DUTY){
			strcpy(buf4, measParamTexts[measure4.param]);
			strcat(buf4, ":");
			itoa(calcMeasure(measure4.src, measure4.param), buf4 + 5, 10);
			strcat(buf4, "%");
		}
		else{
			strcpy(buf4, measParamTexts[measure4.param]);
			strcat(buf4, ":");
			voltsToStr(calcMeasure(measure4.src, measure4.param), buf4 + 5);
		}
		UG_TextboxSetBackColor(&window_2, TXB_ID_6, (measure4.src == CHANNEL1) ? CH1_COLOR : CH2_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_6, buf4);
	}
	else{
		UG_TextboxSetBackColor(&window_2, TXB_ID_6, INACTIVE_ICON_COLOR);
		UG_TextboxSetText(&window_2, TXB_ID_6, "--");
	}

	return;
}

/**
  * @brief  Sets the given field as the active field.
  * @param  field
  * @retval None
  */
void goToField(uint8_t field)
{
	selectField(currField, 0);		/* de-select current field */

	currField = field;

	selectField(currField, 1);		/* select next field */

	return;
}

/**
  * @brief  Get the current active field.
  * @param  None
  * @retval Currently active field
  */
uint8_t getCurrField(void)
{
	return currField;
}

/**
  * @brief  Cycles through the fields in the top & bottom menubars.
  * @param  None
  * @retval None
  */
void switchNextField(void)
{
	selectField(currField, 0);		/* de-select current field */

	currField = (currField + 1) % FLD_MAXVALS;

	selectField(currField, 1);		/* select next field */

	return;
}

/**
  * @brief  Highlights or de-highlights a particular field.
  * @param  field: field to operate on
  * @param  sel: 1=highlight, 0=de-highlight.
  * @retval None
  */
static void selectField(uint8_t field, uint8_t sel)
{
	const UG_FONT* font;

	if(sel == 1)
		font = &FONT_7X12;	/* enlarge font to highlight */
	else
		font = &FONT_6X8;

	switch(field)
	{
		case FLD_NONE:
			break;

		case FLD_CH1_VSCALE:
			UG_TextboxSetFont(&window_1, TXB_ID_0, font);
			break;

		case FLD_CH2_VSCALE:
			UG_TextboxSetFont(&window_1, TXB_ID_1, font);
			break;

		case FLD_TSCALE:
			UG_TextboxSetFont(&window_1, TXB_ID_2, font);
			break;

		case FLD_TRIGSRC:
			UG_TextboxSetFont(&window_1, TXB_ID_3, font);
			UG_TextboxSetFont(&window_1, TXB_ID_4, font);
			break;

		case FLD_TRIGTYPE:
			UG_TextboxSetFont(&window_1, TXB_ID_3, font);
			break;

		case FLD_TRIGLVL:
			UG_TextboxSetFont(&window_1, TXB_ID_4, font);
			break;

		case FLD_TRIGMODE:
			UG_TextboxSetFont(&window_1, TXB_ID_5, font);
			break;

		case FLD_RUNSTOP:
			UG_TextboxSetFont(&window_1, TXB_ID_6, font);
			break;

		case FLD_CH1_VOFF:
			UG_TextboxSetFont(&window_2, TXB_ID_0, font);
			break;

		case FLD_CH2_VOFF:
			UG_TextboxSetFont(&window_2, TXB_ID_1, font);
			break;

		case FLD_TOFF:
			UG_TextboxSetFont(&window_2, TXB_ID_2, font);
			break;
	}

	return;
}

/**
  * @brief  Change value of a field in the top/bottom menubar.
  * @param  dir: 0=increment, 1=decrement
  * @retval None
  */
void changeFieldValue(uint8_t dir)
{
	static char bufw1tb3[8] = "Trg:", bufw1tb4[6], bufw2tb0[6], bufw2tb1[6], bufw2tb2[6];

	/* increment/decrement value of the current field */
	switch(currField)
	{
		case FLD_NONE:
			break;

		case FLD_CH1_VSCALE:
			vscale1 += dir?(vscale1==0?0:-1):(vscale1==VSCALE_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_0, vscaleDispVals[vscale1]);
			break;

		case FLD_CH2_VSCALE:
			vscale2 += dir?(vscale2==0?0:-1):(vscale2==VSCALE_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_1, vscaleDispVals[vscale2]);
			break;

		case FLD_TSCALE:
			tscale += dir?(tscale==0?0:-1):(tscale==TSCALE_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_2, tscaleDispVals[tscale]);
			TimADC_init(tscale);
			break;

		case FLD_TRIGSRC:
			trigsrc += dir?(trigsrc==0?0:-1):(trigsrc==TRIGSRC_MAXVALS-1?0:1);

			UG_COLOR color;
			if(trigsrcVals[trigsrc] == TRIGSRC_CH1)
				color = CH1_COLOR;
			else
				color = CH2_COLOR;
			UG_TextboxSetBackColor(&window_1, TXB_ID_3, color);
			UG_TextboxSetBackColor(&window_1, TXB_ID_4, color);
			break;

		case FLD_TRIGTYPE:
			trigtype += dir?(trigtype==0?0:-1):(trigtype==TRIGTYPE_MAXVALS-1?0:1);
			strcpy(bufw1tb3 + 4, trigtypeDispVals[trigtype]);
			UG_TextboxSetText(&window_1, TXB_ID_3, bufw1tb3);
			break;

		case FLD_TRIGLVL:
			triglvl += dir?(triglvl==0?0:-3):(triglvl==255?0:3);
			gcvt((int32_t)((3.3f*(float32_t)triglvl/(float32_t)255)*100)/100.0f, 3, bufw1tb4);
			strcat(bufw1tb4, "V");
			UG_TextboxSetText(&window_1, TXB_ID_4, bufw1tb4);
			break;

		case FLD_TRIGMODE:
			trigmode += dir?(trigmode==0?0:-1):(trigmode==TRIGMODE_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_5, trigmodeDispVals[trigmode]);
			break;

		case FLD_RUNSTOP:
			runstop += dir?(runstop==0?0:-1):(runstop==RUNSTOP_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_6, runstopDispVals[runstop]);
			if(runstopVals[runstop] == RUNSTOP_RUN){
				ADC_Ch1_reinit();
				ADC_Ch2_reinit();
				UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_RUN);
			}
			else{
				CH1_ADC_DMA_STREAM->CR &= ~0x01;		/* disable ch1 stream and confirm */
				while(CH1_ADC_DMA_STREAM->CR & 0x01);
				CH2_ADC_DMA_STREAM->CR &= ~0x01;		/* disable ch2 stream and confirm */
				while(CH2_ADC_DMA_STREAM->CR & 0x01);
				UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_STP);
			}
			break;

		case FLD_CH1_VOFF:
			voff1 += dir?(voff1==-255?0:-3):(voff1==255?0:3);
			gcvt((int32_t)((3.3f*(float32_t)voff1/(float32_t)255)*100)/100.0f, 3 + (voff1 < -1), bufw2tb0);
			strcat(bufw2tb0, "V");
			UG_TextboxSetText(&window_2, TXB_ID_0, bufw2tb0);
			break;

		case FLD_CH2_VOFF:
			voff2 += dir?(voff2==-255?0:-3):(voff2==255?0:3);
			gcvt((int32_t)((3.3f*(float32_t)voff2/(float32_t)255)*100)/100.0f, 3 + (voff2 < -1), bufw2tb1);
			strcat(bufw2tb1, "V");
			UG_TextboxSetText(&window_2, TXB_ID_1, bufw2tb1);
			break;

		case FLD_TOFF:
			toff += dir?(toff==TOFF_LIMIT?0:-6):(toff==ADC_TRIGBUF_SIZE-TOFF_LIMIT?0:6);
			secToStr((float32_t)(toff-240)/(float32_t)samprateVals[tscale], bufw2tb2);
			UG_TextboxSetText(&window_2, TXB_ID_2, bufw2tb2);
			break;
	}

	return;
}
