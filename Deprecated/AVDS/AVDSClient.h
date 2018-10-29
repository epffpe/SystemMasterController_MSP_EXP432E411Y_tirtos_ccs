/*
 * AVDSClient.h
 *
 *  Created on: Feb 6, 2018
 *      Author: epenate
 */

#ifndef AVDSCLIENT_H_
#define AVDSCLIENT_H_


#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#define AVDS_TASK_STACK_SIZE        2048
#define AVDS_TASK_PRIORITY          5

#ifdef __cplusplus
extern "C"  {
#endif

/*
 * SFDP.1
 * Signed Fixed Decimal Point. A 16 bit signed value (multiplied by 10, so 30.3 becomes 303)
 */
typedef int16_t    sfdp1_t;


/*
 * The AVDS Ethernet protocol uses a TCP socket for command and control.
 * The subnet for the AVDS is always 10.0.0.0 with a subnet mask of 255.0.0.0.
 *
 * AVDS operates with a primary node, but allows any node to take over this
 * responsibility. This means that the IP address that connects to the AVDS
 * may change from time to time. To allow the control system to select the correct
 * IP address, a UDP socket is used to periodically transmit the correct IP address and Port number.
 *
 * To make a connection, follow these steps:
 *      Insure that Ethernet port is running at address 10.100.100.100, the reserved control system IP address.
 *      Open a UDP socket on this port, listening to port 2010.
 *      Wait for a Connection Info message (See Section 2.1.1)
 *      Process the Connection Info message to get the IP address and port number of the manager.
 *      Open a TCP socket to the IP address and Port number given.
 *
 *
 */


typedef enum {
    AVDS_CMD_Set_Channel = 0x10,
    AVDS_CMD_Get_Channel = 0x11,
}teAVDSCommandsID;

typedef struct {
    uint8_t commandID;
    uint8_t commandData[];
}tsAVDSCommad;


/*
 * Connection Info Message
 * The Connection Info Message is sent periodically by the AVDS Manager to inform
 * the control system of the IP address and port of the manager and some system
 * status information. Any node can assume the role of manager in the AVDS, so
 * this message must be processed when making a new connection over Ethernet.
 *
 * The IP Address is sent as 4 bytes where each byte reflects one octet of the
 * address. The byte is sent with large endian, so the address of 10.2.3.5 would be sent as:
 *      0x0A, 0x02, 0x03, 0x05
 *
 * The Manager port number is sent as two bytes in large endian format, so a port number
 * of 2001 would be sent as:
 *      0x07, 0xD1
 * The system status byte has not yet been implemented and is not currently defined.
 * It is reserved for future use.
 *
 * The PA state returns the status of any PA events. This includes both software PA
 * events (those initiated by the control system) and hardware PA events (those triggered
 * by keyline input from third party devices).
 * The PA state is sent using one byte as the number of entries followed by 4 bytes for
 * every entry. The 4 bytes are allocated as follows:
 *      zzzzzzzz zzzzzzzz pppppppp pppppppp
 * where z refelects the zone (in large endian format) and p represents a bitmap of the PA state.
 *
 * The zone number is a 16 bit number that will match the number for a configured zone or
 * will reflect one of the reserved zone values of 0xFFFF (all channels) or 0x0000 (unassigned channels).
 * The zone is sent in large endian format, so zone 1 would be sent as:
 *      0x00, 0x01
 *
 * The PA State is a bitmapped value reflecting the active PA events in the zone.
 * The PA bits are defined as follows:
 * While there may be multiple PA events set, only the one with the highest priority is active
 * at any given time. The bits are ordered by priority, so the highest bit is the highest priority.
 */

typedef struct {
    uint16_t zone;
    union {
        uint16_t bitmapPAState;
        struct {
            uint16_t softwareChime      : 1;
            uint16_t softwareBriefer    : 1;
            uint16_t aux2PTT            : 1;
            uint16_t aux1PTT            : 1;
            uint16_t mic3PTT            : 1;
            uint16_t mic2PTT            : 1;
            uint16_t mic1PTT            : 1;
            uint16_t ordinance6         : 1;
            uint16_t ordinance5         : 1;
            uint16_t ordinance4         : 1;
            uint16_t ordinance3         : 1;
            uint16_t ordinance2         : 1;
            uint16_t ordinance1         : 1;
            uint16_t reserved           : 3;
        };
    };
}tsAVDSPAState;

typedef struct {
    uint32_t ipAddress;
    uint16_t portNumber;
    uint8_t systemStatus;
    uint8_t numPAStateEntries;
    tsAVDSPAState ptsPAStateInfo[];
}tsAVDSInfoMsg;



/*
 * Set Channel Command (0x10)
 *
 * This command is used to make a channel change.
 *
 * Send an input channel of 0 to turn off the audio or video
 * Send an input channel of 0xFFFF to leave the channel unchanged.
 */
typedef struct {
    uint16_t outputChannel;
    uint16_t audioInputChannel;
    uint16_t videoInputChannel;
}tsAVDSCommandSetChannel;

typedef struct {
    uint32_t result;
}tsAVDSCommandSetChannelResponse;

/*
 * Get Channel Command (0x11)
 *
 * This command is used to request the current audio and video channel assigned to an input channel.
 *
 * A returned input channel of 0 indicates that the audio or video is turned off.
 */

typedef struct {
    uint16_t outputChannel;
}tsAVDSCommandGetChannel;

//#pragma pack(push,1)
typedef struct {
    uint32_t result;
    uint16_t audioInputChannel;
    uint16_t videoInputChannel;
}tsAVDSCommandGetChannelResponse;
//#pragma pack(pop)

/*
 * Set Zone Command (0x12)
 *
 * This command is called to set a zone to an input channel. This performs a normal channel change; it does not activate an override.
 *
 * A zone of 0 activates all channels that are not assigned to a zone.
 * A zone of 0xFFFF assigns all channels regardless of zone.
 * An input channel of 0 turns off the audio or video.
 * An input channel of 0xFFFF leaves the channel unchanged.
 */
typedef struct {
    uint16_t zone;
    uint16_t audioInputChannel;
    uint16_t videoInputChannel;
}tsAVDSCommandSetZone;


typedef struct {
    uint32_t result;
}tsAVDSCommandSetZoneResponse;

/*
 * Set Override Command (0x13)
 *
 * This command sets an override to a single channel. See section 1.3, Override Control for more information.
 * An input channel of 0 turns off the audio or video.
 * An input channel of 0xFFFF leaves the channel unchanged.
 *
 * Volume (-80.0 to +20)
 */

typedef struct {
    uint16_t outputChannel;
    uint16_t audioInputChannel;
    uint16_t videoInputChannel;
    sfdp1_t  volume;
}tsAVDSCommandSetOverride;


typedef struct {
    uint32_t result;
}tsAVDSCommandSetOverrideResponse;

/*
 * Restore Override Command (0x14)
 *
 * This command restores an override to a single channel.
 */

typedef struct {
    uint16_t outputChannel;
}tsAVDSCommandRestoreOverride;


typedef struct {
    uint32_t result;
}tsAVDSCommandRestoreOverrideResponse;



/*
 * Set Zone Override Command (x015)
 *
 * This command sets an override to an entire zone. See section 1.3, Override Control for more information.
 *
 * A zone of 0 activates all channels that are not assigned to a zone.
 * A zone of 0xFFFF assigns all channels regardless of zone.
 *
 * An input channel of 0 turns off the audio or video.
 * An input channel of 0xFFFF leaves the channel unchanged.
 *
 * The Chime Tone can be 0 (no chime) or a chime index value of 1-6. The chime must be provided by a third
 * party device connected to the AVDS.
 */
#pragma pack(push,1)
typedef struct {
    uint16_t zone;
    uint16_t audioInputChannel;
    uint16_t videoInputChannel;
    sfdp1_t  volume;
    uint8_t chimeTone;
}tsAVDSSetZoneOverrideCommand;
#pragma pack(pop)


typedef struct {
    uint32_t result;
}tsAVDSSetZoneOverrideCommandResponse;


/*
 * Restore Zone Override Command (0x16)
 *
 * This command releases the override for all channels within the given zone.
 * A zone of 0 restores all channels that are not assigned to a zone.
 * A zone of 0xFFFF restores all channels regardless of zone.
 *
 * Channels that were not participating in an override are unaffected by this command.
 *
 */

typedef struct {
    uint16_t zone;
}tsAVDSRestoreZoneOverrideCommand;

typedef struct {
    uint32_t result;
}tsAVDSRestoreZoneOverrideCommandResponse;


/*
 * Control Properties Command
 *
 * Control properties include volume and mute. The control properties commands allow
 * access to these properties by sending a property ID and a Dword containing the value.
 *
 *      Property            Range           Description
 *      Volume (0x04)       -80.0 to +20    Treat like a SFDP.1 value, but cast to Dword
 *      Mute (0x05)         0, 1            1 is mute enabled. Cast to Dword
 *      Treble (0x06)       -6.0 to +6.0    Treat like a SFDP.1 value, but cast to Dword
 *      Bass (0x07)         -6.0 to +6.0    Treat like a SFDP.1 value, but cast to Dword
 *      Compression(0x17)   0,1             1 is compression enabled, Cast to Dword
 */

typedef enum {
    AVDS_Property_Volume = 0x04,
    AVDS_Property_Mute = 0x05,
    AVDS_Property_Treble = 0x06,
    AVDS_Property_Bass = 0x07,
    AVDS_Property_Compression = 0x17,
}teAVDSProperty;

/*
 * Set Control Property Command (0x19)
 *
 * This command is used to set any of the property values listed in Table
 */
#pragma pack(push,1)
typedef struct {
    uint16_t outputChannel;
    uint8_t  property;
    uint32_t value;
}tsAVDSSetControlPropertyCommand;
#pragma pack(pop)

typedef struct {
    uint32_t result;
}tsAVDSSetControlPropertyCommandResponse;

/*
 * Get Control Property Command (0x1A)
 *
 * This command requests the current value of a control property.
 */
#pragma pack(push,1)
typedef struct {
    uint16_t outputChannel;
    uint8_t  property;
}tsAVDSGetControlPropertyCommand;
#pragma pack(pop)

typedef struct {
    uint32_t result;
    uint32_t value;
}tsAVDSGetControlPropertyCommandResponse;


/*
 * Set Zone Control Property Command (0x1B)
 *
 * This command sets the value of a control property for an entire zone.
 * A zone of 0 activates all channels that are not assigned to a zone.
 * A zone of 0xFFFF assigns all channels regardless of zone.
 */

#pragma pack(push,1)
typedef struct {
    uint16_t zone;
    uint8_t  property;
    uint32_t value;
}tsAVDSSetZoneControlPropertyCommand;
#pragma pack(pop)


typedef struct {
    uint32_t result;
}tsAVDSSetZoneControlPropertyCommandResponse;

/*
 * Channel Properties
 *
 * Channel properties allow settings set in the configuration file to
 * be temporarily overridden. The valid properties are listed
 *
 *      Property            Range           Description
 *      Priority (0x01)     Low (0),        Allows the channel priority to be modified during runtime. This may be useful if the VIP changes seats, etc.
 *                          Normal (1),
 *                          High (2),
 *                          Highest (3)
 */

typedef enum {
    AVDS_Channel_Property_Priority = 0x01,
}teAVDSChannelProperty;

/*
 * Set Channel Property Command (0x17)
 *
 * This command sets a channel property.
 */
#pragma pack(push,1)
typedef struct {
    uint8_t  outputChannelFlag;    // 1 if output channel
    uint16_t channel;
    uint8_t  property;
    uint32_t value;
}tsAVDSSetChannelPropertyCommand;
#pragma pack(pop)

typedef struct {
    uint32_t result;
}tsAVDSSetChannelPropertyCommandResponse;


/*
 * Get Channel Property Command (0x18)
 *
 * This command requests a channel property.
 */
#pragma pack(push,1)
typedef struct {
    uint8_t  outputChannelFlag;    // 1 if output channel
    uint16_t channel;
    uint8_t  property;
}tsAVDSGetChannelPropertyCommand;
#pragma pack(pop)

typedef struct {
    uint32_t result;
    uint32_t value;
}tsAVDSGetChannelPropertyCommandResponse;

/*
 * Get Channel Information Command (0x20)
 *
 * This command retrieves the status of the requested output channel.
 */

typedef struct {
    uint32_t outputChannelNumber;
}tsAVDSGetChannelInformationCommand;


#pragma pack(push,1)
typedef struct {
    uint32_t result;
    uint16_t outputChannel;
    uint16_t audioInputChannel;
    uint16_t videoInputChannel;
    sfdp1_t  volume;
    sfdp1_t  treble;
    sfdp1_t  bass;
    uint8_t  muteActive;
    uint8_t  audioCompressionActive;
    uint16_t overrideAudioChannel;
    uint16_t overrideVideoChannel;
    sfdp1_t  overrideVolume;
}tsAVDSGetChannelInformationCommandResponse;
#pragma pack(pop)


/*
 * Set Channel Information Command (0x21)
 *
 * This command allows the control system to set all properties associated with an output channel.
 * Any channel value of 0 indicates off. This includes override, which can be set by placing a
 * non 0 value in the override channel property.
 */


#pragma pack(push,1)
typedef struct {
    uint16_t outputChannel;
    uint16_t audioInputChannel;
    uint16_t videoInputChannel;
    sfdp1_t  volume;
    sfdp1_t  treble;
    sfdp1_t  bass;
    uint8_t  muteActive;
    uint8_t  audioCompressionActive;
    uint16_t overrideAudioChannel;
    uint16_t overrideVideoChannel;
    sfdp1_t  overrideVolume;
}tsAVDSSetChannelInformationCommand;
#pragma pack(pop)


typedef struct {
    uint32_t result;
}tsAVDSSetChannelInformationCommandResponse;

/*
 * Get System Status Command (0x24)
 *
 * This command is used to retrieve the status of the AVDS. The command returns status values for
 * the entire system and status for individual nodes.
 */




#ifdef  __AVDSCLIENT_GLOBAL
    #define __AVDSCLIENT_EXT
#else
    #define __AVDSCLIENT_EXT  extern
#endif


__AVDSCLIENT_EXT Task_Handle xAVDS_init(void *params);

#ifdef __cplusplus
}
#endif



#endif /* AVDSCLIENT_H_ */
