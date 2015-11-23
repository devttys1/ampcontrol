#include "pga2310.h"

#include <avr/pgmspace.h>
#include "../pins.h"

static sndParam *sndPar;
static uint8_t input;

static void pga2310Strob(void)
{
	PORT(PGA2310_SCLK) |= PGA2310_SCLK_LINE;
	asm("nop");
	PORT(PGA2310_SCLK) &= ~PGA2310_SCLK_LINE;

	return;
}

static void pga2310SendByte(uint8_t data)
{
	int8_t i;

	for (i = 7; i >= 0; i--) {
		if (data & (1<<i))
			PORT(PGA2310_SDI) |= PGA2310_SDI_LINE;
		else
			PORT(PGA2310_SDI) &= ~PGA2310_SDI_LINE;
		pga2310Strob();
	}

	return;
}

void pga2310SendGainLevels(uint8_t right, uint8_t left)
{
	PORT(PGA2310_CS) &= ~PGA2310_CS_LINE;
	pga2310SendByte(right << 1);
	pga2310SendByte(left << 1);
	PORT(PGA2310_CS) |= PGA2310_CS_LINE;

	return;
}

void pga2310Init(sndParam *sp)
{
	sndPar = sp;

	DDR(PGA2310_SDI) |= PGA2310_SDI_LINE;
	DDR(PGA2310_SCLK) |= PGA2310_SCLK_LINE;
	DDR(PGA2310_CS) |= PGA2310_CS_LINE;

	PORT(PGA2310_SDI) |= PGA2310_SDI_LINE;
	PORT(PGA2310_SCLK) &= ~PGA2310_SCLK_LINE;
	PORT(PGA2310_CS) |= PGA2310_CS_LINE;

	DDR(PGA2310_IN0) |= PGA2310_IN0_LINE;
	DDR(PGA2310_IN1) |= PGA2310_IN1_LINE;
	DDR(PGA2310_IN2) |= PGA2310_IN2_LINE;

	pga2310SetInput(PGA2310_IN_CNT);

	return;
}

void pga2310SetSpeakers(int8_t val)
{
	int8_t spLeft = sndPar[MODE_SND_VOLUME].value;
	int8_t spRight = sndPar[MODE_SND_VOLUME].value;
	int8_t volMin = pgm_read_byte(&sndPar[MODE_SND_VOLUME].grid->min);

	if (sndPar[MODE_SND_BALANCE].value > 0) {
		spLeft -= sndPar[MODE_SND_BALANCE].value;
		if (spLeft < volMin)
			spLeft = volMin;
	} else {
		spRight += sndPar[MODE_SND_BALANCE].value;
		if (spRight < volMin)
			spRight = volMin;
	}

	pga2310SendGainLevels(96 + spRight, 96 + spLeft);

	return;
}

void pga2310SetMute(uint8_t val)
{
	if (val == MUTE_ON) {
		pga2310SendGainLevels(PGA2310_MUTE, PGA2310_MUTE);
		pga2310SetInput(PGA2310_IN_CNT);
	} else {
		pga2310SetSpeakers(0);
		pga2310SetInput(input);
	}

	return;
}

void pga2310SetInput(uint8_t in)
{
	PORT(PGA2310_IN0) &= ~PGA2310_IN0_LINE;
	PORT(PGA2310_IN1) &= ~PGA2310_IN1_LINE;
	PORT(PGA2310_IN2) &= ~PGA2310_IN2_LINE;

	if (in == 0) {
		PORT(PGA2310_IN0) |= PGA2310_IN0_LINE;
		input = in;
	}
	if (in == 1) {
		PORT(PGA2310_IN1) |= PGA2310_IN1_LINE;
		input = in;
	}
	if (in == 2) {
		PORT(PGA2310_IN2) |= PGA2310_IN2_LINE;
		input = in;
	}

	return;
}
