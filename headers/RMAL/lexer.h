#ifndef _RMAL_LEXER_H
#define _RMAL_LEXER_H

// Lexer of RMAL ( Random minimal assembly language )

#include "../../headers/types.h"

#include "../../headers/ugsm.h"

typedef uint16 LabelID;

enum RMALTokenValueType {
	INSTRUCTION,

	REGISTER,
	
	RMALNUMBER,
	
	RMALSIGNNUMBER,

	LABELNAME,

	RMALCOMMA, // ,

	RMALCOLON, // :

	RMALLBRACKET, // [

	RMALRBRACKET, // ]

	RMALEOF // end of file
};

enum RMALInstructions {
	NOP,

	MOV,

	PUSH,

	POP,

	ADD,

	SUB,

	MUL,

	DIV,

	INC,

	DEC,

	INX8,

	INX16,

	INX32,

	OUTX8,

	OUTX16,

	OUTX32,

	JMP,

	RET,

	LABEL,

	EOFINSTRUCTION,

	UNKNOWN_INSTRUCTION
};

enum RMALRegisters {
	EAX, // Accumulator

	ECX, // Counter

	EDX, // Data

	EBX, // Battery

	ESP, // Stack pointer

	EBP, // Base pointer

	ESI, // Source index

	EDI, // Destination index

	UNKNOWN_REGISTER // Unknown
};

enum RMALInstructionArgumentType {
	NULLTYPE,

	REGISTERTYPE,

	NUMBERTYPE,

	SIGNNUMBERTYPE,

	ADDRESSTYPE,

	LABELTYPE
};

struct RMALInstructionArgument {
	enum RMALInstructionArgumentType type;

	enum RMALRegisters reg;

	uint32 number;

	int32 signNumber;

	bool hasSquareBrackets; // A flag to check whether an argument has square brackets

	UGSMGlyphCode labelName[6];
};

struct RMALInstruction {
	enum RMALInstructions type;

	struct RMALInstructionArgument arguments[2];

	uint16 argumentsNumber;

	UGSMGlyphCode labelName[6];
};

struct RMALToken { // maybe instruction, register or number
	enum RMALTokenValueType type;

	enum RMALInstructions instruction;

	enum RMALRegisters reg;

	uint32 number;

	int32 signNumber;

	UGSMGlyphCode labelName[6];
};

void RMALTokenize(UGSMGlyphCode argCode[384]);

enum RMALInstructions findInstructionByName(UGSMGlyphCode name[6]);

enum RMALRegisters findRegisterByName(UGSMGlyphCode name[6]);

bool nameIsInstructionName(UGSMGlyphCode name[6]);

bool nameIsRegisterName(UGSMGlyphCode name[6]);

void RMALTokenizeWord(void);

void RMALTokenizeSignNumber(void);

void RMALTokenizeNumber(void);

void RMALTokenizeComment(void);

UGSMGlyphCode RMALNext(void);

UGSMGlyphCode RMALPeek(int16 relativePosition);

void RMALTokensView(void);

#endif