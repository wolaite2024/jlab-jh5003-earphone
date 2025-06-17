#ifndef __APP_DMA_COMMON_H
#define __APP_DMA_COMMON_H


#include "stdint.h"


#define DMA_DEFAULT_CONN_ID     (0xff)


typedef enum
{
    APP_DMA_CONN_CMPL   = 0,
    APP_DMA_DISC_CMPL   = 1,
    APP_DMA_DATA_IND    = 2,
} APP_DMA_TRANSPORT_EVT;


typedef struct
{
    uint8_t *bd_addr;
    uint8_t  conn_id;
    union
    {
        struct
        {
            uint8_t *data;
            uint32_t data_len;
        } data_ind;

        struct
        {
            uint32_t mtu;
        } conn_cmpl;

        struct
        {
        } disc_cmpl;
    };

} APP_DMA_TRANSPORT_PARAM;


typedef void (*APP_DMA_TRANSPORT_CB)(APP_DMA_TRANSPORT_EVT evt, APP_DMA_TRANSPORT_PARAM *p_param);



#endif


