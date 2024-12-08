#include "headers/multiboot.h"

#include "headers/std.h"

#include "headers/pit.h"

#include "headers/keyboardps2.h"

#include "headers/time.h"

#include "headers/mem.h"

#include "headers/ugsm.h"

#include "headers/ascii.h"

#include "headers/rus.h"

#include "headers/speaker.h"

#include "headers/warning.h"

#include "headers/error.h"

#include "headers/power.h"

#include "headers/acpi.h"

#include "headers/RL/lexer.h"

#include "headers/RMAL/lexer.h"

#include "headers/RMAL/parser.h"

extern const uint32 columns;

extern const uint32 rows;

extern int16 x;

extern int16 y;

extern struct Time now;

extern void (*everySecond);

extern void (*overflowSymbolsOnScreen);

extern uint32 foregroundColor;

extern uint32 backgroundColor;

extern struct Size RAMSize;

extern UGSMGlyphSetCode ASCIIOffset;

extern Opcode opcodes[512];

extern uint32 opcodesLength;

extern uint32 framebuffer[width][height];

extern bool bKeyboardPS2Updated;

extern enum KeyState keyStates[128];

extern enum Key updatedKey;

extern Address heapStart;

extern AbsoluteSize AbsoluteRAMSizeInBytes;

extern AbsoluteSize AbsoluteHeapSizeInBytes;

extern struct Size heapSize;

struct multibootInfo* bootInfo;

enum Commands {
	HELP,

	ECHO,

	COLOR,

	PROGRAM,

	SHUTDOWN,

	REBOOT,

	CLEAR
} Commands;

void EverySecond(void) {
	uint16 tempX = x;

	uint16 tempY = y;

	x = 0;

	y = 0;

	uint32 tempForegroundColor = foregroundColor;

	uint32 tempBackgroundColor = backgroundColor;

	foregroundColor = 0xffffff;

	backgroundColor = 0x800000;

	int8* str = "Random OS";

	uint16 length = 0;

	for (; str[length]; length++);

	putChar(1);

	if (now.day < 10) {
		UGSMASCIIputChar('0');
	}

	putX8Integer(now.day);

	UGSMASCIIputChar('.');

	if (now.month < 10) {
		UGSMASCIIputChar('0');
	}

	putX8Integer(now.month + 1);

	UGSMASCIIputChar('.');

	uint8 digitsCount = 0;

	for (uint32 tempNum = now.year4Digits; tempNum != 0; tempNum /= 10) 
		digitsCount++;

	for (uint8 i = 0; i < 3 - digitsCount; i++) {
		UGSMASCIIputChar('0');
	}

	putX32Integer(now.year4Digits);

	uint16 pos = (columns / 2) - (length / 2);

	for (uint16 i = x; i < pos; i++) {
		UGSMASCIIputString(" ");
	}

	UGSMASCIIputString(str);

	for (uint16 i = pos + length; i < columns; i++) {
		UGSMASCIIputString(" ");
	}

	x = 91;

	y = 0;

	if (now.hour < 10) {
		UGSMASCIIputChar('0');
	}

	putX8Integer(now.hour);

	UGSMASCIIputChar(':');

	if (now.minute < 10) {
		UGSMASCIIputChar('0');
	}

	putX8Integer(now.minute);

	UGSMASCIIputChar(':');

	if (now.second < 10) {
		UGSMASCIIputChar('0');
	}

	putX8Integer(now.second);

	swap();

	foregroundColor = tempForegroundColor;

	backgroundColor = tempBackgroundColor;

	x = tempX;

	y = tempY;
}

void OverflowSymbolsOnScreen() {
	EverySecond();

	y++;
}

void RMALcompileASCII(int8* code) {
	putChar(1);

	UGSMGlyphCode UGSMCode[384] = { 0 };

	for (uint16 i = 0; code[i]; i++) {
		int8 c = code[i];

		if (c == 32) {
			UGSMCode[i] = 1;
		}

		else if (c == 10) {
			UGSMCode[i] = 2;
		}

		else if (c == 13) {
			UGSMCode[i] = 3;
		}

		else if (c == 9) {
			UGSMCode[i] = 4;
		}

		else {
			UGSMCode[i] = (UGSMGlyphCode)(c - 33) + ASCIIOffset + 5;
		}
	}

	putString(UGSMCode);

	RMALTokenize(UGSMCode);

	RMALTokensView();

	UGSMASCIIputString("\n");

	RMALParse();

	RMALCompile();

	UGSMASCIIputString("\n");
}

void main(uint32 magic, struct multibootInfo* bootInfoArg) {
	bootInfo = bootInfoArg;

	STDInit();

	ASCIIInit();

	y += 3;

	foregroundColor = 0x00ff00;

	backgroundColor = 0x000000;

	UGSMASCIIputString("STD is initialized!\n");

	UGSMASCIIputString("ASCII is initialized!\n");

	TimeInit();

	UGSMASCIIputString("Time is initialized!\n");

	MEMInit(2 * 1024 * 1024); // 4 MB

	UGSMASCIIputString("RAM is initialized!\n");

	UGSMASCIIputString("RAM Size is:\n");

	if (RAMSize.gigabytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(RAMSize.gigabytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" gigabytes\n");
	}

	if (RAMSize.megabytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(RAMSize.megabytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" megabytes\n");
	}

	if (RAMSize.kilobytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(RAMSize.kilobytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" kilobytes\n");
	}

	if (RAMSize.bytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(RAMSize.bytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" bytes\n");
	}

	UGSMASCIIputString("Heap start is: ");

	putUX32Integer(heapStart);

	putChar(2);

	UGSMASCIIputString("Heap size is:\n");

	if (heapSize.gigabytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(heapSize.gigabytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" gigabytes\n");
	}

	if (heapSize.megabytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(heapSize.megabytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" megabytes\n");
	}

	if (heapSize.kilobytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(heapSize.kilobytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" kilobytes\n");
	}

	if (heapSize.bytes != 0) {
		foregroundColor = 0xffff00;

		putUX16Integer(heapSize.bytes);

		foregroundColor = 0x00ff00;

		UGSMASCIIputString(" bytes\n");
	}

	RUSInit();

	UGSMASCIIputString("Russian is initialized!\n");

	everySecond = &EverySecond;

	overflowSymbolsOnScreen = &OverflowSymbolsOnScreen;

	EverySecond();

	KeyboardPS2Init();

	UGSMASCIIputString("Interrupt for PS/2 keyboard is ready to use!\n");

	ACPIInit();

	UGSMASCIIputString("ACPI is initialized!\n");

	UGSMASCIIputString("OS is booted succesfully!\n");

	foregroundColor = 0xffffff;

	backgroundColor = 0x000000;

	RMALcompileASCII("push ebp mov ebp, esp mov eax, [ ebp +8 ] mov ebx, [ ebp +12 ] add eax, ebx pop ebp ret");

	UGSMASCIIputString("Loading the program into memory at ");

	uint32 address = (uint32)(malloc(opcodesLength, RESERVED_EXECUTABLE_MEMORY));

	uint8* mem = (uint8*)address;

	putUX32Integer(address);

	UGSMASCIIputString("...\n");

	for (uint32 i = 0; i < opcodesLength; i++) {
		mem[i] = opcodes[i];
	}

	uint32 (*program) (uint32, uint32) = (void*)(address); // Program definition

	UGSMASCIIputString("Random OS --> ");

	swap();

	UGSMGlyphCode buffer[256] = { 0 };

	int16 bufferLength = 0;

	while (true) {
		if (bKeyboardPS2Updated) {
			if (keyStates[updatedKey] == KEY_PRESSED || keyStates[updatedKey] == KEY_HOLDED) {
				if (updatedKey == KEY_ENTER) {
					uint16 checkLength = 0;

					for (; buffer[checkLength]; checkLength++);

					if (bufferLength > 0 && checkLength != 0) {
						UGSMASCIIputString("\n");

						UGSMGlyphCode commands[7][8] = {
							{ 76, 73, 80, 84, 0, 0, 0, 0 },

							{ 73, 71, 76, 83, 0, 0, 0, 0 },

							{ 71, 83, 80, 83, 86, 0, 0, 0 },

							{ 84, 86, 83, 75, 86, 69, 81, 0 },

							{ 87, 76, 89, 88, 72, 83, 91, 82 },

							{ 86, 73, 70, 83, 83, 88, 0, 0 },

							{ 71, 80, 73, 69, 86, 0, 0, 0 }
						};

						enum Commands command = HELP;

						bool bFinded = false;

						for (; command < 7; command++) {
							uint16 length = 0;

							for (; commands[command][length] && length < 8; length++);

							uint16 checkLength = 0;

							for (uint16 j = 0; buffer[j] && j < 8; j++) {
								if (commands[command][j] == buffer[j]) {
									checkLength++;
								}
							}
						
							if (checkLength == length) {
								bFinded = true;

								break;
							}
						}

						if (bFinded) {
							uint16 i = 0;

							for (; buffer[i] && i < 8; i++);

							i++;

							UGSMGlyphCode arguments[7][8] = { { 0 } };

							uint16 argumentLength = 0;

							uint16 argumentCharIndex = 0;

							for (; i < bufferLength; i++) {
								if (buffer[i] == 0) {
									argumentCharIndex = 0;

									argumentLength++;

									if (argumentLength >= 7) {
										break;
									}
								}

								else {
									arguments[argumentLength][argumentCharIndex] = buffer[i];

									argumentCharIndex++;
								}
							}

							argumentLength++;

							if (command == HELP) {
								UGSMASCIIputString("Commands:\n");

								UGSMASCIIputString("\thelp --> Helps for use commands ( this command )\n");

								UGSMASCIIputString("\techo --> Type something in and get it in response\n");

								UGSMASCIIputString("\tprogram --> Run program\n");

								UGSMASCIIputString("\tcolor --> Changes the color of the text ( foreground and background )\n");

								UGSMASCIIputString("\tshutdown --> Turns off the computer\n");

								UGSMASCIIputString("\treboot --> Restarts the computer\n");

								UGSMASCIIputString("\tclear --> Clears the console\n");
							}

							else if (command == ECHO) {
								for (i = 0; i < argumentLength; i++) {
									for (uint16 j = 0; arguments[i][j] && j < 8; j++) {
										putChar(arguments[i][j]);
									}

									putChar(1);
								}
							}

							else if (command == PROGRAM) {
								putUX32Integer(program(5, 528)); // Calling

								UGSMASCIIputString("\n");
							}

							else if (command == COLOR) {
								if (argumentLength == 1) {
									foregroundColor = 0xffffff;

									backgroundColor = 0;

									continue;
								}

								UGSMGlyphCode startSymbol = UGSMGlyphToLowerCase(arguments[0][0]);

								bool bIsForegroundColor = startSymbol == ('f' - 33 + 5 + ASCIIOffset);

								bool bIsBackgroundColor = startSymbol == ('b' - 33 + 5 + ASCIIOffset);

								uint8 temp = 0;

								if (bIsForegroundColor) {
									foregroundColor = 0;
								}

								else if (bIsBackgroundColor) {
									backgroundColor = 0;
								}

								for (uint16 i = 0; i < 3; i++) {
									temp = 0;

									if (bIsForegroundColor) {
										UGSMGlyphCode current = arguments[i + 1][0];

										for (uint16 j = 0; arguments[i + 1][j] && j < 8; j++) {
											temp *= 10;

											temp += (uint8)(current - 19);

											foregroundColor += temp << ((2 - i) * 8);

											current = arguments[i + 1][j];
										}
									}

									else if (bIsBackgroundColor) {
										UGSMGlyphCode current = arguments[i + 1][0];

										for (uint16 j = 0; arguments[i + 1][j] && j < 8; j++) {
											temp *= 10;

											temp += (uint8)(current - 19);

											backgroundColor += temp << ((2 - i) * 8);

											current = arguments[i + 1][j];
										}
									}
								}
							}

							else if (command == SHUTDOWN) {
								UGSMASCIIputString("Clearing the program...\n");

								free(mem, opcodesLength); // Clear the program into memory

								swap();

								shutdown();
							}

							else if (command == REBOOT) {
								UGSMASCIIputString("Clearing the program...\n");

								free(mem, opcodesLength); // Clear the program into memory

								swap();

								reboot();
							}

							else if (command == CLEAR) {
								clear(backgroundColor);

								EverySecond();
							}
						}

						else {
							uint16 checkLength = 0;

							for (; buffer[checkLength]; checkLength++);

							if (checkLength != 0) {
								uint32 tempForegroundColor = foregroundColor;

								foregroundColor = 0xff0000;

								for (uint16 i = 0; buffer[i] && i < 256; i++) {
									putChar(buffer[i]);
								}

								UGSMASCIIputString(": Command not finded!\n");

								foregroundColor = tempForegroundColor;
							}
						}
					}

					for (uint16 i = 0; i < 256; i++) {
						buffer[i] = 0;
					}

					bufferLength = 0;

					UGSMASCIIputString("\nRandom OS --> ");
				}

				else if (updatedKey == KEY_BACKSPACE) {
					if (bufferLength <= 0) {
						if (keyStates[updatedKey] == KEY_RELEASED) {
							keyStates[updatedKey] = KEY_FREE;
						}

						bKeyboardPS2Updated = false;

						continue;
					}

					if (x <= 0) {
						x = columns - 1;

						y--;
					}

					else x--;

					setChar(x, y, 1);

					bufferLength--;

					buffer[bufferLength] = 0;
				}

				else if (updatedKey == KEY_SPACE) {
					putChar(1);

					buffer[bufferLength] = 0;

					bufferLength++;
				}

				else {
					UGSMGlyphCode glyphCode = scancodeToUGSM(updatedKey);

					if (glyphCode != 0) {
						putChar(glyphCode);

						buffer[bufferLength] = glyphCode;

						bufferLength++;
					}
				}

				swap();
			}

			if (keyStates[updatedKey] == KEY_RELEASED) {
				keyStates[updatedKey] = KEY_FREE;
			}

			bKeyboardPS2Updated = false;
		}
	}

	for (;;);
}