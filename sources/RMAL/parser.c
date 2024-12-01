#include "../../headers/RMAL/parser.h"

#include "../../headers/types.h"

#include "../../headers/std.h"

#include "../../headers/ugsm.h"

#include "../../headers/multiboot.h"

#include "../../headers/RMAL/lexer.h"

#include "../../headers/warning.h"

#include "../../headers/error.h"

extern struct RMALToken RMALTokens[128];

extern uint16 RMALTokenPos;

extern struct multibootInfo* bootInfo;

struct RMALInstruction instructions[32];

uint16 instructionPos = 0;

Opcode opcodes[512] = { 0 };

OpcodesLength opcodesLength = 0;

UGSMGlyphCode labelsName[32][6];

Opcode labelsOpcodes[32];

LabelID labelPos = 0;

void RMALParse(void) {
	instructionPos = 0;

	uint16 i = 0;

	for (; RMALTokens[i].type != RMALEOF && i < RMALTokenPos; i++) {
		if (RMALTokens[i].type == LABELNAME && RMALTokens[i + 1].type == RMALCOLON) {
			instructions[instructionPos].type = LABEL;

			for (uint16 j = 0; j < 6; j++) {
				instructions[instructionPos].labelName[j] = RMALTokens[i].labelName[j];
			}

			i++;

			instructionPos++;
		}
	}

	instructionPos = 0;

	for (i = 0; RMALTokens[i].type != INSTRUCTION && i < RMALTokenPos; i++);

	for (; RMALTokens[i].type != RMALEOF && i < RMALTokenPos; i++) {
		if (RMALTokens[i].type == LABELNAME || RMALTokens[i].type == RMALCOLON) continue;

		instructions[instructionPos].argumentsNumber = 0;

		instructions[instructionPos].type = RMALTokens[i].instruction;

		i++;

		for (; RMALTokens[i].type != INSTRUCTION && i < RMALTokenPos; i++) {
			if (RMALTokens[i].type == REGISTER) {
				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].type = REGISTERTYPE;

				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].reg = RMALTokens[i].reg;
			}

			else if (RMALTokens[i].type == RMALNUMBER) {
				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].type = NUMBERTYPE;

				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].number = RMALTokens[i].number;
			}

			else if (RMALTokens[i].type == RMALSIGNNUMBER) {
				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].type = SIGNNUMBERTYPE;

				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].signNumber = RMALTokens[i].signNumber;
			}

			else if (RMALTokens[i].type == LABELNAME) {
				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].type = LABELTYPE;

				for (uint16 j = 0; j < 6; j++) {
					instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].labelName[j] = RMALTokens[i].labelName[j];
				}
			}

			else if (RMALTokens[i].type == RMALLBRACKET) {
				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].hasSquareBrackets = true;
			}

			else if (RMALTokens[i].type == RMALRBRACKET) {
				instructions[instructionPos].arguments[instructions[instructionPos].argumentsNumber].type = ADDRESSTYPE;
			}

			else if (RMALTokens[i].type == RMALCOMMA) {
				instructions[instructionPos].argumentsNumber++;
			}
		}

		instructions[instructionPos].argumentsNumber++;

		i--;

		instructionPos++;
	}

	for (i = 0; i < instructionPos; i++) {
		if (instructions[i].type == NOP) {
			UGSMASCIIputString("NOP");
		}

		else if (instructions[i].type == MOV) {
			UGSMASCIIputString("MOV");
		}

		else if (instructions[i].type == PUSH) {
			UGSMASCIIputString("PUSH");
		}

		else if (instructions[i].type == POP) {
			UGSMASCIIputString("POP");
		}

		else if (instructions[i].type == ADD) {
			UGSMASCIIputString("ADD");
		}

		else if (instructions[i].type == SUB) {
			UGSMASCIIputString("SUB");
		}

		else if (instructions[i].type == MUL) {
			UGSMASCIIputString("MUL");
		}

		else if (instructions[i].type == DIV) {
			UGSMASCIIputString("DIV");
		}

		else if (instructions[i].type == INX8) {
			UGSMASCIIputString("INX8");
		}

		else if (instructions[i].type == INX16) {
			UGSMASCIIputString("INX16");
		}

		else if (instructions[i].type == INX32) {
			UGSMASCIIputString("INX32");
		}

		else if (instructions[i].type == OUTX8) {
			UGSMASCIIputString("OUTX8");
		}

		else if (instructions[i].type == OUTX16) {
			UGSMASCIIputString("OUTX16");
		}

		else if (instructions[i].type == OUTX32) {
			UGSMASCIIputString("OUTX32");
		}

		else if (instructions[i].type == JMP) {
			UGSMASCIIputString("JMP");
		}

		else if (instructions[i].type == RET) {
			UGSMASCIIputString("RET");
		}

		else if (instructions[i].type == LABEL) {
			putString(instructions[i].labelName);

			UGSMASCIIputString(":");
		}

		else if (instructions[i].type == UNKNOWN_INSTRUCTION) {
			UGSMASCIIputString("UNKNOWN_INSTRUCTION");
		}

		UGSMASCIIputString(" ");

		for (uint16 j = 0; j < instructions[i].argumentsNumber; j++) {
			if (instructions[i].arguments[j].type == REGISTERTYPE) {
				if (instructions[i].arguments[j].reg == EAX) {
					UGSMASCIIputString("EAX");
				}

				else if (instructions[i].arguments[j].reg == ECX) {
					UGSMASCIIputString("ECX");
				}

				else if (instructions[i].arguments[j].reg == EDX) {
					UGSMASCIIputString("EDX");
				}

				else if (instructions[i].arguments[j].reg == EBX) {
					UGSMASCIIputString("EBX");
				}

				else if (instructions[i].arguments[j].reg == ESP) {
					UGSMASCIIputString("ESP");
				}

				else if (instructions[i].arguments[j].reg == EBP) {
					UGSMASCIIputString("EBP");
				}

				else if (instructions[i].arguments[j].reg == ESI) {
					UGSMASCIIputString("ESI");
				}

				else if (instructions[i].arguments[j].reg == EDI) {
					UGSMASCIIputString("EDI");
				}

				else if (instructions[i].arguments[j].reg == UNKNOWN_REGISTER) {
					UGSMASCIIputString("UNKNOWN_REGISTER");
				}
			}
				
			else if (instructions[i].arguments[j].type == NUMBERTYPE) {
				putUX32Integer(instructions[i].arguments[j].number);
			}
				
			else if (instructions[i].arguments[j].type == SIGNNUMBERTYPE) {
				putX32Integer(instructions[i].arguments[j].signNumber);
			}

			else if (instructions[i].arguments[j].type == ADDRESSTYPE) {
				UGSMASCIIputString("[ ");

				if (instructions[i].arguments[j].reg == EAX) {
					UGSMASCIIputString("EAX");
				}

				else if (instructions[i].arguments[j].reg == ECX) {
					UGSMASCIIputString("ECX");
				}

				else if (instructions[i].arguments[j].reg == EDX) {
					UGSMASCIIputString("EDX");
				}

				else if (instructions[i].arguments[j].reg == EBX) {
					UGSMASCIIputString("EBX");
				}

				else if (instructions[i].arguments[j].reg == ESP) {
					UGSMASCIIputString("ESP");
				}

				else if (instructions[i].arguments[j].reg == EBP) {
					UGSMASCIIputString("EBP");
				}

				else if (instructions[i].arguments[j].reg == ESI) {
					UGSMASCIIputString("ESI");
				}

				else if (instructions[i].arguments[j].reg == EDI) {
					UGSMASCIIputString("EDI");
				}

				else if (instructions[i].arguments[j].reg == UNKNOWN_REGISTER) {
					UGSMASCIIputString("UNKNOWN_REGISTER");
				}

				UGSMASCIIputString(" ");

				putX32Integer(instructions[i].arguments[j].signNumber);

				UGSMASCIIputString(" ]");
			}

			else if (instructions[i].arguments[j].type == LABELTYPE) {
				putString(instructions[i].arguments[j].labelName);
			}

			if (j != (instructions[i].argumentsNumber - 1)) {
				UGSMASCIIputString(", ");
			}
		}

		UGSMASCIIputString("\n");
	}
}

void RMALCompile(void) { // Compile all instructions
	for (uint16 i = 0; i < instructionPos; i++) { // Save labels opcode position
		struct RMALInstruction* instruction = &instructions[i];

		if (instruction->type == LABEL) {
			for (uint16 j = 0; j < 6; j++) {
				labelsName[labelPos][j] = instruction->labelName[j];
			}

			labelsOpcodes[labelPos] = opcodesLength;

			labelPos++;
		}
	}

	for (uint16 i = 0; i < instructionPos; i++) {
		struct RMALInstruction* instruction = &instructions[i];

		opcodes[opcodesLength] = NOPBASE; // nop opcode base

		if (instruction->type == MOV) { // Generate opcode for MOV instruction
			if (instruction->arguments[0].type == REGISTERTYPE) {
				if (instruction->arguments[1].type == REGISTERTYPE) {
					opcodes[opcodesLength] = MOVRRBASEBYTE0;

					opcodes[opcodesLength + 1] = MOVRRBASEBYTE1;

					opcodes[opcodesLength + 2] = MOVRRBASEBYTE2;

					opcodes[opcodesLength + 2] += (Opcode)(instruction->arguments[0].reg);

					opcodes[opcodesLength + 2] += ((Opcode)(instruction->arguments[1].reg)) << 3;

					opcodesLength += 3;
				}

				else if (instruction->arguments[1].type == NUMBERTYPE) {
					opcodes[opcodesLength] = MOVRNBASEBYTE0;

					opcodes[opcodesLength] += (Opcode)(instruction->arguments[0].reg);

					opcodes[opcodesLength + 1] = MOVRNBASEBYTE1;

					opcodes[opcodesLength + 2] = MOVRNBASEBYTE2;

					if (instruction->arguments[1].number < 256) {
						opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[1].number);
					}

					else if (instruction->arguments[1].number < 65536) {
						uint32 number = instruction->arguments[1].number;

						opcodes[opcodesLength + 1] = (number & 0xff);

						opcodes[opcodesLength + 2] = ((number >> 8) & 0xff);
					}

					opcodes[opcodesLength + 3] = MOVRNBASEBYTE3;

					opcodes[opcodesLength + 4] = MOVRNBASEBYTE4;

					opcodesLength += 5;
				}

				else if (instruction->arguments[1].type == ADDRESSTYPE) {
					if (instruction->arguments[1].signNumber == 0) {
						opcodes[opcodesLength] = MOVRADDRESSAZBASEBYTE0;

						opcodes[opcodesLength + 1] = MOVRADDRESSAZBASEBYTE1;

						opcodes[opcodesLength + 1] += ((Opcode)(instruction->arguments[0].reg)) << 3;

						opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[1].reg);

						opcodesLength += 2;
					}

					else {
						int32 signNumber = instruction->arguments[1].signNumber;

						signNumber = signNumber < 0 ? 0xff + signNumber : signNumber;

						opcodes[opcodesLength] = MOVRADDRESSBASEBYTE0;

						opcodes[opcodesLength + 1] = MOVRADDRESSBASEBYTE1;

						opcodes[opcodesLength + 1] += ((Opcode)(instruction->arguments[0].reg)) << 3;

						opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[1].reg);

						opcodes[opcodesLength + 2] = MOVRADDRESSBASEBYTE2;

						opcodes[opcodesLength + 2] += (Opcode)((signNumber) & 0xff);

						opcodesLength += 3;
					}
				}
			}
		}

		else if (instruction->type == PUSH) { // Generate opcode for PUSH instruction
			if (instruction->arguments[0].type == REGISTERTYPE) {
				opcodes[opcodesLength] = PUSHRBASE;

				opcodes[opcodesLength] += (Opcode)(instruction->arguments[0].reg);

				opcodesLength++;
			}

			else if (instruction->arguments[0].type == NUMBERTYPE) {
				if (instruction->arguments[0].number < 128) {
					opcodes[opcodesLength] = PUSHNLTOTSBASEBYTE0;

					opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[0].number);

					opcodesLength += 2;
				}

				else if (instruction->arguments[0].number < 65536) {
					uint32 number = instruction->arguments[0].number;

					opcodes[opcodesLength] = PUSHNBTOTSBASEBYTE0;

					opcodes[opcodesLength + 1] = (number & 0xff);

					opcodes[opcodesLength + 2] = ((number >> 8) & 0xff);

					opcodes[opcodesLength + 3] = PUSHNBTOTSBASEBYTE3;

					opcodes[opcodesLength + 4] = PUSHNBTOTSBASEBYTE4;

					opcodesLength += 5;
				}
			}
		}

		else if (instruction->type == POP) { // Generate opcode for POP instruction
			opcodes[opcodesLength] = POPBASE;

			opcodes[opcodesLength] += (Opcode)(instruction->arguments[0].reg);

			opcodesLength++;
		}

		else if (instruction->type == ADD) { // Generate opcode for ADD instruction
			if (instruction->arguments[0].type == REGISTERTYPE) {
				if (instruction->arguments[1].type == REGISTERTYPE) {
					opcodes[opcodesLength] = ADDRRBASEBYTE0;

					opcodes[opcodesLength + 1] = ADDRRBASEBYTE1;

					opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[0].reg);

					opcodes[opcodesLength + 1] += ((Opcode)(instruction->arguments[1].reg)) << 3;

					opcodesLength += 2;
				}

				else if (instruction->arguments[1].type == NUMBERTYPE) {
					if (instruction->arguments[1].number < 128) {
						opcodes[opcodesLength] = ADDRNNLTOTSBASEBYTE0;

						opcodes[opcodesLength + 1] = ADDRNNLTOTSBASEBYTE1;

						opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[0].reg);

						opcodes[opcodesLength + 2] = ADDRNNLTOTSBASEBYTE2;

						opcodes[opcodesLength + 2] += (Opcode)(instruction->arguments[1].number);

						opcodesLength += 3;
					}

					else if (instruction->arguments[1].number < 65536) {
						uint32 number = instruction->arguments[1].number;

						if (instruction->arguments[0].reg == EAX) {
							opcodes[opcodesLength] = ADDEAXNNBTOTSBASEBYTE0;

							opcodes[opcodesLength + 1] = ADDEAXNNBTOTSBASEBYTE1;

							opcodes[opcodesLength + 1] += (Opcode)(number & 0xff);

							opcodes[opcodesLength + 2] = ADDEAXNNBTOTSBASEBYTE2;

							opcodes[opcodesLength + 2] += (Opcode)((number >> 8) & 0xff);

							opcodes[opcodesLength + 3] = ADDEAXNNBTOTSBASEBYTE3;

							opcodes[opcodesLength + 4] = ADDEAXNNBTOTSBASEBYTE4;

							opcodesLength += 5;
						}

						else {
							opcodes[opcodesLength] = ADDRNEAXNNBTOTSBASEBYTE0;

							opcodes[opcodesLength + 1] = ADDRNEAXNNBTOTSBASEBYTE1;

							opcodes[opcodesLength + 1] += instruction->arguments[0].reg;

							opcodes[opcodesLength + 2] = ADDRNEAXNNBTOTSBASEBYTE2;

							opcodes[opcodesLength + 2] += (Opcode)(number & 0xff);

							opcodes[opcodesLength + 3] = ADDRNEAXNNBTOTSBASEBYTE3;

							opcodes[opcodesLength + 3] += (Opcode)((number >> 8) & 0xff);

							opcodes[opcodesLength + 4] = ADDRNEAXNNBTOTSBASEBYTE4;

							opcodes[opcodesLength + 5] = ADDRNEAXNNBTOTSBASEBYTE5;

							opcodesLength += 6;
						}
					}
				}
			}
		}

		else if (instruction->type == SUB) { // Generate opcode for SUB instruction
			if (instruction->arguments[0].type == REGISTERTYPE) {
				if (instruction->arguments[1].type == REGISTERTYPE) {
					opcodes[opcodesLength] = SUBRRBASEBYTE0;

					opcodes[opcodesLength + 1] = SUBRRBASEBYTE1;

					opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[0].reg);

					opcodes[opcodesLength + 1] += ((Opcode)(instruction->arguments[1].reg)) << 3;

					opcodesLength += 2;
				}

				else if (instruction->arguments[1].type == NUMBERTYPE) {
					if (instruction->arguments[1].number < 128) {
						opcodes[opcodesLength] = SUBRNNLTOTSBASEBYTE0;

						opcodes[opcodesLength + 1] = SUBRNNLTOTSBASEBYTE1;

						opcodes[opcodesLength + 1] += (Opcode)(instruction->arguments[0].reg);

						opcodes[opcodesLength + 2] = SUBRNNLTOTSBASEBYTE2;

						opcodes[opcodesLength + 2] += (Opcode)(instruction->arguments[1].number);

						opcodesLength += 3;
					}

					else if (instruction->arguments[1].number < 65536) {
						uint32 number = instruction->arguments[1].number;

						if (instruction->arguments[0].reg == EAX) {
							opcodes[opcodesLength] = SUBEAXNNBTOTSBASEBYTE0;

							opcodes[opcodesLength + 1] = SUBEAXNNBTOTSBASEBYTE1;

							opcodes[opcodesLength + 1] += (Opcode)(number & 0xff);

							opcodes[opcodesLength + 2] = SUBEAXNNBTOTSBASEBYTE2;

							opcodes[opcodesLength + 2] += (Opcode)((number >> 8) & 0xff);

							opcodes[opcodesLength + 3] = SUBEAXNNBTOTSBASEBYTE3;

							opcodes[opcodesLength + 4] = SUBEAXNNBTOTSBASEBYTE4;

							opcodesLength += 5;
						}

						else {
							opcodes[opcodesLength] = SUBRNEAXNNBTOTSBASEBYTE0;

							opcodes[opcodesLength + 1] = SUBRNEAXNNBTOTSBASEBYTE1;

							opcodes[opcodesLength + 1] += instruction->arguments[0].reg;

							opcodes[opcodesLength + 2] = SUBRNEAXNNBTOTSBASEBYTE2;

							opcodes[opcodesLength + 2] += (Opcode)(number & 0xff);

							opcodes[opcodesLength + 3] = SUBRNEAXNNBTOTSBASEBYTE3;

							opcodes[opcodesLength + 3] += (Opcode)((number >> 8) & 0xff);

							opcodes[opcodesLength + 4] = SUBRNEAXNNBTOTSBASEBYTE4;

							opcodes[opcodesLength + 5] = SUBRNEAXNNBTOTSBASEBYTE5;

							opcodesLength += 6;
						}
					}
				}
			}
		}

		else if (instruction->type == MUL) { // Generate opcode for MUL instruction
			opcodes[opcodesLength] = MULRBASEBYTE0;

			opcodes[opcodesLength + 1] = MULRBASEBYTE1;

			opcodes[opcodesLength + 1] += instruction->arguments[0].reg;

			opcodesLength += 2;
		}

		else if (instruction->type == DIV) { // Generate opcode for DIV instruction
			opcodes[opcodesLength] = DIVRBASEBYTE0;

			opcodes[opcodesLength + 1] = DIVRBASEBYTE1;

			opcodes[opcodesLength + 1] += instruction->arguments[0].reg;

			opcodesLength += 2;
		}

		else if (instruction->type == INC) { // Generate opcode for INC instruction
			opcodes[opcodesLength] = INCRBASEBYTE0;

			opcodes[opcodesLength + 1] = INCRBASEBYTE1;

			opcodes[opcodesLength + 1] += instruction->arguments[0].reg;

			opcodesLength += 2;
		}

		else if (instruction->type == DEC) { // Generate opcode for DEC instruction
			opcodes[opcodesLength] = DECRBASEBYTE0;

			opcodes[opcodesLength + 1] = DECRBASEBYTE1;

			opcodes[opcodesLength + 1] += instruction->arguments[0].reg;

			opcodesLength += 2;
		}

		else if (instruction->type == INX8) { // Generate opcode for INX8 instruction
			opcodes[opcodesLength] = INX8BASEBYTE0;

			opcodesLength++;
		}

		else if (instruction->type == INX16) { // Generate opcode for INX16 instruction
			opcodes[opcodesLength] = INX16BASEBYTE0;

			opcodes[opcodesLength + 1] = INX16BASEBYTE1;

			opcodesLength += 2;
		}

		else if (instruction->type == INX32) { // Generate opcode for INX32 instruction
			opcodes[opcodesLength] = INX32BASEBYTE0;

			opcodesLength++;
		}

		else if (instruction->type == OUTX8) { // Generate opcode for OUTX8 instruction
			opcodes[opcodesLength] = OUTX8BASEBYTE0;

			opcodesLength++;
		}

		else if (instruction->type == OUTX16) { // Generate opcode for OUTX16 instruction
			opcodes[opcodesLength] = OUTX16BASEBYTE0;

			opcodes[opcodesLength + 1] = OUTX16BASEBYTE1;

			opcodesLength += 2;
		}

		else if (instruction->type == OUTX32) { // Generate opcode for OUTX32 instruction
			opcodes[opcodesLength] = OUTX32BASEBYTE0;

			opcodesLength++;
		}

		else if (instruction->type == JMP) { // Generate opcode for JMP instruction
			if (instruction->arguments[0].type == REGISTERTYPE) {
				opcodes[opcodesLength] = JMPRBASEBYTE0;

				opcodes[opcodesLength + 1] = JMPRBASEBYTE1;

				opcodes[opcodesLength + 1] += instruction->arguments[0].reg;

				opcodesLength += 2;
			}

			else if (instruction->arguments[0].type == NUMBERTYPE) {
				int32 number = instruction->arguments[0].number;

				if (number < 2) {
					opcodes[opcodesLength] = JMPNLTTZBASEBYTE0;

					opcodes[opcodesLength + 1] = 0xfe + (Opcode)(number);

					opcodesLength += 2;
				}

				else if (number < 130) {
					opcodes[opcodesLength] = JMPNLTTZBASEBYTE0;

					opcodes[opcodesLength + 1] = JMPNLTTZBASEBYTE1;

					opcodes[opcodesLength + 1] += (Opcode)(number & 0xff);

					opcodesLength += 2;
				}

				else if (number < 65541) {
					opcodes[opcodesLength] = JMPNLTSFFTFBASEBYTE0;

					opcodes[opcodesLength + 1] = JMPNLTSFFTFBASEBYTE1;

					opcodes[opcodesLength + 1] = (number & 0xff);

					opcodes[opcodesLength + 2] = JMPNLTSFFTFBASEBYTE2;

					opcodes[opcodesLength + 2] = ((number >> 8) & 0xff);

					opcodes[opcodesLength + 3] = JMPNLTSFFTFBASEBYTE3;

					opcodes[opcodesLength + 4] = JMPNLTSFFTFBASEBYTE4;

					opcodesLength += 5;
				}
			}

			else if (instruction->arguments[0].type == SIGNNUMBERTYPE) {
				int32 signNumber = instruction->arguments[0].signNumber;

				if (signNumber < 0) {
					signNumber += 0xebfe;
				}

				if (signNumber < 2) {
					opcodes[opcodesLength] = JMPNLTTZBASEBYTE0;

					opcodes[opcodesLength + 1] = 0xfe + (Opcode)(signNumber);

					opcodesLength += 2;
				}

				else if (signNumber < 130) {
					opcodes[opcodesLength] = JMPNLTTZBASEBYTE0;

					opcodes[opcodesLength + 1] = JMPNLTTZBASEBYTE1;

					opcodes[opcodesLength + 1] += (Opcode)(signNumber & 0xff);

					opcodesLength += 2;
				}

				else if (signNumber < 65541) {
					opcodes[opcodesLength] = JMPNLTSFFTFBASEBYTE0;

					opcodes[opcodesLength + 1] = JMPNLTSFFTFBASEBYTE1;

					opcodes[opcodesLength + 1] = (signNumber & 0xff);

					opcodes[opcodesLength + 2] = JMPNLTSFFTFBASEBYTE2;

					opcodes[opcodesLength + 2] = ((signNumber >> 8) & 0xff);

					opcodes[opcodesLength + 3] = JMPNLTSFFTFBASEBYTE3;

					opcodes[opcodesLength + 4] = JMPNLTSFFTFBASEBYTE4;

					opcodesLength += 5;
				}
			}

			else if (instruction->arguments[0].type == LABELTYPE) {
				OpcodesLength labelOpcodesLength = 0;

				bool bFinded = false;

				for (uint16 j = 0; j < 32; j++) {
					uint16 checkLength = 0;

					uint16 k = 0;

					for (; instruction->arguments[0].labelName[k]; k++) {
						if (instruction->arguments[0].labelName[k] == labelsName[j][k]) checkLength++;
					}

					if (k == checkLength) {
						labelOpcodesLength = labelsOpcodes[j];

						bFinded = true;

						break;
					}
				}

				if (bFinded) {
					labelOpcodesLength -= opcodesLength;

					putX32Integer(labelOpcodesLength);

					if (labelOpcodesLength < 0) {
						labelOpcodesLength += 0xebfe;
					}

					if (labelOpcodesLength < 2) {
						opcodes[opcodesLength] = JMPNLTTZBASEBYTE0;

						opcodes[opcodesLength + 1] = 0xfe + (Opcode)(labelOpcodesLength);

						opcodesLength += 2;
					}

					else if (labelOpcodesLength < 130) {
						opcodes[opcodesLength] = JMPNLTTZBASEBYTE0;

						opcodes[opcodesLength + 1] = JMPNLTTZBASEBYTE1;

						opcodes[opcodesLength + 1] += (Opcode)(labelOpcodesLength & 0xff);

						opcodesLength += 2;
					}

					else if (labelOpcodesLength < 65541) {
						opcodes[opcodesLength] = JMPNLTSFFTFBASEBYTE0;

						opcodes[opcodesLength + 1] = JMPNLTSFFTFBASEBYTE1;

						opcodes[opcodesLength + 1] = (labelOpcodesLength & 0xff);

						opcodes[opcodesLength + 2] = JMPNLTSFFTFBASEBYTE2;

						opcodes[opcodesLength + 2] = ((labelOpcodesLength >> 8) & 0xff);

						opcodes[opcodesLength + 3] = JMPNLTSFFTFBASEBYTE3;

						opcodes[opcodesLength + 4] = JMPNLTSFFTFBASEBYTE4;

						opcodesLength += 5;
					}
				}
			}
		}

		else if (instruction->type == RET) { // Generate opcode for RET instruction
			opcodes[opcodesLength] = RETBASE; // ret opcode base

			opcodesLength++;
		}
	}

	UGSMASCIIputString("\n");

	UGSMASCIIputString("Generated opcodes: ");

	for (uint16 i = 0; i < opcodesLength; i++) {
		putUX8Integer(opcodes[i]);

		UGSMASCIIputString(" ");
	}
}