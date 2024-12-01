#include "../headers/types.h"

uint32 colorRGBToBGR(uint32 color) {
	return ((color >> 11) & 0x1f) + (((color >> 5) & 0x3f) << 5) + ((color & 0x1f) << 11);
}