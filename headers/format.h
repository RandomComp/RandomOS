#ifndef _BCD_H
#define _BCD_H

#include "../headers/types.h"

int8 toBCDX8(int8 num);

int16 toBCDX16(int16 num);

Hour hourToBCD(Hour hour);

int8 fromBCDX8(int8 num);

int16 fromBCDX16(int16 num);

Hour hourFromBCD(Hour hour);

#endif