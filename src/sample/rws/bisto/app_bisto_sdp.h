#ifndef _APP_BISTO_SDP_H
#define _APP_BISTO_SDP_H


#include "bt_types.h"



#define UUID16_LEN                      (2)
#define UUID128_LEN                     (16)
#define UUID_DATA_ELEM_HDR_LEN          (1)

#define UUID16_DATE_ELEM_LEN            (UUID16_LEN + UUID_DATA_ELEM_HDR_LEN)
#define UUID128_DATE_ELEM_LEN           (UUID128_LEN + UUID_DATA_ELEM_HDR_LEN)

#define GENERAL_ATTR_ID_LEN             (3)

#define L2CAP_PROTO_DATA_ELEM_LEN       (2 + UUID16_DATE_ELEM_LEN)

#define L2CAP_PROTO_DATA_ELEM               \
    SDP_DATA_ELEM_SEQ_HDR,              \
    UUID16_DATE_ELEM_LEN,               \
    SDP_UUID16_HDR,                     \
    (uint8_t)(UUID_L2CAP >> 8),         \
    (uint8_t)UUID_L2CAP


#define RFC_CHANN_DATA_ELEM_LEN         (2)
#define RFC_DATA_ELEM_LEN               (UUID16_DATE_ELEM_LEN + RFC_CHANN_DATA_ELEM_LEN)
#define RFCOMM_PROTO_DATA_ELEM_LEN      (2 + RFC_DATA_ELEM_LEN)

#define RFCOMM_PROTO_DATA_ELEM          \
    SDP_DATA_ELEM_SEQ_HDR,          \
    RFC_DATA_ELEM_LEN,              \
    SDP_UUID16_HDR,                 \
    (uint8_t)(UUID_RFCOMM >> 8),    \
    (uint8_t)UUID_RFCOMM,           \
    SDP_UNSIGNED_ONE_BYTE,          \
    RFC_SPP_BISTO_CTRL_CHANN_NUM


#define BISTO_LANG_BASE_ID_LIST_ELEM_SEQ_LEN   (UUID16_DATE_ELEM_LEN * 3)


#define BISTO_LANG_ENGLISH_ELEM        \
    SDP_UNSIGNED_TWO_BYTE,              \
    (uint8_t)(SDP_LANG_ENGLISH >> 8),   \
    (uint8_t)SDP_LANG_ENGLISH


#define BISTO_LANG_CHAR_SET_ELEM       \
    SDP_UNSIGNED_TWO_BYTE,              \
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8), \
    (uint8_t)SDP_CHARACTER_UTF8


#define BISTO_LANG_OFFSET_ELEM             \
    SDP_UNSIGNED_TWO_BYTE,                  \
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),   \
    (uint8_t)SDP_BASE_LANG_OFFSET


#define BISTO_SERV_NAME_STR        "serial port"
#define BISTO_SERV_NAME_STR_LEN    12


/*******************************************************************************************************/

/* Bisto Control Class List ID Attribute */
#define BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_ID_LEN     (GENERAL_ATTR_ID_LEN)
#define BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_VAL_LEN    (2 + UUID128_DATE_ELEM_LEN)
#define BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_LEN    (BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_ID_LEN + BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_VAL_LEN)

#define BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_ID                 \
    (uint8_t)SDP_UNSIGNED_TWO_BYTE,             \
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8), \
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST


#define BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_VAL    \
    (uint8_t)SDP_DATA_ELEM_SEQ_HDR,         \
    (uint8_t)UUID128_DATE_ELEM_LEN,         \
    (uint8_t)SDP_UUID128_HDR,               \
    (uint8_t)BISTO_BT_CONTROL_SDP_UUID


#define BISTO_CTRL_SERV_CLS_ID_LIST_ATTR        \
    BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_ID,    \
    BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_VAL


/*Bisto Control Protocol Descriptor List Attribute*/
#define BISTO_CTRL_PROTO_ELEM_SEQ_LEN   (L2CAP_PROTO_DATA_ELEM_LEN + RFCOMM_PROTO_DATA_ELEM_LEN)


#define BISTO_CTRL_PROTO_DESC_ATTR_ID_LEN   (GENERAL_ATTR_ID_LEN)
#define BISTO_CTRL_PROTO_DESC_ATTR_VAL_LEN  (2 + BISTO_CTRL_PROTO_ELEM_SEQ_LEN)
#define BISTO_CTRL_PROTO_DESC_ATTR_LEN      (BISTO_CTRL_PROTO_DESC_ATTR_ID_LEN + BISTO_CTRL_PROTO_DESC_ATTR_VAL_LEN)




#define BISTO_CTRL_PROTO_DESC_ATTR_ID               \
    (uint8_t)SDP_UNSIGNED_TWO_BYTE,             \
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),   \
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST


#define BISTO_CTRL_PROTO_DESC_ATTR_VAL      \
    SDP_DATA_ELEM_SEQ_HDR,              \
    BISTO_CTRL_PROTO_ELEM_SEQ_LEN,      \
    L2CAP_PROTO_DATA_ELEM,              \
    RFCOMM_PROTO_DATA_ELEM


#define BISTO_CTRL_PROTO_DESC_ATTR          \
    BISTO_CTRL_PROTO_DESC_ATTR_ID,      \
    BISTO_CTRL_PROTO_DESC_ATTR_VAL      \



/*Bisto Control Browse Group List Attribute*/

#define BISTO_CTRL_GROUP_LIST_ATTR_ID_LEN   (GENERAL_ATTR_ID_LEN)
#define BISTO_CTRL_GROUP_LIST_ATTR_VAL_LEN  (2 + UUID16_DATE_ELEM_LEN)
#define BISTO_CTRL_GROUP_LIST_ATTR_LEN  (BISTO_CTRL_GROUP_LIST_ATTR_ID_LEN + BISTO_CTRL_GROUP_LIST_ATTR_VAL_LEN)


#define BISTO_CTRL_GROUP_LIST_ATTR_ID               \
    SDP_UNSIGNED_TWO_BYTE,                      \
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8), \
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST         \


#define BISTO_CTRL_GROUP_LIST_ATTR_VAL              \
    SDP_DATA_ELEM_SEQ_HDR,                      \
    UUID16_DATE_ELEM_LEN,                       \
    SDP_UUID16_HDR,                             \
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),   \
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP


#define BISTO_CTRL_GROUP_LIST_ATTR      \
    BISTO_CTRL_GROUP_LIST_ATTR_ID,  \
    BISTO_CTRL_GROUP_LIST_ATTR_VAL


/*Bisto Control Language Base Attribute ID List Attribute*/

#define BISTO_CTRL_LANG_BASE_ID_LIST_ELEM_SEQ_LEN   BISTO_LANG_BASE_ID_LIST_ELEM_SEQ_LEN


#define BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_ID_LEN    (GENERAL_ATTR_ID_LEN)
#define BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_VAL_LEN   (2 + BISTO_CTRL_LANG_BASE_ID_LIST_ELEM_SEQ_LEN)
#define BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_LEN       (BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_ID_LEN + BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_VAL_LEN)


#define BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_ID                \
    SDP_UNSIGNED_TWO_BYTE,                              \
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),    \
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST


#define BISTO_CTRL_LANG_ENGLISH_ELEM        BISTO_LANG_ENGLISH_ELEM



#define BISTO_CTRL_LANG_CHAR_SET_ELEM       BISTO_LANG_CHAR_SET_ELEM



#define BISTO_CTRL_LANG_OFFSET_ELEM         BISTO_LANG_CHAR_SET_ELEM



#define BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_VAL       \
    SDP_DATA_ELEM_SEQ_HDR,                      \
    BISTO_CTRL_LANG_BASE_ID_LIST_ELEM_SEQ_LEN,  \
    BISTO_CTRL_LANG_ENGLISH_ELEM,               \
    BISTO_CTRL_LANG_CHAR_SET_ELEM,              \
    BISTO_CTRL_LANG_OFFSET_ELEM


#define BISTO_CTRL_LANG_BASE_ID_LIST_ATTR       \
    BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_ID,    \
    BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_VAL


/* Bisto Control BT Profile Description List Attribute */
#define BISTO_CTRL_SPP_PROFILE_DESC_ELEM_SEQ_LEN        (UUID16_DATE_ELEM_LEN + 3)
#define BISTO_CTRL_BT_PROFILE_DESC_LIST_ELEM_SEQ_LEN    (BISTO_CTRL_SPP_PROFILE_DESC_ELEM_SEQ_LEN + 2)

#define BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_ID_LEN     (GENERAL_ATTR_ID_LEN)
#define BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_VAL_LEN    (2 + BISTO_CTRL_BT_PROFILE_DESC_LIST_ELEM_SEQ_LEN)
#define BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_LEN    (BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_ID_LEN + BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_VAL_LEN)


#define BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_ID     \
    SDP_UNSIGNED_TWO_BYTE,                      \
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8), \
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST


#define BISTO_CTRL_SPP_PROFILE_DESC_ELEM            \
    SDP_DATA_ELEM_SEQ_HDR,                      \
    BISTO_CTRL_SPP_PROFILE_DESC_ELEM_SEQ_LEN,   \
    SDP_UUID16_HDR,                             \
    (uint8_t)(UUID_SERIAL_PORT >> 8),           \
    (uint8_t)UUID_SERIAL_PORT,                  \
    SDP_UNSIGNED_TWO_BYTE,                      \
    0x01,                                       \
    0x02
//version 1.2


#define BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_VAL        \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    BISTO_CTRL_BT_PROFILE_DESC_LIST_ELEM_SEQ_LEN,   \
    BISTO_CTRL_SPP_PROFILE_DESC_ELEM                \


#define BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR        \
    BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_ID,    \
    BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_VAL    \


/*Bisto Control Service Name Attribute*/
#define BISTO_CTRL_SERV_NAME_STR        BISTO_SERV_NAME_STR
#define BISTO_CTRL_SERV_NAME_STR_LEN    BISTO_SERV_NAME_STR_LEN


#define BISTO_CTRL_SERV_NAME_ATTR_ID_LEN    (3)
#define BISTO_CTRL_SERV_NAME_ATTR_VAL_LEN   (BISTO_CTRL_SERV_NAME_STR_LEN + 2)
#define BISTO_CTRL_SERV_NAME_ATTR_LEN       (BISTO_CTRL_SERV_NAME_ATTR_ID_LEN + BISTO_CTRL_SERV_NAME_ATTR_VAL_LEN)



#define BISTO_CTRL_SERV_NAME_ATTR_ID                                \
    SDP_UNSIGNED_TWO_BYTE,                                      \
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8), \
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET)


#define BISTO_CTRL_SERV_NAME_ATTR_VAL       \
    SDP_STRING_HDR,                     \
    BISTO_CTRL_SERV_NAME_STR_LEN,       \
    BISTO_CTRL_SERV_NAME_STR[0],        \
    BISTO_CTRL_SERV_NAME_STR[1],        \
    BISTO_CTRL_SERV_NAME_STR[2],        \
    BISTO_CTRL_SERV_NAME_STR[3],        \
    BISTO_CTRL_SERV_NAME_STR[4],        \
    BISTO_CTRL_SERV_NAME_STR[5],        \
    BISTO_CTRL_SERV_NAME_STR[6],        \
    BISTO_CTRL_SERV_NAME_STR[7],        \
    BISTO_CTRL_SERV_NAME_STR[8],        \
    BISTO_CTRL_SERV_NAME_STR[9],        \
    BISTO_CTRL_SERV_NAME_STR[10],       \
    BISTO_CTRL_SERV_NAME_STR[11]



#define BISTO_CTRL_SERV_NAME_ATTR       \
    BISTO_CTRL_SERV_NAME_ATTR_ID,    \
    BISTO_CTRL_SERV_NAME_ATTR_VAL




/*Bisto Control Service Record*/

#define BISTO_CTRL_SERV_RECORD_ELEM_SEQ_LEN             \
    (                                                   \
                                                        BISTO_CTRL_SERV_CLS_ID_LIST_ATTR_LEN        +   \
                                                        BISTO_CTRL_PROTO_DESC_ATTR_LEN              +   \
                                                        BISTO_CTRL_GROUP_LIST_ATTR_LEN              +   \
                                                        BISTO_CTRL_LANG_BASE_ID_LIST_ATTR_LEN       +   \
                                                        BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR_LEN    +   \
                                                        BISTO_CTRL_SERV_NAME_ATTR_LEN                   \
    )


#if 0
#define BISTO_CTRL_SERV_RECORD              \
    SDP_DATA_ELEM_SEQ_HDR,                  \
    BISTO_CTRL_SERV_RECORD_ELEM_SEQ_LEN,    \
    BISTO_CTRL_SERV_CLS_ID_LIST_ATTR,       \
    BISTO_CTRL_PROTO_DESC_ATTR,             \
    BISTO_CTRL_GROUP_LIST_ATTR,             \
    BISTO_CTRL_LANG_BASE_ID_LIST_ATTR,      \
    BISTO_CTRL_BT_PROFILE_DESC_LIST_ATTR,   \
    BISTO_CTRL_SERV_NAME_ATTR
#endif


/*******************************************************************************************************/

/* Bisto Audio Class List ID Attribute */

#define BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_ID_LEN    GENERAL_ATTR_ID_LEN
#define BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_VAL_LEN   (2 + UUID128_DATE_ELEM_LEN)
#define BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_LEN       (BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_ID_LEN + BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_VAL_LEN)


#define BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_ID        \
    SDP_UNSIGNED_TWO_BYTE,                      \
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8), \
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST


#define BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_VAL   \
    SDP_DATA_ELEM_SEQ_HDR,                  \
    UUID128_DATE_ELEM_LEN,                  \
    SDP_UUID128_HDR,                        \
    BISTO_BT_AUDIO_SDP_UUID


#define BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR       \
    BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_ID,   \
    BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_VAL


/*Bisto Audio Protocol Descriptor List Attribute*/
#define BISTO_AUDIO_PROTO_ELEM_SEQ_LEN      L2CAP_PROTO_DATA_ELEM_LEN + RFCOMM_PROTO_DATA_ELEM_LEN

#define BISTO_AUDIO_PROTO_DESC_ATTR_ID_LEN      GENERAL_ATTR_ID_LEN
#define BISTO_AUDIO_PROTO_DESC_ATTR_VAL_LEN     (2 + BISTO_AUDIO_PROTO_ELEM_SEQ_LEN)
#define BISTO_AUDIO_PROTO_DESC_ATTR_LEN         (BISTO_AUDIO_PROTO_DESC_ATTR_ID_LEN + BISTO_AUDIO_PROTO_DESC_ATTR_VAL_LEN)

#define BISTO_AUDIO_PROTO_DESC_ATTR_ID              \
    SDP_UNSIGNED_TWO_BYTE,                      \
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),   \
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST


#define BISTO_AUDIO_PROTO_DESC_ATTR_VAL     \
    SDP_DATA_ELEM_SEQ_HDR,              \
    BISTO_AUDIO_PROTO_ELEM_SEQ_LEN,     \
    L2CAP_PROTO_DATA_ELEM,              \
    RFCOMM_PROTO_DATA_ELEM



#define BISTO_AUDIO_PROTO_DESC_ATTR     \
    BISTO_AUDIO_PROTO_DESC_ATTR_ID, \
    BISTO_AUDIO_PROTO_DESC_ATTR_VAL


/*Bisto Audio Browse Group List Attribute*/
#define BISTO_AUDIO_GROUP_LIST_ATTR_ID_LEN      GENERAL_ATTR_ID_LEN
#define BISTO_AUDIO_GROUP_LIST_ATTR_VAL_LEN     (2 + UUID16_DATE_ELEM_LEN)
#define BISTO_AUDIO_GROUP_LIST_ATTR_LEN         (BISTO_AUDIO_GROUP_LIST_ATTR_ID_LEN + BISTO_AUDIO_GROUP_LIST_ATTR_VAL_LEN)

#define BISTO_AUDIO_GROUP_LIST_ATTR_ID              \
    SDP_UNSIGNED_TWO_BYTE,                      \
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8), \
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST


#define BISTO_AUDIO_GROUP_LIST_ATTR_VAL             \
    SDP_DATA_ELEM_SEQ_HDR,                      \
    UUID16_DATE_ELEM_LEN,                       \
    SDP_UUID16_HDR,                             \
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),   \
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP


#define BISTO_AUDIO_GROUP_LIST_ATTR     \
    BISTO_AUDIO_GROUP_LIST_ATTR_ID, \
    BISTO_AUDIO_GROUP_LIST_ATTR_VAL


/*Bisto Control Language Base Attribute ID List Attribute*/
#define BISTO_AUDIO_LANG_BASE_ID_LIST_ELEM_SEQ_LEN   BISTO_LANG_BASE_ID_LIST_ELEM_SEQ_LEN

#define BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_ID_LEN       GENERAL_ATTR_ID_LEN
#define BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_VAL_LEN      (2 + BISTO_AUDIO_LANG_BASE_ID_LIST_ELEM_SEQ_LEN)
#define BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_LEN          (BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_ID_LEN + BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_VAL_LEN)


#define BISTO_AUDIO_LANG_ENGLISH_ELEM       BISTO_LANG_ENGLISH_ELEM

#define BISTO_AUDIO_LANG_CHAR_SET_ELEM      BISTO_LANG_CHAR_SET_ELEM

#define BISTO_AUDIO_LANG_OFFSET_ELEM        BISTO_LANG_OFFSET_ELEM


#define BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_ID               \
    SDP_UNSIGNED_TWO_BYTE,                              \
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),    \
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST


#define BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_VAL      \
    SDP_DATA_ELEM_SEQ_HDR,                      \
    BISTO_AUDIO_LANG_BASE_ID_LIST_ELEM_SEQ_LEN, \
    BISTO_AUDIO_LANG_ENGLISH_ELEM,              \
    BISTO_AUDIO_LANG_CHAR_SET_ELEM,             \
    BISTO_AUDIO_LANG_OFFSET_ELEM


#define BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR      \
    BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_ID,  \
    BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_VAL


/* Bisto Control BT Profile Description List Attribute */
#define BISTO_AUDIO_SPP_PROFILE_DESC_ELEM_SEQ_LEN        (UUID16_DATE_ELEM_LEN + 3)
#define BISTO_AUDIO_BT_PROFILE_DESC_LIST_ELEM_SEQ_LEN    (BISTO_AUDIO_SPP_PROFILE_DESC_ELEM_SEQ_LEN + 2)

#define BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_ID_LEN        GENERAL_ATTR_ID_LEN
#define BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_VAL_LEN       (2 + BISTO_AUDIO_BT_PROFILE_DESC_LIST_ELEM_SEQ_LEN)
#define BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_LEN           (BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_ID_LEN + BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_VAL_LEN)


#define BISTO_AUDIO_SPP_PROFILE_DESC_ELEM           \
    SDP_DATA_ELEM_SEQ_HDR,                      \
    BISTO_AUDIO_SPP_PROFILE_DESC_ELEM_SEQ_LEN,  \
    SDP_UUID16_HDR,                             \
    (uint8_t)(UUID_SERIAL_PORT >> 8),           \
    (uint8_t)UUID_SERIAL_PORT,                  \
    SDP_UNSIGNED_TWO_BYTE,                      \
    0x01,                                       \
    0x02
//version 1.2


#define BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_ID    \
    SDP_UNSIGNED_TWO_BYTE,                      \
    (uint8_t)(SDP_ATTR_PROFILE_DESC_LIST >> 8), \
    (uint8_t)SDP_ATTR_PROFILE_DESC_LIST


#define BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_VAL       \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    BISTO_AUDIO_BT_PROFILE_DESC_LIST_ELEM_SEQ_LEN,  \
    BISTO_AUDIO_SPP_PROFILE_DESC_ELEM



#define BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR       \
    BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_ID,   \
    BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_VAL


/*Bisto Control Service Name Attribute*/
#define BISTO_AUDIO_SERV_NAME_STR        BISTO_SERV_NAME_STR
#define BISTO_AUDIO_SERV_NAME_STR_LEN    BISTO_SERV_NAME_STR_LEN

#define BISTO_AUDIO_SERV_NAME_ATTR_ID_LEN       GENERAL_ATTR_ID_LEN
#define BISTO_AUDIO_SERV_NAME_ATTR_VAL_LEN      (2 + BISTO_SERV_NAME_STR_LEN)
#define BISTO_AUDIO_SERV_NAME_ATTR_LEN          (BISTO_AUDIO_SERV_NAME_ATTR_ID_LEN + BISTO_AUDIO_SERV_NAME_ATTR_VAL_LEN)

#define BISTO_AUDIO_SERV_NAME_ATTR_ID                               \
    SDP_UNSIGNED_TWO_BYTE,                                      \
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8), \
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET)


#define BISTO_AUDIO_SERV_NAME_ATTR_VAL                  \
    SDP_STRING_HDR,                 \
    BISTO_AUDIO_SERV_NAME_STR_LEN,  \
    BISTO_AUDIO_SERV_NAME_STR[0],   \
    BISTO_AUDIO_SERV_NAME_STR[1],   \
    BISTO_AUDIO_SERV_NAME_STR[2],   \
    BISTO_AUDIO_SERV_NAME_STR[3],   \
    BISTO_AUDIO_SERV_NAME_STR[4],   \
    BISTO_AUDIO_SERV_NAME_STR[5],   \
    BISTO_AUDIO_SERV_NAME_STR[6],   \
    BISTO_AUDIO_SERV_NAME_STR[7],   \
    BISTO_AUDIO_SERV_NAME_STR[8],   \
    BISTO_AUDIO_SERV_NAME_STR[9],   \
    BISTO_AUDIO_SERV_NAME_STR[10],  \
    BISTO_AUDIO_SERV_NAME_STR[11]



#define BISTO_AUDIO_SERV_NAME_ATTR      \
    BISTO_AUDIO_SERV_NAME_ATTR_ID,  \
    BISTO_AUDIO_SERV_NAME_ATTR_VAL




#define BISTO_AUDIO_SERV_RECORD_ELEM_SEQ_LEN            \
    (                                                   \
                                                        BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR_LEN        +  \
                                                        BISTO_AUDIO_PROTO_DESC_ATTR_LEN              +  \
                                                        BISTO_AUDIO_GROUP_LIST_ATTR_LEN              +  \
                                                        BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR_LEN       +  \
                                                        BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR_LEN    +  \
                                                        BISTO_AUDIO_SERV_NAME_ATTR_LEN                  \
    )



#define BISTO_AUDIO_SERV_RECORD                 \
    SDP_DATA_ELEM_SEQ_HDR,                  \
    BISTO_AUDIO_SERV_RECORD_ELEM_SEQ_LEN,   \
    BISTO_AUDIO_SERV_CLS_ID_LIST_ATTR,      \
    BISTO_AUDIO_PROTO_DESC_ATTR,            \
    BISTO_AUDIO_GROUP_LIST_ATTR,            \
    BISTO_AUDIO_LANG_BASE_ID_LIST_ATTR,     \
    BISTO_AUDIO_BT_PROFILE_DESC_LIST_ATTR,  \
    BISTO_AUDIO_SERV_NAME_ATTR

#endif




