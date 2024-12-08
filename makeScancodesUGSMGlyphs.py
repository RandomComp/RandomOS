from pyperclip import copy

qwerty = "zxcvbnm,./"

scancodesToUGSMGlyphs = ""

j = 1

for i, c in enumerate(qwerty):
	scancodesToUGSMGlyphs += f"{ord(c) - 33 + 4}"

	if (i != (len(qwerty) - 1)):
		scancodesToUGSMGlyphs += ", "

	if (j >= 4):
		scancodesToUGSMGlyphs += "\n\n"

		j = 0

	j += 1

copy(scancodesToUGSMGlyphs)