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
    /* Initialize display and try to open both UART and LCD types of display. */
    Display_Params params;
    Display_Params_init(&params);
//    params.lineClearMode = DISPLAY_CLEAR_BOTH;
    /* Open an UART display.
     * Whether the open call is successful depends on what is present in the
     * Display_config[] array of the board file.
     */
    g_SMCDisplay = Display_open(Display_Type_UART, &params);
    if (g_SMCDisplay == NULL) {
        /* Failed to open display driver */
        while(1);
    }
}


void SMCDisplay_close()
{
    Display_close(g_SMCDisplay);
}
