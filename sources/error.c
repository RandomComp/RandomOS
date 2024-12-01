#include "../headers/error.h"

#include "../headers/types.h"

#include "../headers/std.h"

#include "../headers/time.h"

#include "../headers/pit.h"

#include "../headers/speaker.h"

#include "../headers/ugsm.h"

#include "../headers/colors.h"

extern uint16 y;

extern uint32 foregroundColor;

int8* getErrorDescription(enum ErrorsType errorType) {
	if (errorType == VBENotSupportedError) return "Your video card does not support VBE 2.0 ( go fuck yourself, it's not our fault here :) )\n";

	else if (errorType == VBESetupError) return "And now we've fucked up, I'm sorry, but you know, fuck you!\n\n";

	else if (errorType == USBTDBitstuffError) return "You broke the USB again, HOW MUCH CAN you DO? I'VE ALREADY STARTED PAYING FOR IT.\n\n";

	else if (errorType == USBTDTimeoutError) return "The free period of your USB is over, will you pay for the subscription?\n\n";

	else if (errorType == USBTDBabbleError) return "Why did you stick gum to my USB?!\n\n";

	else if (errorType == USBTDDataBufferError) return "What are you doing?! have you lost the USB information?! COME HERE, YOU BASTARD!!!\n\n";
	
	else if (errorType == USBTDStalledError) return "Well, it's all over for us, the USB CAUGHT FIRE!!!\n\n";

	else if (errorType == USBTDNAKError) return "Wait, what did you do? How did you teach my USB to speak?!\n\n";

	else if (errorType == USBAllocQHError) return "Why did you BREAK MY USB?!?!?!\n\n";

	else if (errorType == USBAllocTDError) return "I don't know what funny mistakes to make anymore, I'm sorry. :( in short -- USB 1.1 allocation TD error\n\n";

	else if (errorType == USBMissedMicroFrameError) return "FUCK";

	else if (errorType == USBTransactionError) return "A forbidden joke, in short USBTransactionError!\n\n";

	else if (errorType == USBUnexpectedEHCIQHSize) return "This is a big USB device! Not big like this message :)\n\n";

	else if (errorType == GlyphNotReservedError) return "What? What other crab is not reserved?\n\n";

	else if (errorType == GlyphReservedError) return "Everything seems to be OK, or not...\n\n";

	else if (errorType == UGSMIncorrectGlyphSetSizeError) return "Why are there so many of your crabs?! Now it's my turn :)\n\n";

	else if (errorType == UGSMGlyphNotReservedButWeTryUse) return "Hey, there's nothing there!\n\n";

	else if (errorType == ACPIS5ParseError) return "Why parse samsung s5? Really dumb?\n\n";

	else if (errorType == ACPIS5NotPresentError) return "What kind of junk is this?\n\nWhat did you give me?\n\nNo matter what, I'll throw it away better than I'll use it.\n\n";

	else if (errorType == ACPIDSDTError) return "What?\n\nWhat kind of obscenity is this?\n\n";

	else if (errorType == ACPINoValidFACPPresentError) return "What's it? We don't have a hospital here, we have an operating system here! Get out!\n\n";

	else if (errorType == ACPINoACPIError) return "Wait, WHAT?!\n\n";

	else if (errorType == ACPICannotBeEnabledError) return "No way, I hid the power button with tape :)\n\n";

	else if (errorType == ACPINoKnownWayToEnableACPIError) return "What, how the fuck do I turn this on?\n\n";

	else if (errorType == MemoryLackError) return "Is memory a woman?!\n\nWhy does memory need nail polish, and where is her husband, for that matter?\n\n";

	return "There are no words.\n\n";
}

void cause(enum ErrorsType errorType) {
	TimeInit();

	uint16 tempForegroundColor = foregroundColor;

	foregroundColor = 0xff0000;

	if (y) UGSMASCIIputChar('\n');

	UGSMASCIIputString(getErrorDescription(errorType));

	foregroundColor = tempForegroundColor;

	swap();

	for (uint16 i = 0; i < 65535; i++) {
		uint16 temp = (i % 100) * 10;

		makeSound((temp < 500 ? 500 - temp : temp - 500) + 1000);

		sleepMilliseconds(10);
	}

	for (;;);
}