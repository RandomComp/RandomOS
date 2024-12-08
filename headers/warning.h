#ifndef _WARNING_H
#define _WARNING_H

#include "../headers/types.h"

enum WarningsType {
	USBBabbleDetectedWarning = 0,

	AllGlyphsAreReservedWarning = 1,

	RequiredNumberOfArgumentsNotReached = 2
};

int8* getWarningDescription(enum WarningsType warningType);

void warn(enum WarningsType warningType);

#endif