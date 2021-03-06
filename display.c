#include "display.h"

#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#include "eeprom.h"
#include "input.h"
#include "tuner/tuner.h"
#include "ds18x20.h"
#include "temp.h"
#include "adc.h"
#include "tuner/rds.h"
#include "alarm.h"

int8_t brStby;								/* Brightness in standby mode */
int8_t brWork;								/* Brightness in working mode */

uint8_t spMode;								/* Spectrum mode */
static uint8_t fallSpeed;					/* Spectrum fall speed */

static uint8_t rcIndex = CMD_RC_STBY;

static uint8_t defDisplay;					/* Default display mode */

char strbuf[STR_BUFSIZE + 1];				/* String buffer */
uint8_t *txtLabels[LABEL_END];				/* Array with text label pointers */

const char STR_RC_STBY[]		PROGMEM = "Standby mode";
const char STR_RC_MUTE[]		PROGMEM = "Mute sound";
const char STR_RC_NEXT_SNDPAR[] PROGMEM = "Sound menu";
const char STR_RC_VOL_UP[]		PROGMEM = "Volume +";
const char STR_RC_VOL_DOWN[]	PROGMEM = "Volume -";
const char STR_RC_IN_0[]		PROGMEM = "Input 1";
const char STR_RC_IN_1[]		PROGMEM = "Input 2";
const char STR_RC_IN_2[]		PROGMEM = "Input 3";
const char STR_RC_IN_3[]		PROGMEM = "Input 4";
const char STR_RC_IN_4[]		PROGMEM = "Input 5";
const char STR_RC_IN_PREV[]		PROGMEM = "Prev input";
const char STR_RC_IN_NEXT[]		PROGMEM = "Next input";
const char STR_RC_LOUDNESS[]	PROGMEM = "Loudness";
const char STR_RC_SURROUND[]	PROGMEM = "Surround";
const char STR_RC_EFFECT_3D[]	PROGMEM = "3D effect";
const char STR_RC_TONE_DEFEAT[]	PROGMEM = "Tone defeat";

const char STR_RC_DEF_DISPLAY[]	PROGMEM = "Display mode";
const char STR_RC_FM_INC[]		PROGMEM = "Channel +";
const char STR_RC_FM_DEC[]		PROGMEM = "Channel -";
const char STR_RC_FM_MODE[]		PROGMEM = "FM tune";
const char STR_RC_FM_MONO[]		PROGMEM = "FM mono";
const char STR_RC_FM_STORE[]	PROGMEM = "Store freq";
const char STR_RC_FM_0[]		PROGMEM = "Button 0";
const char STR_RC_FM_1[]		PROGMEM = "Button 1";
const char STR_RC_FM_2[]		PROGMEM = "Button 2";
const char STR_RC_FM_3[]		PROGMEM = "Button 3";
const char STR_RC_FM_4[]		PROGMEM = "Button 4";
const char STR_RC_FM_5[]		PROGMEM = "Button 5";
const char STR_RC_FM_6[]		PROGMEM = "Button 6";
const char STR_RC_FM_7[]		PROGMEM = "Button 7";
const char STR_RC_FM_8[]		PROGMEM = "Button 8";
const char STR_RC_FM_9[]		PROGMEM = "Button 9";

const char STR_RC_TIME[]		PROGMEM = "Time";
const char STR_RC_ALARM[]		PROGMEM = "Alarm";
const char STR_RC_TIMER[]		PROGMEM = "Timer";
const char STR_RC_BRIGHTNESS[]	PROGMEM = "Brightness";
const char STR_RC_NEXT_SPMODE[]	PROGMEM = "Spectrum mode";
const char STR_RC_FALLSPEED[]	PROGMEM = "Fall speed";

PGM_P const rcLabels[] PROGMEM = {
	STR_RC_STBY,
	STR_RC_MUTE,
	STR_RC_NEXT_SNDPAR,
	STR_RC_VOL_UP,
	STR_RC_VOL_DOWN,
	STR_RC_IN_0,
	STR_RC_IN_1,
	STR_RC_IN_2,
	STR_RC_IN_3,
	STR_RC_IN_4,
	STR_RC_IN_PREV,
	STR_RC_IN_NEXT,
	STR_RC_LOUDNESS,
	STR_RC_SURROUND,
	STR_RC_EFFECT_3D,
	STR_RC_TONE_DEFEAT,

	STR_RC_DEF_DISPLAY,
	STR_RC_FM_INC,
	STR_RC_FM_DEC,
	STR_RC_FM_MODE,
	STR_RC_FM_MONO,
	STR_RC_FM_STORE,
	STR_RC_FM_0,
	STR_RC_FM_1,
	STR_RC_FM_2,
	STR_RC_FM_3,
	STR_RC_FM_4,
	STR_RC_FM_5,
	STR_RC_FM_6,
	STR_RC_FM_7,
	STR_RC_FM_8,
	STR_RC_FM_9,

	STR_RC_TIME,
	STR_RC_ALARM,
	STR_RC_TIMER,
	STR_RC_BRIGHTNESS,
	STR_RC_NEXT_SPMODE,
	STR_RC_FALLSPEED,
};

const char STR_IN_STATUS[]		PROGMEM = "BUTTONS & ENCODER";
const char STR_REMOTE[]			PROGMEM = "Remote";
const char STR_BUTTONS[]		PROGMEM = "Status";
const char STR_LEARN[]			PROGMEM = "REMOTE CONTROL";
const char STR_FUNCTION[]		PROGMEM = "Function";
const char STR_ADDRESS[]		PROGMEM = "Address";
const char STR_COMMAND[]		PROGMEM = "Command";
const char STR_RC_RC5[]			PROGMEM = "RC5";
const char STR_RC_NEC[]			PROGMEM = "NEC";
const char STR_RC_NONE[]		PROGMEM = "---";

const char STR_THRESHOLD[]		PROGMEM = "Threshold";
const char STR_DEGREE[]			PROGMEM = "\xDF""C";
const char STR_SENSOR1[]		PROGMEM = "Sensor 1: ";
const char STR_SENSOR2[]		PROGMEM = "Sensor 2: ";

const char STR_SPDIVSP[]		PROGMEM = "\x7F/\x7F";
const char STR_SPARRSP[]		PROGMEM = " => ";
const char STR_SPDOTSP[]		PROGMEM = "\x7F.\x7F";
const char STR_SPCOLSP[]		PROGMEM = "\x7F:\x7F";
const char STR_SP[]				PROGMEM = "\x7F";
const char STR_SPACE2[]			PROGMEM = "  ";
const char STR_SPACE3[]			PROGMEM = "   ";
const char STR_MINUS1[]			PROGMEM = "-";
const char STR_MINUS2[]			PROGMEM = "--";
const char STR_SPMINUS2[]		PROGMEM = " --";

const char STR_FM[]				PROGMEM = "FM ";
const char STR_STEREO[]			PROGMEM = "ST";
const char STR_TUNE[]			PROGMEM = "\xDB\xDB\xD0\xDC\xDC";
const char STR_RDS[]			PROGMEM = "RDS";

const char STR_YEAR20[]			PROGMEM = "20";

#ifdef KS0066
static uint8_t userSybmols = LCD_END;		/* Generated user symbols for ks0066 */
static uint8_t userAddSym = SYM_END;		/* Additional user symbol */
#endif

#ifdef KS0066
static void lcdGenLevels(void)
{
	if (userSybmols != LCD_LEVELS) {		/* Generate 7 level symbols */
		userSybmols = LCD_LEVELS;

		uint8_t i, j;
		ks0066StartSym(0);
		for (i = 0; i < 7; i++)
			for (j = 0; j < 8; j++)
				if (i + j >= 7)
					ks0066WriteData(0xFF);
				else
					ks0066WriteData(0x00);
	}

	return;
}

static void lcdGenBar(uint8_t sym)
{
	static const uint8_t bar[] PROGMEM = {
		0b00000,
		0b10000,
		0b10100,
		0b10101,
		0b00101,
		0b00001,
	};
	static const uint8_t speakerIcon[] PROGMEM = {
		0b00001,
		0b00011,
		0b11101,
		0b10101,
		0b11101,
		0b00011,
		0b00001,
		0b00000,
	};
	static const uint8_t loudnessIcon[] PROGMEM = {
		0b00000,
		0b10000,
		0b10001,
		0b10101,
		0b10101,
		0b10101,
		0b10101,
		0b00000,
	};
	static const uint8_t surroundIcon[] PROGMEM = {
		0b01010,
		0b10001,
		0b10101,
		0b10101,
		0b10101,
		0b10001,
		0b01010,
		0b00000,
	};
	static const uint8_t effect3dIcon[] PROGMEM = {
		0b01110,
		0b10001,
		0b00000,
		0b01110,
		0b00000,
		0b10001,
		0b01110,
		0b00000,
	};
	static const uint8_t toneDefeatIcon[] PROGMEM = {
		0b01000,
		0b10101,
		0b00010,
		0b00000,
		0b11111,
		0b00000,
		0b11111,
		0b00000,
	};
	static const uint8_t stereoIcon[] PROGMEM = {
		0b00000,
		0b11011,
		0b10101,
		0b10101,
		0b10101,
		0b11011,
		0b00000,
		0b00000,
	};
	static const uint8_t crossIcon[] PROGMEM = {
		0b00000,
		0b10001,
		0b01010,
		0b00100,
		0b01010,
		0b10001,
		0b00000,
		0b00000,
	};
	static const uint8_t degreeIcon[] PROGMEM = {
		0b00110,
		0b01001,
		0b01001,
		0b00110,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
	};

	uint8_t i;

	if (userSybmols != LCD_BAR || userAddSym != sym) {
		/* Generate main 6 bar symbols */
		userSybmols = LCD_BAR;
		ks0066StartSym(0);
		for (i = 0; i < 48; i++) {
			if ((i & 0x07) == 0x03) {
				ks0066WriteData(0x15);
			} else if ((i & 0x07) == 0x07) {
				ks0066WriteData(0x00);
			} else {
				ks0066WriteData(pgm_read_byte(&bar[i>>3]));
			}
		}
		/* Generate two additional symbols */
		userAddSym = sym;
		switch (sym) {
		case SYM_MUTE_CROSS:
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&speakerIcon[i]));
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&crossIcon[i]));
			break;
		case SYM_LOUDNESS_CROSS:
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&loudnessIcon[i]));
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&crossIcon[i]));
			break;
		case SYM_SURROUND_CROSS:
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&surroundIcon[i]));
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&crossIcon[i]));
			break;
		case SYM_EFFECT_3D_CROSS:
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&effect3dIcon[i]));
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&crossIcon[i]));
			break;
		case SYM_TONE_DEFEAT_CROSS:
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&toneDefeatIcon[i]));
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&crossIcon[i]));
			break;
		default:
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&stereoIcon[i]));
			for (i = 0; i < 8; i++)
				ks0066WriteData(pgm_read_byte(&degreeIcon[i]));
			break;
		}
	}

	return;
}

static void lcdGenAlarm(void)
{
	static const uint8_t alarmSym[] PROGMEM = {
		/* Filled rectangle */
		0b11111,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b00000,
		0b11111,
		0b00000,
		/* Empty rectangle */
		0b11111,
		0b00000,
		0b11111,
		0b11111,
		0b11111,
		0b00000,
		0b11111,
		0b00000,
		/* Space between rectangles */
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b00000,
		/* Visible left bracket */
		0b11001,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b11001,
		0b00000,
		/* Invisible left bracket */
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00001,
		0b00000,
		/* Visible right bracket */
		0b10011,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10001,
		0b10011,
		0b00000,
		/* Invisible right bracket */
		0b10000,
		0b10000,
		0b10000,
		0b10000,
		0b10000,
		0b10000,
		0b10000,
		0b00000,

	};
	if (userSybmols != LCD_ALARM) {		/* Generate alarm symbols */
		userSybmols = LCD_ALARM;

		uint8_t i;
		ks0066StartSym(0);
		for (i = 0; i < sizeof(alarmSym); i++)
			ks0066WriteData(pgm_read_byte(&alarmSym[i]));
	}

	return;
}
#endif

static void showBar(int16_t min, int16_t max, int16_t value)
{
#ifdef KS0066
	uint8_t i;

	lcdGenBar(userAddSym);

	ks0066SetXY(0, 1);

	if (min + max) {
		value = (int16_t)48 * (value - min) / (max - min);
		for (i = 0; i < 16; i++) {
			if (value / 3 > i) {
				ks0066WriteData(0x03);
			} else {
				if (value / 3 < i) {
					ks0066WriteData(0x00);
				} else {
					ks0066WriteData(value % 3);
				}
			}
		}
	} else {
		value = (int16_t)23 * value / max;
		if (value >= 0) {
			value++;
			for (i = 0; i < 7; i++) {
				ks0066WriteData(0x00);
			}
			ks0066WriteData(0x05);
			for (i = 0; i < 8; i++) {
				if (value / 3 > i) {
					ks0066WriteData(0x03);
				} else {
					if (value / 3 < i) {
						ks0066WriteData(0x00);
					} else {
						ks0066WriteData(value % 3);
					}
				}
			}
		} else {
			value += 23;
			for (i = 0; i < 8; i++) {
				if (value / 3 > i) {
					ks0066WriteData(0x00);
				} else {
					if (value / 3 < i) {
						ks0066WriteData(0x03);
					} else {
						ks0066WriteData(value % 3 + 3);
					}
				}
			}
			ks0066WriteData(0x01);
			for (i = 0; i < 7; i++) {
				ks0066WriteData(0x00);
			}
		}
	}
#else
	uint8_t i, j;
	uint8_t color;

	if (min + max) {
		value = (int16_t)91 * (value - min) / (max - min);
	} else {
		value = (int16_t)45 * value / max;
	}

	for (i = 0; i < 91; i++) {
		if (((min + max) && (value <= i)) || (!(min + max) &&
											  (((value > 0) && ((i < 45) || (value + 45 < i))) ||
											   ((value <= 0) && ((i > 45) || (value + 45 > i)))))) {
			color = 0x00;
		} else {
			color = 0x01;
		}
		if (!(i & 0x01)) {
			for (j = 28; j < 39; j++) {
				if (j == 33) {
					gdDrawPixel(i, j, 1);
				} else {
					gdDrawPixel(i, j, color);
				}
			}
		}
	}
	return;
#endif
}

static void writeString(char *string)
{
#ifdef KS0066
	ks0066WriteString (string);
#else
	gdWriteString(string);
#endif
	return;
}

static void writeStringEeprom(const uint8_t *string)
{
	uint8_t i;

	for (i = 0; i < STR_BUFSIZE; i++)
		strbuf[i] = eeprom_read_byte(&string[i]);

	writeString(strbuf);

	return;
}

static void writeStringPgm(const char *string) {
	strcpy_P(strbuf, string);
	writeString(strbuf);

	return;
}

static void writeNum(int16_t number, uint8_t width, uint8_t lead, uint8_t radix)
{
	uint8_t numdiv;
	uint8_t sign = lead;
	int8_t i;

	if (number < 0) {
		sign = '-';
		number = -number;
	}

	for (i = 0; i < width; i++)
		strbuf[i] = lead;
	strbuf[width] = '\0';
	i = width - 1;

	while (number > 0 || i == width - 1) {
		numdiv = number % radix;
		strbuf[i] = numdiv + 0x30;
		if (numdiv >= 10)
			strbuf[i] += 7;
		i--;
		number /= radix;
	}

	if (i >= 0)
		strbuf[i] = sign;

	writeString(strbuf);

	return;
}

static void showParValue(int8_t value)
{
#ifdef KS0066
	ks0066SetXY(11, 0);
	writeNum(value, 3, ' ', 10);
#else
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(94, 30);
	writeNum(value, 3, ' ', 10);
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	return;

#endif
}
static void showParLabel(const uint8_t *parLabel)
{
#ifdef KS0066
	ks0066SetXY (0, 0);
	writeStringEeprom(parLabel);
#else
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(0, 0);
	writeStringEeprom(parLabel);
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	return;

#endif
}
#ifdef KS0066
#else
static void showParIcon(const uint8_t iconNum)
{
	gdSetXY(104, 2);
	gdWriteIcon24(iconNum);

	return;
}
#endif

#ifndef KS0066
static void drawSpCol(uint8_t xbase, uint8_t w, uint8_t btm, uint8_t val, uint8_t max)
{
	uint8_t i;

	val = (val < max ? btm - val : btm - max);

	for (i = 0; i < w; i++) {
		gdDrawVertLine(xbase + i, btm, val, 1);
		gdDrawVertLine(xbase + i, val > (btm - max) ? val - 1 : val, btm - max, 0);
	}

	return;
}

static void drawBarSpectrum(void)
{
	uint8_t x, xbase;
	uint8_t ybase;

	getSpData(fallSpeed);

	for (x = 0; x < GD_SIZE_X / 4 - 1; x++) {
		xbase = x * 3;

		ybase = (buf[x] + buf[x + 32]) * 3 / 8;
		drawSpCol(xbase, 2, 63, ybase, 23);
		gdDrawVertLine(xbase + 2, 63, 63 - 23, 0); // Clear space between bars
	}

	return;
}
#endif


static void drawMiniSpectrum(void)
{
	getSpData(fallSpeed);

#ifdef KS0066
	uint16_t data;
	uint8_t i;

	lcdGenBar (userAddSym);
	data = 0;
	for (i = 0; i < FFT_SIZE / 2; i++) {
		data += buf[i];
		data += buf[FFT_SIZE / 2 + i];
	}
	data >>= 5;

	ks0066SetXY(0, 1);
	for (i = 0; i < KS0066_SCREEN_WIDTH; i++) {
		if (data / 3 > i) {
			ks0066WriteData(0x03);
		} else {
			if (data / 3 < i) {
				ks0066WriteData(0x00);
			} else {
				ks0066WriteData(data % 3);
			}
		}
	}
#else
	uint8_t x, xbase;
	uint8_t ybase;

	for (x = 0; x < GD_SIZE_X / 4 - 1; x++) {
		xbase = x * 3;
		ybase = (buf[x] * 5 / 2 + buf[x + 32] * 5 / 2) / 4;
		drawSpCol(xbase, 2, 63, ybase, 39);
	}

	return;
#endif
}

#ifdef KS0066
static void drawTm(uint8_t tm)
{
	if (getEtm() != tm || (getSecTimer() % 512) < 200) {
		writeNum(getTime(tm), 2, '0', 10);
	} else {
		writeString("  ");
	}
#else
static void drawTm(uint8_t tm, const uint8_t *font)
{
	if (getEtm() == tm)
		gdLoadFont(font, 0, FONT_DIR_0);
	else
		gdLoadFont(font, 1, FONT_DIR_0);
	writeNum(getTime(tm), 2, '0', 10);
	gdLoadFont(font, 1, FONT_DIR_0);
#endif
	return;
}

#ifdef KS0066
static void drawAm(uint8_t am)
{
	if (getEam() != am || (getSecTimer() % 512) < 200) {
		writeNum(getAlarm(am), 2, '0', 10);
	} else {
		writeString("  ");
	}
#else
static void drawAm(uint8_t am, const uint8_t *font)
{
	if (getEam() == am)
		gdLoadFont(font, 0, FONT_DIR_0);
	else
		gdLoadFont(font, 1, FONT_DIR_0);
	writeNum(getAlarm(am), 2, '0', 10);
	gdLoadFont(font, 1, FONT_DIR_0);
#endif
	return;
}

void displayInit(void)
{
	uint8_t i;
	uint8_t *addr;

	addr = (uint8_t*)EEPROM_LABELS_ADDR;
	i = 0;

	while (i < LABEL_END && addr < (uint8_t*)EEPROM_SIZE) {
		if (eeprom_read_byte(addr) != '\0') {
			txtLabels[i] = addr;
			addr++;
			i++;
			while (eeprom_read_byte(addr) != '\0' &&
				   addr < (uint8_t*)EEPROM_SIZE) {
				addr++;
			}
		} else {
			addr++;
		}
	}

#ifdef KS0066
	ks0066Init();
	lcdGenLevels();
#else
	gdInit();
#endif

	brStby = eeprom_read_byte((uint8_t*)EEPROM_BR_STBY);
	brWork = eeprom_read_byte((uint8_t*)EEPROM_BR_WORK);
	spMode  = eeprom_read_byte((uint8_t*)EEPROM_SP_MODE);
	defDisplay = eeprom_read_byte((uint8_t*)EEPROM_DISPLAY);
	fallSpeed = eeprom_read_byte((uint8_t*)EEPROM_FALL_SPEED);
	if (fallSpeed > FALL_SPEED_FAST)
		fallSpeed = FALL_SPEED_FAST;

	return;
}

void displayClear(void)
{
#ifdef KS0066
	ks0066Clear();
#else
	gdClear();
#endif

	return;
}

uint8_t **getTxtLabels(void)
{
	return txtLabels;
}

void setDefDisplay(uint8_t value)
{
	defDisplay = value;

	return;
}

uint8_t getDefDisplay()
{
	return defDisplay;
}

void nextRcCmd(void)
{
	IRData irBuf = getIrData();

	eeprom_update_byte((uint8_t*)EEPROM_RC_TYPE, irBuf.type);
	eeprom_update_byte((uint8_t*)EEPROM_RC_ADDR, irBuf.address);
	eeprom_update_byte((uint8_t*)EEPROM_RC_CMD + rcIndex, irBuf.command);

	// Re-read new codes array from EEPROM
	rcCodesInit();

	if (++rcIndex >= CMD_RC_END)
		rcIndex = CMD_RC_STBY;

	switchTestMode(rcIndex);

	return;
}

void switchTestMode(uint8_t index)
{
	rcIndex = index;
	setIrData(eeprom_read_byte((uint8_t*)EEPROM_RC_TYPE),
			  eeprom_read_byte((uint8_t*)EEPROM_RC_ADDR),
							eeprom_read_byte((uint8_t*)EEPROM_RC_CMD + rcIndex));

	return;
}

void showRcInfo(void)
{
	IRData irBuf = getIrData();
	uint8_t btnBuf = getBtnBuf();
	uint8_t encBuf = getEncBuf();

#ifdef KS0066
	ks0066SetXY(0, 0);
	writeString("B=");
	writeNum(btnBuf + (encBuf << 6), 2, '0', 16);
	writeString(" ");
	writeStringPgm((const char *)pgm_read_word(&rcLabels[rcIndex]));

	ks0066SetXY(0, 1);
	writeString("A=");
	writeNum(irBuf.address, 2, '0', 16);
	writeString(" C=");
	writeNum(irBuf.command, 2, '0', 16);
	writeString(" T=");
	switch (irBuf.type) {
	case IR_TYPE_RC5:
		writeStringPgm(STR_RC_RC5);
		break;
	case IR_TYPE_NEC:
		writeStringPgm(STR_RC_NEC);
		break;
	default:
		writeStringPgm(STR_RC_NONE);
		break;
	}
#else
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	gdSetXY(10, 0);
	writeStringPgm(STR_IN_STATUS);

	gdSetXY(0, 10);
	writeStringPgm(STR_BUTTONS);
	gdSetXY(48, 10);
	writeNum(btnBuf, 5, '0', 2);
	writeStringPgm(STR_SPDIVSP);
	writeNum(encBuf, 2, '0', 2);

	gdSetXY(10, 20);
	writeStringPgm(STR_LEARN);

	gdSetXY(0, 30);
	writeStringPgm(STR_REMOTE);
	gdSetXY(48, 30);
	switch (irBuf.type) {
	case IR_TYPE_RC5:
		writeStringPgm(STR_RC_RC5);
		break;
	case IR_TYPE_NEC:
		writeStringPgm(STR_RC_NEC);
		break;
	default:
		writeStringPgm(STR_RC_NONE);
		break;
	}

	gdSetXY(0, 39);
	writeStringPgm(STR_ADDRESS);
	gdSetXY(48, 39);
	writeNum(irBuf.address, 2, '0', 16);
	writeStringPgm(STR_SPARRSP);
	writeNum(eeprom_read_byte((uint8_t*)EEPROM_RC_ADDR), 2, '0', 16);

	gdSetXY(0, 48);
	writeStringPgm(STR_FUNCTION);
	gdSetXY(48, 48);
	writeStringPgm((const char *)pgm_read_word(&rcLabels[rcIndex]));

	gdSetXY(0, 57);
	writeStringPgm(STR_COMMAND);
	gdSetXY(48, 57);
	writeNum(irBuf.command, 2, '0', 16);
	writeStringPgm(STR_SPARRSP);
	writeNum(eeprom_read_byte((uint8_t*)EEPROM_RC_CMD + rcIndex), 2, '0', 16);
#endif
	return;
}

void showTemp(void)
{
	int8_t tempTH = getTempTH();

#ifdef KS0066
	lcdGenBar (SYM_STEREO_DEGREE);
	ks0066SetXY (0, 0);
	writeStringPgm(STR_THRESHOLD);
	showParValue (tempTH);
	writeString("\x07""C");

	ks0066SetXY (0, 1);
	writeString("1:");
	writeNum(ds18x20GetTemp(0) / 10, 3, ' ', 10);
	writeString("\x07""C");
	writeString("  2:");
	writeNum(ds18x20GetTemp(1) / 10, 3, ' ', 10);
	writeString("\x07""C");
#else
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);

	gdSetXY(0, 48);
	writeStringPgm(STR_SENSOR1);
	writeNum(ds18x20GetTemp(0) / 10, 3, ' ', 10);
	writeStringPgm(STR_DEGREE);

	gdSetXY(0, 56);
	writeStringPgm(STR_SENSOR2);
	writeNum(ds18x20GetTemp(1) / 10, 3, ' ', 10);
	writeStringPgm(STR_DEGREE);

	showParValue(tempTH);
	showBar(MIN_TEMP, MAX_TEMP, tempTH);
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(0, 0);
	writeStringPgm(STR_THRESHOLD);
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	showParIcon(ICON24_THRESHOLD);

#endif

	return;
}

void showRadio(uint8_t tune)
{
	tunerReadStatus();

	uint8_t i;

	uint16_t freq = tunerGetFreq();
	uint8_t level = tunerLevel();
	uint8_t num = tunerStationNum();
	int8_t favNum = tunerFavStationNum();

#ifdef KS0066
	lcdGenBar(SYM_STEREO_DEGREE);

	/* Frequency value */
	ks0066SetXY(0, 0);
	writeString("FM ");
	writeNum(freq / 100, 3, ' ', 10);
	ks0066WriteData('.');
	writeNum(freq % 100, 2, '0', 10);

	/* Signal level */
	ks0066SetXY(12, 0);
	level = level * 2 / 5;
	if (level < 3) {
		ks0066WriteData(level);
		ks0066WriteData(0x00);
	} else {
		ks0066WriteData(0x03);
		ks0066WriteData(level - 3);
	}

	/* Stereo indicator */
	ks0066SetXY(10, 0);
	if (tunerStereo())
		writeString("\x06");
	else
		writeString(" ");

	/* Favourite station number */
	ks0066SetXY(15, 0);
	if (favNum >= 0) {
		writeNum(favNum, 1, ' ', 10);
	} else {
		writeString("-");
	}

	/* Station number */
	ks0066SetXY(14, 1);
	if (num) {
		writeNum(num, 2, ' ', 10);
	} else {
		writeString("--");
	}

	/* Select between RDS and spectrum mode */
	if (rdsGetFlag()) {
		/* RDS data */
		ks0066SetXY(0, 1);
		writeString("RDS:");
		writeString(rdsGetText ());
	} else {
		/* Frequency scale */
		uint8_t value = (int16_t)36 * ((freq - FM_FREQ_MIN)>>4) / ((FM_FREQ_MAX - FM_FREQ_MIN)>>4);
		ks0066SetXY(0, 1);
		for (i = 0; i < 12; i++) {
			if (value / 3 > i) {
				ks0066WriteData(0x03);
			} else {
				if (value / 3 < i) {
					ks0066WriteData(0x00);
				} else {
					ks0066WriteData(value % 3);
				}
			}
		}

	}

	/* Tune status */
	ks0066SetXY (12, 1);
	if (tune == MODE_RADIO_TUNE) {
		writeString("<>");
	} else {
		writeString("  ");
	}

#else
	/* Frequency value */
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(0, 0);
	writeStringPgm(STR_FM);
	writeNum(freq / 100, 3, ' ', 10);
	writeStringPgm(STR_SPDOTSP);
	writeNum(freq % 100, 2, '0', 10);
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);

	/* Signal level */
	gdSetXY (112, 0);
	for (i = 0; i < 16; i+=2) {
		if (i <= level) {
			gdDrawLine(112 + i, 7, 112 + i, 7 - i / 2, 0x01);
		} else {
			gdDrawLine(112 + i, 6, 112 + i, 0, 0x00);
			gdDrawPixel(112 + i, 7, 0x01);
		}
	}

	/* Stereo indicator */
	gdSetXY(116, 12);
	if (tunerStereo())
		writeStringPgm(STR_STEREO);
	else
		writeStringPgm(STR_SPACE2);

	/* Favourite station number */
	gdSetXY(114, 23);
	gdWriteChar(0xF5);						/* Heart symbol */
	gdSetXY(122, 23);
	if (favNum >= 0)
		writeNum(favNum, 1, ' ', 10);
	else
		writeStringPgm(STR_MINUS1);

	/* Station number */
	gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
	gdSetXY(94, 30);
	if (num)
		writeNum(num, 3, ' ', 10);
	else
		writeStringPgm(STR_SPMINUS2);
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);

	/* Frequency scale */
	showBar(FM_FREQ_MIN>>4, FM_FREQ_MAX>>4, freq>>4);

	/* Select between RDS and spectrum mode */
	if (rdsGetFlag()) {
		gdLoadFont(font_ks0066_ru_24, 1, FONT_DIR_0);
		gdSetXY(0, 40);
		writeString(rdsGetText());
		gdDrawFilledRect(gdGetX(), 40, 103 - gdGetX(), 24, 0);
	} else {
		drawBarSpectrum();
	}

	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	if (tune == MODE_RADIO_TUNE) {
		gdSetXY(103, 56);
		writeStringPgm(STR_TUNE);
	} else {
		gdSetXY(110, 56);
		if (rdsGetFlag())
			writeStringPgm(STR_RDS);
		else
			writeStringPgm(STR_SPACE3);
	}
#endif

	return;
}

void showMute(void)
{
	showParLabel(txtLabels[LABEL_MUTE]);
	drawMiniSpectrum();

#ifdef KS0066
	lcdGenBar(SYM_MUTE_CROSS);
	ks0066SetXY(14, 0);
	ks0066WriteData(0x06);
	if (sndGetMute ())
		ks0066WriteData(0x07);
	else
		ks0066WriteData(' ');
#else
	gdSetXY(96, 32);
	if (sndGetMute())
		gdWriteIcon32(ICON32_MUTE_ON);
	else
		gdWriteIcon32(ICON32_MUTE_OFF);
#endif

	return;
}

void showLoudness(void)
{
	showParLabel(txtLabels[LABEL_LOUDNESS]);
	drawMiniSpectrum();
#ifdef KS0066
	lcdGenBar(SYM_LOUDNESS_CROSS);
	ks0066SetXY(14, 0);
	ks0066WriteData(0x06);
	if (sndGetLoudness ())
		ks0066WriteData(0x07);
	else
		ks0066WriteData(' ');
#else
	gdSetXY(96, 32);
	if (sndGetLoudness())
		gdWriteIcon32(ICON32_LOUDNESS_ON);
	else
		gdWriteIcon32(ICON32_LOUDNESS_OFF);
#endif

	return;
}

void showSurround()
{
	showParLabel(txtLabels[LABEL_SURROUND]);
	drawMiniSpectrum();
#ifdef KS0066
	lcdGenBar(SYM_SURROUND_CROSS);
	ks0066SetXY(14, 0);
	ks0066WriteData(0x06);
	if (sndGetSurround())
		ks0066WriteData(0x07);
	else
		ks0066WriteData(' ');
#else
	gdSetXY(96, 32);
	if (sndGetSurround())
		gdWriteIcon32(ICON32_SURROUND_ON);
	else
		gdWriteIcon32(ICON32_SURROUND_OFF);
#endif
	return;
}

void showEffect3d()
{
	showParLabel(txtLabels[LABEL_EFFECT_3D]);
	drawMiniSpectrum();
#ifdef KS0066
	lcdGenBar(SYM_EFFECT_3D_CROSS);
	ks0066SetXY(14, 0);
	ks0066WriteData(0x06);
	if (sndGetEffect3d())
		ks0066WriteData(0x07);
	else
		ks0066WriteData(' ');
#else
	gdSetXY(96, 32);
	if (sndGetEffect3d())
		gdWriteIcon32(ICON32_EFFECT_3D_ON);
	else
		gdWriteIcon32(ICON32_EFFECT_3D_OFF);
#endif
	return;
}

void showToneDefeat()
{
	showParLabel(txtLabels[LABEL_TONE_DEFEAT]);
	drawMiniSpectrum();
#ifdef KS0066
	lcdGenBar(SYM_TONE_DEFEAT_CROSS);
	ks0066SetXY(14, 0);
	ks0066WriteData(0x06);
	if (sndGetToneDefeat())
		ks0066WriteData(0x07);
	else
		ks0066WriteData(' ');
#else
	gdSetXY(96, 32);
	if (sndGetToneDefeat())
		gdWriteIcon32(ICON32_TONE_DEFEAT_ON);
	else
		gdWriteIcon32(ICON32_TONE_DEFEAT_OFF);
#endif
	return;
}


void showBrWork(void)
{
	showParLabel(txtLabels[LABEL_BR_WORK]);
	showBar(MIN_BRIGHTNESS, MAX_BRIGHTNESS, brWork);
#ifdef KS0066
	ks0066SetXY(13, 0);
	writeNum(brWork, 3, ' ', 10);
#else
	showParValue(brWork);
	drawBarSpectrum();
	showParIcon(ICON24_BRIGHTNESS);
#endif
	return;
}

void changeBrWork(int8_t diff)
{
	brWork += diff;
	if (brWork > MAX_BRIGHTNESS)
		brWork = MAX_BRIGHTNESS;
	if (brWork < MIN_BRIGHTNESS)
		brWork = MIN_BRIGHTNESS;
	setWorkBrightness();

	return;
}

void showSndParam(sndMode mode)
{
	sndParam *param = sndParAddr(mode);

	showParLabel(param->label);
	showParValue(((int16_t)(param->value) * (int8_t)pgm_read_byte(&param->grid->step) + 4) >> 3);
	showBar((int8_t)pgm_read_byte(&param->grid->min), (int8_t)pgm_read_byte(&param->grid->max), param->value);
#ifdef KS0066
	ks0066SetXY(14, 0);
#else
	drawBarSpectrum();
	showParIcon(param->icon);
	gdSetXY(116, 56);
#endif
	writeStringEeprom(txtLabels[LABEL_DB]);

	return;
}

void showTime(void)
{
#ifdef KS0066
	ks0066SetXY(0, 0);
	drawTm (DS1307_HOUR);
	ks0066WriteData (':');
	drawTm (DS1307_MIN);
	ks0066WriteData (':');
	drawTm (DS1307_SEC);

	ks0066SetXY(11, 0);
	drawTm(DS1307_DATE);
	ks0066WriteData('.');
	drawTm(DS1307_MONTH);

	ks0066SetXY(12, 1);
	ks0066WriteString("20");
	drawTm(DS1307_YEAR);

	ks0066SetXY(0, 1);
#else
	gdSetXY(4, 0);

	drawTm(DS1307_HOUR, font_digits_32);
	writeStringPgm(STR_SPCOLSP);
	drawTm(DS1307_MIN, font_digits_32);
	writeStringPgm(STR_SPCOLSP);
	drawTm(DS1307_SEC, font_digits_32);

	gdSetXY(9, 32);

	drawTm(DS1307_DATE, font_ks0066_ru_24);
	writeStringPgm(STR_SPDOTSP);
	drawTm(DS1307_MONTH, font_ks0066_ru_24);
	writeStringPgm(STR_SPDOTSP);
	if (getEtm() == DS1307_YEAR)
		gdLoadFont(font_ks0066_ru_24, 0, FONT_DIR_0);
	writeStringPgm(STR_YEAR20);
	writeStringPgm(STR_SP);
	drawTm(DS1307_YEAR, font_ks0066_ru_24);

	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	gdSetXY(32, 56);
#endif
	writeStringEeprom(txtLabels[LABEL_SUNDAY + (getTime(DS1307_WDAY) - 1) % 7]);

	return;
}

void showAlarm(void)
{
#ifdef KS0066
	uint8_t i;

	/* Draw alarm value */
	ks0066SetXY(0, 0);
	drawAm(DS1307_A0_HOUR);
	ks0066WriteData(':');
	drawAm(DS1307_A0_MIN);

	/* Check that input number less than CHAN_CNT */
	i = getAlarm(DS1307_A0_INPUT);
	if (i >= sndInputCnt())
		i = 0;

	/* Draw selected input */
	ks0066SetXY(6, 0);
	if (getEam() != DS1307_A0_INPUT || (getSecTimer() % 512) < 200) {
		writeStringEeprom(sndParAddr (MODE_SND_GAIN0 + i)->label);
	}
	/* Clear string tail */
	ks0066WriteTail (' ', 15);

	/* Draw weekdays */
	lcdGenAlarm ();
	ks0066SetXY(0, 1);
	if (getEam() != DS1307_A0_WDAY || (getSecTimer() % 512) < 200) {
		ks0066WriteData (0x04);
	} else {
		ks0066WriteData (0x03);
	}
	for (i = 0; i < 7; i++) {
		if (getAlarm(DS1307_A0_WDAY) & (0x40 >> i)) {
			ks0066WriteData (0x01);
		} else {
			ks0066WriteData (0x00);
		}
		if (i != 6)
			ks0066WriteData (0x02);
	}
	if (getEam() != DS1307_A0_WDAY || (getSecTimer() % 512) < 200) {
		ks0066WriteData (0x06);
	} else {
		ks0066WriteData (0x05);
	}
#else
	uint8_t i;
	uint8_t *label;

	gdSetXY(4, 0);

	drawAm(DS1307_A0_HOUR, font_digits_32);
	writeStringPgm(STR_SPCOLSP);
	drawAm(DS1307_A0_MIN, font_digits_32);

	/* Draw input icon selection rectangle */
	if (getEam() == DS1307_A0_INPUT) {
		gdDrawRect(96, 0, 32, 32, 1);
		gdDrawRect(97, 1, 30, 30, 1);
	} else {
		gdDrawRect(96, 0, 32, 32, 0);
		gdDrawRect(97, 1, 30, 30, 0);
	}

	gdSetXY(100, 4);
	/* Check that input number less than CHAN_CNT */
	i = getAlarm(DS1307_A0_INPUT);
	if (i >= sndInputCnt())
		i = 0;
	gdWriteIcon24(sndParAddr(MODE_SND_GAIN0 + i)->icon);

	/* Draw weekdays selection rectangle */
	if (getEam() == DS1307_A0_WDAY) {
		gdDrawRect(0, 34, 128, 30, 1);
		gdDrawRect(1, 35, 126, 28, 1);
	} else {
		gdDrawRect(0, 34, 128, 30, 0);
		gdDrawRect(1, 35, 126, 28, 0);
	}

	/* Draw weekdays */
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
	label = txtLabels[LABEL_WEEKDAYS];
	for (i = 0; i < 7; i++) {
		gdSetXY(5 + 18 * i, 38);
		gdWriteChar(eeprom_read_byte(&label[i * 2]));
		gdWriteChar(0x7F);
		gdWriteChar(eeprom_read_byte(&label[i * 2 + 1]));

		gdDrawRect(3 + 18 * i, 47, 14, 14, 1);
		if (getAlarm(DS1307_A0_WDAY) & (0x40 >> i))
			gdDrawFilledRect(5 + 18 * i, 49, 10, 10, 1);
		else
			gdDrawFilledRect(5 + 18 * i, 49, 10, 10, 0);
	}
#endif
	return;
}

void showTimer(int16_t timer)
{

#ifdef KS0066
	ks0066SetXY(0, 0);
	writeStringEeprom(txtLabels[LABEL_TIMER]);

	ks0066SetXY(8, 0);

	if (timer >= 0) {
		writeNum(timer / 3600, 2, '0', 10);
		writeString(":");
		writeNum(timer / 60 % 60, 2, '0', 10);
		writeString(":");
		writeNum(timer % 60, 2, '0', 10);
	} else {
		writeString("--");
		writeString(":");
		writeString("--");
		writeString(":");
		writeString("--");
	}
	drawMiniSpectrum();
#else
	uint8_t x, xbase;
	uint8_t ybase;

	gdSetXY(4, 0);

	gdLoadFont(font_digits_32, 1, FONT_DIR_0);
	if (timer >= 0) {
		writeNum(timer / 3600, 2, '0', 10);
		writeStringPgm(STR_SPCOLSP);
		writeNum(timer / 60 % 60, 2, '0', 10);
		writeStringPgm(STR_SPCOLSP);
		writeNum(timer % 60, 2, '0', 10);
	} else {
		writeStringPgm(STR_MINUS2);
		writeStringPgm(STR_SPCOLSP);
		writeStringPgm(STR_MINUS2);
		writeStringPgm(STR_SPCOLSP);
		writeStringPgm(STR_MINUS2);
	}
	gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);

	getSpData(fallSpeed);

	for (x = 0; x < GD_SIZE_X / 4; x++) {
		xbase = x << 2;

		ybase = (buf[x] + buf[x + 32]) / 2;
		drawSpCol(xbase, 3, 63, ybase, 31);
	}
#endif

	return;
}

void switchSpMode(void)
{
	if (++spMode >= SP_MODE_END)
		spMode = SP_MODE_METER;

	return;
}

void switchFallSpeed(void)
{
	fallSpeed++;
	if (fallSpeed > FALL_SPEED_FAST)
		fallSpeed = FALL_SPEED_LOW;

	return;
}

void showSpectrum(void)
{
	getSpData(fallSpeed);
	uint16_t left, right;

#ifdef KS0066
	uint8_t i, data;

	switch (spMode) {
	case SP_MODE_STEREO:
		lcdGenLevels();
		ks0066SetXY(0, 0);
		for (i = 0; i < KS0066_SCREEN_WIDTH; i++) {
			data = buf[i] >> 2;
			if (data >= 7)
				data = 0xFF;
			ks0066WriteData(data);
		}
		ks0066SetXY(0, 1);
		for (i = 0; i < KS0066_SCREEN_WIDTH; i++) {
			data = buf[FFT_SIZE / 2 + i] >> 2;
			if (data >= 7)
				data = 0xFF;
			ks0066WriteData(data);
		}
		break;
	case SP_MODE_MIXED:
		lcdGenLevels();
		for (i = 0; i < KS0066_SCREEN_WIDTH; i++) {
			data = buf[i];
			data += buf[FFT_SIZE / 2 + i];
			data >>= 2;
			ks0066SetXY(i, 0);
			if (data < 8)
				ks0066WriteData(' ');
			else if (data < 15)
				ks0066WriteData(data - 8);
			else
				ks0066WriteData(0xFF);
			ks0066SetXY(i, 1);
			if (data < 7)
				ks0066WriteData(data);
			else
				ks0066WriteData(0xFF);
		}
		break;
	default:
		lcdGenBar(userAddSym);
		left = 0;
		right = 0;
		for (i = 0; i < FFT_SIZE / 2; i++) {
			left += buf[i];
			right += buf[FFT_SIZE / 2 + i];
		}
		left >>= 4;
		right >>= 4;

		ks0066SetXY(0, 0);
		ks0066WriteData(eeprom_read_byte(txtLabels[LABEL_LEFT_CHANNEL]));
		for (i = 0; i < KS0066_SCREEN_WIDTH - 1; i++) {
			if (left / 3 > i) {
				ks0066WriteData(0x03);
			} else {
				if (left / 3 < i) {
					ks0066WriteData(0x00);
				} else {
					ks0066WriteData(left % 3);
				}
			}
		}
		ks0066SetXY(0, 1);
		ks0066WriteData(eeprom_read_byte(txtLabels[LABEL_RIGHT_CHANNEL]));
		for (i = 0; i < KS0066_SCREEN_WIDTH - 1; i++) {
			if (right / 3 > i) {
				ks0066WriteData(0x03);
			} else {
				if (right / 3 < i) {
					ks0066WriteData(0x00);
				} else {
					ks0066WriteData(right % 3);
				}
			}
		}
		break;
	}
#else
	uint8_t x, xbase;
	uint8_t y, ybase;

	switch (spMode) {
	case SP_MODE_STEREO:
		for (x = 0; x < GD_SIZE_X / 4; x++) {
			xbase = x << 2;

			ybase = buf[x];
			drawSpCol(xbase, 3, 31, ybase, 31);

			ybase = buf[x + 32];
			drawSpCol(xbase, 3, 63, ybase, 31);
		}
		break;
	case SP_MODE_MIXED:
		for (x = 0; x < GD_SIZE_X / 4; x++) {
			xbase = x << 2;

			ybase = buf[x] + buf[x + 32];
			drawSpCol(xbase, 3, 63, ybase, 63);
		}
		break;
	case SP_MODE_STEREO_DOUBLE:
		for (x = 0; x < GD_SIZE_X / 6 + 1; x++) {
			xbase = x * 6;

			ybase = buf[x];
			drawSpCol(xbase, 2, 31, ybase, 31);
			ybase += buf[x + 1];
			ybase /= 2;
			drawSpCol(xbase + 3, 2, 31, ybase, 31);

			ybase = buf[x + 32];
			drawSpCol(xbase, 2, 63, ybase, 31);
			if (x < GD_SIZE_X / 6) {
				ybase += buf[x + 32 + 1];
				ybase /= 2;
			}
			drawSpCol(xbase + 3, 2, 63, ybase, 31);
		}
		break;
	case SP_MODE_MIXED_DOUBLE:
		for (x = 0; x < GD_SIZE_X / 6 + 1; x++) {
			xbase = x * 6;

			ybase = buf[x] + buf[x + 32];
			drawSpCol(xbase, 2, 63, ybase, 63);
			if (x < GD_SIZE_X / 6) {
				ybase += buf[x + 1] + buf[x + 32 + 1];
				ybase /= 2;
			}
			drawSpCol(xbase + 3, 2, 63, ybase, 63);
		}
		break;
	case SP_MODE_STEREO_THIN:
		for (x = 0; x < GD_SIZE_X / 4; x++) {
			xbase = x << 2;

			ybase = buf[x];
			drawSpCol(xbase, 1, 31, ybase, 31);
			if (x != GD_SIZE_X / 4 - 1) {
				ybase += buf[x + 1];
				ybase /= 2;
			}
			drawSpCol(xbase + 2, 1, 31, ybase, 31);

			ybase = buf[x + 32];
			drawSpCol(xbase, 1, 63, ybase, 31);
			if (x != GD_SIZE_X / 4 - 1) {
				ybase += buf[x + 32 + 1];
				ybase /= 2;
			}
			drawSpCol(xbase + 2, 1, 63, ybase, 31);
		}
		break;
	case SP_MODE_MIXED_THIN:
		for (x = 0; x < GD_SIZE_X / 4; x++) {
			xbase = x << 2;

			ybase = buf[x] + buf[x + 32];
			drawSpCol(xbase, 1, 63, ybase, 63);
			if (x != GD_SIZE_X / 4 - 1) {
				ybase += buf[x + 1] + buf[x + 32 + 1];
				ybase /= 2;
			}
			drawSpCol(xbase + 2, 1, 63, ybase, 63);
		}
		break;
	default:
		gdLoadFont(font_ks0066_ru_08, 1, FONT_DIR_0);
		gdSetXY(0, 0);
		writeStringEeprom(txtLabels[LABEL_LEFT_CHANNEL]);
		gdSetXY(0, 36);
		writeStringEeprom(txtLabels[LABEL_RIGHT_CHANNEL]);
		left = 0;
		right = 0;
		for (x = 0; x < GD_SIZE_X / 4; x++) {
			left += buf[x];
			right += buf[x + 32];
		}
		left >>= 4;
		right >>= 4;

		for (x = 0; x < 43; x++) {
			for (y = 12; y < 27; y++) {
				if (x < left || y == 19) {
					gdDrawPixel(3 * x + 0, y, 1);
					gdDrawPixel(3 * x + 1, y, 1);
				} else {
					gdDrawPixel(3 * x + 0, y, 0);
					gdDrawPixel(3 * x + 1, y, 0);
				}
			}
			for (y = 48; y < 63; y++) {
				if (x < right || y == 55) {
					gdDrawPixel(3 * x + 0, y, 1);
					gdDrawPixel(3 * x + 1, y, 1);
				} else {
					gdDrawPixel(3 * x + 0, y, 0);
					gdDrawPixel(3 * x + 1, y, 0);
				}
			}
		}
		break;
	}
#endif
	return;
}

void setWorkBrightness(void)
{
#ifdef KS0066
	ks0066SetBrightness(brWork);
#if defined(KS0066_WIRE_PCF8574)
	pcf8574SetBacklight(brWork);
#endif
#else
	gdSetBrightness(brWork);
#endif
	return;
}

void setStbyBrightness(void)
{
#ifdef KS0066
	ks0066SetBrightness(brStby);
#if defined(KS0066_WIRE_PCF8574)
	pcf8574SetBacklight(KS0066_BCKL_OFF);
#endif
#else
	gdSetBrightness(brStby);
#endif
	return;
}

void displayPowerOff(void)
{
	eeprom_update_byte((uint8_t*)EEPROM_BR_STBY, brStby);
	eeprom_update_byte((uint8_t*)EEPROM_BR_WORK, brWork);
	eeprom_update_byte((uint8_t*)EEPROM_SP_MODE, spMode);
	eeprom_update_byte((uint8_t*)EEPROM_DISPLAY, defDisplay);
	eeprom_update_byte((uint8_t*)EEPROM_FALL_SPEED, fallSpeed);

	return;
}
