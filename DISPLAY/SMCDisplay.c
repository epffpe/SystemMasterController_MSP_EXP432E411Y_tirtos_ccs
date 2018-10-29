/*
 * SMCDisplay.c
 *
 *  Created on: Oct 24, 2018
 *      Author: epenate
 */

#define SMCDISPLAY_GLOBALS
#include "includes.h"

void SMCDisplay_init()
{
    Display_init();

    g_SMCDisplay = Display_open(Display_Type_UART, NULL);
    if (g_SMCDisplay == NULL) {
        /* Failed to open display driver */
        while(1);
    }
}
