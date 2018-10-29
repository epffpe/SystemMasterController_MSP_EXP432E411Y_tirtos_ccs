/*
 * Rosen.h
 *
 *  Created on: Feb 13, 2018
 *      Author: epenate
 */

#ifndef ROSEN_H_
#define ROSEN_H_

#define ROSEN_TASK_STACK_SIZE        512
#define ROSEN_TASK_PRIORITY          2


#ifdef __cplusplus
extern "C"  {
#endif



typedef enum {
    Rosen_Header_Display_Power = 0x81,
    Rosen_Header_Display_Input_Source_Selection = 0x82,
    Rosen_Header_Display_Input_Source_Video = 0x83,
    Rosen_Header_Rosen_Control_Functions = 0x85,
    Rosen_Header_Universal_Lift = 0x97,
    Rosen_Header_Rosen_View = 0xA0,
    Rosen_Header_Briefing = 0xA3,
    Rosen_Header_Status_request = 0xA8,
    Rosen_Header_RosenView_Status_response = 0xA9,
    Rosen_Header_Single_Disc_DVD = 0xB0,
    Rosen_Header_Single_BlueRay_DVD = 0xB0,
    Rosen_Header_Dual_BlueRay_DVD = 0xB0,
    Rosen_Header_DVD_General_Status_Request = 0xB8,
    Rosen_Header_DVD_Status_Response = 0xB9,
    Rosen_Header_Ping = 0x88,
    Rosen_Header_Ping_Response = 0x77,
}teROSENProductsHeader;



typedef struct {
    char header;
    char command;
    char address;
}tsROSENCommand;

#ifdef  __ROSEN_GLOBAL
    #define __ROSEN_EXT
#else
    #define __ROSEN_EXT  extern
#endif


__ROSEN_EXT Task_Handle xROSEN_init(void *params);
__ROSEN_EXT uint32_t xROSENCreateMsgFrame(tsROSENCommand *ptsCmdBuf, uint8_t header, uint8_t command, uint8_t address);



#ifdef __cplusplus
}
#endif




#endif /* ROSEN_H_ */
