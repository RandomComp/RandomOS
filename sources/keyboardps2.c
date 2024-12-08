#include "../headers/keyboardps2.h"

#include "../headers/idt.h"

#include "../headers/ugsm.h"

#include "../headers/abc.h"

#include "../headers/std.h"

#include "../headers/io.h"

extern UGSMGlyphSetCode ASCIIOffset;

extern uint32 foregroundColor;

bool bKeyboardPS2Initialized = false;

bool bKeyboardPS2Available = false;

bool bKeyboardPS2Updated = false;

bool bKeyboardPS2CapsLock = false;

bool bKeyboardPS2Shift = false;

UGSMGlyphCode QWERTY[47] = {
	20, 10, 22, 23, 24, 26, 28, 19, 

	27, 32, 15, 16, 17, 18, 30, 21, 

	68, 69, 70, 71, 72, 73, 74, 75, 

	76, 77, 78, 79, 80, 81, 82, 83, 

	84, 85, 86, 87, 88, 89, 90, 91, 

	92, 93, 62, 63, 64, 25
};

UGSMGlyphCode QWERTYToShiftQWERTY[47] = {
	4, 5, 6, 7, 8, 9, 11, 12, 

	13, 14, 31, 66, 33, 34, 29, 35,

	36, 37, 38, 39, 40, 41, 42, 43, 

	44, 45, 46, 47, 48, 49, 50, 51, 

	52, 53, 54, 55, 56, 57, 58, 59, 

	60, 61, 94, 95, 96, 65
};

enum KeyState keyStates[128] = { KEY_FREE };

enum KeyState oldKeyStates[128] = { KEY_FREE };

enum Key updatedKey = KEY_NULL;

void KeyboardPS2Init(void) {
	if (bKeyboardPS2Initialized) return;

	IDTIRQInstallHandler(1, &KeyboardPS2Event);

	bKeyboardPS2Initialized = true;
}

void KeyboardPS2Event(struct Registers* regs) {
	if (!bKeyboardPS2Available) {
		uint32 tempForegroundColor = foregroundColor;

		foregroundColor = 0x00ff00;

		UGSMASCIIputString("PS/2 keyboard is detected!\n");

		foregroundColor = tempForegroundColor;

		swap();

		bKeyboardPS2Available = true;
	}

	bool bIsActive = false;

	in8(0x64, &bIsActive);

	if (bIsActive & 1) {
		enum Key scancode = 0;

		in8(0x60, &scancode);

		bool bIsReleased = scancode & 0x80;

		scancode &= 0x7f;

		if (keyStates[scancode] == KEY_FREE) {
			keyStates[scancode] = KEY_PRESSED;
		}

		if (bIsReleased) {
			keyStates[scancode] = KEY_RELEASED; // released
		}

		else if (oldKeyStates[scancode] == KEY_RELEASED) {
			keyStates[scancode] = KEY_PRESSED; // pressed
		}

		else if (oldKeyStates[scancode] == KEY_PRESSED) {
			keyStates[scancode] = KEY_HOLDED; // holded
		}

		if (keyStates[scancode]) {
			if (scancode == KEY_CAPSLOCK && keyStates[scancode] == KEY_PRESSED) {
				bKeyboardPS2CapsLock = !bKeyboardPS2CapsLock;
			}

			else if (scancode == KEY_LEFT_SHIFT && (keyStates[scancode] == KEY_PRESSED || keyStates[scancode] == KEY_RELEASED)) {
				bKeyboardPS2Shift = !bKeyboardPS2Shift;
			}
		}

		oldKeyStates[scancode] = keyStates[scancode];

		updatedKey = scancode;

		bKeyboardPS2Updated = true;
	}
}

UGSMGlyphCode scancodeToUGSM(enum Key key) {
	UGSMGlyphCode result = 0;

	if (key == KEY_0) {
		result = 20 + ASCIIOffset;
	}

	if (key >= KEY_1 && key <= KEY_9) {
		result = key - KEY_1 + 21 + ASCIIOffset;
	}

	if (key == KEY_MINUS) {
		result = 17;
	}

	if (key == KEY_EQUAL) {
		result = 33;
	}

	if (key >= KEY_Q && key <= KEY_RIGHT_BRACKET) {
		UGSMGlyphCode QTRB[] = {
			84, 90, 72, 85, 

			87, 92, 88, 76, 

			82, 83, 62, 64
		};

		result = QTRB[key - KEY_Q] + 1;
	}

	if (key == KEY_ENTER) {
		result = 2;
	}

	if (key >= KEY_A && key <= KEY_BACK_TICK) {
		UGSMGlyphCode ATBT[] = {
			68, 86, 71, 73, 

			74, 75, 77, 78, 

			79, 30, 10, 67
		};

		result = ATBT[key - KEY_A] + 1;
	}

	if (key == KEY_BACKSLASH) {
		result = 64;
	}

	if (key >= KEY_Z && key <= KEY_SLASH) {
		UGSMGlyphCode ZTS[] = {
			93, 91, 70, 89, 

			69, 81, 80, 15, 

			17, 18
		};

		result = ZTS[key - KEY_Z] + 1;
	}

	if (bKeyboardPS2CapsLock) {
		result = UGSMGlyphToUpperCase(result);
	}

	if (bKeyboardPS2Shift) {
		for (uint16 i = 0; i < 47; i++) {
			if (QWERTY[i] == result) {
				result = QWERTYToShiftQWERTY[i];

				break;
			}
		}

		if (bKeyboardPS2CapsLock) {
			result = UGSMGlyphToLowerCase(result);
		}
	}

	return result;
}