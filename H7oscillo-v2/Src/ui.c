/**
  ******************************************************************************
  * @file    ui.c
  * @author  anirudhr
  * @brief   Implements functions to create and manage the graphical user interface.
  ******************************************************************************
  */

/* Includes */
#include "ui.h"


static void drawGridHoriz(void);
static void drawGridVerti(void);
static void selectField(uint8_t field, uint8_t sel);
static void window_1_callback(UG_MESSAGE* msg);
static void window_2_callback(UG_MESSAGE* msg);
static void window_3_callback(UG_MESSAGE* msg);
static void window_4_callback(UG_MESSAGE* msg);
static void window_5_callback(UG_MESSAGE* msg);
static void window_6_callback(UG_MESSAGE* msg);
static void window_7_callback(UG_MESSAGE* msg);
static void window_8_callback(UG_MESSAGE* msg);
static void window_9_callback(UG_MESSAGE* msg);
static void window_10_callback(UG_MESSAGE* msg);
static uint16_t findNextValidFile(uint16_t x);
static uint16_t findPrevValidFile(uint16_t x);
static uint8_t readssinfo(uint8_t* nScrnshots, uint16_t* maxfilename);
static uint8_t writessinfo(uint8_t nScrnshots, uint16_t maxfilename);

/* uGUI related globals */
UG_GUI gui;
/* window 1 - Top menubar */
UG_WINDOW window_1;
UG_OBJECT obj_buff_wnd_1[8];
UG_BUTTON button1_0;
UG_TEXTBOX txtb1_0;
UG_TEXTBOX txtb1_1;
UG_TEXTBOX txtb1_2;
UG_TEXTBOX txtb1_3;
UG_TEXTBOX txtb1_4;
UG_TEXTBOX txtb1_5;
UG_TEXTBOX txtb1_6;
/* window 2 - Bottom menubar */
UG_WINDOW window_2;
UG_OBJECT obj_buff_wnd_2[7];
UG_TEXTBOX txtb2_0;
UG_TEXTBOX txtb2_1;
UG_TEXTBOX txtb2_2;
UG_TEXTBOX txtb2_3;
UG_TEXTBOX txtb2_4;
UG_TEXTBOX txtb2_5;
UG_TEXTBOX txtb2_6;
/* window 3 - Menu page 1 */
UG_WINDOW window_3;
UG_OBJECT obj_buff_wnd_3[6];
UG_TEXTBOX txtb3_0;
UG_BUTTON button3_0;
UG_BUTTON button3_1;
UG_BUTTON button3_2;
UG_BUTTON button3_3;
UG_BUTTON button3_4;
/* window 4 - Menu page 2 */
UG_WINDOW window_4;
UG_OBJECT obj_buff_wnd_4[6];
UG_TEXTBOX txtb4_0;
UG_BUTTON button4_0;
UG_BUTTON button4_1;
UG_BUTTON button4_2;
UG_BUTTON button4_3;
UG_BUTTON button4_4;
/* window 5 - Measure submenu */
UG_WINDOW window_5;
UG_OBJECT obj_buff_wnd_5[4];
UG_TEXTBOX txtb5_0;
UG_TEXTBOX txtb5_1;
UG_BUTTON button5_0;
UG_BUTTON button5_1;
/* window 6 - Display mode submenu */
UG_WINDOW window_6;
UG_OBJECT obj_buff_wnd_6[2];
UG_TEXTBOX txtb6_0;
UG_BUTTON button6_0;
/* window 7 - FFT submenu */
UG_WINDOW window_7;
UG_OBJECT obj_buff_wnd_7[2];
UG_TEXTBOX txtb7_0;
UG_BUTTON button7_0;
/* window 8 - FFT submenu sub-menu */
UG_WINDOW window_8;
UG_OBJECT obj_buff_wnd_8[1];
UG_TEXTBOX txtb8_0;
/* window 9 - Math submenu */
UG_WINDOW window_9;
UG_OBJECT obj_buff_wnd_9[6];
UG_TEXTBOX txtb9_0;
UG_TEXTBOX txtb9_1;
UG_TEXTBOX txtb9_2;
UG_BUTTON button9_0;
UG_BUTTON button9_1;
UG_BUTTON button9_2;
/* window 10 - Cursors submenu */
UG_WINDOW window_10;
UG_OBJECT obj_buff_wnd_10[8];
UG_TEXTBOX txtb10_0;
UG_TEXTBOX txtb10_1;
UG_TEXTBOX txtb10_2;
UG_TEXTBOX txtb10_3;
UG_TEXTBOX txtb10_4;
UG_BUTTON button10_0;
UG_BUTTON button10_1;
UG_BUTTON button10_2;

/* Menu page display selector flags */
static uint8_t showWindow3 = 0;
static uint8_t showWindow4 = 0;
static uint8_t showWindow5 = 0;
static uint8_t showWindow6 = 0;
static uint8_t showWindow7 = 0;
static uint8_t showWindow8 = 0;
static uint8_t showWindow9 = 0;
static uint8_t showWindow10 = 0;

static uint8_t wind5OpenedBy = MEASURE_NONE;
static uint8_t currField = FLD_NONE;					/* currently selected field in the top and bottom menubar */

static uint8_t mathField = MATHFLD_NONE;				/* currently selected field in the math menu */

static uint8_t cursorMode = CURSOR_MODE_OFF;
static uint16_t cursorApos = CURSORA_SPLITCH1_INITPOS;
static uint16_t cursorBpos = CURSORB_SPLITCH2_INITPOS;
static uint8_t cursorField = CURSORFLD_NONE;			/* currently selected field in the cursor menu */

static uint16_t currScrnshot = 0;						/* currently displayed screenshot */

/* strings to store button & textbox texts */
static char bufw1tb3[8] = "Trg:", bufw1tb4[6], bufw2tb0[6], bufw2tb1[6], bufw2tb2[8], bufw8tb0[9], bufw9btn2[6], bufw10btn1[8], bufw10btn2[8], bufw10tb4[8];

static uint8_t trigCursorImg[CURSOR_WIDTH][CURSOR_LENGTH] = {
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0},
	   {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}};

static uint8_t ch1RefCursorImg[CURSOR_WIDTH][CURSOR_LENGTH] = {
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	   {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0},
	   {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}};

static uint8_t ch2RefCursorImg[CURSOR_WIDTH][CURSOR_LENGTH] = {
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	   {1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0},
	   {1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0},
	   {1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0},
	   {1, 1, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0},
	   {1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}};

static uint8_t toffCursorImg[CURSOR_LENGTH][CURSOR_WIDTH] = {
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	   {0, 0, 1, 1, 1, 1, 1, 1, 0, 0},
	   {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {0, 0, 0, 0, 1, 1, 0, 0, 0, 0}};

static uint8_t toffLeftCursorImg[CURSOR_LENGTH][CURSOR_WIDTH] = {
	   {0, 0, 0, 0, 1, 1, 1, 0, 0, 0},
	   {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
	   {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	   {0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	   {0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
	   {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {0, 0, 0, 0, 1, 1, 1, 0, 0, 0}};

static uint8_t toffRightCursorImg[CURSOR_LENGTH][CURSOR_WIDTH] = {
	   {0, 0, 0, 1, 1, 1, 0, 0, 0, 0},
	   {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
	   {0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
	   {0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	   {0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
	   {0, 0, 0, 1, 1, 1, 1, 1, 1, 0},
	   {0, 0, 0, 1, 1, 1, 1, 1, 0, 0},
	   {0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
	   {0, 0, 0, 1, 1, 1, 0, 0, 0, 0}};

static uint8_t mathRefCursorImg[CURSOR_WIDTH][CURSOR_LENGTH] = {
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
	   {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0}};

/**
  * @brief  Create windows and other basic ui elements.
  * @param  None
  * @retval None
  */
void initUI(void)
{
	/*** Create Window 1 (Top menubar) ***/
	UG_WindowCreate(&window_1, obj_buff_wnd_1, 8, window_1_callback);
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
	UG_WindowCreate(&window_2, obj_buff_wnd_2, 7, window_2_callback);
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
	UG_WindowCreate(&window_3, obj_buff_wnd_3, 6, window_3_callback);
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

	/*** Create Window 4 (Display mode / FFT / Cursors / Math) ***/
	UG_WindowCreate(&window_4, obj_buff_wnd_4, 6, window_4_callback);
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
	UG_ButtonSetText(&window_4, BTN_ID_0, "DispMd");

	UG_ButtonCreate(&window_4, &button4_1, BTN_ID_1, 6, MENUBAR_HEIGHT + 2*WIND3_BTN_SPACING + WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 2*WIND3_BTN_SPACING + 2*WIND3_BTN_HEIGHT);	/* Math */
	UG_ButtonSetFont(&window_4, BTN_ID_1, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_1, C_ORANGE);
	UG_ButtonSetText(&window_4, BTN_ID_1, "FFT");

	UG_ButtonCreate(&window_4, &button4_2, BTN_ID_2, 6, MENUBAR_HEIGHT + 3*WIND3_BTN_SPACING + 2*WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 3*WIND3_BTN_SPACING + 3*WIND3_BTN_HEIGHT);	/* Memory depth */
	UG_ButtonSetFont(&window_4, BTN_ID_2, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_2, C_ORANGE);
	UG_ButtonSetText(&window_4, BTN_ID_2, "Cursors");

	UG_ButtonCreate(&window_4, &button4_3, BTN_ID_3, 6, MENUBAR_HEIGHT + 4*WIND3_BTN_SPACING + 3*WIND3_BTN_HEIGHT + 1, WIND3_WIDTH - 13, MENUBAR_HEIGHT + 4*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT);	/* Sample rate */
	UG_ButtonSetFont(&window_4, BTN_ID_3, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_3, C_ORANGE);
	UG_ButtonSetText(&window_4, BTN_ID_3, "Math");

	UG_ButtonCreate(&window_4, &button4_4, BTN_ID_4, 6, MENUBAR_HEIGHT + 5*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT + 1, 35, MENUBAR_HEIGHT + 5*WIND3_BTN_SPACING + 4*WIND3_BTN_HEIGHT + 25);		/* Prev menu */
	UG_ButtonSetFont(&window_4, BTN_ID_4, &FONT_6X8);
	UG_ButtonSetBackColor(&window_4, BTN_ID_4, C_MEDIUM_VIOLET_RED);
	UG_ButtonSetText(&window_4, BTN_ID_4, "<");

	/*** Create Window 5 (Measurement sub-menu) ***/
	UG_WindowCreate(&window_5, obj_buff_wnd_5, 4, window_5_callback);
	UG_WindowSetStyle(&window_5, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_5, WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1);
	UG_WindowSetBackColor(&window_5, C_WHITE);

	UG_TextboxCreate(&window_5, &txtb5_0, TXB_ID_0, 1, 1, WIND5_BTN_WIDTH, WIND5_BTN_HEIGHT);		/* label */
	UG_TextboxSetFont(&window_5, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetAlignment(&window_5, TXB_ID_0, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_5, TXB_ID_0, "source:");

	UG_TextboxCreate(&window_5, &txtb5_1, TXB_ID_1, 1, WIND5_BTN_HEIGHT + WIND5_BTN_SPACING + 1, WIND5_BTN_WIDTH, 2*WIND5_BTN_HEIGHT + WIND5_BTN_SPACING);		/* label */
	UG_TextboxSetFont(&window_5, TXB_ID_1, &FONT_6X8);
	UG_TextboxSetAlignment(&window_5, TXB_ID_1, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_5, TXB_ID_1, "param:");

	UG_ButtonCreate(&window_5, &button5_0, BTN_ID_0, 71, 1, 71 + WIND5_BTN_WIDTH - 1, WIND5_BTN_HEIGHT);	/* source select */
	UG_ButtonSetFont(&window_5, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_5, BTN_ID_0, C_OLIVE);
	UG_ButtonSetText(&window_5, BTN_ID_0, "CH1");

	UG_ButtonCreate(&window_5, &button5_1, BTN_ID_1, 71, WIND5_BTN_HEIGHT + WIND5_BTN_SPACING + 1, 71 + WIND5_BTN_WIDTH - 1, 2*WIND5_BTN_HEIGHT + WIND5_BTN_SPACING);	/* measure parameter select */
	UG_ButtonSetFont(&window_5, BTN_ID_1, &FONT_6X8);
	UG_ButtonSetBackColor(&window_5, BTN_ID_1, C_OLIVE);
	UG_ButtonSetText(&window_5, BTN_ID_1, "Freq");

	/*** Create Window 6 (Display mode sub-menu) ***/
	UG_WindowCreate(&window_6, obj_buff_wnd_6, 2, window_6_callback);
	UG_WindowSetStyle(&window_6, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_6, WIND6_X_START, WIND6_Y_START, WIND6_X_START + WIND6_WIDTH - 1, WIND6_Y_START + WIND6_HEIGHT - 1);
	UG_WindowSetBackColor(&window_6, C_WHITE);

	UG_TextboxCreate(&window_6, &txtb6_0, TXB_ID_0, 1, 1, WIND6_BTN_WIDTH, WIND6_BTN_HEIGHT);	/* label */
	UG_TextboxSetFont(&window_6, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetAlignment(&window_6, TXB_ID_0, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_6, TXB_ID_0, "Mode:");

	UG_ButtonCreate(&window_6, &button6_0, BTN_ID_0, 71, 1, 71 + WIND6_BTN_WIDTH - 1, WIND6_BTN_HEIGHT);	/* mode select */
	UG_ButtonSetFont(&window_6, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_6, BTN_ID_0, C_OLIVE);
	UG_ButtonSetText(&window_6, BTN_ID_0, "Split");

	/*** Create Window 7 (FFT sub-menu) ***/
	UG_WindowCreate(&window_7, obj_buff_wnd_7, 2, window_7_callback);
	UG_WindowSetStyle(&window_7, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_7, WIND7_X_START, WIND7_Y_START, WIND7_X_START + WIND7_WIDTH - 1, WIND7_Y_START + WIND7_HEIGHT - 1);
	UG_WindowSetBackColor(&window_7, C_WHITE);

	UG_TextboxCreate(&window_7, &txtb7_0, TXB_ID_0, 1, 1, WIND7_BTN_WIDTH, WIND7_BTN_HEIGHT);	/* label */
	UG_TextboxSetFont(&window_7, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetAlignment(&window_7, TXB_ID_0, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_7, TXB_ID_0, "Source:");

	UG_ButtonCreate(&window_7, &button7_0, BTN_ID_0, 71, 1, 71 + WIND7_BTN_WIDTH - 1, WIND7_BTN_HEIGHT);	/* FFT source select */
	UG_ButtonSetFont(&window_7, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_7, BTN_ID_0, C_OLIVE);
	UG_ButtonSetText(&window_7, BTN_ID_0, "OFF");

	/*** Create Window 8 (FFT sub-menu sub-menu) ***/
	UG_WindowCreate(&window_8, obj_buff_wnd_8, 1, window_8_callback);
	UG_WindowSetStyle(&window_8, WND_STYLE_2D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_8, WIND8_X_START, WIND8_Y_START, WIND8_X_START + WIND8_WIDTH - 1, WIND8_Y_START + WIND8_HEIGHT - 1);
	UG_WindowSetBackColor(&window_8, C_WHITE);

	UG_TextboxCreate(&window_8, &txtb8_0, TXB_ID_0, 1, 1, WIND8_BTN_WIDTH, WIND8_BTN_HEIGHT);	/* label */
	UG_TextboxSetFont(&window_8, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetAlignment(&window_8, TXB_ID_0, ALIGN_CENTER_LEFT);
	UG_TextboxSetText(&window_8, TXB_ID_0, "0Hz");

	/*** Create Window 9 (Math sub-menu) ***/
	UG_WindowCreate(&window_9, obj_buff_wnd_9, 6, window_9_callback);
	UG_WindowSetStyle(&window_9, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_9, WIND9_X_START, WIND9_Y_START, WIND9_X_START + WIND9_WIDTH - 1, WIND9_Y_START + WIND9_HEIGHT - 1);
	UG_WindowSetBackColor(&window_9, C_WHITE);

	UG_TextboxCreate(&window_9, &txtb9_0, TXB_ID_0, 1, 1, WIND9_BTN_WIDTH, WIND9_BTN_HEIGHT);	/* label */
	UG_TextboxSetFont(&window_9, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetAlignment(&window_9, TXB_ID_0, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_9, TXB_ID_0, "Op:");

	UG_TextboxCreate(&window_9, &txtb9_1, TXB_ID_1, 1, WIND9_BTN_HEIGHT + WIND9_BTN_SPACING + 1, WIND9_BTN_WIDTH, 2*WIND9_BTN_HEIGHT + WIND9_BTN_SPACING);	/* label */
	UG_TextboxSetFont(&window_9, TXB_ID_1, &FONT_6X8);
	UG_TextboxSetAlignment(&window_9, TXB_ID_1, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_9, TXB_ID_1, "Vscale:");

	UG_TextboxCreate(&window_9, &txtb9_2, TXB_ID_2, 1, 2*WIND9_BTN_HEIGHT + 2*WIND9_BTN_SPACING + 1, WIND9_BTN_WIDTH, 3*WIND9_BTN_HEIGHT + 2*WIND9_BTN_SPACING);	/* label */
	UG_TextboxSetFont(&window_9, TXB_ID_2, &FONT_6X8);
	UG_TextboxSetAlignment(&window_9, TXB_ID_2, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_9, TXB_ID_2, "Offset:");

	UG_ButtonCreate(&window_9, &button9_0, BTN_ID_0, 71, 1, 71 + WIND9_BTN_WIDTH - 1, WIND9_BTN_HEIGHT);	/* Operation select */
	UG_ButtonSetFont(&window_9, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_9, BTN_ID_0, C_OLIVE);
	UG_ButtonSetText(&window_9, BTN_ID_0, "OFF");

	UG_ButtonCreate(&window_9, &button9_1, BTN_ID_1, 71, WIND9_BTN_HEIGHT + WIND9_BTN_SPACING + 1, 71 + WIND9_BTN_WIDTH - 1, 2*WIND9_BTN_HEIGHT + WIND9_BTN_SPACING + 1);	/* Vertical scale */
	UG_ButtonSetFont(&window_9, BTN_ID_1, &FONT_6X8);
	UG_ButtonSetBackColor(&window_9, BTN_ID_1, C_OLIVE);
	UG_ButtonSetText(&window_9, BTN_ID_1, "1V");

	UG_ButtonCreate(&window_9, &button9_2, BTN_ID_2, 71, 2*WIND9_BTN_HEIGHT + 2*WIND9_BTN_SPACING + 1, 71 + WIND9_BTN_WIDTH - 1, 3*WIND9_BTN_HEIGHT + 2*WIND9_BTN_SPACING + 1);	/* Vertical offset */
	UG_ButtonSetFont(&window_9, BTN_ID_2, &FONT_6X8);
	UG_ButtonSetBackColor(&window_9, BTN_ID_2, C_OLIVE);
	UG_ButtonSetText(&window_9, BTN_ID_2, "0V");

	/*** Create Window 10 (Cursors sub-menu) ***/
	UG_WindowCreate(&window_10, obj_buff_wnd_10, 8, window_10_callback);
	UG_WindowSetStyle(&window_10, WND_STYLE_3D | WND_STYLE_HIDE_TITLE);
	UG_WindowResize(&window_10, WIND10_X_START, WIND10_Y_START, WIND10_X_START + WIND10_WIDTH - 1, WIND10_Y_START + WIND10_HEIGHT - 1);
	UG_WindowSetBackColor(&window_10, C_WHITE);

	UG_TextboxCreate(&window_10, &txtb10_0, TXB_ID_0, 1, 1, WIND10_BTN_WIDTH, WIND10_BTN_HEIGHT);	/* label */
	UG_TextboxSetFont(&window_10, TXB_ID_0, &FONT_6X8);
	UG_TextboxSetAlignment(&window_10, TXB_ID_0, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_10, TXB_ID_0, "Type:");

	UG_TextboxCreate(&window_10, &txtb10_1, TXB_ID_1, 1, WIND10_BTN_HEIGHT + WIND10_BTN_SPACING2 + 1, WIND10_BTN_WIDTH, 2*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING2);	/* label */
	UG_TextboxSetFont(&window_10, TXB_ID_1, &FONT_6X8);
	UG_TextboxSetAlignment(&window_10, TXB_ID_1, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_10, TXB_ID_1, "CurA:");

	UG_TextboxCreate(&window_10, &txtb10_2, TXB_ID_2, 1, 2*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + WIND10_BTN_SPACING2 + 1, WIND10_BTN_WIDTH, 3*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + WIND10_BTN_SPACING2);	/* label */
	UG_TextboxSetFont(&window_10, TXB_ID_2, &FONT_6X8);
	UG_TextboxSetAlignment(&window_10, TXB_ID_2, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_10, TXB_ID_2, "CurB:");

	UG_TextboxCreate(&window_10, &txtb10_3, TXB_ID_3, 1, 3*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + 2*WIND10_BTN_SPACING2 + 1, WIND10_BTN_WIDTH, 4*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + 2*WIND10_BTN_SPACING2);	/* label */
	UG_TextboxSetFont(&window_10, TXB_ID_3, &FONT_6X8);
	UG_TextboxSetAlignment(&window_10, TXB_ID_3, ALIGN_CENTER_RIGHT);
	UG_TextboxSetText(&window_10, TXB_ID_3, "A-B:");

	UG_TextboxCreate(&window_10, &txtb10_4, TXB_ID_4, 71, 3*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + 2*WIND10_BTN_SPACING2 + 1, 71 + WIND10_BTN_WIDTH, 4*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + 2*WIND10_BTN_SPACING2);	/* label */
	UG_TextboxSetFont(&window_10, TXB_ID_4, &FONT_6X8);
	UG_TextboxSetBackColor(&window_10, TXB_ID_4, C_OLIVE);
	UG_TextboxSetAlignment(&window_10, TXB_ID_4, ALIGN_CENTER);
	UG_TextboxSetText(&window_10, TXB_ID_4, "--");

	UG_ButtonCreate(&window_10, &button10_0, BTN_ID_0, 71, 1, 71 + WIND10_BTN_WIDTH, WIND10_BTN_HEIGHT);		/* Cursor type */
	UG_ButtonSetFont(&window_10, BTN_ID_0, &FONT_6X8);
	UG_ButtonSetBackColor(&window_10, BTN_ID_0, C_OLIVE);
	UG_ButtonSetText(&window_10, BTN_ID_0, "OFF");

	UG_ButtonCreate(&window_10, &button10_1, BTN_ID_1, 71, WIND10_BTN_HEIGHT + WIND10_BTN_SPACING2 + 1, 71 + WIND10_BTN_WIDTH, 2*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING2);	/* Cursor A position */
	UG_ButtonSetFont(&window_10, BTN_ID_1, &FONT_6X8);
	UG_ButtonSetBackColor(&window_10, BTN_ID_1, C_OLIVE);
	UG_ButtonSetText(&window_10, BTN_ID_1, "--");

	UG_ButtonCreate(&window_10, &button10_2, BTN_ID_2, 71, 2*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + WIND10_BTN_SPACING2 + 1, 71 + WIND10_BTN_WIDTH, 3*WIND10_BTN_HEIGHT + WIND10_BTN_SPACING1 + WIND10_BTN_SPACING2);	/* Cursor B position */
	UG_ButtonSetFont(&window_10, BTN_ID_2, &FONT_6X8);
	UG_ButtonSetBackColor(&window_10, BTN_ID_2, C_OLIVE);
	UG_ButtonSetText(&window_10, BTN_ID_2, "--");
}

/**
  * @brief  Draw horizontal and vertical grid lines.
  * @param  None
  * @retval None
  */
void drawGrid(void)
{
	drawGridHoriz();

	drawGridVerti();
}

/**
  * @brief  Draw horizontal grid lines and channel separator.
  * @param  None
  * @retval None
  */
static void drawGridHoriz(void)
{
	__IO uint16_t (*pFrame)[LCD_WIDTH];
	int32_t i, j;

	pFrame = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	/* Draw horizontal grid lines */
	for(i = MENUBAR_HEIGHT; i < LCD_HEIGHT - MENUBAR_HEIGHT; i += (LCD_HEIGHT - 2*MENUBAR_HEIGHT)/GRID_VERT_DIVS){
		for(j = 0; j < LCD_WIDTH; j += GRID_DOT_SPACING){
				pFrame[i][j] = GRID_COLOR;
			}
	}

	if(chDispMode == CHDISPMODE_SPLIT){
		/* Draw separator between the two channels */
		i = CH_SEPARATOR_POS;
		for(j = 0; j < LCD_WIDTH; j += 1){
			pFrame[i][j] = CH_SEPARATOR_COLOR;
		}
	}
}

/**
  * @brief  Draw vertical grid lines.
  * @param  None
  * @retval None
  */
static void drawGridVerti(void)
{
	__IO uint16_t (*pFrame)[LCD_WIDTH];
	int32_t i, j;

	pFrame = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	/* Draw vertical grid lines */
	for(j = LCD_WIDTH/GRID_HORZ_DIVS - 1; j < LCD_WIDTH - 1; j += LCD_WIDTH/GRID_HORZ_DIVS){
		for(i = MENUBAR_HEIGHT; i < LCD_HEIGHT - MENUBAR_HEIGHT; i += GRID_DOT_SPACING){
			pFrame[i][j] = GRID_COLOR;
		}
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

	volts = (val * 3.3f) / 255;

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
	float32_t Khz, Mhz;
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
	else if(freq < 1000000){
		Khz = freq/1000.0f;
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
	else{
		Mhz = freq/1000000.0f;
		gcvt(Mhz, 4, buf2);

		/* append spaces to make it constant-length string */
		len = strlen(buf2);
		if(len == 1)
			strcat(buf, "    ");
		else if(len == 3)
			strcat(buf, "  ");
		else if(len == 4)
			strcat(buf, " ");

		strcat(buf, buf2);
		strcat(buf, "MHz");
	}
}

/**
  * @brief  Convert a time value to us/ms string.
  * @param  t: time to display (in seconds)
  * @param  buf: output string of length 7
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
static void window_1_callback(UG_MESSAGE* msg)
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

						/* de-select any active cursor fields */
						if(cursorField != CURSORFLD_NONE){
							cursorField = CURSORFLD_NONE;
							UG_ButtonSetFont(&window_10, BTN_ID_1, &FONT_6X8);
							UG_ButtonSetFont(&window_10, BTN_ID_2, &FONT_6X8);
						}

						menu_button_state = 1;
					}
					else{
						showWindow3 = 0;			/* close menus and sub-menus */
						showWindow4 = 0;
						fillFrameUGUI(LCD_WIDTH - WIND3_WIDTH, MENUBAR_HEIGHT + 10, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 11, C_BLACK);	/* clear menu area */
						if(showWindow5){
							fillFrameUGUI(WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1, C_BLACK);
							drawGrid();
						}
						else{
							clearWind4Submenus();
						}

						/* redraw red border since it would get erased due to the menu */
						if(staticMode){
							drawRedBorder();
						}

						/* redraw cursors, if required */
						if(cursorMode != CURSOR_MODE_OFF){
							changeFieldValueCursor(2);
						}

						showWindow5 = 0;
						showWindow6 = 0;
						showWindow7 = 0;
						showWindow9 = 0;
						showWindow10 = 0;
						wind5OpenedBy = MEASURE_NONE;
						mathField = MATHFLD_NONE;
						menu_button_state = 0;
					}
					break;
			 }
		  }
	  }
	}
}

/* Callback function for window 2 (bottom menubar) */
static void window_2_callback(UG_MESSAGE* msg)
{
	/* Nothing to do */
}

/* Callback function for window 3 (Measurement menu) */
static void window_3_callback(UG_MESSAGE* msg)
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
					showWindow3 = 0;
					showWindow5 = 0;			/* close any window 3 sub-menus */
					fillFrameUGUI(WIND5_X_START, WIND5_Y_START, WIND5_X_START + WIND5_WIDTH - 1, WIND5_Y_START + WIND5_HEIGHT - 1, C_BLACK);
					wind5OpenedBy = MEASURE_NONE;
					showWindow4 = 1;			/* Go to Page 2 */
					drawGrid();
					break;
			 }
		  }
	  }
	}
}

/* Callback function for window 5 (Measurement sub-menu) */
static void window_5_callback(UG_MESSAGE* msg)
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

/* Callback function for window 4 (Menu page 2) */
static void window_4_callback(UG_MESSAGE* msg)
{
	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
			 	 /* Display mode */
			 	 case BTN_ID_0:
					if(showWindow6 == 0){
						clearWind4Submenus();
						showWindow6 = 1;			/* show submenu */
						showWindow7 = 0;			/* close other submenus */
						showWindow9 = 0;
						showWindow10 = 0;
						mathField = MATHFLD_NONE;
						cursorField = CURSORFLD_NONE;
					}
					else{
						showWindow6 = 0;			/* close submenu */
						fillFrameUGUI(WIND6_X_START, WIND6_Y_START, WIND6_X_START + WIND6_WIDTH - 1, WIND6_Y_START + WIND6_HEIGHT - 1, C_BLACK);
						drawGrid();
					}
			 		break;

				 /* FFT */
				 case BTN_ID_1:
					if(showWindow7 == 0){
						clearWind4Submenus();
						showWindow7 = 1;			/* show submenu */
						showWindow6 = 0;			/* close other submenus */
						showWindow9 = 0;
						showWindow10 = 0;
						mathField = MATHFLD_NONE;
						cursorField = CURSORFLD_NONE;
					}
					else{
						showWindow7 = 0;			/* close submenu */
						fillFrameUGUI(WIND7_X_START, WIND7_Y_START, WIND7_X_START + WIND7_WIDTH - 1, WIND7_Y_START + WIND7_HEIGHT - 1, C_BLACK);
						drawGrid();
					}
					break;

				 /* Cursors */
				 case BTN_ID_2:
					if(showWindow10 == 0){
						clearWind4Submenus();
						showWindow10 = 1;			/* show submenu */
						showWindow6 = 0;			/* close other submenus */
						showWindow7 = 0;
						showWindow9 = 0;
						UG_ButtonSetFont(&window_10, BTN_ID_1, &FONT_6X8);	/* de-highlight the cursor fields */
						UG_ButtonSetFont(&window_10, BTN_ID_2, &FONT_6X8);
						mathField = MATHFLD_NONE;
					}
					else{
						showWindow10 = 0;			/* close submenu */
						fillFrameUGUI(WIND10_X_START, WIND10_Y_START, WIND10_X_START + WIND10_WIDTH - 1, WIND10_Y_START + WIND10_HEIGHT - 1, C_BLACK);
						cursorField = CURSORFLD_NONE;
						drawGrid();
					}
					break;

				 /* Math */
				 case BTN_ID_3:
					if(showWindow9 == 0){
						clearWind4Submenus();
						showWindow9 = 1;			/* show submenu */
						showWindow6 = 0;			/* close other submenus */
						showWindow7 = 0;
						showWindow10 = 0;
						UG_ButtonSetFont(&window_9, BTN_ID_1, &FONT_6X8);	/* de-highlight the math fields */
						UG_ButtonSetFont(&window_9, BTN_ID_2, &FONT_6X8);
						cursorField = CURSORFLD_NONE;
					}
					else{
						showWindow9 = 0;			/* close submenu */
						fillFrameUGUI(WIND9_X_START, WIND9_Y_START, WIND9_X_START + WIND9_WIDTH - 1, WIND9_Y_START + WIND9_HEIGHT - 1, C_BLACK);
						mathField = MATHFLD_NONE;
						drawGrid();
					}
					break;

			 	 /* Previous button */
			 	 case BTN_ID_4:
			 		showWindow4 = 0;
			 		clearWind4Submenus();	/* close any window 4 sub-menus */
					showWindow6 = 0;
					showWindow7 = 0;
					showWindow9 = 0;
					showWindow10 = 0;
					mathField = MATHFLD_NONE;
					cursorField = CURSORFLD_NONE;
					showWindow3 = 1;			/* Go to Page 1 */
					break;
			 }
		  }
	  }
	}
}

/* Callback function for window 6 (Display mode sub-menu) */
static void window_6_callback(UG_MESSAGE* msg)
{
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;
	int32_t i, j;

	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
			 	 /* change display mode */
			 	 case BTN_ID_0:
			 		if(chDispMode != CHDISPMODE_FFT && mathOp == MATH_OP_NONE && cursorMode == CURSOR_MODE_OFF){
						chDispMode = (chDispMode + 1) % 3;
						UG_ButtonSetText(&window_6, BTN_ID_0, chDispMode == CHDISPMODE_SPLIT ? "Split" : (chDispMode == CHDISPMODE_MERGE ? "Merge" : "Single"));

						if(chDispMode != CHDISPMODE_SPLIT){
							/* Erase separator between the two channels */
							i = CH_SEPARATOR_POS;
							for(j = 0; j < LCD_WIDTH; j += 1){
								pFrame[i][j] = C_BLACK;
							}

							drawGrid();
						}

						/* force trigsrc to be ch1 in single display mode */
						if(chDispMode == CHDISPMODE_SNGL && trigsrcVals[trigsrc] == TRIGSRC_CH2){
							goToField(FLD_TRIGSRC);
							changeFieldValue(1);
						}

						/* force the cursors to be redrawn */
						goToField(FLD_NONE);
						changeFieldValue(2);
			 		}
			 		break;
			 }
		  }
	  }
	}
}

/* Callback function for window 7 (FFT sub-menu) */
static void window_7_callback(UG_MESSAGE* msg)
{
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;
	int32_t i, j;

	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
			 	 /* change FFT source channel */
			 	 case BTN_ID_0:
					fftSrcChannel = (fftSrcChannel + 1) % 3;
					UG_ButtonSetText(&window_7, BTN_ID_0, fftSrcChannel == CHANNEL1 ? "CH1" : (fftSrcChannel == CHANNEL2 ? "CH2" : "OFF"));

					if(fftSrcChannel != CHANNELNONE){
						chDispMode = CHDISPMODE_FFT;

						/* Erase separator between the two channels */
						i = CH_SEPARATOR_POS;
						for(j = 0; j < LCD_WIDTH; j += 1){
							pFrame[i][j] = C_BLACK;
						}

						bufw8tb0[0] = '\0';		/* update frequency display value */
						hertzToStr(0.5f*toff*samprateVals[tscale]/(float32_t)LCD_WIDTH, bufw8tb0);
						UG_TextboxSetText(&window_8, TXB_ID_0, bufw8tb0);
						showWindow8 = 1;		/* show sub-submenu */
						drawGridVerti();
					}
					else{
						UG_S16 xs;

						chDispMode = CHDISPMODE_SPLIT;
						UG_ButtonSetText(&window_6, BTN_ID_0, "Split");

						showWindow8 = 0;		/* close sub-submenu */
						xs = UG_WindowGetXStart(&window_8);
						fillFrameUGUI(xs, WIND8_Y_START, xs + WIND8_WIDTH - 1, WIND8_Y_START + WIND8_HEIGHT - 1, C_BLACK);
						drawGrid();
					}

					/* force the cursors to be redrawn */
					goToField(FLD_NONE);
					changeFieldValue(2);
			 		break;
			 }
		  }
	  }
	}
}

/* Callback function for window 8 (FFT submenu sub-menu) */
static void window_8_callback(UG_MESSAGE* msg)
{
	/* Nothing to do */
}

/* Callback function for window 9 (Math sub-menu) */
static void window_9_callback(UG_MESSAGE* msg)
{
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;
	int32_t i, j;

	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
			 	 /* change math operation */
			 	 case BTN_ID_0:
					mathOp = (mathOp + 1) % 5;
					if(mathOp == MATH_OP_NONE)
						UG_ButtonSetText(&window_9, BTN_ID_0, "OFF");
					else if(mathOp == MATH_OP_1P2)
						UG_ButtonSetText(&window_9, BTN_ID_0, "1 + 2");
					else if(mathOp == MATH_OP_1M2)
						UG_ButtonSetText(&window_9, BTN_ID_0, "1 - 2");
					else if(mathOp == MATH_OP_2M1)
						UG_ButtonSetText(&window_9, BTN_ID_0, "2 - 1");
					else
						UG_ButtonSetText(&window_9, BTN_ID_0, "1 x 2");

					if(mathOp != MATH_OP_NONE){
						/* Erase separator between the two channels */
						i = CH_SEPARATOR_POS;
						for(j = 0; j < LCD_WIDTH; j += 1){
							pFrame[i][j] = C_BLACK;
						}

						chDispMode = CHDISPMODE_MERGE;
						UG_ButtonSetText(&window_6, BTN_ID_0, "Merge");

						mathField = MATHFLD_VOFF;
						changeFieldValueMath(2);		/* draw offset cursor */

						mathField = MATHFLD_NONE;

						drawGridVerti();
					}
					else{
						chDispMode = CHDISPMODE_SPLIT;
						UG_ButtonSetText(&window_6, BTN_ID_0, "Split");

						mathField = MATHFLD_VOFF;
						changeFieldValueMath(-1);		/* clear offset cursor */

						mathField = MATHFLD_NONE;
						UG_ButtonSetFont(&window_9, BTN_ID_1, &FONT_6X8);	/* de-highlight the math fields */
						UG_ButtonSetFont(&window_9, BTN_ID_2, &FONT_6X8);

						drawGrid();
					}

					/* force the cursors to be redrawn */
					goToField(FLD_NONE);
					changeFieldValue(2);
			 		break;

			 	 /* Vertical scale */
				 case BTN_ID_1:
					if(mathOp != MATH_OP_NONE){
						mathField = MATHFLD_VSCALE;
						goToField(FLD_NONE);			/* de-highlight any fields in top & bottom menubars */
						UG_ButtonSetFont(&window_9, BTN_ID_1, &FONT_7X12);	/* highlight vertical scale field */
						UG_ButtonSetFont(&window_9, BTN_ID_2, &FONT_6X8);	/* de-highlight offset field */
					}
					break;

				 /* Vertical offset */
				 case BTN_ID_2:
					if(mathOp != MATH_OP_NONE){
						mathField = MATHFLD_VOFF;
						goToField(FLD_NONE);
						UG_ButtonSetFont(&window_9, BTN_ID_1, &FONT_6X8);	/* de-highlight vertical scale field */
						UG_ButtonSetFont(&window_9, BTN_ID_2, &FONT_7X12);	/* highlight offset field */
					}
					break;
			 }
		  }
	  }
	}
}

/* Callback function for window 10 (Cursors sub-menu) */
static void window_10_callback(UG_MESSAGE* msg)
{
	if (msg->type == MSG_TYPE_OBJECT)
	{
	  if (msg->id == OBJ_TYPE_BUTTON)
	  {
		  if(msg->event == OBJ_EVENT_PRESSED)
		  {
			 switch(msg->sub_id)
			 {
			 	 /* change cursor mode */
			 	 case BTN_ID_0:
					cursorField = CURSORFLD_NONE;	/* clear previous cursors */
					changeFieldValueCursor(-1);
					UG_ButtonSetFont(&window_10, BTN_ID_1, &FONT_6X8);	/* de-highlight the cursor fields */
					UG_ButtonSetFont(&window_10, BTN_ID_2, &FONT_6X8);

			 		cursorMode = (cursorMode + 1) % 4;

					if(cursorMode == CURSOR_MODE_OFF)
						UG_ButtonSetText(&window_10, BTN_ID_0, "OFF");
					else if(cursorMode == CURSOR_MODE_DT)
						UG_ButtonSetText(&window_10, BTN_ID_0, "dt");
					else if(cursorMode == CURSOR_MODE_DVCH1)
						UG_ButtonSetText(&window_10, BTN_ID_0, "dV CH1");
					else
						UG_ButtonSetText(&window_10, BTN_ID_0, "dV CH2");

					if(cursorMode != CURSOR_MODE_OFF){
						if(chDispMode == CHDISPMODE_SPLIT && cursorMode == CURSOR_MODE_DVCH1){
							cursorApos = CURSORA_SPLITCH1_INITPOS;
							cursorBpos = CURSORB_SPLITCH1_INITPOS;
						}
						else if(chDispMode == CHDISPMODE_SPLIT && cursorMode == CURSOR_MODE_DVCH2){
							cursorApos = CURSORA_SPLITCH2_INITPOS;
							cursorBpos = CURSORB_SPLITCH2_INITPOS;
						}
						else if(cursorMode == CURSOR_MODE_DT){
							cursorApos = CURSORA_DT_INITPOS;
							cursorBpos = CURSORB_DT_INITPOS;
						}
						else{
							cursorApos = CURSORA_MERGE_INITPOS;
							cursorBpos = CURSORB_MERGE_INITPOS;
						}

						cursorField = CURSORFLD_NONE;
						changeFieldValueCursor(2);		/* draw cursors */
					}
					else{
						cursorField = CURSORFLD_NONE;
						changeFieldValueCursor(-1);		/* clear cursors */

						goToField(FLD_NONE);
						changeFieldValue(2);			/* force the channel cursors to be redrawn */

						UG_ButtonSetFont(&window_10, BTN_ID_1, &FONT_6X8);	/* de-highlight the cursor fields */
						UG_ButtonSetFont(&window_10, BTN_ID_2, &FONT_6X8);

						/* Invalidate cursor displays */
						UG_ButtonSetText(&window_10, BTN_ID_1, "--");
						UG_ButtonSetText(&window_10, BTN_ID_2, "--");
						UG_TextboxSetText(&window_10, TXB_ID_4, "--");
					}
			 		break;

			 	 /* cursor A position */
			     case BTN_ID_1:
			    	if(cursorMode != CURSOR_MODE_OFF){
			    		cursorField = CURSORFLD_CURA;
						goToField(FLD_NONE);			/* de-highlight any fields in top & bottom menubars */
						UG_ButtonSetFont(&window_10, BTN_ID_1, &FONT_7X12);	/* highlight cursor A field */
						UG_ButtonSetFont(&window_10, BTN_ID_2, &FONT_6X8);	/* de-highlight cursor B field */
			    	}
			 		break;

				 /* cursor B position */
				 case BTN_ID_2:
					if(cursorMode != CURSOR_MODE_OFF){
						cursorField = CURSORFLD_CURB;
						goToField(FLD_NONE);			/* de-highlight any fields in top & bottom menubars */
						UG_ButtonSetFont(&window_10, BTN_ID_1, &FONT_6X8);	/* de-highlight cursor A field */
						UG_ButtonSetFont(&window_10, BTN_ID_2, &FONT_7X12);	/* highlight cursor B field */
					}
					break;
			 }
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
	uint8_t done = 0;

	if(!done && currWind == WINDOW1){
		UG_WindowShow(&window_2);	/* bottom menubar */
		currWind = WINDOW2;
		done = 1;
	}
	if(!done && currWind <= WINDOW2 && showWindow3){
		UG_WindowShow(&window_3);	/* menu page 1 */
		currWind = WINDOW3;
		done = 1;
	}
	if(!done && currWind <= WINDOW3 && showWindow4){
		UG_WindowShow(&window_4);
		currWind = WINDOW4;
		done = 1;
	}
	if(!done && currWind <= WINDOW4 && showWindow5){
		UG_WindowShow(&window_5);
		currWind = WINDOW5;
		done = 1;
	}
	if(!done && currWind <= WINDOW5 && showWindow6){
		UG_WindowShow(&window_6);
		currWind = WINDOW6;
		done = 1;
	}
	if(!done && currWind <= WINDOW6 && showWindow7){
		UG_WindowShow(&window_7);
		currWind = WINDOW7;
		done = 1;
	}
	if(!done && currWind <= WINDOW7 && showWindow8){
		UG_WindowShow(&window_8);
		currWind = WINDOW8;
		done = 1;
	}
	if(!done && currWind <= WINDOW8 && showWindow9){
		UG_WindowShow(&window_9);
		currWind = WINDOW9;
		done = 1;
	}
	if(!done && currWind <= WINDOW9 && showWindow10){
		UG_WindowShow(&window_10);
		currWind = WINDOW10;
		done = 1;
	}
	if(!done){
		UG_WindowShow(&window_1);
		currWind = WINDOW1;
		done = 1;
	}

	return;
}

/**
  * @brief  Clear the open window 4 submenu.
  * @param  None
  * @retval None
  */
void clearWind4Submenus(void)
{
	if(showWindow6)
		fillFrameUGUI(WIND6_X_START, WIND6_Y_START, WIND6_X_START + WIND6_WIDTH - 1, WIND6_Y_START + WIND6_HEIGHT - 1, C_BLACK);
	else if(showWindow7)
		fillFrameUGUI(WIND7_X_START, WIND7_Y_START, WIND7_X_START + WIND7_WIDTH - 1, WIND7_Y_START + WIND7_HEIGHT - 1, C_BLACK);
	else if(showWindow9)
		fillFrameUGUI(WIND9_X_START, WIND9_Y_START, WIND9_X_START + WIND9_WIDTH - 1, WIND9_Y_START + WIND9_HEIGHT - 1, C_BLACK);
	else if(showWindow10)
		fillFrameUGUI(WIND10_X_START, WIND10_Y_START, WIND10_X_START + WIND10_WIDTH - 1, WIND10_Y_START + WIND10_HEIGHT - 1, C_BLACK);

	drawGrid();
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
  * @brief  Get currently active field.
  * @param  None
  * @retval Currently active field
  */
uint8_t getCurrField(void)
{
	return currField;
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

	selectField(currField, 1);		/* select field */

	return;
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
  * @param  dir: 0=increment, 1=decrement, 2=redraw cursors
  * @retval None
  */
void changeFieldValue(uint8_t dir)
{
	static int32_t trigCurPosPrev = 52, ch1offCurPosPrev = 130, ch2offCurPosPrev = 250,
			toffCurPosPrev = TOFF_INITVAL - TOFF_CURSOR_WIDTH/2 + 1, wind8PosPrev = TOFF_INITVAL + 10;
	int32_t temp, trigCurPos, ch1offCurPos, ch2offCurPos, toffCurPos, wind8Pos, i, j;
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	/* increment/decrement value of the current field */
	switch(currField)
	{
		case FLD_NONE:
			break;

		case FLD_CH1_VSCALE:
			vscale1 += dir?(vscale1==0?0:-1):(vscale1==VSCALE_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_0, vscaleDispVals[vscale1]);
			vscale1Changed = 1;
			break;

		case FLD_CH2_VSCALE:
			vscale2 += dir?(vscale2==0?0:-1):(vscale2==VSCALE_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_1, vscaleDispVals[vscale2]);
			vscale2Changed = 1;
			break;

		case FLD_TSCALE:
			tscale += dir?(tscale==0?0:-1):(tscale==TSCALE_MAXVALS-1?0:1);
			UG_TextboxSetText(&window_1, TXB_ID_2, tscaleDispVals[tscale]);
			TimADC_init(tscale);
			break;

		case FLD_TRIGSRC:
			trigsrc += dir?(trigsrc==0?0:-1):(trigsrc==TRIGSRC_MAXVALS-1?0:1);

			/* force trigsrc to be ch1 in single display mode */
			if(chDispMode == CHDISPMODE_SNGL)
				trigsrc = 0;

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
			if(runstopVals[runstop] == RUNSTOP_RUN){
				ADC_Ch1_reinit();
				ADC_Ch2_reinit();
				UG_TextboxSetText(&window_1, TXB_ID_6, runstopDispVals[runstop]);
				UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_RUN);
			}
			else{
				CH1_ADC_DMA_STREAM->CR &= ~0x01;		/* disable ch1 stream and confirm */
				while(CH1_ADC_DMA_STREAM->CR & 0x01);
				CH2_ADC_DMA_STREAM->CR &= ~0x01;		/* disable ch2 stream and confirm */
				while(CH2_ADC_DMA_STREAM->CR & 0x01);
				UG_TextboxSetText(&window_1, TXB_ID_6, runstopDispVals[runstop]);
				UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_STP);
			}
			break;

		case FLD_CH1_VOFF:
			voff1 += dir?(voff1==-255?0:-3):(voff1==510?0:3);
			gcvt((int32_t)((3.3f*(float32_t)voff1/(float32_t)255)*100)/100.0f, 3 + (voff1 < -1), bufw2tb0);
			strcat(bufw2tb0, "V");
			UG_TextboxSetText(&window_2, TXB_ID_0, bufw2tb0);
			voff1Changed = 1;
			break;

		case FLD_CH2_VOFF:
			voff2 += dir?(voff2==-255?0:-3):(voff2==510?0:3);
			gcvt((int32_t)((3.3f*(float32_t)voff2/(float32_t)255)*100)/100.0f, 3 + (voff2 < -1), bufw2tb1);
			strcat(bufw2tb1, "V");
			UG_TextboxSetText(&window_2, TXB_ID_1, bufw2tb1);
			voff2Changed = 1;
			break;

		case FLD_TOFF:
			toff += dir?(toff==TOFF_LIMIT?0:-6):(toff==ADC_TRIGBUF_SIZE-TOFF_LIMIT?0:6);
			secToStr((float32_t)(toff-LCD_WIDTH/2)/(float32_t)samprateVals[tscale], bufw2tb2);
			UG_TextboxSetText(&window_2, TXB_ID_2, bufw2tb2);

			/* clear previous toff cursor and redraw */
			for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
				for(j = toffCurPosPrev; j < toffCurPosPrev + TOFF_CURSOR_WIDTH; j++)
					pFrame[i][j] = C_BLACK;

			toffCurPos = toff - TOFF_CURSOR_WIDTH/2 + 1;
			for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
				for(j = toffCurPos; j < toffCurPos + TOFF_CURSOR_WIDTH; j++)
					pFrame[i][j] = toffCursorImg[i-MENUBAR_HEIGHT][j-toffCurPos] ? TOFF_CURSOR_COLOR : C_BLACK;

			toffCurPosPrev = toffCurPos;

			if(staticMode){
				toffStm += dir ? -6: 6;

				toff = (toffStm < TOFF_LIMIT) ? TOFF_LIMIT : (toffStm > ADC_TRIGBUF_SIZE - TOFF_LIMIT ? ADC_TRIGBUF_SIZE - TOFF_LIMIT : toff);

				if(toffStm < TOFF_LIMIT || toffStm > ADC_TRIGBUF_SIZE - TOFF_LIMIT){
					/* clear previous toff cursor and redraw */
					for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
						for(j = toffCurPos; j < toffCurPos + TOFF_CURSOR_WIDTH; j++)
							pFrame[i][j] = C_BLACK;
				}

				toffCurPosPrev = toffCurPos = toff - TOFF_CURSOR_WIDTH/2 + 1;

				if(toffStm < TOFF_LIMIT){
					for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
						for(j = toffCurPos; j < toffCurPos + TOFF_CURSOR_WIDTH; j++)
							pFrame[i][j] = toffLeftCursorImg[i-MENUBAR_HEIGHT][j-toffCurPos] ? TOFF_CURSOR_COLOR : C_BLACK;
				}
				else if(toffStm > ADC_TRIGBUF_SIZE - TOFF_LIMIT){
					for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
						for(j = toffCurPos; j < toffCurPos + TOFF_CURSOR_WIDTH; j++)
							pFrame[i][j] = toffRightCursorImg[i-MENUBAR_HEIGHT][j-toffCurPos] ? TOFF_CURSOR_COLOR : C_BLACK;
				}

				secToStr((float32_t)(toffStm-LCD_WIDTH/2)/(float32_t)samprateVals[tscale], bufw2tb2);
				UG_TextboxSetText(&window_2, TXB_ID_2, bufw2tb2);
			}
			else if(chDispMode == CHDISPMODE_FFT){
				if(toff > 400)
					wind8Pos = toff - 80;
				else
					wind8Pos = toff + 10;

				UG_WindowHide(&window_8);
				fillFrameUGUI(wind8PosPrev, WIND8_Y_START, wind8PosPrev + WIND8_WIDTH - 1, WIND8_Y_START + WIND8_HEIGHT - 1, C_BLACK);
				UG_WindowResize(&window_8, wind8Pos, WIND8_Y_START, wind8Pos + WIND8_WIDTH - 1, WIND8_Y_START + WIND8_HEIGHT - 1);
				wind8PosPrev = wind8Pos;

				bufw8tb0[0] = '\0';
				hertzToStr(0.5f*toff*samprateVals[tscale]/(float32_t)LCD_WIDTH, bufw8tb0);
				UG_TextboxSetText(&window_8, TXB_ID_0, bufw8tb0);
			}

			drawGridVerti();

			toffChanged = 1;
			break;

		default:
			break;
	}

	/* if trig source changed, clear off the existing trigger cursor */
	if(currField == FLD_TRIGSRC){
		for(i = trigCurPosPrev; i < trigCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = trigCursorImg[i-trigCurPosPrev][j] ? CH1_COLOR : C_BLACK;
	}

	/* redraw cursors, first clear existing ones */
	if(dir == 2){
		/* clear trigger cursor */
		for(i = trigCurPosPrev; i < trigCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = C_BLACK;

		/* clear ch1 offset cursor */
		for(i = ch1offCurPosPrev; i < ch1offCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = C_BLACK;

		/* clear ch2 offset cursor */
		for(i = ch2offCurPosPrev; i < ch2offCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = C_BLACK;
	}

	/* Update the positions of ch1 cursors, if required */
	if((currField == FLD_CH1_VSCALE || currField == FLD_TRIGSRC ||
			(currField == FLD_TRIGLVL && trigsrcVals[trigsrc] == TRIGSRC_CH1) || currField == FLD_CH1_VOFF ||
			(chDispMode == CHDISPMODE_MERGE && (currField == FLD_CH2_VSCALE ||
			(currField == FLD_TRIGLVL && trigsrcVals[trigsrc] == TRIGSRC_CH2) || currField == FLD_CH2_VOFF)) ||
			dir == 2) && chDispMode != CHDISPMODE_FFT){

		if(trigsrcVals[trigsrc] == TRIGSRC_CH1){
			/* clear previous trigger cursor and redraw */
			for(i = trigCurPosPrev; i < trigCurPosPrev + CURSOR_WIDTH; i++)
				for(j = 0; j < CURSOR_LENGTH; j++)
					pFrame[i][j] = C_BLACK;

			temp = (float32_t)(voff1 + triglvl)/vscaleVals[vscale1];
			if(chDispMode == CHDISPMODE_SPLIT){
				if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
			}
			else{
				if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
			}
			if(temp < 0)  temp = 0;
			if(chDispMode == CHDISPMODE_SPLIT)
				trigCurPos = CHDISPMODE_SPLIT_CH1BOT - temp - CURSOR_WIDTH/2 + 1;
			else
				trigCurPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;
			for(i = trigCurPos; i < trigCurPos + CURSOR_WIDTH; i++)
				for(j = 0; j < CURSOR_LENGTH; j++)
					pFrame[i][j] = trigCursorImg[i-trigCurPos][j] ? CH1_COLOR : C_BLACK;

			trigCurPosPrev = trigCurPos;
		}

		/* clear previous offset cursor and redraw */
		for(i = ch1offCurPosPrev; i < ch1offCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = C_BLACK;

		temp = (float32_t)(voff1)/vscaleVals[vscale1];
		if(chDispMode == CHDISPMODE_SPLIT){
			if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
		}
		else{
			if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
		}
		if(temp < 0)  temp = 0;
		if(chDispMode == CHDISPMODE_SPLIT)
			ch1offCurPos = CHDISPMODE_SPLIT_CH1BOT - temp - CURSOR_WIDTH/2 + 1;
		else
			ch1offCurPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;
		for(i = ch1offCurPos; i < ch1offCurPos + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = ch1RefCursorImg[i-ch1offCurPos][j] ? CH1_COLOR : C_BLACK;

		ch1offCurPosPrev = ch1offCurPos;

		drawGridHoriz();
	}

	/* Update the positions of ch2 cursors, if required */
	if((currField == FLD_CH2_VSCALE || currField == FLD_TRIGSRC ||
			(currField == FLD_TRIGLVL && trigsrcVals[trigsrc] == TRIGSRC_CH2) || currField == FLD_CH2_VOFF ||
			(chDispMode == CHDISPMODE_MERGE && (currField == FLD_CH1_VSCALE ||
			(currField == FLD_TRIGLVL && trigsrcVals[trigsrc] == TRIGSRC_CH1) || currField == FLD_CH1_VOFF)) ||
			dir == 2) && chDispMode != CHDISPMODE_SNGL && chDispMode != CHDISPMODE_FFT){

		if(trigsrcVals[trigsrc] == TRIGSRC_CH2){
			/* clear previous trigger cursor and redraw */
			for(i = trigCurPosPrev; i < trigCurPosPrev + CURSOR_WIDTH; i++)
				for(j = 0; j < CURSOR_LENGTH; j++)
					pFrame[i][j] = C_BLACK;

			temp = (float32_t)(voff2 + triglvl)/vscaleVals[vscale2];
			if(chDispMode == CHDISPMODE_SPLIT){
				if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
			}
			else{
				if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
			}
			if(temp < 0)  temp = 0;
			if(chDispMode == CHDISPMODE_SPLIT)
				trigCurPos = CHDISPMODE_SPLIT_CH2BOT - temp - CURSOR_WIDTH/2 + 1;
			else
				trigCurPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;
			for(i = trigCurPos; i < trigCurPos + CURSOR_WIDTH; i++)
				for(j = 0; j < CURSOR_LENGTH; j++)
					pFrame[i][j] = trigCursorImg[i-trigCurPos][j] ? CH2_COLOR : C_BLACK;

			trigCurPosPrev = trigCurPos;
		}

		/* clear previous offset cursor and redraw */
		for(i = ch2offCurPosPrev; i < ch2offCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = C_BLACK;

		temp = (float32_t)(voff2)/vscaleVals[vscale2];
		if(chDispMode == CHDISPMODE_SPLIT){
			if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
		}
		else{
			if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
		}
		if(temp < 0)  temp = 0;
		if(chDispMode == CHDISPMODE_SPLIT)
			ch2offCurPos = CHDISPMODE_SPLIT_CH2BOT - temp - CURSOR_WIDTH/2 + 1;
		else
			ch2offCurPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;

		for(i = ch2offCurPos; i < ch2offCurPos + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = ch2RefCursorImg[i-ch2offCurPos][j] ? CH2_COLOR : C_BLACK;

		ch2offCurPosPrev = ch2offCurPos;

		drawGridHoriz();
	}

	/* update horizontal offset if there is a change in time scale */
	if(currField == FLD_TSCALE){
		secToStr((float32_t)(toff-LCD_WIDTH/2)/(float32_t)samprateVals[tscale], bufw2tb2);
		UG_TextboxSetText(&window_2, TXB_ID_2, bufw2tb2);
	}

	/* update frequency display value */
	if(chDispMode == CHDISPMODE_FFT && currField == FLD_TSCALE){
		bufw8tb0[0] = '\0';
		hertzToStr(0.5f*toff*samprateVals[tscale]/(float32_t)LCD_WIDTH, bufw8tb0);
		UG_TextboxSetText(&window_8, TXB_ID_0, bufw8tb0);
	}

	/* redraw cursors, if required */
	if(cursorMode != CURSOR_MODE_OFF && (currField == FLD_CH1_VSCALE || currField == FLD_CH2_VSCALE ||
			currField == FLD_CH1_VOFF || currField == FLD_CH2_VOFF || currField == FLD_TSCALE)){
		changeFieldValueCursor(2);
	}

	return;
}

/**
  * @brief  Display Toff field value and redraw the cursor.
  * @param  None
  * @retval None
  */
void dispToff(void)
{
	int32_t toffCurPos, i, j;
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	secToStr((float32_t)(toff-LCD_WIDTH/2)/(float32_t)samprateVals[tscale], bufw2tb2);
	UG_TextboxSetText(&window_2, TXB_ID_2, bufw2tb2);

	toffCurPos = toff - TOFF_CURSOR_WIDTH/2 + 1;

	/* clear toff cursor and redraw */
	for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
		for(j = toffCurPos; j < toffCurPos + TOFF_CURSOR_WIDTH; j++)
			pFrame[i][j] = C_BLACK;

	for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
		for(j = toffCurPos; j < toffCurPos + TOFF_CURSOR_WIDTH; j++)
			pFrame[i][j] = toffCursorImg[i-MENUBAR_HEIGHT][j-toffCurPos] ? TOFF_CURSOR_COLOR : C_BLACK;
}

/**
  * @brief  Change field values in math mode.
  * @param  dir: 0=increment, 1=decrement, 2=redraw offset cursor, -1=clear offset cursor
  * @retval None
  */
void changeFieldValueMath(int8_t dir)
{
	static int32_t voffCurPosPrev = CHDISPMODE_MERGE_CHBOT - CURSOR_WIDTH/2 + 1;
	int32_t voffCurPos, temp, i, j;
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	if(mathField == MATHFLD_VOFF){
		/* clear previous offset cursor */
		for(i = voffCurPosPrev; i < voffCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = C_BLACK;

		if(dir != -1){
			if(dir != 2)
				mathVoff += dir?(mathVoff==-255?0:-3):(mathVoff==510?0:3);

			temp = (float32_t)(mathVoff)/vscaleVals[mathVscale];
			if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
			if(temp < 0)  temp = 0;
			voffCurPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;

			for(i = voffCurPos; i < voffCurPos + CURSOR_WIDTH; i++)
				for(j = 0; j < CURSOR_LENGTH; j++)
					pFrame[i][j] = mathRefCursorImg[i-voffCurPos][j] ? MATH_COLOR : C_BLACK;

			voffCurPosPrev = voffCurPos;

			if(dir != 2){
				gcvt((int32_t)((3.3f*(float32_t)mathVoff/(float32_t)255)*100)/100.0f, 3 + (mathVoff < -1), bufw9btn2);
				strcat(bufw9btn2, "V");
				UG_ButtonSetText(&window_9, BTN_ID_2, bufw9btn2);

				goToField(FLD_NONE);
				changeFieldValue(2);
			}
		}
	}
	else if(mathField == MATHFLD_VSCALE && dir != 2 && dir != -1){
		mathVscale += dir?(mathVscale==0?0:-1):(mathVscale==VSCALE_MAXVALS-1?0:1);
		UG_ButtonSetText(&window_9, BTN_ID_1, vscaleDispVals[mathVscale]);

		/* clear previous offset cursor */
		for(i = voffCurPosPrev; i < voffCurPosPrev + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = C_BLACK;

		/* recalculate offset cursor position */
		temp = (float32_t)(mathVoff)/vscaleVals[mathVscale];
		if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
		if(temp < 0)  temp = 0;
		voffCurPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;

		for(i = voffCurPos; i < voffCurPos + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = mathRefCursorImg[i-voffCurPos][j] ? MATH_COLOR : C_BLACK;

		voffCurPosPrev = voffCurPos;
	}
}

/**
  * @brief  Change field values in cursor mode.
  * @param  dir: 0=increment, 1=decrement, 2=redraw cursors, -1=clear cursors
  * @retval None
  */
void changeFieldValueCursor(int8_t dir)
{
	float32_t curAval, curBval;
	int32_t temp, i, j;
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	/* clear previous cursors */
	if(cursorMode == CURSOR_MODE_DT){
		for(i = CHDISPMODE_MERGE_CHTOP; i <= CHDISPMODE_MERGE_CHBOT - 2; i += 6){
			pFrame[i][cursorApos] = pFrame[i + 1][cursorApos] = pFrame[i + 2][cursorApos] = C_BLACK;
			pFrame[i][cursorBpos] = pFrame[i + 1][cursorBpos] = pFrame[i + 2][cursorBpos] = C_BLACK;
		}
	}
	else{
		for(j = 0; j < LCD_WIDTH - 2; j += 6){
			pFrame[cursorApos][j] = pFrame[cursorApos][j + 1] = pFrame[cursorApos][j + 2] = C_BLACK;
			pFrame[cursorBpos][j] = pFrame[cursorBpos][j + 1] = pFrame[cursorBpos][j + 2] = C_BLACK;
		}
	}

	if(dir != -1){
		if(dir != 2 && cursorField != CURSORFLD_NONE){
			if(chDispMode == CHDISPMODE_SPLIT && cursorMode == CURSOR_MODE_DVCH1){
				if(cursorField == CURSORFLD_CURA)	cursorApos += dir?(cursorApos==CHDISPMODE_SPLIT_CH1TOP?0:1):(cursorApos==CHDISPMODE_SPLIT_CH1BOT?0:-1);
				else   cursorBpos += dir?(cursorBpos==CHDISPMODE_SPLIT_CH1TOP?0:1):(cursorBpos==CHDISPMODE_SPLIT_CH1BOT?0:-1);
			}
			else if(chDispMode == CHDISPMODE_SPLIT && cursorMode == CURSOR_MODE_DVCH2){
				if(cursorField == CURSORFLD_CURA)	cursorApos += dir?(cursorApos==CHDISPMODE_SPLIT_CH2TOP?0:1):(cursorApos==CHDISPMODE_SPLIT_CH2BOT?0:-1);
				else   cursorBpos += dir?(cursorBpos==CHDISPMODE_SPLIT_CH2TOP?0:1):(cursorBpos==CHDISPMODE_SPLIT_CH2BOT?0:-1);
			}
			else if(cursorMode == CURSOR_MODE_DT){
				if(cursorField == CURSORFLD_CURA)	cursorApos += dir?(cursorApos==0?0:-1):(cursorApos==LCD_WIDTH-1?0:1);
				else   cursorBpos += dir?(cursorBpos==0?0:-1):(cursorBpos==LCD_WIDTH-1?0:1);
			}
			else{
				if(cursorField == CURSORFLD_CURA)	cursorApos += dir?(cursorApos==CHDISPMODE_MERGE_CHTOP?0:1):(cursorApos==CHDISPMODE_MERGE_CHBOT?0:-1);
				else   cursorBpos += dir?(cursorBpos==CHDISPMODE_MERGE_CHTOP?0:1):(cursorBpos==CHDISPMODE_MERGE_CHBOT?0:-1);
			}
		}

		/* draw cursors */
		if(cursorMode == CURSOR_MODE_DT){
			for(i = CHDISPMODE_MERGE_CHTOP; i <= CHDISPMODE_MERGE_CHBOT - 2; i += 6){
				pFrame[i][cursorApos] = pFrame[i + 1][cursorApos] = pFrame[i + 2][cursorApos] = CURSOR_COLOR;
				pFrame[i][cursorBpos] = pFrame[i + 1][cursorBpos] = pFrame[i + 2][cursorBpos] = CURSOR_COLOR;
			}
		}
		else{
			for(j = 0; j < LCD_WIDTH - 2; j += 6){
				pFrame[cursorApos][j] = pFrame[cursorApos][j + 1] = pFrame[cursorApos][j + 2] = CURSOR_COLOR;
				pFrame[cursorBpos][j] = pFrame[cursorBpos][j + 1] = pFrame[cursorBpos][j + 2] = CURSOR_COLOR;
			}
		}

		/* calculate cursor position values */
		if(chDispMode == CHDISPMODE_SPLIT && cursorMode == CURSOR_MODE_DVCH1){
			temp = CHDISPMODE_SPLIT_CH1BOT - cursorApos;
			curAval = round(temp*vscaleVals[vscale1] - voff1);
			temp = CHDISPMODE_SPLIT_CH1BOT - cursorBpos;
			curBval = round(temp*vscaleVals[vscale1] - voff1);
		}
		else if(chDispMode == CHDISPMODE_SPLIT && cursorMode == CURSOR_MODE_DVCH2){
			temp = CHDISPMODE_SPLIT_CH2BOT - cursorApos;
			curAval = round(temp*vscaleVals[vscale2] - voff2);
			temp = CHDISPMODE_SPLIT_CH2BOT - cursorBpos;
			curBval = round(temp*vscaleVals[vscale2] - voff2);
		}
		else if(chDispMode == CHDISPMODE_MERGE && cursorMode == CURSOR_MODE_DVCH1){
			temp = CHDISPMODE_MERGE_CHBOT - cursorApos;
			curAval = round(temp*vscaleVals[vscale1] - voff1);
			temp = CHDISPMODE_MERGE_CHBOT - cursorBpos;
			curBval = round(temp*vscaleVals[vscale1] - voff1);
		}
		else if(chDispMode == CHDISPMODE_MERGE && cursorMode == CURSOR_MODE_DVCH2){
			temp = CHDISPMODE_MERGE_CHBOT - cursorApos;
			curAval = round(temp*vscaleVals[vscale2] - voff2);
			temp = CHDISPMODE_MERGE_CHBOT - cursorBpos;
			curBval = round(temp*vscaleVals[vscale2] - voff2);
		}
		else{
			curAval = cursorApos;
			curBval = cursorBpos;
		}

		/* display cursor positions and difference */
		if(cursorMode == CURSOR_MODE_DT){
			secToStr((curAval - LCD_WIDTH/2)/(float32_t)samprateVals[tscale], bufw10btn1);
			UG_ButtonSetText(&window_10, BTN_ID_1, bufw10btn1);

			secToStr((curBval - LCD_WIDTH/2)/(float32_t)samprateVals[tscale], bufw10btn2);
			UG_ButtonSetText(&window_10, BTN_ID_2, bufw10btn2);

			secToStr((curAval - curBval)/(float32_t)samprateVals[tscale], bufw10tb4);
			UG_TextboxSetText(&window_10, TXB_ID_4, bufw10tb4);
		}
		else{
			gcvt((int32_t)((3.3f*curAval/255.0f)*100)/100.0f, 3 + (curAval < -1), bufw10btn1);
			strcat(bufw10btn1, "V");
			UG_ButtonSetText(&window_10, BTN_ID_1, bufw10btn1);

			gcvt((int32_t)((3.3f*curBval/255.0f)*100)/100.0f, 3 + (curBval < -1), bufw10btn2);
			strcat(bufw10btn2, "V");
			UG_ButtonSetText(&window_10, BTN_ID_2, bufw10btn2);

			gcvt((int32_t)((3.3f*(curAval - curBval)/255.0f)*100)/100.0f, 3 + ((curAval - curBval) < -1), bufw10tb4);
			strcat(bufw10tb4, "V");
			UG_TextboxSetText(&window_10, TXB_ID_4, bufw10tb4);
		}

		uint8_t tempFld = currField;
		/* redraw the channel cursors and grid, in case they got erased from cursor movement */
		goToField(FLD_NONE);
		changeFieldValue(2);
		goToField(tempFld);		/* restore the field which was active */

		/* in static mode, redraw the red border, in case it got erased from cursor movement */
		if(staticMode){
			drawRedBorder();
		}
	}
}

/**
  * @brief  Get currently active math field.
  * @param  None
  * @retval Currently active math field
  */
uint8_t getCurrMathField(void)
{
	return mathField;
}

/**
  * @brief  Get currently active cursor field.
  * @param  None
  * @retval Currently active cursor field
  */
uint8_t getCurrCursorField(void)
{
	return cursorField;
}

/**
  * @brief  Initialize field values.
  * @param  None
  * @retval None
  */
void initFields(void)
{
	int32_t temp, curPos, i, j;
	__IO uint16_t (*pFrame)[LCD_WIDTH] = (__IO uint16_t (*)[LCD_WIDTH])LCD_DRAW_BUFFER_UGUI;

	/* ch1 vertical scale */
	UG_TextboxSetText(&window_1, TXB_ID_0, vscaleDispVals[vscale1]);

	/* ch2 vertical scale */
	UG_TextboxSetText(&window_1, TXB_ID_1, vscaleDispVals[vscale2]);

	/* time scale */
	UG_TextboxSetText(&window_1, TXB_ID_2, tscaleDispVals[tscale]);

	/* trigger source */
	UG_COLOR color;
	if(trigsrcVals[trigsrc] == TRIGSRC_CH1)
		color = CH1_COLOR;
	else
		color = CH2_COLOR;
	UG_TextboxSetBackColor(&window_1, TXB_ID_3, color);
	UG_TextboxSetBackColor(&window_1, TXB_ID_4, color);

	/* trigger type */
	strcpy(bufw1tb3 + 4, trigtypeDispVals[trigtype]);
	UG_TextboxSetText(&window_1, TXB_ID_3, bufw1tb3);

	/* trigger level */
	gcvt((int32_t)((3.3f*(float32_t)triglvl/(float32_t)255)*100)/100.0f, 3, bufw1tb4);
	strcat(bufw1tb4, "V");
	UG_TextboxSetText(&window_1, TXB_ID_4, bufw1tb4);
	if(trigsrcVals[trigsrc] == TRIGSRC_CH1){
		temp = (float32_t)(voff1 + triglvl)/vscaleVals[vscale1];
		if(chDispMode == CHDISPMODE_SPLIT){
			if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
		}
		else{
			if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
		}
		if(temp < 0)  temp = 0;
		if(chDispMode == CHDISPMODE_SPLIT)
			curPos = CHDISPMODE_SPLIT_CH1BOT - temp - CURSOR_WIDTH/2 + 1;
		else
			curPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;
		for(i = curPos; i < curPos + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = trigCursorImg[i-curPos][j] ? CH1_COLOR : C_BLACK;
	}
	else{
		if(chDispMode != CHDISPMODE_SNGL){
			temp = (float32_t)(voff2 + triglvl)/vscaleVals[vscale2];
			if(chDispMode == CHDISPMODE_SPLIT){
				if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
			}
			else{
				if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
			}
			if(temp < 0)  temp = 0;
			if(chDispMode == CHDISPMODE_SPLIT)
				curPos = CHDISPMODE_SPLIT_CH2BOT - temp - CURSOR_WIDTH/2 + 1;
			else
				curPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;
			for(i = curPos; i < curPos + CURSOR_WIDTH; i++)
				for(j = 0; j < CURSOR_LENGTH; j++)
					pFrame[i][j] = trigCursorImg[i-curPos][j] ? CH2_COLOR : C_BLACK;
		}
	}

	/* trigger mode */
	UG_TextboxSetText(&window_1, TXB_ID_5, trigmodeDispVals[trigmode]);

	/* Run/Stop */
	UG_TextboxSetText(&window_1, TXB_ID_6, runstopDispVals[runstop]);
	UG_TextboxSetBackColor(&window_1, TXB_ID_6, RUNSTOP_ICON_COLOR_RUN);

	/* ch1 vertical offset */
	gcvt((int32_t)((3.3f*(float32_t)voff1/(float32_t)255)*100)/100.0f, 3 + (voff1 < -1), bufw2tb0);
	strcat(bufw2tb0, "V");
	UG_TextboxSetText(&window_2, TXB_ID_0, bufw2tb0);
	temp = (float32_t)(voff1)/vscaleVals[vscale1];
	if(chDispMode == CHDISPMODE_SPLIT){
		if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
	}
	else{
		if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
	}
	if(temp < 0)  temp = 0;
	if(chDispMode == CHDISPMODE_SPLIT)
		curPos = CHDISPMODE_SPLIT_CH1BOT - temp - CURSOR_WIDTH/2 + 1;
	else
		curPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;
	for(i = curPos; i < curPos + CURSOR_WIDTH; i++)
		for(j = 0; j < CURSOR_LENGTH; j++)
			pFrame[i][j] = ch1RefCursorImg[i-curPos][j] ? CH1_COLOR : C_BLACK;

	/* ch2 vertical offset */
	if(chDispMode != CHDISPMODE_SNGL){
		gcvt((int32_t)((3.3f*(float32_t)voff2/(float32_t)255)*100)/100.0f, 3 + (voff2 < -1), bufw2tb1);
		strcat(bufw2tb1, "V");
		UG_TextboxSetText(&window_2, TXB_ID_1, bufw2tb1);
		temp = (float32_t)(voff2)/vscaleVals[vscale1];
		if(chDispMode == CHDISPMODE_SPLIT){
			if(temp > CHDISPMODE_SPLIT_SIGMAX)	temp = CHDISPMODE_SPLIT_SIGMAX;
		}
		else{
			if(temp > CHDISPMODE_MERGE_SIGMAX)	temp = CHDISPMODE_MERGE_SIGMAX;
		}
		if(temp < 0)  temp = 0;
		if(chDispMode == CHDISPMODE_SPLIT)
			curPos = CHDISPMODE_SPLIT_CH2BOT - temp - CURSOR_WIDTH/2 + 1;
		else
			curPos = CHDISPMODE_MERGE_CHBOT - temp - CURSOR_WIDTH/2 + 1;
		for(i = curPos; i < curPos + CURSOR_WIDTH; i++)
			for(j = 0; j < CURSOR_LENGTH; j++)
				pFrame[i][j] = ch2RefCursorImg[i-curPos][j] ? CH2_COLOR : C_BLACK;
	}

	/* time offset */
	secToStr((float32_t)(toff-LCD_WIDTH/2)/(float32_t)samprateVals[tscale], bufw2tb2);
	UG_TextboxSetText(&window_2, TXB_ID_2, bufw2tb2);
	curPos = toff - TOFF_CURSOR_WIDTH/2 + 1;
	for(i = MENUBAR_HEIGHT; i < MENUBAR_HEIGHT + TOFF_CURSOR_LENGTH; i++)
		for(j = curPos; j < curPos + TOFF_CURSOR_WIDTH; j++)
			pFrame[i][j] = toffCursorImg[i-MENUBAR_HEIGHT][j-curPos] ? TOFF_CURSOR_COLOR : C_BLACK;

	/* math vertical scale */
	UG_ButtonSetText(&window_9, BTN_ID_1, vscaleDispVals[mathVscale]);

	/* math vertical offset */
	gcvt((int32_t)((3.3f*(float32_t)mathVoff/(float32_t)255)*100)/100.0f, 3 + (mathVoff < -1), bufw9btn2);
	strcat(bufw9btn2, "V");
	UG_ButtonSetText(&window_9, BTN_ID_2, bufw9btn2);

	return;
}

/**
  * @brief  Draws a red border around the waveform area.
  * @param  None
  * @retval None
  */
void drawRedBorder(void)
{
	/* Top */
	UG_DrawLine(0, MENUBAR_HEIGHT, LCD_WIDTH - 1, MENUBAR_HEIGHT, C_RED);
	UG_DrawLine(0, MENUBAR_HEIGHT + 1, LCD_WIDTH - 1, MENUBAR_HEIGHT + 1, C_RED);
	/* Bottom */
	UG_DrawLine(0, LCD_HEIGHT - MENUBAR_HEIGHT - 1, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_RED);
	UG_DrawLine(0, LCD_HEIGHT - MENUBAR_HEIGHT - 2, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 2, C_RED);
	/* Left */
	UG_DrawLine(0, MENUBAR_HEIGHT, 0, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_RED);
	UG_DrawLine(1, MENUBAR_HEIGHT, 1, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_RED);
	/* Right */
	UG_DrawLine(LCD_WIDTH - 1, MENUBAR_HEIGHT, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_RED);
	UG_DrawLine(LCD_WIDTH - 2, MENUBAR_HEIGHT, LCD_WIDTH - 2, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_RED);

	return;
}

/**
  * @brief  Clears the red border around the waveform area.
  * @param  None
  * @retval None
  */
void clearRedBorder(void)
{
	/* Top */
	UG_DrawLine(0, MENUBAR_HEIGHT, LCD_WIDTH - 1, MENUBAR_HEIGHT, C_BLACK);
	UG_DrawLine(0, MENUBAR_HEIGHT + 1, LCD_WIDTH - 1, MENUBAR_HEIGHT + 1, C_BLACK);
	/* Bottom */
	UG_DrawLine(0, LCD_HEIGHT - MENUBAR_HEIGHT - 1, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_BLACK);
	UG_DrawLine(0, LCD_HEIGHT - MENUBAR_HEIGHT - 2, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 2, C_BLACK);
	/* Left */
	UG_DrawLine(0, MENUBAR_HEIGHT, 0, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_BLACK);
	UG_DrawLine(1, MENUBAR_HEIGHT, 1, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_BLACK);
	/* Right */
	UG_DrawLine(LCD_WIDTH - 1, MENUBAR_HEIGHT, LCD_WIDTH - 1, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_BLACK);
	UG_DrawLine(LCD_WIDTH - 2, MENUBAR_HEIGHT, LCD_WIDTH - 2, LCD_HEIGHT - MENUBAR_HEIGHT - 1, C_BLACK);

	drawGrid();

	return;
}

/**
  * @brief  Take a full-screen screenshot and store it.
  * @param  None
  * @retval None
  */
void captureScreenshot(void)
{
	char filename[10];
	uint8_t nScrnshots;
	uint16_t maxfilename;
	UINT bytesTfr;

	if(readssinfo(&nScrnshots, &maxfilename) != 0){
		printf("Screenshot failed\n");
		return;
	}

	if(nScrnshots < MAX_SCRNSHOTS){
		nScrnshots = nScrnshots + 1;
		maxfilename = maxfilename + 1;

		itoa(maxfilename, filename, 10);							/* name the files starting from "1.bmp" */
		strcat(filename, ".bmp");

		raw2bmp((uint8_t *)LCD_FRAME_BUFFER, LCD_WIDTH*LCD_HEIGHT, (uint8_t *)BMP_BUFFER);		/* convert to BMP format */
		f_open(&fp, filename, FA_CREATE_ALWAYS | FA_WRITE);
		f_write(&fp, (const void *)BMP_BUFFER, BMP_FILE_SZ, &bytesTfr);
		f_close(&fp);

		if(bytesTfr != BMP_FILE_SZ){
			printf("Screenshot failed\n");
			return;
		}

		if(writessinfo(nScrnshots, maxfilename) != 0){
			printf("Screenshot failed\n");
			return;
		}

		printf("Screenshot success\n");
	}
	else{
		printf("No. of screenshots exceeded\n");
		return;
	}

	return;
}

/**
  * @brief  Delete the currently displayed screenshot.
  * @param  None
  * @retval None
  */
void deleteCurrScreenshot(void)
{
	char filename[10];
	FRESULT fsres;
	uint8_t nScrnshots = 0;
	uint16_t maxfilename = 0;

	itoa(currScrnshot, filename, 10);
	strcat(filename, ".bmp");

	fsres = f_unlink(filename);

	if(fsres != FR_NO_FILE){
		readssinfo(&nScrnshots, &maxfilename);

		if(nScrnshots != 0){
			nScrnshots = nScrnshots - 1;
		}
		if(currScrnshot == maxfilename){
			maxfilename = findPrevValidFile(currScrnshot);
		}

		writessinfo(nScrnshots, maxfilename);
	}

	return;
}

/**
  * @brief  Display x.bmp.
  * @param  x: file to be displayed
  * @retval None
  */
void displayScreenshot(uint16_t x)
{
	char filename[10];
	FRESULT fsres;
	UINT bytesTfr;

	itoa(x, filename, 10);
	strcat(filename, ".bmp");

	fsres = f_open(&fp, filename, FA_READ);

	if(fsres != FR_NO_FILE){
		f_read(&fp, (void *)BMP_BUFFER, BMP_FILE_SZ, &bytesTfr);
		f_close(&fp);

		if(bytesTfr != BMP_FILE_SZ){
			return;
		}

		bmp2raw((const uint8_t *)BMP_BUFFER, LCD_WIDTH*LCD_HEIGHT, (uint8_t *)LCD_FRAME_BUFFER);
	}

	return;
}

/**
  * @brief  Display the next older screenshot to the currently displayed one.
  * @param  None
  * @retval None
  */
void displayOlderScreenshot(void)
{
	uint16_t x;

	x = findPrevValidFile(currScrnshot);		/* older screenshots will have a lower filename */

	if(x == 0){									/* no older screenshot found */
		return;
	}

	currScrnshot = x;
	displayScreenshot(currScrnshot);

	return;
}

/**
  * @brief  Display the next recent screenshot to the currently displayed one.
  * @param  None
  * @retval 0=no newer screenshots found, 1=newer screenshots found
  */
uint8_t displayNewerScreenshot(void)
{
	uint16_t x;

	x = findNextValidFile(currScrnshot);		/* newer screenshots will have a higher filename */

	if(x == 0){									/* no newer screenshot found */
		return 0;
	}

	currScrnshot = x;
	displayScreenshot(currScrnshot);

	return 1;
}

/**
  * @brief  Enter screenshot view mode.
  * @param  None
  * @retval 0=no screenshots found, 1=screenshots found
  */
uint8_t enterScrnshtViewMode(void)
{
	uint8_t nScrnshots;
	uint16_t maxfilename = 0;

	readssinfo(&nScrnshots, &maxfilename);

	if(maxfilename == 0){						/* no screenshots found */
		return 0;
	}

	currScrnshot = maxfilename;
	displayScreenshot(currScrnshot);

	return 1;
}

/**
  * @brief  Find the next valid file to x.bmp.
  * @param  x: find file with name greater than x
  * @retval Next filename or 0 if not found
  */
static uint16_t findNextValidFile(uint16_t x)
{
	char filename[10];
	uint8_t nScrnshots;
	uint16_t maxfilename = 0, i;

	readssinfo(&nScrnshots, &maxfilename);

	for(i = x + 1; i <= maxfilename; i++){
		itoa(i, filename, 10);
		strcat(filename, ".bmp");

		if(f_open(&fp, filename, FA_OPEN_EXISTING) != FR_NO_FILE){	/* i.bmp exists */
			f_close(&fp);
			return i;
		}
	}

	return 0;
}

/**
  * @brief  Find the previous valid file to x.bmp.
  * @param  x: find file with name lesser than x
  * @retval Previous filename or 0 if not found
  */
static uint16_t findPrevValidFile(uint16_t x)
{
	char filename[10];
	uint16_t i;

	if(x == 0){
		return 0;
	}

	for(i = x - 1; i >= 1; i--){
		itoa(i, filename, 10);
		strcat(filename, ".bmp");

		if(f_open(&fp, filename, FA_OPEN_EXISTING) != FR_NO_FILE){	/* i.bmp exists */
			f_close(&fp);
			return i;
		}
	}

	return 0;
}

/**
  * @brief  Read the stored screenshots info.
  * @param  nScrnshots: pointer to number of screenshots
  * @param  maxfilename: pointer to max filename
  * @retval success=0, failure=1
  */
static uint8_t readssinfo(uint8_t* nScrnshots, uint16_t* maxfilename)
{
	uint8_t fdata[3];
	UINT bytesTfr;

	f_open(&fp, "ssinfo", FA_READ);
	f_read(&fp, fdata, 3, &bytesTfr);
	f_close(&fp);

	if(bytesTfr != 3){
		return 1;
	}

	*nScrnshots = fdata[0];
	*maxfilename = fdata[1] << 8 | fdata[2];

	return 0;
}

/**
  * @brief  Update info in the screenshot info file.
  * @param  nScrnshots: number of screenshots
  * @param  maxfilename: max filename
  * @retval success=0, failure=1
  */
static uint8_t writessinfo(uint8_t nScrnshots, uint16_t maxfilename)
{
	uint8_t fdata[3];
	UINT bytesTfr;

	fdata[0] = nScrnshots;
	fdata[1] = maxfilename >> 8;
	fdata[2] = maxfilename;
	f_open(&fp, "ssinfo", FA_WRITE);
	f_write(&fp, fdata, 3, &bytesTfr);
	f_close(&fp);

	if(bytesTfr != 3){
		return 1;
	}

	return 0;
}
