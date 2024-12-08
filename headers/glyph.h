#ifndef _GLYPH_H
#define _GLYPH_H

#include "../headers/types.h"

typedef uint16 UGSMGlyphCode;

bool checkGlyphCodeIsReserved(UGSMGlyphCode glyphCode);

void reserveGlyphCode(UGSMGlyphCode glyphCode);

void freeGlyphCode(UGSMGlyphCode glyphCode);

UGSMGlyphCode getFreeSpaceForGlyph(void);

UGSMGlyphCode generateGlyphCode(void);

#endif