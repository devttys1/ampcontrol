#ifndef AUDIO_H
#define AUDIO_H

#include <inttypes.h>

typedef struct {
	const int8_t min;
	const int8_t max;
	const int8_t step;
} sndGrid;

typedef struct {
	int8_t value;
	const sndGrid *grid;
	uint8_t *label;
	uint8_t icon;
	void (*set)(void);
} sndParam;

typedef enum {
	MODE_SND_VOLUME = 0,
	MODE_SND_BASS,
	MODE_SND_MIDDLE,
	MODE_SND_TREBLE,
	MODE_SND_PREAMP,
	MODE_SND_FRONTREAR,
	MODE_SND_BALANCE,
	MODE_SND_CENTER,
	MODE_SND_SUBWOOFER,
	MODE_SND_GAIN0,
	MODE_SND_GAIN1,
	MODE_SND_GAIN2,
	MODE_SND_GAIN3,
	MODE_SND_GAIN4,

	MODE_SND_END
} sndMode;

extern sndParam sndPar[MODE_SND_END];

#endif /* AUDIO_H */
