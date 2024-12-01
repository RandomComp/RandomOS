from pyperclip import copy

commands = [
	"help",

	"echo",

	"color",

	"program",

	"shutdown",

	"reboot",

	"clear"
]

maxCommandNameLength = len(max(commands, key=len))

result = f"                    UGSMGlyphCode commands[{len(commands)}][{maxCommandNameLength}] = {{"

for i, command in enumerate(commands):
	result += "\n                        { "

	if (len(command) == 0):
		result += "0"

	for j, char in enumerate(command):
		result += str(ord(char) - 33 + 5)

		if (j != (len(command) - 1)):
			result += ", "

	if (len(command) < maxCommandNameLength):
		result += ", "

	for j in range(maxCommandNameLength - len(command)):
		result += "0"

		if (j != (maxCommandNameLength - len(command) - 1)):
			result += ", "

	result += " }"

	if (i != (len(commands) - 1)):
		result += ","

	result += "\n"

result += "                    };\n"

copy(result)