#include "../../headers/RMAL/lexer.h"

#include "../../headers/types.h"

#include "../../headers/ugsm.h"

#include "../../headers/abc.h"

#include "../../headers/warning.h"

#include "../../headers/error.h"

UGSMGlyphCode RMALCode[384];

struct RMALToken RMALTokens[128];

uint16 RMALTokenPos = 0;

int16 RMALPos = 0, RMALPosx = 1, RMALPosy = 1, RMALLength = 0;

UGSMGlyphCode instructionsName[18][6] = {
	{ 81, 82, 83, 0, 0, 0 },

	{ 80, 82, 89, 0, 0, 0 },

	{ 83, 88, 86, 75, 0, 0 },

	{ 83, 82, 83, 0, 0, 0 },

	{ 68, 71, 71, 0, 0, 0 },

	{ 86, 88, 69, 0, 0, 0 },

	{ 80, 88, 79, 0, 0, 0 },

	{ 71, 76, 89, 0, 0, 0 },

	{ 76, 81, 70, 0, 0, 0 },

	{ 71, 72, 70, 0, 0, 0 },

	{ 76, 81, 91, 27, 0, 0 },

	{ 76, 81, 91, 20, 25, 0 },

	{ 76, 81, 91, 22, 21, 0 },

	{ 82, 88, 87, 91, 27, 0 },

	{ 82, 88, 87, 91, 20, 25 },

	{ 82, 88, 87, 91, 22, 21 },

	{ 77, 80, 83, 0, 0, 0 },

	{ 85, 72, 87, 0, 0, 0 }
};


UGSMGlyphCode registersName[8][3] = {
	{ 72, 68, 91 },

	{ 72, 70, 91 },

	{ 72, 71, 91 },

	{ 72, 69, 91 },

	{ 72, 86, 83 },

	{ 72, 69, 83 },

	{ 72, 86, 76 },

	{ 72, 71, 76 }
};

void RMALTokenize(UGSMGlyphCode argRMALCode[384]) {
	RMALTokenPos = 0;

	RMALPos = 0;

	RMALPosx = 1;

	RMALPosy = 1;

	RMALLength = 0;

	for (; argRMALCode[RMALLength]; RMALLength++)
		RMALCode[RMALLength] = argRMALCode[RMALLength];

	for (uint16 i = 0; i < 127; i++) {
		RMALTokens[i].type = INSTRUCTION;

		RMALTokens[i].instruction = NOP;
	}

	while (RMALPos < RMALLength) {
		UGSMGlyphCode current = RMALPeek(0);

		if (UGSMGlyphIsLetter(current)) RMALTokenizeWord();

		else if (UGSMGlyphIsDigit(current)) RMALTokenizeNumber();

		else if (current == 14 || current == 16) RMALTokenizeSignNumber();

		else if (current == 15) {
			RMALTokens[RMALTokenPos].type = RMALCOMMA;

			RMALTokenPos++;

			RMALNext();
		}

		else if (current == 62) {
			RMALTokens[RMALTokenPos].type = RMALLBRACKET;

			RMALTokenPos++;

			RMALNext();
		}

		else if (current == 64) {
			RMALTokens[RMALTokenPos].type = RMALRBRACKET;

			RMALTokenPos++;

			RMALNext();
		}

		else if (current == 29) {
			RMALTokens[RMALTokenPos].type = RMALCOLON;

			RMALTokenPos++;

			RMALNext();
		}

		else if (current == 30) {
			RMALTokenizeComment();
		}

		else RMALNext(); // We just ignore spaces or other nonsense
	}

	RMALTokens[RMALTokenPos].type = RMALEOF;

	RMALTokenPos++;
}

enum RMALInstructions findInstructionByName(UGSMGlyphCode name[6]) {
	uint16 i = 0, length = 0, checkLength = 0;

	for (; i < 18; i++) {
		checkLength = 0;

		for (length = 0; instructionsName[length]; length++) {
			if (length >= 6) break;
		}

		for (uint16 j = 0; j < 6; j++) {
			if (instructionsName[i][j] == name[j]) checkLength++;
		}

		if (checkLength == length) break;
	}

	return (checkLength == length) ? i : UNKNOWN_INSTRUCTION;
}

enum RMALRegisters findRegisterByName(UGSMGlyphCode name[6]) {
	uint16 i = 0, checkLength = 0;

	for (; i < 8; i++) {
		checkLength = 0;

		for (uint16 j = 0; j < 3; j++) {
			if (registersName[i][j] == name[j]) checkLength++;
		}

		if (checkLength == 3) break;
	}

	return (checkLength == 3) ? i : UNKNOWN_REGISTER;
}

bool nameIsInstructionName(UGSMGlyphCode name[6]) {
	uint16 i = 0, length = 0, checkLength = 0;

	for (; i < 18; i++) {
		checkLength = 0;

		for (length = 0; instructionsName[length]; length++) {
			if (length >= 6) break;
		}

		for (uint16 j = 0; j < 6; j++) {
			if (instructionsName[i][j] == name[j]) checkLength++;
		}

		if (checkLength == length) break;
	}

	return checkLength == length;
}

bool nameIsRegisterName(UGSMGlyphCode name[6]) {
	uint16 i = 0, checkLength = 0;

	for (; i < 8; i++) {
		checkLength = 0;

		for (uint16 j = 0; j < 3; j++) {
			if (registersName[i][j] == name[j]) checkLength++;
		}

		if (checkLength == 3) break;
	}

	return checkLength == 3;
}

void RMALTokenizeWord(void) {
	UGSMGlyphCode name[6] = { 0 };

	uint16 tempPos = 0;

	UGSMGlyphCode current = UGSMGlyphToLowerCase(RMALPeek(0));

	while (UGSMGlyphIsLetterOrDigit(current) && tempPos < 6 && RMALPos < RMALLength) {
		name[tempPos] = current;

		tempPos++;

		current = UGSMGlyphToLowerCase(RMALNext());
	}

	if (nameIsInstructionName(name)) {
		RMALTokens[RMALTokenPos].type = INSTRUCTION;

		RMALTokens[RMALTokenPos].instruction = findInstructionByName(name);
	}

	else if (nameIsRegisterName(name)) {
		RMALTokens[RMALTokenPos].type = REGISTER;

		RMALTokens[RMALTokenPos].reg = findRegisterByName(name);
	}

	else {
		RMALTokens[RMALTokenPos].type = LABELNAME;

		for (uint16 i = 0; i < 6; i++) {
			RMALTokens[RMALTokenPos].labelName[i] = name[i];
		}
	}

	RMALTokenPos++;
}

void RMALTokenizeSignNumber(void) {
	int32 number = 0;

	bool bMinus = RMALPeek(0) == 16;

	UGSMGlyphCode current = RMALNext();

	while (UGSMGlyphIsDigit(current)) {
		number *= 10;

		number += (int32)(current - 19);

		current = RMALNext();
	}

	if (bMinus) number = -number;

	RMALTokens[RMALTokenPos].type = RMALSIGNNUMBER;

	RMALTokens[RMALTokenPos].signNumber = number;

	RMALTokenPos++;
}

void RMALTokenizeNumber(void) {
	uint32 number = 0;

	UGSMGlyphCode current = RMALPeek(0);

	while (UGSMGlyphIsDigit(current)) {
		number *= 10;

		number += (uint32)(current - 19);

		current = RMALNext();
	}

	RMALTokens[RMALTokenPos].type = RMALNUMBER;

	RMALTokens[RMALTokenPos].number = number;

	RMALTokenPos++;
}

void RMALTokenizeComment(void) {
	UGSMGlyphCode current = RMALNext();

	while (current != 30) {
		current = RMALNext();
	}

	RMALNext();
}

UGSMGlyphCode RMALNext(void) {
	RMALPos++;

	UGSMGlyphCode result = RMALPeek(0);

	if (result == 1) {
		RMALPosy++;

		RMALPosx = 1;
	}

	else {
		RMALPosx++;
	}

	return result;
}

UGSMGlyphCode RMALPeek(int16 relativePosition) {
	int16 position = RMALPos + relativePosition;

	if (position >= RMALLength) return 0;

	else if (position < 0) position = 0;

	return RMALCode[position];
}

void RMALTokensView(void) {
	for (uint16 i = 0; i < RMALTokenPos; i++) {
		struct RMALToken* token = &RMALTokens[i];

		if (token->type == INSTRUCTION) {
			if (token->instruction == NOP) {
				UGSMASCIIputString("NOP");
			}

			else if (token->instruction == MOV) {
				UGSMASCIIputString("MOV");
			}

			else if (token->instruction == PUSH) {
				UGSMASCIIputString("PUSH");
			}

			else if (token->instruction == POP) {
				UGSMASCIIputString("POP");
			}

			else if (token->instruction == ADD) {
				UGSMASCIIputString("ADD");
			}

			else if (token->instruction == SUB) {
				UGSMASCIIputString("SUB");
			}

			else if (token->instruction == MUL) {
				UGSMASCIIputString("MUL");
			}

			else if (token->instruction == DIV) {
				UGSMASCIIputString("DIV");
			}

			else if (token->instruction == INX8) {
				UGSMASCIIputString("INX8");
			}

			else if (token->instruction == INX16) {
				UGSMASCIIputString("INX16");
			}

			else if (token->instruction == INX32) {
				UGSMASCIIputString("INX32");
			}

			else if (token->instruction == OUTX8) {
				UGSMASCIIputString("OUTX8");
			}

			else if (token->instruction == OUTX16) {
				UGSMASCIIputString("OUTX16");
			}

			else if (token->instruction == OUTX32) {
				UGSMASCIIputString("OUTX32");
			}

			else if (token->instruction == JMP) {
				UGSMASCIIputString("JMP");
			}

			else if (token->instruction == RET) {
				UGSMASCIIputString("RET");
			}

			else if (token->instruction == UNKNOWN_INSTRUCTION) {
				UGSMASCIIputString("UNKNOWN_INSTRUCTION");
			}
		}

		else if (token->type == REGISTER) {
			if (token->reg == EAX) {
				UGSMASCIIputString("EAX");
			}

			else if (token->reg == ECX) {
				UGSMASCIIputString("ECX");
			}

			else if (token->reg == EDX) {
				UGSMASCIIputString("EDX");
			}

			else if (token->reg == EBX) {
				UGSMASCIIputString("EBX");
			}

			else if (token->reg == ESP) {
				UGSMASCIIputString("ESP");
			}

			else if (token->reg == EBP) {
				UGSMASCIIputString("EBP");
			}

			else if (token->reg == ESI) {
				UGSMASCIIputString("ESI");
			}

			else if (token->reg == EDI) {
				UGSMASCIIputString("EDI");
			}

			else if (token->reg == UNKNOWN_REGISTER) {
				UGSMASCIIputString("UNKNOWN_REGISTER");
			}
		}

		else if (token->type == RMALNUMBER) {
			putUX32Integer(token->number); // easy
		}

		else if (token->type == RMALSIGNNUMBER) {
			putX32Integer(token->signNumber); // easy
		}

		else if (token->type == RMALCOMMA) {
			UGSMASCIIputString(","); // easy
		}

		else if (token->type == LABELNAME) {
			putString(token->labelName); // easy
		}

		else if (token->type == RMALCOLON) {
			UGSMASCIIputString(":"); // easy
		}

		else if (token->type == RMALLBRACKET) {
			UGSMASCIIputString("["); // easy
		}

		else if (token->type == RMALRBRACKET) {
			UGSMASCIIputString("]"); // easy
		}

		UGSMASCIIputString(" ");
	}
}