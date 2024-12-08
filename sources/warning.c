#include "../headers/warning.h"

#include "../headers/types.h"

#include "../headers/std.h"

#include "../headers/time.h"

#include "../headers/pit.h"

#include "../headers/speaker.h"

#include "../headers/colors.h"

extern uint16 y;

extern uint32 foregroundColor;

int8* getWarningDescription(enum WarningsType warningType) {
	if (warningType == USBBabbleDetectedWarning) return "What? One more time. I don't understand what you're mumbling. \n\nMy friend taught me how to talk to my USB port, so now I have to mess with it for days without understanding it...\n\n";

	else if (warningType == AllGlyphsAreReservedWarning) return "Dude, everything is booked, what can we do, we'll come tomorrow :(\n\n";

	else if (warningType == RequiredNumberOfArgumentsNotReached) return "So wait, where's everything else?! ( The required number of arguments has not been reached )";

	return "There are no words. ( Unknown warning type )\n\n";
}

void warn(enum WarningsType warningType) {
	TimeInit();

	uint16 tempForegroundColor = foregroundColor;

	foregroundColor = 0xffff00;

	if (y) UGSMASCIIputChar('\n');

	UGSMASCIIputString(getWarningDescription(warningType));

	foregroundColor = tempForegroundColor;

	swap();

	for (uint8 i = 0; i < 4; i++) {
		makeSound(100);

		sleepMilliseconds(500);

		makeSound(500);

		sleepMilliseconds(500);
	}

	stopSound();
}