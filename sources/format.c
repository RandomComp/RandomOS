#include "../headers/types.h"

#include "../headers/time.h"

extern RegisterB registerB;

int8 toBCDX8(int8 num) {
	int8 result = 0;

	for (uint16 i = 0; num != 0; num /= 10) {
		result += (num % 10) << (i * 4);

		i++;
	}

	return result;
}

int16 toBCDX16(int16 num) {
	int16 result = 0;

	for (int16 i = 0; num != 0; num /= 10) {
		result += (num % 10) << (i * 4);

		i++;
	}

	return result;
}

Hour hourToBCD(Hour hour) {
	Hour result = hour;

	if (!(registerB & 0x02) && hour >= 12) {
		result %= 12;
		
		result |= 0x80;
	}

	return (result % 10) | ((result / 10) << 4);
}

int8 fromBCDX8(int8 num) {
	return (num & 0x0F) + ((num / 16) * 10);
}

int16 fromBCDX16(int16 num) {
	return (num & 0x0F) + ((num / 16) * 10);
}

Hour hourFromBCD(Hour hour) {
	Hour result = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);

	if (!(registerB & 0x02) && (result & 0x80))
		result = ((result & 0x7F) + 12) % 24;

	return result;
}