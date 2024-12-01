#ifndef _UGSM_H
#define _UGSM_H

#include "../headers/types.h"

#include "../headers/glyph.h"

#define UGSMMaxSize 1024

#define UGSMMaxLoadableSize 128

#define UGSMGlyphWidth 8

#define UGSMGlyphHeight 16

typedef uint8 UGSMGlyph[UGSMGlyphHeight]; // width, height - 8, 16

typedef uint8 UGSMGlyphSet[UGSMMaxLoadableSize][UGSMGlyphHeight];

typedef uint8 IUGSM[UGSMMaxSize][UGSMGlyphHeight];

typedef uint16 IUGSMSize;

typedef uint16 UGSMGlyphSetSize;

typedef uint16 UGSMGlyphSetCode;

typedef uint16 UTF8GlyphCode;

UGSMGlyphSetCode UGSMloadGlyphSet(UGSMGlyphSet glyphSet, UGSMGlyphSetSize length);

UGSMGlyphCode UGSMloadGlyph(UGSMGlyph glyph);

void UGSMcheckGlyphCode(UGSMGlyphCode glyphCode);

UGSMGlyph* UGSMgetGlyph(UGSMGlyphCode glyphCode);

void UGSMASCIIputString(uint8* str);

void UGSMASCIIputChar(uint8 c);

void UGSMRUSputString(uint16* str);

void UGSMRUSputChar(uint16 c);

void UGSMASCIIsetString(uint16 x, uint16 y, int8* str);

void UGSMASCIIsetChar(uint16 x, uint16 y, int8 c);

void UGSMRUSsetString(uint16 x, uint16 y, uint16* str);

void UGSMRUSsetChar(uint16 x, uint16 y, uint16 c);

#endif