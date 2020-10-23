/*
 * CANTest.h
 *
 *  Created on: Oct 6, 2017
 *      Author: epenate
 */

#ifndef CANTEST_H_
#define CANTEST_H_


typedef enum {
    CANDEMO_OD_INDEX_INPUT = 0x2000,
    CANDEMO_OD_INDEX_OUTPUT = 0x2001,
}CANDEMO_OD_Index;

typedef enum {
    CANDEMO_OD_INPUT_NUMBER_OF_ENTRIES = 0,
    CANDEMO_OD_INPUT_SWITCHES,
    CANDEMO_OD_INPUT_INDICATORS,
    CANDEMO_OD_INPUT_DIP_SWITCHES,
    CANDEMO_OD_INPUT_VIRTUAL_TRIGGERS_INPUT,

    CANDEMO_OD_INPUT_SUBINDEX_COUNT,
}CANDEMO_OD_Input_subindex;


typedef enum {
    CANDEMO_OD_OUTPUT_NUMBER_OF_ENTRIES = 0,
    CANDEMO_OD_OUTPUT_RELAYS,
    CANDEMO_OD_OUTPUT_INDICATORS,
    CANDEMO_OD_OUTPUT_USER_LEDS,

    CANDEMO_OD_OUTPUT_SUBINDEX_COUNT,
}CANDEMO_OD_Output_subindex;


typedef enum {
    CANDEMO_MSK_NODEID = 0x7F,
    CANDEMO_MSK_MSGTYPE = 0x780,
}CANDEMO_masks;

typedef enum {
    CANDEMO_NMT_BOOTUP = 0,
    CANDEMO_NMT_STOPED = 4,
    CANDEMO_NMT_OPERATIONAL = 5,
    CANDEMO_NMT_PRE_OPERATIONAL = 127,
}CANDEMO_NMT_state;

typedef enum {
    CANDEMO_COIDTYPE_TPDO1 = 0x180,
    CANDEMO_COIDTYPE_RPDO1 = 0x200,
    CANDEMO_COIDTYPE_TPDO2 = 0x280,
    CANDEMO_COIDTYPE_RPDO2 = 0x300,
    CANDEMO_COIDTYPE_TPDO3 = 0x380,
    CANDEMO_COIDTYPE_RPDO3 = 0x400,
    CANDEMO_COIDTYPE_TPDO4 = 0x480,
    CANDEMO_COIDTYPE_RPDO4 = 0x500,
    CANDEMO_COIDTYPE_TSDO = 0x580, //0xB,
    CANDEMO_COIDTYPE_RSDO = 0x600, //0xC,
    CANDEMO_COIDTYPE_HEARTBEAT = 0x700, //0xC,
}CANDemo_CANOpenIDTypeNames;


typedef enum {
    CANDEMO_MSGIDTYPE_TPDO1 = (CANDEMO_COIDTYPE_TPDO1 >> 7),
    CANDEMO_MSGIDTYPE_RPDO1 = (CANDEMO_COIDTYPE_RPDO1 >> 7),
    CANDEMO_MSGIDTYPE_TPDO2 = (CANDEMO_COIDTYPE_TPDO2 >> 7),
    CANDEMO_MSGIDTYPE_RPDO2 = (CANDEMO_COIDTYPE_RPDO2 >> 7),
    CANDEMO_MSGIDTYPE_TPDO3 = (CANDEMO_COIDTYPE_TPDO3),
    CANDEMO_MSGIDTYPE_RPDO3 = (CANDEMO_COIDTYPE_RPDO3 >> 7),
    CANDEMO_MSGIDTYPE_TPDO4 = (CANDEMO_COIDTYPE_TPDO4 >> 7),
    CANDEMO_MSGIDTYPE_RPDO4 = (CANDEMO_COIDTYPE_RPDO4 >> 7),
    CANDEMO_MSGIDTYPE_TSDO = (CANDEMO_COIDTYPE_TSDO >> 7), //0xB,
    CANDEMO_MSGIDTYPE_RSDO = (CANDEMO_COIDTYPE_RSDO >> 7), //0xC,
}CANDemo_msgIDTypeNames;

typedef enum {
    CANDEMO_SDO_CCS_DOWNLOAD_REQUEST = 1,
    CANDEMO_SDO_SCS_DOWNLOAD_RESPONSE = 3,
    CANDEMO_SDO_CCS_DOWNLOAD_SEGMENT_REQUEST = 0,
    CANDEMO_SDO_SCS_DOWNLOAD_SEGMENT_RESPONSE = 1,
    CANDEMO_SDO_CCS_UPLOAD_REQUEST = 2,
    CANDEMO_SDO_SCS_UPLOAD_RESPONSE = 2,
    CANDEMO_SDO_CCS_UPLOAD_SEGMENT_REQUEST = 3,
    CANDEMO_SDO_SCS_UPLOAD_SEGMENT_RESPONSE = 0,
    CANDEMO_SDO_ABORT = 4,
}CANDEMO_clientCommandsSpecifiersSDO;

typedef struct {
    union {
        uint32_t ui32MsgID;
        struct {
            uint32_t nodeID     : 7;
            uint32_t msgType    : 4;
            uint32_t unused     : 21;
        };
    };
}CANDemo_msgID;


typedef union {
    uint8_t byte0;
    struct {
        uint8_t xReserved               :5;
        uint8_t clientCommandSpecifier  :3;
    };
}CANDemo_msgSDOClientCommandSpecifier;


typedef struct __attribute__ ((__packed__)) {
    union{
        uint8_t byte0;
        struct {
            uint8_t sizeSet                 :1;
            uint8_t expeditedTransfer       :1;
            uint8_t lengthNoData            :2;
            uint8_t xReserved               :1;
            uint8_t clientCommandSpecifier  :3;
        };
    };
    uint16_t Index;
    uint8_t subIndex;
    uint32_t expeditedData;
}CANDemo_msgSDODownloadRequest;


typedef struct __attribute__ ((__packed__)) {
    union{
        uint8_t byte0;
        struct {
            uint8_t xReserved               :5;
            uint8_t serverCommandSpecifier  :3;
        };
    };
    uint16_t Index;
    uint8_t subIndex;
    uint32_t reservedData;
}CANDemo_msgSDODownloadRequestResponse;


typedef struct __attribute__ ((__packed__)) {
    union{
        uint8_t byte0;
        struct {
            uint8_t xReserved               :5;
            uint8_t clientCommandSpecifier  :3;
        };
    };
    uint16_t Index;
    uint8_t subIndex;
    uint32_t reservedData;
}CANDemo_msgSDOUploadRequest;


typedef struct __attribute__ ((__packed__)) {
    union{
        uint8_t byte0;
        struct {
            uint8_t sizeSet                 :1;
            uint8_t expeditedTransfer       :1;
            uint8_t lengthNoData            :2;
            uint8_t xReserved               :1;
            uint8_t serverCommandSpecifier  :3;
        };
    };
    uint16_t Index;
    uint8_t subIndex;
    uint32_t expeditedData;
}CANDemo_msgSDOUploadRequestResponse;




typedef struct __attribute__ ((__packed__)) {
    uint8_t Index;
    uint8_t Value;
    union{
        uint16_t statusInput;
        struct {
            uint16_t sw0    :1;
            uint16_t sw1    :1;
            uint16_t sw2    :1;

            uint16_t ind0   :1;
            uint16_t ind1   :1;
            uint16_t ind2   :1;

            uint16_t dipMT0  :1;
            uint16_t dipMT1  :1;
            uint16_t dipMT2  :1;
            uint16_t dipMT3  :1;

            uint16_t dipMS0  :1;
            uint16_t dipMS1  :1;
            uint16_t dipMS2  :1;
            uint16_t dipMS3  :1;

            uint16_t reservedInp  :2;
        };
    };
    union{
        uint16_t statusOutput;
        struct {
            uint16_t relay0    :1;
            uint16_t relay1    :1;
            uint16_t relay2    :1;

            uint16_t slave0   :1;
            uint16_t slave1   :1;
            uint16_t slave2   :1;

            uint16_t uled0  :1;
            uint16_t uled1  :1;
            uint16_t uled2  :1;

            uint16_t reservedOut  :7;
        };
    };
}CANDemo_msgTPDO1;

typedef struct __attribute__ ((__packed__)) {
    union{
        uint8_t heartbeat;
        struct {
            uint8_t nmtState  :7;
            uint8_t reserved  :1;
        };
    };
//    uint32_t unitSerialNumber;
}CANDemo_msgHeartbeat;

#ifdef _CANTEST_GLOBALS
#define CANTEST_EXT
#else
#define CANTEST_EXT extern
#endif


#ifdef __cplusplus
extern "C"
{
#endif

CANTEST_EXT volatile uint32_t g_ui32HostErrorCount;
CANTEST_EXT volatile uint32_t g_ui32TXMsgData;

CANTEST_EXT void CANTTest_init();

#ifdef __cplusplus
}
#endif



#endif /* CANTEST_H_ */
