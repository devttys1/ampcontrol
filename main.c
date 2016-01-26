#include <avr/interrupt.h>
#include <avr/eeprom.h>

#include "eeprom.h"
#include "adc.h"
#include "input.h"
#include "remote.h"
#include "i2c.h"
#include "display.h"
//#include "tuner/tuner.h"
//#include "temp.h"
#include "actions.h"

static uint8_t extFunc;

/* Hardware initialization */
static void hwInit(void)
{
	extFunc = eeprom_read_byte((uint8_t*)EEPROM_EXT_FUNC);
	#if 0
	loadTempParams();
	if (extFunc == USE_DS18B20) {
		ds18x20SearchDevices();
		tempInit();							/* Init temperature control */
	}
	#endif
	I2CInit();								/* I2C bus */
	displayInit();							/* Load params and text labels before fb scan started */
	rcInit();								/* IR Remote control */
	inputInit();							/* Buttons/encoder polling */
	adcInit();								/* Analog-to-digital converter */

	sei();									/* Gloabl interrupt enable */

	//tunerInit(extFunc);						/* Tuner */

	DDR(STMU_STBY) |= STMU_STBY_LINE;		/* Standby port */
	DDR(STMU_MUTE) |= STMU_MUTE_LINE;		/* Mute port */
	sndInit(extFunc);						/* Load labels/icons/etc */

	setStbyTimer(0);

	return;
}

int main(void)
{
	int8_t encCnt = 0;
	uint8_t action = ACTION_NOACTION;

	/* Init hardware */
	hwInit();
	#if 0
	if (extFunc == USE_DS18B20) {
		ds18x20ConvertTemp();
		setSensTimer(TEMP_MEASURE_TIME);
	}
	#endif
	while (1) {
		#if 0
		/* Control temperature */
		if (extFunc == USE_DS18B20) {
			if (getSensTimer() == 0) {
				ds18x20GetAllTemps();
				ds18x20ConvertTemp();
				setSensTimer(SENSOR_POLL_INTERVAL);
			}
			tempControlProcess();
		}
		#endif
		/* Emulate poweroff if any of timers expired */
		if (getStbyTimer() == 0 || getSilenceTimer() == 0)
			action = CMD_RC_STBY;

		/* Check alarm and update time */
		if (action == ACTION_NOACTION)
			action = checkAlarmAndTime();

		/* Convert input command to action */
		if (action == ACTION_NOACTION)
			action = getAction();

		/* Handle action */
		handleAction(action);

		/* Handle encoder */
		encCnt = getEncoder();				/* Get value from encoder */
		if (action == CMD_RC_VOL_UP)		/* Emulate VOLUME_UP action as encoder action */
			encCnt++;
		if (action == CMD_RC_VOL_DOWN)	/* Emulate VOLUME_DOWN action as encoder action */
			encCnt--;
		handleEncoder(encCnt);

		/* Reset silence timer on any action */
		if (encCnt || (action != ACTION_NOACTION && action != ACTION_EXIT_STANDBY))
			enableSilenceTimer();

		/* Reset handled action */
		action = ACTION_NOACTION;

		/* Check if we need exit to default mode*/
		handleExitDefaultMode();

		/* Switch to timer mode if it expires */
		handleTimers();

		/* Clear screen if mode has changed */
		handleModeChange();

		/* Show things */
		showScreen();
	}

	return 0;
}
