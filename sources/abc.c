#include "../headers/types.h"

#include "../headers/ugsm.h"

#include "../headers/ascii.h"

#include "../headers/rus.h"

extern UGSMGlyphSetCode ASCIIOffset;

extern UGSMGlyphSetCode RUSOffset;

bool UGSMGlyphIsLetter(UGSMGlyphCode glyphCode) {
	return  ((glyphCode >= (ASCIIOffset + 36)) && (glyphCode <= (ASCIIOffset + 61))) ||
			((glyphCode >= (ASCIIOffset + 68)) && (glyphCode <= (ASCIIOffset + 93)));
}

bool UGSMGlyphIsDigit(UGSMGlyphCode glyphCode) {
	return (glyphCode >= 19) && (glyphCode <= 28);
}

bool UGSMGlyphIsLetterOrDigit(UGSMGlyphCode glyphCode) {
	return UGSMGlyphIsLetter(glyphCode) || UGSMGlyphIsDigit(glyphCode);
}

UGSMGlyphCode UGSMGlyphToUpperCase(UGSMGlyphCode glyphCode) {
	return (((glyphCode >= (ASCIIOffset + 68)) && (glyphCode <= (ASCIIOffset + 93))) ? (glyphCode - 32) : glyphCode);
}

UGSMGlyphCode UGSMGlyphToLowerCase(UGSMGlyphCode glyphCode) {
	return (((glyphCode >= (ASCIIOffset + 36)) && (glyphCode <= (ASCIIOffset + 61))) ? (glyphCode + 32) : glyphCode);
}