from pyperclip import copy

instructions = [
	"nop",
	"mov",
	"push",
	"pop",
	"add",
	"sub",
	"mul",
	"div",
	"inc",
	"dec",
	"inx8",
	"inx16",
	"inx32",
	"outx8",
	"outx16",
	"outx32",
	"ret"
]

registers = [
	"eax",
	"ecx",
	"edx",
	"ebx",
	"esp",
	"ebp",
	"esi",
	"edi"
]

maxInstructionNameLength = len(max(instructions, key=len))

maxRegisterNameLength = len(max(registers, key=len))

instructionsName = f"UGSMGlyphCode instructionsName[{len(instructions)}][{maxInstructionNameLength}] = {{"

registersName = f"UGSMGlyphCode registersName[{len(registers)}][{maxRegisterNameLength}] = {{"

for i, instruction in enumerate(instructions):
	instructionsName += "\n    { "

	if (len(instruction) == 0):
		instructionsName += "0"

	for j, char in enumerate(instruction):
		instructionsName += str(ord(char) - 33 + 4)

		if (j != (len(instruction) - 1)):
			instructionsName += ", "

	if (len(instruction) < maxInstructionNameLength):
		instructionsName += ", "

	for j in range(maxInstructionNameLength - len(instruction)):
		instructionsName += "0"

		if (j != (maxInstructionNameLength - len(instruction) - 1)):
			instructionsName += ", "

	instructionsName += " }"

	if (i != (len(instructions) - 1)):
		instructionsName += ","

	instructionsName += "\n"

instructionsName += "};\n"

for i, register in enumerate(registers):
	registersName += "\n    { "

	for j, char in enumerate(register):
		registersName += str(ord(char) - 33 + 4)

		if (j != (len(register) - 1)):
			registersName += ", "

	if (len(register) < maxRegisterNameLength):
		registersName += ", "

	for j in range(maxRegisterNameLength - len(register)):
		registersName += "0"

		if (j != (maxRegisterNameLength - len(register) - 1)):
			registersName += ", "

	registersName += " }"

	if (i != (len(registers) - 1)):
		registersName += ","

	registersName += "\n"

registersName += "};\n"

result = f"{instructionsName}\n\n{registersName}"

copy(result)