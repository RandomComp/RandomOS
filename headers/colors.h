#ifndef _COLORS_H
#define _COLORS_H

#include "types.h"

#define TEXTBLACK 0

#define TEXTBLUE 0x00001f

#define TEXTGREEN 0x003f00

#define TEXTAQUA 0x3

#define TEXTRED 0x4

#define TEXTPURPLE 0x5

#define TEXTYELLOW 0x6

#define TEXTWHITE 0x7

#define TEXTGRAY 0x8

#define TEXTLIGHTBLUE 0x9

#define TEXTLIGHTGREEN 0xa

#define TEXTLIGHTAQUA 0xb

#define TEXTLIGHTRED 0xc

#define TEXTLIGHTPURPLE 0xd

#define TEXTLIGHTYELLOW 0xe

#define TEXTBRIGHTWHITE 0xf

uint16 emulateColor24Bit(uint32 color);

uint16 colorRGBToBGR(uint16 color);

#endif