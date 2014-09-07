#!/bin/sh

make clean

for AUDIOPROC in TDA7313 TDA7318 TDA7439
do
	for DISPLAY in ST7920
	do
		for TUNER in LM7001 NOTUNER RDA5807 TEA5767 TUX032
		do
			make AUDIOPROC=${AUDIOPROC} DISPLAY=${DISPLAY} TUNER=${TUNER}
			make AUDIOPROC=${AUDIOPROC} DISPLAY=${DISPLAY} TUNER=${TUNER} clean_var
		done
	done
done

make clean
