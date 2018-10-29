/*
 * SMCDisplay.h
 *
 *  Created on: Oct 24, 2018
 *      Author: epenate
 */

#ifndef DISPLAY_SMCDISPLAY_H_
#define DISPLAY_SMCDISPLAY_H_


#ifdef SMCDISPLAY_GLOBALS
#define SMCDISPLAY_EXT
#else
#define SMCDISPLAY_EXT extern
#endif




#ifdef __cplusplus
extern "C"
{
#endif

SMCDISPLAY_EXT Display_Handle g_SMCDisplay;

SMCDISPLAY_EXT void SMCDisplay_init();


#ifdef __cplusplus
}
#endif






#endif /* DISPLAY_SMCDISPLAY_H_ */
