#ifndef _ABC_H
#define _ABC_H

#include "types.h"

#include "ugsm.h"

bool UGSMGlyphIsLetter(UGSMGlyphCode glyphCode);

bool UGSMGlyphIsDigit(UGSMGlyphCode glyphCode);

bool UGSMGlyphIsLetterOrDigit(UGSMGlyphCode glyphCode);

UGSMGlyphCode UGSMGlyphToUpperCase(UGSMGlyphCode glyphCode);

UGSMGlyphCode UGSMGlyphToLowerCase(UGSMGlyphCode glyphCode);

#endif