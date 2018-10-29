/*
 * RosenBlueRayDVD.h
 *
 *  Created on: Feb 14, 2018
 *      Author: epenate
 */

#ifndef ROSENBLUERAYDVD_H_
#define ROSENBLUERAYDVD_H_




#ifdef __cplusplus
extern "C"  {
#endif




typedef enum {
    ROSENSingleBlueRayDVDCommand_Enter = 0x01,
    ROSENSingleBlueRayDVDCommand_Menu_Left = 0x02,
    ROSENSingleBlueRayDVDCommand_Menu_Right = 0x03,
    ROSENSingleBlueRayDVDCommand_Menu_Up = 0x04,
    ROSENSingleBlueRayDVDCommand_Menu_Down = 0x05,
    ROSENSingleBlueRayDVDCommand_Eject = 0x06,
    ROSENSingleBlueRayDVDCommand_Stop = 0x07,
    ROSENSingleBlueRayDVDCommand_Next_Chapter = 0x08,
    ROSENSingleBlueRayDVDCommand_Previous_Chapter = 0x09,
    ROSENSingleBlueRayDVDCommand_Subtitle_Toggle = 0x0A,
    ROSENSingleBlueRayDVDCommand_Fast_Forward = 0x0E,
    ROSENSingleBlueRayDVDCommand_Reverse = 0x0F,
    ROSENSingleBlueRayDVDCommand_Play = 0x10,
    ROSENSingleBlueRayDVDCommand_Pause = 0x11,
    ROSENSingleBlueRayDVDCommand_PlayPause_Toggle = 0x2B,
    ROSENSingleBlueRayDVDCommand_Mute = 0x12,
    ROSENSingleBlueRayDVDCommand_DVD_Menu = 0x13,
    ROSENSingleBlueRayDVDCommand_Title_Menu = 0x14,
    ROSENSingleBlueRayDVDCommand_Stereo_Volume_Up = 0x16,
    ROSENSingleBlueRayDVDCommand_Stereo_Volume_Down = 0x17,
    ROSENSingleBlueRayDVDCommand_Repeat_Title = 0x19,
    ROSENSingleBlueRayDVDCommand_Repeat_Chapter = 0x1C,
    ROSENSingleBlueRayDVDCommand_Volume_Up = 0x0B,
    ROSENSingleBlueRayDVDCommand_Volume_Down = 0x0C,
    ROSENSingleBlueRayDVDCommand_Green_Button = 0x1E,
    ROSENSingleBlueRayDVDCommand_Yellow_Button = 0x1F,
    ROSENSingleBlueRayDVDCommand_Blue_Button = 0x23,
    ROSENSingleBlueRayDVDCommand_Red_Button = 0x1D,
    ROSENSingleBlueRayDVDCommand_About = 0x26,
    ROSENSingleBlueRayDVDCommand_Back_Button = 0x25,
    ROSENSingleBlueRayDVDCommand_Set_BlueRay_To_Region_A = 0x28,
    ROSENSingleBlueRayDVDCommand_Set_BlueRay_To_Region_B = 0x29,
    ROSENSingleBlueRayDVDCommand_Set_BlueRay_To_Region_C = 0x2A,
    ROSENSingleBlueRayDVDCommand_Factory_Reset = 0x24,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_0 = 0x70,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_1 = 0x71,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_2 = 0x72,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_3 = 0x73,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_4 = 0x74,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_5 = 0x75,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_6 = 0x76,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_7 = 0x77,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_8 = 0x78,
    ROSENSingleBlueRayDVDCommand_Numeric_Value_9 = 0x79,
}teROSENSingleBlueRayDVDCommands;

typedef enum {
    ROSENDualBlueRayDVDCommand_Enter = 0x01,
    ROSENDualBlueRayDVDCommand_Menu_Left = 0x02,
    ROSENDualBlueRayDVDCommand_Menu_Right = 0x03,
    ROSENDualBlueRayDVDCommand_Menu_Up = 0x04,
    ROSENDualBlueRayDVDCommand_Menu_Down = 0x05,
    ROSENDualBlueRayDVDCommand_Eject = 0x06,
    ROSENDualBlueRayDVDCommand_Stop = 0x07,
    ROSENDualBlueRayDVDCommand_Next_Chapter = 0x08,
    ROSENDualBlueRayDVDCommand_Previous_Chapter = 0x09,
    ROSENDualBlueRayDVDCommand_Subtitle_Toggle = 0x0A,
    ROSENDualBlueRayDVDCommand_Fast_Forward = 0x0E,
    ROSENDualBlueRayDVDCommand_Reverse = 0x0F,
    ROSENDualBlueRayDVDCommand_Play = 0x10,
    ROSENDualBlueRayDVDCommand_Pause = 0x11,
    ROSENDualBlueRayDVDCommand_PlayPause_Toggle = 0x2B,
    ROSENDualBlueRayDVDCommand_Mute = 0x12,
    ROSENDualBlueRayDVDCommand_DVD_Menu = 0x13,
    ROSENDualBlueRayDVDCommand_Title_Menu = 0x14,
    ROSENDualBlueRayDVDCommand_Stereo_Volume_Up = 0x16,
    ROSENDualBlueRayDVDCommand_Stereo_Volume_Down = 0x17,
    ROSENDualBlueRayDVDCommand_Repeat_Title = 0x19,
    ROSENDualBlueRayDVDCommand_Repeat_Chapter = 0x1C,
    ROSENDualBlueRayDVDCommand_Volume_Up = 0x0B,
    ROSENDualBlueRayDVDCommand_Volume_Down = 0x0C,
    ROSENDualBlueRayDVDCommand_Green_Button = 0x1E,
    ROSENDualBlueRayDVDCommand_Yellow_Button = 0x1F,
    ROSENDualBlueRayDVDCommand_Blue_Button = 0x23,
    ROSENDualBlueRayDVDCommand_Red_Button = 0x1D,
    ROSENDualBlueRayDVDCommand_About = 0x26,
    ROSENDualBlueRayDVDCommand_Back_Button = 0x25,
    ROSENDualBlueRayDVDCommand_Set_BlueRay_To_Region_A = 0x28,
    ROSENDualBlueRayDVDCommand_Set_BlueRay_To_Region_B = 0x29,
    ROSENDualBlueRayDVDCommand_Set_BlueRay_To_Region_C = 0x2A,
    ROSENDualBlueRayDVDCommand_Factory_Reset = 0x24,
    ROSENDualBlueRayDVDCommand_Power_Toggle = 0x4F,             // the only difference with singleBlueRay
    ROSENDualBlueRayDVDCommand_Numeric_Value_0 = 0x70,
    ROSENDualBlueRayDVDCommand_Numeric_Value_1 = 0x71,
    ROSENDualBlueRayDVDCommand_Numeric_Value_2 = 0x72,
    ROSENDualBlueRayDVDCommand_Numeric_Value_3 = 0x73,
    ROSENDualBlueRayDVDCommand_Numeric_Value_4 = 0x74,
    ROSENDualBlueRayDVDCommand_Numeric_Value_5 = 0x75,
    ROSENDualBlueRayDVDCommand_Numeric_Value_6 = 0x76,
    ROSENDualBlueRayDVDCommand_Numeric_Value_7 = 0x77,
    ROSENDualBlueRayDVDCommand_Numeric_Value_8 = 0x78,
    ROSENDualBlueRayDVDCommand_Numeric_Value_9 = 0x79,
}teROSENDualBlueRayDVDCommands;

typedef enum {
    ROSSEN_BlueRayDVD_Network_Address_20 = 20,
    ROSSEN_BlueRayDVD_Network_Address_21 = 21,
    ROSSEN_BlueRayDVD_Network_Address_22 = 22,
    ROSSEN_BlueRayDVD_Network_Address_23 = 23,
    ROSSEN_BlueRayDVD_Network_Address_24 = 24,
    ROSSEN_BlueRayDVD_Network_Address_25 = 25,
    ROSSEN_BlueRayDVD_Network_Address_26 = 26,
    ROSSEN_BlueRayDVD_Network_Address_27 = 27,
}teROSENBlueRayDVDNetworkAddresses;


typedef struct {
    uint8_t discLoaded          : 1;  // 0 = disc not loaded, 1 = disc loaded
    uint8_t discPlaying         : 1;  // 0 = disc paused or stopped, 1 = disc playing
    uint8_t tempOutOfRange      : 1;  // 0 = temperature normal, 1 = temperature out of range
    uint8_t discSelfTest        : 1;  // 0 = self test passed, 1 = self test failure
    uint8_t discSourceSelection : 1;  // source selection, 0 = internal, 1 = external
    uint8_t discPAOverrideStatus: 1;  // PA/Briefing override status, 0 = input low, 1 = input high
    uint8_t discUnitPowerStatus : 1;  // Unit Power Status, 0 = standby, 1 = on
    uint8_t discAlways0         : 1;  // always 0
}tsROSENBlueRayDVDStatusResponseData;





#ifdef  __ROSENBLUERAYDVD_GLOBAL
    #define __ROSENBLUERAYDVD_EXT
#else
    #define __ROSENBLUERAYDVD_EXT  extern
#endif

__ROSENBLUERAYDVD_EXT uint32_t xROSENBlueRayDVDCreateMsgFrame(tsROSENCommand *ptsCmdBuf, uint32_t command, uint32_t address);


#ifdef __cplusplus
}
#endif



#endif /* ROSENBLUERAYDVD_H_ */
