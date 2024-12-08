#ifndef _ERROR_H
#define _ERROR_H

#include "../headers/types.h"

enum FatalErrorsType {
	DebugError = 0,

	DivisionByZeroError = 1,

	NonMaskableInterruptError = 2,

	BreakPoint = 3,

	StackOverflowError = 4,

	IndexOverpowered = 5,

	InvalidInstruction = 6,

	NoCoprocessorError = 7,

	DoubleFaultError = 8,

	CoprocessorSegmentOverflowError = 9,

	BadTSSError = 10,

	SegmentTSNotFoundError = 11,

	StackError = 12,

	ProtectionError = 13,

	PageError = 14,

	UnknownInterruptError = 15,

	CoprocessorError = 16,

	AlignmentError = 17,

	MachineCheckError = 18,

	ReservedError = 19
};

int8* getFatalErrorDescription(enum FatalErrorsType errorType);

void causeFatal(enum FatalErrorsType errorType);

#endif