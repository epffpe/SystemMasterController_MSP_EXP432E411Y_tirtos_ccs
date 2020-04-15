/*
 * RosenRS485.h
 *
 *  Created on: Apr 6, 2020
 *      Author: epf
 */

#ifndef DEVICES_ROSEN_ROSENRS485DEVICE_H_
#define DEVICES_ROSEN_ROSENRS485DEVICE_H_


#define DEVICES_ROSEN_ROSENRS485DEVICE_TASK_STACK_SIZE        2048
#define DEVICES_ROSEN_ROSENRS485DEVICE_TASK_PRIORITY          2

#define DEVICES_ROSEN_ROSENRS485DEVICE_CLOCK_TIMEOUT        1000
#define DEVICES_ROSEN_ROSENRS485DEVICE_CLOCK_PERIOD         1000

#define ROSENRS485DEVICE_NETWORKID_MASK                     0xFF000000
#define ROSENRS485DEVICE_NETWORKID_SHIFT                    24



typedef enum {
    Rosen485_Header_Display_Power = 0x81,
    Rosen485_Header_Display_Input_Source_Selection = 0x82,
    Rosen485_Header_Display_Input_Source_Video = 0x83,
    Rosen485_Header_Rosen_Control_Functions = 0x85,
    Rosen485_Header_Universal_Lift = 0x97,
    Rosen485_Header_Universal_Lift_Status = 0x98,
    Rosen485_Header_Rosen_View = 0xA0,
    Rosen485_Header_Briefing = 0xA3,
    Rosen485_Header_Status_request = 0xA8,
    Rosen485_Header_RosenView_Status_response = 0xA9,
    Rosen485_Header_Single_Disc_DVD = 0xB0,
    Rosen485_Header_Single_BlueRay_DVD = 0xB0,
    Rosen485_Header_Dual_BlueRay_DVD = 0xB0,
    Rosen485_Header_DVD_General_Status_Request = 0xB8,
    Rosen485_Header_DVD_Status_Response = 0xB9,
    Rosen485_Header_Ping = 0x88,
    Rosen485_Header_Ping_Response = 0x77,
} Rosen485Device_ProductsHeader;


typedef enum
{
    SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_UUID = 0x0001,
    SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_UUID,
    SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_UUID,
    SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_UUID,
    SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_UUID,
    SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_UUID,
    SERVICE_ROSENRS485DEVICE_DUAL_BLUERAY_DVD_UUID,
    SERVICE_ROSENRS485DEVICE_STEVE_COMMANDS_UUID,
} Rosen485Device_service_UUID_t;


//Characteristic defines
typedef enum
{
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_GET_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ALTO_EMULATOR_SET_ID,
} Rosen485Device_service_alto_emulator_characteristics_t;

typedef enum
{
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_UP_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_UNIVERSAL_LIFT_STATUS_REQUEST_ID,
} Rosen485Device_service_universal_lift_characteristics_t;

typedef enum
{
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_POWER_ON_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_POWER_OFF_OLD_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_POWER_OFF_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_POWER_TOGGLE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_COMPOSITE_VIDEO_1_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_COMPOSITE_VIDEO_2_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_S_VIDEO_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_ANALOG_RGB_1_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_ANALOG_RGB_2_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_COMPONENT_VIDEO_1_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_COMPONENT_VIDEO_2_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_DVI_1_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_DVI_2_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_SDI_1_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_SDI_2_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_AUTO_SDI_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_NEXT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SOURCE_NEXT_SCALING_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_LEFT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_RIGHT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_MENU_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_BRIGHTNESS_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_BRIGHTNESS_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_CONTRAST_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_CONTRAST_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_HUE_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_HUE_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_SHARPNESS_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_SHARPNESS_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_SATURATION_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_SATURATION_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_SWAP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_OK_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_EXIT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_BUTTON_TECH_MENU_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_RJ45_CABLE_COMPENSATION_ON_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_RJ45_CABLE_COMPENSATION_OFF_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_OPERATION_MODE_STANDARD_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_OPERATION_MODE_AUTO_SDI_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SDI_OVERSCAN_EN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_SDI_OVERSCAN_DIS_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_INCREASE_SDI_OVERSCAN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DISPLAY_COMMANDS_DECREASE_SDI_OVERSCAN_ID,
} Rosen485Device_service_display_commands_characteristics_t;

typedef enum
{
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_ENTER_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_LEFT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_RIGHT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_MENU_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_ZOOM_IN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_CONTROL_COMMANDS_ZOOM_OUT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_1_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_2_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_3_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_4_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_5_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_6_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_7_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_8_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_9_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_10_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_11_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_12_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_13_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_14_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_15_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_PLAY_BRIEFING_16_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_BRIEFING_COMMANDS_CANCEL_BRIEFING_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_ROSEN_VIEW_STATUS_REQUEST_ID,
} Rosen485Device_service_rosen_view_characteristics_t;


typedef enum
{
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_ENTER_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_MENU_LEFT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_MENU_RIGHT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_MENU_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_MENU_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_EJECT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_STOP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_NEXT_CHAPTER_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_PREVIOUS_CHAPTER_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_SUBTITLE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_SOURCE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_FAST_FORWARD_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_REVERSE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_PLAY_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_PLAY_PAUSE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_MENU_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_DISC_DVD_CONTROL_COMMANDS_DVD_POWER_TOGGLE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_DVD_GENERAL_STATUS_REQUEST_REQUEST_GENERAL_STATUS_ID,
} Rosen485Device_service_single_disc_dvd_characteristics_t;


typedef enum
{
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_ENTER_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_MENU_LEFT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_MENU_RIGHT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_MENU_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_MENU_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_EJECT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_STOP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NEXT_CHAPTER_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_PREVIOUS_CHAPTER_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_SUBTITLE_TOGGLE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_FAST_FORWARD_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_REVERSE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_PLAY_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_PAUSE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_PLAY_PAUSE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_MUTE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_MENU_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_TITLE_MENU_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_STEREO_VOLUME_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_STEREO_VOLUME_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_REPEAT_TITLE_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_REPEAT_CHAPTER_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_REQUEST_GENERAL_STATUS_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_VOLUME_UP_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_VOLUME_DOWN_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_GREEN_BUTTON_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_YELLOW_BUTTON_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_BLUE_BUTTON_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_RED_BUTTON_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_ABOUT_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_BACK_BUTTON_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_SET_BLURAY_REGION_A_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_SET_BLURAY_REGION_B_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_SET_BLURAY_REGION_C_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_FACTORY_RESET_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_0_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_1_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_2_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_3_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_4_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_5_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_6_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_7_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_8_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_NUMERIC_VALUE_9_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_SINGLE_BLUERAY_DVD_CONTROL_COMMANDS_DVD_STATUS_RESPONSE_ID,
} Rosen485Device_service_single_blu_ray_characteristics_t;


typedef enum
{
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_POWER_SET_ID = 0x01,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_POWER_GET_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_SOURCE_SET_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_SOURCE_GET_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_CONTROL_SET_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_POWER_GET_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_LOADED_GET_ID,
    CHARACTERISTIC_SERVICE_ROSENRS485DEVICE_STEVE_COMMAND_DVD_PLAYING_GET_ID,
} Rosen485Device_service_Steve_Command_characteristics_t;








typedef enum {
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_5_6INCH_MONITOR = 0,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_8_4INCH_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_12INCH_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_15INCH_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_17INCH_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_17INCH_WS_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_20INCH_SL_II_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_24INCH_WS_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_7INCH_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_6_5INCH_MONITOR,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_UNIVERSAL_LIFT = 12,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_DVD_PLAYER,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_BLUERAY_DVD_PLAYER,
    ROSEN485DEVICE_PINGRESPONSE_DEVICEID_ROSEN_VIEW_UNIT
}Rosen485Device_pingCommandResponse_deviceIdentification;

typedef enum {
    ROSEN485DEVICE_PINGRESPONSE_BYTE2_DISPLAY_POWER_SAVE_ONLY = 1,
    ROSEN485DEVICE_PINGRESPONSE_BYTE2_DISPLAY_VIDEO_SAVE_ONLY = 2,
    ROSEN485DEVICE_PINGRESPONSE_BYTE2_DISPLAY_POWER_VIDEO_SAVE_ONLY = 3,
    ROSEN485DEVICE_PINGRESPONSE_BYTE2_DISPLAY_RS485_MASTER = 4,
    ROSEN485DEVICE_PINGRESPONSE_BYTE2_DISPLAY_OTHER = 0,  // (DVD, Universal Lift or RosenView)
}Rosen485Device_pingCommandResponse_byte2_others;


typedef struct {
    uint8_t header;
    uint8_t command;
    uint8_t address;
}Rosen485Device_directCommandData_t;

typedef struct {
    uint8_t address;
    uint8_t command;
}Rosen485Device_SteveCommandData_t;

typedef struct {
    uint8_t ui8Header;
    union {
        uint8_t ui8Byte2;
        struct {
            uint8_t ui4DeviceIdentification     : 4;
            uint8_t ui4Power                    : 4;
        };
    };
    union{
        uint8_t ui8Byte3;
        uint8_t ui8ForMonitor;
        struct {
            uint8_t ui4CurrentSource            : 4;
            uint8_t ui1PowerOnOff               : 1;
            uint8_t ui3Reserved1                : 3;
        };
        uint8_t ui8ForDvdandBluRayPlayers;
        struct {
            uint8_t ui1DiscLoaded           : 1;  // 0 = disc not loaded, 1 = disc loaded
            uint8_t ui1DiscPlaying          : 1;  // 0 = disc paused or stopped, 1 = disc playing
            uint8_t ui1TempOutOfRange       : 1;  // 0 = temperature normal, 1 = temperature out of range
            uint8_t ui1DiscSelfTest         : 1;  // 0 = self test passed, 1 = self test failure
            uint8_t ui4Reserved2            : 4;
        };
    };
}Rosen485Device_pingResponse;


typedef struct {
    uint8_t ui8Header;
    union {
        uint8_t ui8Byte2;
        uint8_t ui8Status;
        struct {
            uint8_t ui1DiscLoaded          : 1;  // 0 = disc not loaded, 1 = disc loaded
            uint8_t ui1DiscPlaying         : 1;  // 0 = disc paused or stopped, 1 = disc playing
            uint8_t ui1TempOutOfRange      : 1;  // 0 = temperature normal, 1 = temperature out of range
            uint8_t ui1DiscSelfTest        : 1;  // 0 = self test passed, 1 = self test failure
            uint8_t ui1DiscSourceSelection : 1;  // source selection, 0 = internal, 1 = external
            uint8_t ui1DiscPAOverrideStatus: 1;  // PA/Briefing override status, 0 = input low, 1 = input high
            uint8_t ui1DiscUnitPowerStatus : 1;  // Unit Power Status, 0 = standby, 1 = on
            uint8_t ui1DiscAlways0         : 1;  // always 0
        };
    };
}Rosen485Device_generalStatusResponse;


typedef struct {
    uint8_t ui8NetworkID;
    uint8_t ui8Value;
}Rosen485Device_SteveCommandReturn1Data_t;



#ifdef  __DEVICES_ROSEN_ROSENRS485DEVICE_GLOBAL
    #define __DEVICES_ROSEN_ROSENRS485DEVICE_EXT
#else
    #define __DEVICES_ROSEN_ROSENRS485DEVICE_EXT  extern
#endif


#ifdef __cplusplus
extern "C"  {
#endif



__DEVICES_ROSEN_ROSENRS485DEVICE_EXT
void vRosen485Device_Params_init(Device_Params *params, uint32_t address);


#ifdef __cplusplus
}
#endif






#endif /* DEVICES_ROSEN_ROSENRS485DEVICE_H_ */
