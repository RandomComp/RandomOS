#include "../headers/glyph.h"

#include "../headers/types.h"

#include "../headers/std.h"

#include "../headers/warning.h"

#include "../headers/error.h"

uint8 reserved[128] = { 0 }; // bitset for check reservations UGSM ( единая память для хранения глифов ) ( unified glyph storage memory )

bool checkGlyphCodeIsReserved(UGSMGlyphCode glyphCode) {
	uint16 index = glyphCode / 8;

	uint16 bitIndex = glyphCode % 8;

	return reserved[index] & (1 << bitIndex);
}

void reserveGlyphCode(UGSMGlyphCode glyphCode) {
	uint16 index = glyphCode / 8;

	uint16 bitIndex = glyphCode % 8;

	if (reserved[index] & (1 << bitIndex)) // check for reserved
		cause(GlyphReservedError); // if the glyph is reserved, then we issue the corresponding error

	else reserved[index] |= 1 << bitIndex;
}

void freeGlyphCode(UGSMGlyphCode glyphCode) {
	uint16 index = glyphCode / 8;

	uint16 bitIndex = glyphCode % 8;

	if (reserved[index] & (1 << bitIndex)) // check for reserved
		reserved[index] ^= 1 << bitIndex;

	else cause(GlyphNotReservedError); // if the glyph is not reserved, then we issue the corresponding error
}

UGSMGlyphCode getFreeSpaceForGlyph(void) {
	uint16 i = 0;

	for (; reserved[i] >= 255; i++);

	if (i <= 128) {
		UGSMGlyphCode bitIndex = 0;

		for (; (reserved[i] & (1 << bitIndex)); bitIndex++);

		return i * 8 + bitIndex;
	}

	warn(AllGlyphsAreReservedWarning);

	return 0;
}

UGSMGlyphCode generateGlyphCode(void) {
	UGSMGlyphCode glyphCode = getFreeSpaceForGlyph();

	reserveGlyphCode(glyphCode);

	return glyphCode;
}