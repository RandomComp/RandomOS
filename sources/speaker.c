#include "../headers/speaker.h"

#include "../headers/types.h"

#include "../headers/io.h"

bool bSoundIsPlaying = false;

void makeSound(uint32 sample) {
	if (bSoundIsPlaying) stopSound();

	uint32 freq = (sample * 60) / 255;

	bSoundIsPlaying = true;

	uint32 divisor = 1193180 / freq;
	
	out8(0x43, 0xb6);
	
	out8(0x42, (uint8)(divisor & 0xff));
	
	out8(0x42, (uint8)((divisor >> 8) & 0xff));
 
	uint8 temp = 0;

	in8(0x61, &temp);

	if (temp != (temp | 3)) {
		out8(0x61, temp | 3);
	}
}

void stopSound(void) {
	uint8 temp = 0;

	in8(0x61, &temp);

	out8(0x61, temp & 0xfc);

	bSoundIsPlaying = false;
}