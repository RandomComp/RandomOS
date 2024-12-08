#include "../headers/types.h"

#include "../headers/mouseps2.h"

#include "../headers/idt.h"

bool bMousePS2Initialized = false;

void MousePS2Init(void) {
	if (bMousePS2Initialized) return;

	IDTIRQInstallHandler(1, &MousePS2Event);

	bMousePS2Initialized = true;
}

void MousePS2Event(struct Registers* regs) {

}