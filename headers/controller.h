#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include "types.h"

typedef struct UsbController {
    struct UsbController* next;

    void* hc;

    void (*poll)(struct UsbController* controller);
} UsbController;

extern UsbController* g_usbControllerList;

#endif