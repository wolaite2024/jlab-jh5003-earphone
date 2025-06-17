#ifndef __APP_DMA_RFC_H
#define __APP_DMA_RFC_H


#include "app_dma_common.h"
#include "stdbool.h"


#define DMA_RFC_UUID_128    0x51, 0xDB, 0xA1, 0x09, 0x5B, 0xA9, 0x49, 0x81,\
    0x96, 0xB7, 0x6A, 0xFE, 0x13, 0x20, 0x93, 0xDE



#define DMA_CLS_ID_LIST                         \
    SDP_UNSIGNED_TWO_BYTE,                          \
    (uint8_t)(SDP_ATTR_SRV_CLASS_ID_LIST >> 8),     \
    (uint8_t)SDP_ATTR_SRV_CLASS_ID_LIST,            \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    0x11,                                           \
    SDP_UUID128_HDR,                                \
    DMA_RFC_UUID_128


#define DMA_PROTO_DESC_LIST                     \
    SDP_UNSIGNED_TWO_BYTE,                          \
    (uint8_t)(SDP_ATTR_PROTO_DESC_LIST >> 8),       \
    (uint8_t)SDP_ATTR_PROTO_DESC_LIST,              \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    0x0c,                                           \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    03,                                             \
    SDP_UUID16_HDR,                                 \
    (uint8_t)(UUID_L2CAP >> 8),                     \
    (uint8_t)UUID_L2CAP,                            \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    0x05,                                           \
    SDP_UUID16_HDR,                                 \
    (uint8_t)(UUID_RFCOMM >> 8),                    \
    (uint8_t)UUID_RFCOMM,                           \
    SDP_UNSIGNED_ONE_BYTE,                          \
    RFC_SPP_DMA_CHANN_NUM


#define DMA_BROWSE_DESC_LIST                    \
    SDP_UNSIGNED_TWO_BYTE,                          \
    (uint8_t)(SDP_ATTR_BROWSE_GROUP_LIST >> 8),     \
    (uint8_t)SDP_ATTR_BROWSE_GROUP_LIST,            \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    0x03,                                           \
    SDP_UUID16_HDR,                                 \
    (uint8_t)(UUID_PUBLIC_BROWSE_GROUP >> 8),       \
    (uint8_t)UUID_PUBLIC_BROWSE_GROUP


#define DMA_LANG_BASE_LIST                      \
    SDP_UNSIGNED_TWO_BYTE,                          \
    (uint8_t)(SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),\
    (uint8_t)SDP_ATTR_LANG_BASE_ATTR_ID_LIST,       \
    SDP_DATA_ELEM_SEQ_HDR,                          \
    0x09,                                           \
    SDP_UNSIGNED_TWO_BYTE,                          \
    (uint8_t)(SDP_LANG_ENGLISH >> 8),               \
    (uint8_t)SDP_LANG_ENGLISH,                      \
    SDP_UNSIGNED_TWO_BYTE,                          \
    (uint8_t)(SDP_CHARACTER_UTF8 >> 8),             \
    (uint8_t)SDP_CHARACTER_UTF8,                    \
    SDP_UNSIGNED_TWO_BYTE,                          \
    (uint8_t)(SDP_BASE_LANG_OFFSET >> 8),           \
    (uint8_t)SDP_BASE_LANG_OFFSET


#define DMA_SERV_NAME                                               \
    SDP_UNSIGNED_TWO_BYTE,                                              \
    (uint8_t)((SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET) >> 8),         \
    (uint8_t)(SDP_ATTR_SRV_NAME + SDP_BASE_LANG_OFFSET),                \
    SDP_STRING_HDR,                                                     \
    0x0B,                                                               \
    0x73, 0x65, 0x72, 0x69, 0x61, 0x6c, 0x20, 0x70, 0x6f, 0x72, 0x74


void app_dma_rfc_init(APP_DMA_TRANSPORT_CB cb);


bool app_dma_rfc_write(uint8_t *bd_addr, uint8_t *data, uint32_t data_len);


#endif
