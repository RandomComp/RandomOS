#include "../headers/ugsm.h"

#include "../headers/std.h"

#include "../headers/types.h"

#include "../headers/error.h"

#include "../headers/glyph.h"

#include "../headers/ascii.h"

#include "../headers/rus.h"

extern UGSMGlyphSetCode ASCIIOffset;

extern UGSMGlyphSetCode RUSOffset;

extern const uint32 columns;

extern int16 x;

extern int16 y;

IUGSM UGSM = { 0 }; // UGSM itself ( единая память для хранения глифов ) ( unified glyph storage memory )

IUGSMSize UGSMSize = 0;

UTF8GlyphCode UTF8RUSCodes[66] = {
	0xb0d1, 0xb1d0, 0xb2d0, 0xb3d0,

	0xb4d0, 0xb5d0, 0x91d1, 0xb6d0,

	0xb7d0, 0xb8d0, 0xb9d0, 0xbad0,

	0xbbd0, 0xbcd0, 0xbdd0, 0xbed0,

	0xbfd0, 0x80d1, 0x81d1, 0x82d1,

	0x83d1, 0x84d1, 0x85d1, 0x86d1,

	0x87d1, 0x88d1, 0x89d1, 0x8ad1,

	0x8bd1, 0x8cd1, 0x8dd1, 0x8ed1,

	0x8fd1,

	0x90d0, 0x91d0, 0x92d0, 0x93d0,

	0x94d0, 0x95d0, 0x81d0, 0x96d0, 

	0x97d0, 0x98d0, 0x99d0, 0x9ad0,

	0x9bd0, 0x9cd0, 0x9dd0, 0x9ed0,

	0x9fd0, 0xa0d0, 0xa1d0, 0xa2d0,

	0xa3d0, 0xa4d0, 0xa5d0, 0xa6d0,

	0xa7d0, 0xa8d0, 0xa9d0, 0xaad0,

	0xabd0, 0xacd0, 0xadd0, 0xaed0,

	0xafd0
};

UGSMGlyphSetCode UGSMloadGlyphSet(UGSMGlyphSet glyphSet, UGSMGlyphSetSize length) {
	if (length == 1) {
		UGSMloadGlyph(glyphSet[0]);
	}

	UGSMGlyphSetCode glyphSetCode = getFreeSpaceForGlyph();

	UGSMGlyphSetSize glyphSetEnd = glyphSetCode + length;

	if (glyphSetEnd >= UGSMMaxSize)
		cause(UGSMIncorrectGlyphSetSizeError);

	for (UGSMGlyphCode i = 0; i < length; i++) {
		generateGlyphCode();

		for (uint8 j = 0; j < UGSMGlyphHeight; j++) {
			UGSM[i + glyphSetCode][j] = glyphSet[i][j];
		}
	}

	UGSMSize += length;

	return glyphSetCode;
}

UGSMGlyphCode UGSMloadGlyph(UGSMGlyph glyph) {
	UGSMGlyphCode result = generateGlyphCode();

	for (uint8 i = 0; i < UGSMGlyphHeight; i++) {
		UGSM[result][i] = glyph[i];
	}

	UGSMSize++;

	return result;
}

void UGSMcheckGlyphCode(UGSMGlyphCode glyphCode) {
	if (!checkGlyphCodeIsReserved(glyphCode))
		cause(UGSMGlyphNotReservedButWeTryUse);
}

UGSMGlyph* UGSMgetGlyph(UGSMGlyphCode glyphCode) {
	UGSMcheckGlyphCode(glyphCode);

	return &UGSM[glyphCode];
}

void UGSMASCIIputString(uint8* str) {
	for (uint16 i = 0; str[i]; i++)
		UGSMASCIIputChar(str[i]);
}

void UGSMASCIIputChar(uint8 c) {
	if (c == 32) {
		putChar(1);
	}

	else if (c == 10) {
		putChar(2);
	}

	else if (c == 13) {
		putChar(3);
	}

	else if (c == 9) {
		putChar(4);
	}

	else if (c >= 33 && c <= 126) {
		putChar((UGSMGlyphCode)(c - 33) + ASCIIOffset + 5);
	}
}

void UGSMRUSputString(uint16* str) {
	for (uint16 i = 0; str[i]; i++) {
		UGSMRUSputChar(str[i]);
	}
}

void UGSMRUSputChar(uint16 c) {
	bool bFinded = false;

	uint8 chi = (c >> 8) & 0xff;

	uint8 clo = c & 0xff;

	for (UGSMGlyphCode i = 0; i < 66; i++) {
		if (UTF8RUSCodes[i] == c) {
			putChar(i + RUSOffset);

			bFinded = true;

			break;
		}
	}

	if (!bFinded) { // if we not finded russian letter, we printing ascii char
		putUX16Integer(chi);

		putChar(0);

		putUX16Integer(clo);

		putChar(0);
	}
}

void UGSMASCIIsetString(uint16 x, uint16 y, int8* str) {
	for (uint16 i = 0; str[i]; i++)
		UGSMASCIIsetChar(x + i, y, str[i]);
}

void UGSMASCIIsetChar(uint16 x, uint16 y, int8 c) {
	if (c >= 32 && c <= 126) {
		setChar(x, y, (UGSMGlyphCode)(c - 32) + ASCIIOffset + 3);
	}
}

void UGSMRUSsetString(uint16 x, uint16 y, uint16* str) {
	for (uint16 i = 0; str[i]; i++) {
		UGSMRUSsetChar(x + i, y, str[i]);
	}
}

void UGSMRUSsetChar(uint16 x, uint16 y, uint16 c) {
	bool bFinded = false;

	uint8 chi = (c >> 8) & 0xff;

	uint8 clo = c & 0xff;

	for (UGSMGlyphCode i = 0; i < 66; i++) {
		if (UTF8RUSCodes[i] == c) {
			setChar(x, y, i + RUSOffset);

			bFinded = true;

			break;
		}

		//else if (chi == 209 || chi == 208)
	}

	if (!bFinded) { // if we not finded russian letter, we printing ascii char
		UGSMASCIIsetChar(x, y, clo);

		UGSMASCIIsetChar(x, y, chi);
	}
}