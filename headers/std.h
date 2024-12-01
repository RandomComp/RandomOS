#ifndef _STD_H
#define _STD_H

#include "types.h"

#include "glyph.h"

#include "ugsm.h"

#define width 800

#define height 600

void STDInit(void);

bool isFormatSymbol(UGSMGlyphCode glyphCode);

uint32 getStringLength(UGSMGlyphCode str[6]);

void setConsoleCursorPosition(uint8 x, uint8 y);

void hideConsoleCursor(void);

void setString(uint16 x, uint16 y, UGSMGlyphCode str[6]);

void putString(UGSMGlyphCode str[384]);

void setUX64Integer(uint16 x, uint16 y, uint64 num);

void putUX64Integer(uint64 num);

void setX64Integer(uint16 x, uint16 y, int64 num);

void putX64Integer(int64 num);

void setUX32Integer(uint16 x, uint16 y, uint32 num);

void putUX32Integer(uint32 num);

void setX32Integer(uint16 x, uint16 y, int32 num);

void putX32Integer(int32 num);

void setUX16Integer(uint16 x, uint16 y, uint16 num);

void putUX16Integer(uint16 num);

void setX16Integer(uint16 x, uint16 y, int16 num);

void putX16Integer(int16 num);

void setUX8Integer(uint16 x, uint16 y, uint8 num);

void putUX8Integer(uint8 num);

void setX8Integer(uint16 x, uint16 y, int8 num);

void putX8Integer(int8 num);

/*void setFloat(uint16 x, uint16 y, float num);

void putFloat(float num);

void setDouble(uint16 x, uint16 y, double num);

void putDouble(double num);*/

void setChar(uint16 x, uint16 y, UGSMGlyphCode glyphCode);

void putChar(UGSMGlyphCode glyphCode);

void clear(uint32 backgroundColor);

void swap(void);

#endif