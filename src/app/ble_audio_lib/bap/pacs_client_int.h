#ifndef _PACS_CLIENT_INT_H_
#define _PACS_CLIENT_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#define LE_AUDIO_MSG_PACS_CLIENT_READ_RESULT (LE_AUDIO_MSG_GROUP_PACS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x02)
#define LE_AUDIO_MSG_PACS_CLIENT_NOTIFY      (LE_AUDIO_MSG_GROUP_PACS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x03)

typedef enum
{
    PACS_OP_SINK,
    PACS_OP_SOURCE,
    PACS_OP_ALL
} T_PACS_CCCD_OP_TYPE;

typedef struct
{
    bool     is_complete;
    uint16_t handle;
    uint16_t pac_record_len;
    uint8_t *p_record;
} T_PAC_CHAR_DATA;

typedef struct
{
    uint16_t sink_contexts;
    uint16_t source_contexts;
} T_AUDIO_CONTEXTS_DATA;

typedef union
{
    T_AUDIO_CONTEXTS_DATA contexts_data;
    uint32_t audio_locations;
    T_PAC_CHAR_DATA pac_data;
} T_PACS_DATA;

//LE_AUDIO_MSG_PACS_CLIENT_READ_RESULT
typedef struct
{
    uint16_t conn_handle;
    T_PACS_TYPE type;
    uint16_t cause;
    T_PACS_DATA data;
} T_PACS_CLIENT_READ_RESULT;

//LE_AUDIO_MSG_PACS_CLIENT_NOTIFY
typedef struct
{
    uint16_t conn_handle;
    T_PACS_TYPE type;
    T_PACS_DATA data;
} T_PACS_CLIENT_NOTIFY;

bool pacs_read_char_value(uint16_t conn_handle, T_PACS_TYPE type);
void pacs_enable_cccd(uint16_t conn_handle, T_PACS_CCCD_OP_TYPE type);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
