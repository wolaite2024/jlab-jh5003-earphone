#ifndef __USB_AUDIO2_DESC_H__
#define __USB_AUDIO2_DESC_H__
#include <stdint.h>

#define UAC2_DS_INTERVAL                 3

#define IP_VERSION_02_00                 0x20

#define UAC2_FUNCTION_SUBCLASS_UNDEFINED 0x00

#define UAC2_FUNCTION_PROTOCAL_UNDEFINED 0x00
#define UAC2_AF_VERSION_02_00           IP_VERSION_02_00

#define UAC2_CLASS_CODE_AUDIO           0x01

#define UAC2_SUBCLASS_UNDEFINED          0x00
#define UAC2_SUBCLASS_AUDIOCONTROL       0x01
#define UAC2_SUBCLASS_AUDIOSTREAMING     0x02
#define UAC2_SUBCLASS_MIDISTREAMING      0x03

#define UAC2_CATEGORY_DESKTOP_SPEAKER   0x01
#define UAC2_CATEGORY_HEADSET           0x04
#define UAC2_CATEGORY_CONVERTER         0x06

#define UAC2_CATEGORY_IO_BOX            0x08
#define UAC2_CATEGORY_PRO_AUDIO         0x0A

#define UAC2_DT_CS_UNDEFINE                0x20
#define UAC2_DT_CS_DEVICE                  0x21
#define UAC2_DT_CS_CONFIGURATION           0x22
#define UAC2_DT_CS_STRING                  0x23
#define UAC2_DT_CS_INTERFACE               0x24
#define UAC2_DT_CS_ENDPOINT                0x25

#define UAC2_DT_DEVICE                  USB_DT_DEVICE
#define UAC2_DT_CONFIG                  USB_DT_CONFIG
#define UAC2_DT_STRING                  USB_DT_STRING
#define UAC2_DT_INTERFACE               USB_DT_INTERFACE
#define UAC2_DT_ENDPOINT                USB_DT_ENDPOINT
#define UAC2_DT_DEVICE_QUALIFIER        USB_DT_DEVICE_QUALIFIER
#define UAC2_DT_OTHER_SPEED_CONFIG      USB_DT_OTHER_SPEED_CONFIG
#define UAC2_DT_INTERFACE_POWER         USB_DT_INTERFACE_POWER
#define UAC2_DT_OTG                     USB_DT_OTG
#define UAC2_DT_DEBUG                   USB_DT_DEBUG
#define UAC2_DT_INTERFACE_ASSOCIATION   USB_DT_INTERFACE_ASSOCIATION

#define UAC2_AC_DESC_UNDEFINED          0x00
#define UAC2_HEADER                     0x01
#define UAC2_INPUT_TERMINAL             0x02
#define UAC2_OUTPUT_TERMINAL            0x03
#define UAC2_MIXER_UNIT                 0x04
#define UAC2_SELECTOR_UNIT              0x05
#define UAC2_FEATURE_UNIT               0x06
#define UAC2_EFFECT_UNIT                0x07
#define UAC2_PROCESSING_UNIT            0x08
#define UAC2_EXTENSION_UNIT             0x09
#define UAC2_CLOCK_SOURCE               0x0A
#define UAC2_CLOCK_SELECTOR             0x0B
#define UAC2_CLOCK_MULTIPLIER           0x0C
#define UAC2_SAM_RATE_CONVERTER         0x0D

#define UAC2_FU_CONTROL_UNDEFINED         0x00
#define UAC2_FU_MUTE_CONTROL              0x01
#define UAC2_FU_VOLUME_CONTROL            0x02
#define UAC2_FU_BASS_CONTROL              0x03
#define UAC2_FU_MID_CONTROL               0x04
#define UAC2_FU_TREBLE_CONTROL            0x05
#define UAC2_FU_GRAPHIC_EQUALIZER_CONTROL 0x06
#define UAC2_FU_AUTOMATIC_GAIN_CONTROL    0x07
#define UAC2_FU_DELAY_CONTROL             0x08
#define UAC2_FU_BASS_BOOST_CONTROL        0x09
#define UAC2_FU_LOUDNESS_CONTROL          0x0A

#define UAC2_TERMINAL_UNDEFINED          0x100
#define UAC2_TERMINAL_STREAMING          0x101
#define UAC2_TERMINAL_VENDOR_SPEC        0x1FF

#define UAC2_INPUT_TERMINAL_UNDEFINED                    0x200
#define UAC2_INPUT_TERMINAL_MICROPHONE                   0x201
#define UAC2_INPUT_TERMINAL_DESKTOP_MICROPHONE           0x202
#define UAC2_INPUT_TERMINAL_PERSONAL_MICROPHONE          0x203
#define UAC2_INPUT_TERMINAL_OMNI_DIR_MICROPHONE          0x204
#define UAC2_INPUT_TERMINAL_MICROPHONE_ARRAY             0x205
#define UAC2_INPUT_TERMINAL_PROC_MICROPHONE_ARRAY        0x206

#define UAC2_OUTPUT_TERMINAL_UNDEFINED                   0x300
#define UAC2_OUTPUT_TERMINAL_SPEAKER                     0x301
#define UAC2_OUTPUT_TERMINAL_HEADPHONES                  0x302
#define UAC2_OUTPUT_TERMINAL_HEAD_MOUNTED_DISPLAY_AUDIO  0x303
#define UAC2_OUTPUT_TERMINAL_DESKTOP_SPEAKER             0x304
#define UAC2_OUTPUT_TERMINAL_ROOM_SPEAKER                0x305
#define UAC2_OUTPUT_TERMINAL_COMMUNICATION_SPEAKER       0x306
#define UAC2_OUTPUT_TERMINAL_LOW_FREQ_EFFECTS_SPEAKER    0x307

#define UAC2_AS_DESC_UNDEFINED           0x00
#define UAC2_AS_GENERAL                  0x01
#define UAC2_FORMAT_TYPE                 0x02

#define UAC2_FORMAT_TYPE_I_PCM           0x01

#define UAC2_CS_CONTROL_UNDEFINED        0x00
#define UAC2_CS_SAM_FREQ_CONTROL         0x01
#define UAC2_CS_CLOCK_VALID_CONTROL      0x02

#define UAC2_REQUEST_CODE_UNDEFINED      0x00
#define UAC2_CUR                         0x01
#define UAC2_RANGE                       0x02

#define UAC2_DIR_OUT                     EP_DIR_OUT
#define UAC2_DIR_IN                      EP_DIR_IN

#define UAC2_EP_XFER_CONTROL            EP_XFER_CONTROL
#define UAC2_EP_XFER_ISOC               EP_XFER_ISOC
#define UAC2_EP_XFER_BULK               EP_XFER_BULK
#define UAC2_EP_XFER_INT                EP_XFER_INT

#define UAC2_EP_SYNCTYPE                EP_SYNCTYPE
#define UAC2_EP_SYNC_NONE               EP_SYNC_NONE
#define UAC2_EP_SYNC_ASYNC              EP_SYNC_ASYNC
#define UAC2_EP_SYNC_ADAPTIVE           EP_SYNC_ADAPTIVE
#define UAC2_EP_SYNC_SYNC               EP_SYNC_SYNC

#define UAC2_EP_GENERAL                 0x01

#define UAC2_ISO_OUT_ENDPOINT_ADDRESS    0x03
#define UAC2_ISO_IN_ENDPOINT_ADDRESS     0x83

#define ID_CLOCK_SOURCE1                 0x20
#define ID_CLOCK_SOURCE2                 0x21
#define ID_FEATURE_UNIT                  0x22
#define ID_INPUT_TERMINAL1               0x23
#define ID_OUTPUT_TERMINAL1              0x24
#define ID_CLOCK_SELECTOR                0x25


#define ID_INPUT_TERMINAL2               0x26
#define ID_OUTPUT_TERMINAL2              0x27
#define ID_FEATURE_UNIT2                 0x28

#pragma pack(push,1)


typedef struct _uac2_ac_if_hdr_desc
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint16_t bcdADC;
    uint8_t  bCategory;
    uint16_t wTotalLength;
    uint8_t  bmControls;
} T_UAC2_AC_IF_HDR_DESC;

typedef struct _uac2_clk_src_desc
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bClockID;
    uint8_t  bmAttributes;
    uint8_t  bmControls;
    uint8_t  bAssocTerminal;
    uint8_t  iClockSource;
} T_UAC2_CLK_SRC_DESC;


typedef struct _uac2_clk_sel_desc
{
    uint8_t  bLength;
    uint8_t  bDescriptorType;
    uint8_t  bDescriptorSubtype;
    uint8_t  bClockID;
    uint8_t  bNrInPins;
    uint8_t  baCSourcreID[1];
    uint8_t  bmControls;
    uint8_t  iClockSlector;

} T_UAC2_CLK_SEL_DESC;

typedef struct _uac2_fu_desc
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bUnitID;
    uint8_t   bSourceID;
    uint32_t  bmaControls[3];
    uint8_t   iFeature;
} T_UAC2_FU_DESC;

typedef struct _uac2_fu_desc_mic
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bUnitID;
    uint8_t   bSourceID;
    uint32_t  bmaControls[3];
    uint8_t   iFeature;
} T_UAC2_FU_DESC_MIC;

typedef struct _uac2_it_desc
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bTerminalID;
    uint16_t  wTerminalType;
    uint8_t   bAssocTerminal;
    uint8_t   bCSourceID;
    uint8_t   bNrChannels;
    uint32_t  bmChannelConfig;
    uint8_t   iChannelNames;
    uint16_t  bmControls;
    uint8_t   iTerminal;
} T_UAC2_IT_DESC;

typedef struct _uac2_ot_desc
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bTerminalID;
    uint16_t  wTerminalType;
    uint8_t   bAssocTerminal;
    uint8_t   bSourceID;
    uint8_t   bCSourceID;
    uint16_t  bmControls;
    uint8_t   iTerminal;
} T_UAC2_OT_DESC;

typedef struct _uac2_as_if_desc
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bTerminalLink;
    uint8_t   bmControls;
    uint8_t   bFormatType;
    uint32_t  bmFormats;
    uint8_t   bNrChannels;
    uint32_t  bmChannelConfig;
    uint8_t   iChannelNames;
} T_UAC2_AS_IF_DESC;

typedef struct _uac2_as_type_1_fmt_desc
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bFormatType;
    uint8_t   bSubslotSize;
    uint8_t   bBitResolution;
} T_UAC2_AS_TYPE_1_FMT_DESC;

typedef struct _uac2_as_iso_ep_desc
{
    uint8_t   bLength;
    uint8_t   bDescriptorType;
    uint8_t   bDescriptorSubtype;
    uint8_t   bmAttributes;
    uint8_t   bmControls;
    uint8_t   bLockDelayUnits;
    uint16_t  wLockDelay;
} T_UAC2_AS_ISO_EP_DESC;

#define T_UAC2_STD_IA_DESC      T_USB_IA_DESC
#define T_UAC2_STD_IF_DESC      T_USB_IF_DESC
#define T_UAC2_STD_EP_DESC      T_USB_EP_DESC
#pragma pack(pop)

#endif
