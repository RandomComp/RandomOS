#include "../headers/fatal_error.h"

#include "../headers/types.h"

#include "../headers/std.h"

#include "../headers/ugsm.h"

#include "../headers/colors.h"

extern uint16 y;

extern uint32 foregroundColor;

int8* getFatalErrorDescription(enum FatalErrorsType errorType) {
	if (errorType == 0) return "Where are we? in debugging.\n\n";

	else if (errorType == 1) return "You has divided any number by zero, you get infinity and error to gift :).\n\n";

	else if (errorType == 2) return "Hello non maskable interrupt, i've been looking for you!\n\n";

	else if (errorType == 3) return "Let's break everything, we're rich!\n\n";

	else if (errorType == 4) return "Look what you've done, you've poured it all over...\n\n";

	else if (errorType == 5) return "Where are you going?! My memory!\n\n";

	else if (errorType == 6) return "Why did you break my brain?\n\n";

	else if (errorType == 7) return "Only for the smart, dsjidjsdisidsudi, the dumb won't understand.\n\n";

	else if (errorType == 8) return "Monsieur, it seems to me that you and I have split up!\n\n";

	else if (errorType == 9) return "You write incorrectly instead of \"CoprocessorSegmentOverflowError\" write \"StackOverflow\", give birth to idiots...\n\n";

	else if (errorType == 10) return "Your program is extremely nervous, please calm down your brainchild.\n\n";

	else if (errorType == 11) return "Nerves are not a gift, but they are needed more than ever for a person (and preferably for programs).\n\n";

	else if (errorType == 12) return "The same mistake, STOP WRITING WITHOUT \"OVERFLOW\"!!!\n\n";

	else if (errorType == 13) return "Stop, it's the police, why are you breaking the law?\n\n";

	else if (errorType == 14) return "Wait, where are you flipping through, you passed by our page!\n\n";

	else if (errorType == 15) return "Wait, where are you rushing to, this is not our number.\n\n";

	else if (errorType == 16) return "Stop being smart, I can't do that, I'm having a glitch!\n\n";

	else if (errorType == 17) return "Straighten up! We have an important matter!\n\n";

	else if (errorType == 18) return "Careful... AHHH, IT HURTS!\n\n";

	else if (errorType == 19) return "Go away, this place is reserved!\n\n";

	return "There are no words.\n\n";
}

void causeFatal(enum FatalErrorsType errorType) {
	uint16 tempForegroundColor = foregroundColor;

	foregroundColor = 0xff0000;

	if (y) UGSMASCIIputChar('\n');

	UGSMASCIIputString(getFatalErrorDescription(errorType));

	foregroundColor = tempForegroundColor;

	swap();

	for (;;);
}