#ifndef _ERROR_H
#define _ERROR_H

#include "../headers/types.h"

enum ErrorsType {
	VBENotSupportedError,

	VBESetupError,

	USBTDBitstuffError,

	USBTDTimeoutError,

	USBTDBabbleError,

	USBTDDataBufferError,

	USBTDStalledError,

	USBTDNAKError,

	USBAllocQHError,

	USBAllocTDError,

	USBMissedMicroFrameError,

	USBTransactionError,

	USBUnexpectedEHCIQHSize,

	GlyphNotReservedError,

	GlyphReservedError,

	UGSMIncorrectGlyphSetSizeError,

	UGSMGlyphNotReservedButWeTryUse,

	X16BitEmulatorError,

	ACPIS5ParseError,

	ACPIS5NotPresentError,

	ACPIDSDTError,

	ACPINoValidFACPPresentError,

	ACPINoACPIError,

	ACPICannotBeEnabledError,

	ACPINoKnownWayToEnableACPIError,

	MemoryLackError
};

int8* getErrorDescription(enum ErrorsType errorType);

void cause(enum ErrorsType errorType);

#endif