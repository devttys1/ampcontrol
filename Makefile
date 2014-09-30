AUDIOPROC = TDA7313
DISPLAY = KS0066
TUNER = TEA5767

# Lowercase argument
lc = $(shell echo $1 | tr A-Z a-z)

# Fimware file base name
TARG = ampcontrol_m8_$(call lc,$(AUDIOPROC))_$(call lc,$(DISPLAY))_$(call lc,$(TUNER))

SPECT_SRC = fft.c adc.c
CTRL_SRC = input.c rc5.c

ifeq ($(AUDIOPROC), TDA7313)
  AUDIO_SRC = audio/tda7313.c
else ifeq ($(AUDIOPROC), TDA7318)
  AUDIO_SRC = audio/tda7318.c
else ifeq ($(AUDIOPROC), TDA7439)
  AUDIO_SRC = audio/tda7439.c
endif

ifeq ($(DISPLAY), KS0066)
  DISP_SRC = display.c display/ks0066.c
else ifeq ($(DISPLAY), PCF8574)
  DISP_SRC = display.c display/pcf8574.c
endif

ifeq ($(TUNER), TEA5767)
  TUNER_SRC = tuner.c tuner/tea5767.c
else ifeq ($(TUNER), TUX032)
  TUNER_SRC = tuner.c tuner/tux032.c
else ifeq ($(TUNER), LM7001)
  TUNER_SRC = tuner.c tuner/lm7001.c
else ifeq ($(TUNER), RDA5807)
  TUNER_SRC = tuner.c tuner/rda5807.c
endif

SRCS = main.c eeprom.c i2c.c ds1307.c $(SPECT_SRC) $(CTRL_SRC) $(AUDIO_SRC) $(DISP_SRC) $(TUNER_SRC)

MCU = atmega8
F_CPU = 8000000L

OPTIMIZE = -Os -mcall-prologues -fshort-enums
DEBUG = -g -Wall -Werror
CFLAGS = $(DEBUG) -lm $(OPTIMIZE) -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LDFLAGS = $(DEBUG) -mmcu=$(MCU)

CC = avr-gcc
OBJCOPY = avr-objcopy

AVRDUDE = avrdude
AD_MCU = -p $(MCU)
#AD_PROG = -c stk500v2
#AD_PORT = -P avrdoper

AD_CMDLINE = $(AD_MCU) $(AD_PROG) $(AD_PORT)

OBJS = $(SRCS:.c=.o)

OBJDIR = obj

all: $(TARG)

$(TARG): $(OBJS)
	$(CC) $(LDFLAGS) -o $(addprefix $(OBJDIR)/, $@.elf $(OBJS)) -lm
	mkdir -p flash
	$(OBJCOPY) -O ihex -R .eeprom -R .nwram $(addprefix $(OBJDIR)/, $@.elf) flash/$@.hex
	./size.sh $(addprefix $(OBJDIR)/, $@.elf)

%.o: %.c
	mkdir -p $(dir $(OBJDIR)/$@)
	$(CC) $(CFLAGS) -D$(AUDIOPROC) -D$(DISPLAY) -D$(TUNER) -c -o  $(OBJDIR)/$@ $<

clean:
	rm -rf $(OBJDIR)

mrproper: clean
	rm -rf flash

flash: $(TARG)
	$(AVRDUDE) $(AD_CMDLINE) -V -B 1.1 -U flash:w:flash/$(TARG).hex:i

fuse:
	$(AVRDUDE) $(AD_CMDLINE) -U lfuse:w:0xf8:m -U hfuse:w:0xd1:m

eeprom_en:
	$(AVRDUDE) $(AD_CMDLINE) -V -B 1.1 -U eeprom:w:eeprom/$(EEPROM)_en.bin:r

eeprom_ru:
	$(AVRDUDE) $(AD_CMDLINE) -V -B 1.1 -U eeprom:w:eeprom/$(EEPROM)_ru.bin:r

eeprom_by:
	$(AVRDUDE) $(AD_CMDLINE) -V -B 1.1 -U eeprom:w:eeprom/eeprom_by.bin:r

eeprom_ua:
	$(AVRDUDE) $(AD_CMDLINE) -V -B 1.1 -U eeprom:w:eeprom/eeprom_ua.bin:r
