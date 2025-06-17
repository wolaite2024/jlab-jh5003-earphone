/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     ots_server.c
* @brief    Object Transfer service source file.
* @details  Interfaces to access Media Control service.
* @author   cheng_cai
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include "string.h"
#include "trace.h"
#include "bt_gatt_svc.h"
#include "ots_server.h"
#include "ble_link_util.h"
#include "ots_coc.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"


#if LE_AUDIO_OTS_SERV_SUPPORT
#define OTS_SERV_OACP_CCCD_FLAG        0x1000
#define OTS_SERV_OLCP_CCCD_FLAG        0x2000
#define OTS_SERV_OBJ_CHG_CCCD_FLAG     0x4000

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static T_OTS_SERV_CB *p_ots_mem = NULL;
static uint8_t total_ots_num = 0;
static T_OS_QUEUE  *p_cl_queue = NULL;
static T_APP_RESULT ots_serv_coc_callback(uint8_t coc_type, void *p_coc_data, uint8_t service_id);

static void ind_change_to_clients(T_OTS_SERV_CB *p_entry, uint16_t conn_handle, uint8_t *value,
                                  uint16_t len);

/*----------------- Object Transfer Service -------------------*/
static const T_ATTRIB_APPL ots_prim_srv[] =
{
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_SECONDARY_SERVICE),
            HI_WORD(GATT_UUID_SECONDARY_SERVICE),
            LO_WORD(OBJECT_TRANS_SERVICE_UUID),              /* service UUID */
            HI_WORD(OBJECT_TRANS_SERVICE_UUID)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
};

/* <<Characteristic>>*/
static const T_ATTRIB_APPL ots_feat_mand_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  OTS Feature value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_FEATURE_CHAR_UUID),
            HI_WORD(OTS_FEATURE_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ
    },
};

static const T_ATTRIB_APPL obj_name_mand_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object name value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_NAME_CHAR_UUID),
            HI_WORD(OTS_OBJ_NAME_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)
    },
};

static const T_ATTRIB_APPL obj_type_mand_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object type value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_TYPE_CHAR_UUID),
            HI_WORD(OTS_OBJ_TYPE_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ
    },
};

static const T_ATTRIB_APPL obj_size_mand_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object size value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_SIZE_CHAR_UUID),
            HI_WORD(OTS_OBJ_SIZE_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ
    },
};

static const T_ATTRIB_APPL obj_first_cr_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object first created value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_FIRST_CR_CHAR_UUID),
            HI_WORD(OTS_OBJ_FIRST_CR_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)
    }
};

static const T_ATTRIB_APPL obj_last_mod_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object last modify value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_LAST_MOD_CHAR_UUID),
            HI_WORD(OTS_OBJ_LAST_MOD_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)
    },
};

static const T_ATTRIB_APPL obj_id_mand_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object id value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_ID_CHAR_UUID),
            HI_WORD(OTS_OBJ_ID_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ
    },
};

static const T_ATTRIB_APPL obj_props_mand_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object id value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_PROPS_CHAR_UUID),
            HI_WORD(OTS_OBJ_PROPS_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ
    }
};

static const T_ATTRIB_APPL oacp_mand_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_INDICATE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object Action Control Point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OACP_CHAR_UUID),
            HI_WORD(OTS_OACP_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_WRITE_ENCRYPTED_REQ | GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    }
};

static const T_ATTRIB_APPL olcp_mand_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_INDICATE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object Action Control Point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OLCP_CHAR_UUID),
            HI_WORD(OTS_OLCP_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_WRITE_ENCRYPTED_REQ | GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    }
};

static const T_ATTRIB_APPL obj_list_filter_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object List filter value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OLF_CHAR_UUID),
            HI_WORD(OTS_OLF_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)
    }
};

static const T_ATTRIB_APPL obj_changed_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_INDICATE
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },

    /*  Object change value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(OTS_OBJ_CHANGE_CHAR_UUID),
            HI_WORD(OTS_OBJ_CHANGE_CHAR_UUID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND_ENCRYPTED_REQ
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    }
};

/********************************************************************************************************
* local static function defined here, only used in this source file.
********************************************************************************************************/
static T_OTS_SERV_CB *ots_find_by_srv_id(T_SERVER_ID service_id)
{
    uint8_t  i;

    if (p_ots_mem == NULL)
    {
        return NULL;
    }

    for (i = 0; i < total_ots_num; i++)
    {
        if (p_ots_mem[i].used
            && (p_ots_mem[i].service_id == service_id))
        {
            return (&p_ots_mem[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("[OTS]ots_find_by_srv_id: failed, service_id 0x%x", service_id);

    return NULL;
}

static T_OTS_CL_CB *alloc_cl_cb_by_conn_handle(uint16_t conn_handle)
{
    if (p_cl_queue->count >= OTS_MAX_CONCURRENCY_NUM)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]alloc_cl_cb_by_conn_handle reach the max concurrency number");
        return NULL;
    }
    T_OTS_CL_CB *cl_cb = (T_OTS_CL_CB *)ble_audio_mem_zalloc(sizeof(T_OTS_CL_CB));
    if (cl_cb == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]alloc_cl_cb_by_conn_handle alloc fail");
        return NULL;
    }
    cl_cb->conn_handle = conn_handle;
    os_queue_in(p_cl_queue, (void *)cl_cb);
    return cl_cb;
}

static T_OTS_CL_CB *find_cl_by_conn_handle(uint16_t conn_handle, T_SERVER_ID service_id)
{
    int i;
    for (i = 0; i < p_cl_queue->count; i++)
    {
        T_OTS_CL_CB *cl_cb = (T_OTS_CL_CB *)os_queue_peek(p_cl_queue, i);
        if (cl_cb->conn_handle == conn_handle &&
            (service_id == 0xFF || cl_cb->service_id == service_id))
        {
            return cl_cb;
        }
    }
    return NULL;
}

static T_OTS_CL_CB *find_cl_by_ids(uint16_t conn_handle, uint16_t cid)
{
    int i;
    for (i = 0; i < p_cl_queue->count; i++)
    {
        T_OTS_CL_CB *cl_cb = (T_OTS_CL_CB *)os_queue_peek(p_cl_queue, i);
        if (cl_cb->conn_handle == conn_handle &&
            cl_cb->cid == cid)
        {
            return cl_cb;
        }
    }
    return NULL;
}

static void delete_cl_cb_by_id(uint16_t conn_handle)
{
    T_OTS_CL_CB *cl_cb;
    while ((cl_cb = find_cl_by_conn_handle(conn_handle, 0xFF)) != NULL)
    {
        int i;
        if (cl_cb)
        {
            if (cl_cb->current_obj && cl_cb->current_obj->obj_lock_state)
            {
                cl_cb->current_obj->wr_offset = 0;
                cl_cb->current_obj->wr_len = 0;
                cl_cb->current_obj->mode = 0;
                cl_cb->current_obj->obj_lock_state = OTS_OBJ_LOCK_IDLE_STATE;
                if (cl_cb->current_obj->temp_write_buf)
                {
                    ble_audio_mem_free(cl_cb->current_obj->temp_write_buf);
                    cl_cb->current_obj->temp_write_buf = NULL;
                }
            }
            os_queue_delete(p_cl_queue, (void *)cl_cb);

            for (i = 0; i < 3; i++)
            {
                if (cl_cb->olf_cb[i].p_param != NULL)
                {
                    ble_audio_mem_free(cl_cb->olf_cb[i].p_param);
                }
                cl_cb->olf_cb[i].param_len = 0;
                cl_cb->olf_cb[i].p_param = NULL;
                cl_cb->olf_cb[i].filt_type = OTS_NO_FILTER;
            }
            ble_audio_mem_free((void *) cl_cb);
        }

    }
}


T_OBJECT_CB *find_obj_by_id(T_SERVER_ID service_id, uint64_t obj_id)
{
    int i;
    T_OBJECT_CB *object;
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return NULL;
    }

    for (i = 0; i < p_entry->obj_queue.count; i++)
    {
        object = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
        if (object->obj_id == obj_id)
        {
            return object;
        }
    }
    return NULL;
}


bool update_obj_value_by_id(T_SERVER_ID service_id, uint64_t obj_id, uint8_t *p_value, uint16_t len)
{
    T_OBJECT_CB *p_obj = find_obj_by_id(service_id, obj_id);
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);
    uint8_t ind_change[OTS_OBJ_CHG_CHAR_VAL_LEN] = {0};
    uint8_t *p_temp = NULL;
    if (obj_id == 0 || p_obj == NULL ||
        (p_value == NULL && len != 0))
    {
        PROTOCOL_PRINT_ERROR1("[OTS]update_obj_value_by_id, error obj_id : 0x%x", (int)obj_id);
        return false;
    }
    if (len > 0)
    {
        p_temp = ble_audio_mem_zalloc(len);
        if (p_temp == NULL)
        {
            PROTOCOL_PRINT_ERROR1("[OTS]update_obj_value_by_id, alloc memory fail, len = %d", len);
            return false;
        }
    }

    if (p_obj->alloc_buf != NULL)
    {
        ble_audio_mem_free(p_obj->alloc_buf);
    }

    p_obj->alloc_buf = p_temp;
    if (len > 0)
    {
        memcpy(p_obj->alloc_buf, p_value, len);
    }

    p_obj->obj_size[0] = len;
    p_obj->obj_size[1] = len;
    PROTOCOL_PRINT_TRACE2("[OTS]update_obj_value_by_id, obj_id: 0x%x, size: %d", (int)p_obj->obj_id,
                          len);

    ind_change[0] |= OTS_OBJ_CREATE;
    memcpy(&ind_change[1], (uint8_t *) & (p_obj->obj_id), OTS_OBJ_ID_LEN);
    if (p_entry)
    {
        ind_change_to_clients(p_entry, 0xFF, ind_change, OTS_OBJ_CHG_CHAR_VAL_LEN);
    }
    return true;
}

//FIX TODO
static bool check_obj_exc_filter_list(T_OTS_CL_CB *cl_cb, T_OBJECT_CB *p_obj)
{
    int j;
    bool exc_list = false;
    //skip direct list
    if (p_obj->obj_type.uu.char_uuid16 == OTS_DIRECT_LIST_OBJ_UUID)
    {
        return false;
    }
    //FIX TODO
    for (j = 0; j < 3; j++)
    {
        if (exc_list)
        {
            break;
        }
        switch (cl_cb->olf_cb[j].filt_type)
        {
        case OTS_NO_FILTER:
            break;
        case OTS_NAME_STARTS_WITH:
            {
                if (p_obj->obj_name.name_len < cl_cb->olf_cb[j].param_len)
                {
                    exc_list = true;
                    break;
                }

            }
            break;
        case OTS_NAME_ENDS_WITH:
            {
                if (p_obj->obj_name.name_len < cl_cb->olf_cb[j].param_len)
                {
                    exc_list = true;
                    break;
                }
            }
            break;

        case OTS_NAME_CONTAINS:
            {
                if (p_obj->obj_name.name_len < cl_cb->olf_cb[j].param_len)
                {
                    exc_list = true;
                    break;
                }
            }
            break;

        case OTS_NAME_IS_EXACTLY:
            {
                if (p_obj->obj_name.name_len != cl_cb->olf_cb[j].param_len)
                {
                    exc_list = true;
                    break;
                }
            }
            break;

        case OTS_OBJ_TYPE:
            {
                if (memcmp(p_obj->obj_type.uu.char_uuid128, cl_cb->olf_cb[j].p_param,
                           cl_cb->olf_cb[j].param_len))
                {
                    exc_list = true;
                    break;
                }
            }
            break;

        case OTS_CREATE_BETWEEN_TMS:
        case OTS_MOD_BETWEEN_TMS:
            break;

        case OTS_CUR_SIZE_BETWEEN:
            {
                uint32_t size1 = *(uint32_t *)cl_cb->olf_cb[j].p_param;
                uint32_t size2 = *(uint32_t *)(&cl_cb->olf_cb[j].p_param[4]);
                if (p_obj->obj_size[0] < size1 || p_obj->obj_size[0] > size2)
                {
                    exc_list = true;
                    break;
                }
            }
            break;

        case OTS_ALLOC_SIZE_BETWEEN:
            {
                uint32_t size1 = *(uint32_t *)cl_cb->olf_cb[j].p_param;
                uint32_t size2 = *(uint32_t *)(&cl_cb->olf_cb[j].p_param[4]);
                if (p_obj->obj_size[1] < size1 || p_obj->obj_size[1] > size2)
                {
                    exc_list = true;
                    break;
                }
            }
            break;
        case OTS_MARKED_OBJS:
            {
                if ((p_obj->obj_props & (1 << OBJ_PROP_MARK_BIT_NUM)) == 0)
                {
                    exc_list = true;
                    break;
                }
            }
            break;
        }
    }

    return exc_list;
}

//FIX TODO
static bool order_obj_list_by_cur_size(T_OTS_SERV_CB *p_entry, uint8_t order_type)
{
    int i;
    T_OBJECT_CB  *object;
    T_OS_QUEUE    tmp_queue;
    T_OBJECT_CB  *tmp_object;

    if (order_type != OBJ_CUR_SIZE_ASCEND_SORT_ORDER &&
        order_type != OBJ_CUR_SIZE_DESCEND_SORT_ORDER)
    {
        return false;
    }
    os_queue_init(&tmp_queue);

    while ((object = (T_OBJECT_CB *)os_queue_out(&p_entry->obj_queue)) != NULL)
    {
        tmp_object = NULL;
        i = 0;
        for (i = 0; i < tmp_queue.count; i++)
        {
            tmp_object = (T_OBJECT_CB *)os_queue_peek(&tmp_queue, i);
            if (order_type == OBJ_CUR_SIZE_ASCEND_SORT_ORDER)
            {
                if (tmp_object->obj_size[0] > object->obj_size[0])
                {
                    break;
                }
            }
            else
            {
                if (tmp_object->obj_size[0] < object->obj_size[0])
                {
                    break;
                }
            }
        }
        if (i == 0)
        {
            os_queue_insert(&tmp_queue, NULL, object);
        }
        else
        {
            tmp_object = (T_OBJECT_CB *)os_queue_peek(&tmp_queue, i - 1);
            os_queue_insert(&tmp_queue, tmp_object, object);
        }
    }
    p_entry->obj_queue = tmp_queue;
    return true;
}

static void start_handle_next_oacp_read_req(uint16_t conn_handle, uint16_t cid)
{
    uint32_t transfer_len;
    T_GAP_CAUSE res;
    T_OTS_CL_CB *cl_cb =  find_cl_by_ids(conn_handle, cid);
    PROTOCOL_PRINT_TRACE1("[OTS]start_handle_next_oacp_read_req conn_handle : 0x%x", conn_handle);

    T_OBJECT_CB *current_obj = cl_cb->current_obj;

    if (current_obj->wr_len == 0)
    {
        PROTOCOL_PRINT_TRACE0("[OTS]start_handle_next_oacp_read_req read request is completed");
        goto req_end;
    }

    transfer_len = current_obj->wr_len > ((uint32_t)cl_cb->mtu) ?
                   (uint32_t)cl_cb->mtu : current_obj->wr_len;
    res = le_coc_send_data(cl_cb->cid, &current_obj->alloc_buf[current_obj->wr_offset],
                           (uint16_t)transfer_len);
    if (res != GAP_CAUSE_SUCCESS)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]start_handle_next_oacp_read_req coc_send data fail: 0x%x", res);
        goto req_end;
    }
    else
    {
        current_obj->wr_len -= transfer_len;
        current_obj->wr_offset += transfer_len;
    }

req_end:
    current_obj->wr_len = 0;
    current_obj->wr_offset = 0;
    current_obj->obj_lock_state = OTS_OBJ_LOCK_IDLE_STATE;
}

static void handle_oacp_read_request_cmplt(uint16_t conn_handle, uint16_t cause, uint8_t credit,
                                           uint16_t cid)
{
    if (cause != 0 || credit == 0)
    {
        PROTOCOL_PRINT_ERROR2("[OTS]handle_oacp_read_request_cmplt error cause: 0x%x, credit:%d", cause,
                              credit);
        return;
    }

    start_handle_next_oacp_read_req(conn_handle, cid);
}

static void handle_oacp_write_request(T_OTS_CL_CB *cl_cb, uint8_t *value, uint16_t len)
{
    //The lock state should be check in oacp write request
    T_OBJECT_CB *current_obj = cl_cb->current_obj;
    if ((current_obj->transferd_len + len) > current_obj->wr_len)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]handle_oacp_write_request: excess of the expected number");
        current_obj->obj_lock_state = OTS_OBJ_LOCK_IDLE_STATE;
        le_coc_disc(cl_cb->cid);
    }
    else
    {
        memcpy((current_obj->temp_write_buf + current_obj->transferd_len),
               value, len);
        current_obj->transferd_len += len;

        //still need data, continue
        if (current_obj->transferd_len < current_obj->wr_len)
        {
            return;
        }
        memcpy((current_obj->alloc_buf + current_obj->wr_offset),
               current_obj->temp_write_buf, current_obj->wr_len);
        if (current_obj->obj_lock_state == OTS_OBJ_LOCK_WRITE_APPEND_STATE)
        {
            current_obj->obj_size[0] = 0;
            current_obj->obj_size[0] += (uint64_t)(current_obj->wr_offset + current_obj->wr_len);
        }
    }

    current_obj->wr_offset = 0;
    current_obj->wr_len = 0;
    current_obj->mode = 0;
    current_obj->obj_lock_state = OTS_OBJ_LOCK_IDLE_STATE;
    ble_audio_mem_free(current_obj->temp_write_buf);
    current_obj->temp_write_buf = NULL;
}

static T_APP_RESULT ots_serv_coc_callback(uint8_t coc_type, void *p_coc_data, uint8_t id)
{
    T_APP_RESULT result = APP_RESULT_SUCCESS;
    T_LE_COC_DATA cb_data;
    memcpy((uint8_t *)&cb_data, p_coc_data, sizeof(T_LE_COC_DATA));
    PROTOCOL_PRINT_TRACE1("ots_serv_coc_callback: coc_type = %d", coc_type);
    switch (coc_type)
    {
    case GAP_COC_MSG_LE_CHANN_STATE:
        PROTOCOL_PRINT_INFO4("[OTS]GAP_COC_MSG_LE_CHANN_STATE: conn_id %d, cid 0x%x, conn_state %d, cause 0x%x",
                             cb_data.p_le_chann_state->conn_id,
                             cb_data.p_le_chann_state->cid,
                             cb_data.p_le_chann_state->conn_state,
                             cb_data.p_le_chann_state->cause);
        if (cb_data.p_le_chann_state->conn_state == GAP_CHANN_STATE_CONNECTED)
        {
            uint16_t conn_handle = le_get_conn_handle(cb_data.p_le_chann_state->conn_id);
            T_OTS_CL_CB *cl_cb = find_cl_by_conn_handle(conn_handle, (T_SERVER_ID)id);
            if (cl_cb == NULL)
            {
                PROTOCOL_PRINT_ERROR1("[OTS]ots_serv_coc_callback, cl_cb is NULL, id: %d ", id);
                break;
            }
            if (cl_cb != NULL)
            {
                le_coc_get_chann_param(COC_CHANN_PARAM_MTU, &cl_cb->mtu, cb_data.p_le_chann_state->cid);
                PROTOCOL_PRINT_INFO1("[OTS]GAP_CHANN_STATE_CONNECTED: mtu %d", cl_cb->mtu);
                cl_cb->cid = cb_data.p_le_chann_state->cid;
                cl_cb->service_id = (T_SERVER_ID)id;
            }
        }
        else if (cb_data.p_le_chann_state->conn_state == GAP_CHANN_STATE_DISCONNECTED)
        {
            T_OTS_CL_CB *cl_cb = find_cl_by_ids(le_get_conn_handle(cb_data.p_le_chann_state->conn_id),
                                                cb_data.p_le_chann_state->cid);
            if (cl_cb != NULL)
            {
                if (cl_cb->current_obj != NULL && cl_cb->current_obj->obj_lock_state)
                {
                    cl_cb->current_obj->wr_offset = 0;
                    cl_cb->current_obj->wr_len = 0;
                    cl_cb->current_obj->mode = 0;
                    cl_cb->current_obj->obj_lock_state = OTS_OBJ_LOCK_IDLE_STATE;
                    if (cl_cb->current_obj->temp_write_buf)
                    {
                        ble_audio_mem_free(cl_cb->current_obj->temp_write_buf);
                        cl_cb->current_obj->temp_write_buf = NULL;
                    }
                }
                cl_cb->cid = 0;
            }
        }
        break;

    case GAP_COC_MSG_LE_REG_PSM:
        PROTOCOL_PRINT_INFO2("[OTS]GAP_COC_MSG_LE_REG_PSM: le_psm 0x%x, cause 0x%x",
                             cb_data.p_le_reg_psm_rsp->le_psm,
                             cb_data.p_le_reg_psm_rsp->cause);
        break;

    case GAP_COC_MSG_LE_SET_PSM_SECURITY:
        PROTOCOL_PRINT_INFO1("[OTS]GAP_COC_MSG_LE_SET_PSM_SECURITY: cause 0x%x",
                             cb_data.p_le_set_psm_security_rsp->cause);
        break;

    case GAP_COC_MSG_LE_SEND_DATA:
        PROTOCOL_PRINT_INFO4("[OTS]GAP_COC_MSG_LE_SEND_DATA: conn_id %d, cid 0x%x, cause 0x%x, credit %d",
                             cb_data.p_le_send_data->conn_id,
                             cb_data.p_le_send_data->cid,
                             cb_data.p_le_send_data->cause,
                             cb_data.p_le_send_data->credit);
        handle_oacp_read_request_cmplt(le_get_conn_handle(cb_data.p_le_send_data->conn_id),
                                       cb_data.p_le_send_data->cause,
                                       cb_data.p_le_send_data->credit, cb_data.p_le_send_data->cid);
        break;

    case GAP_COC_MSG_LE_RECEIVE_DATA:
        {
            PROTOCOL_PRINT_INFO3("[OTS]GAP_COC_MSG_LE_RECEIVE_DATA: conn_id %d, cid 0x%x, value_len %d",
                                 cb_data.p_le_receive_data->conn_id,
                                 cb_data.p_le_receive_data->cid,
                                 cb_data.p_le_receive_data->value_len);
            T_OTS_CL_CB *cl_cb = find_cl_by_ids(le_get_conn_handle(cb_data.p_le_receive_data->conn_id),
                                                cb_data.p_le_receive_data->cid);

            if (cl_cb != NULL)
                handle_oacp_write_request(cl_cb,
                                          cb_data.p_le_receive_data->p_data,
                                          cb_data.p_le_receive_data->value_len);
        }
        break;

    default:
        break;
    }
    return result;
}

static uint64_t alloc_obj_id(T_OTS_SERV_CB *p_entry)
{
    uint64_t obj_id = 0;
    int i, v, index;
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_INFO0("[OTS]alloc_obj_id entry is NULL");
        return 0;
    }
    for (i = 0; i < OTS_OBJ_ID_ARRAY_MAX; i++)
    {
        if (p_entry->id_array[i] != 0xFFFFFFFF)
        {
            v = (int)(~(p_entry->id_array[i]));
            v ^= v - 1;
            v = (v & 0x55555555) + ((v >> 1) & 0x55555555);
            v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
            v = (v + (v >> 4)) & 0x0F0F0F0F;
            v += v >> 8;
            v += v >> 16;
            index = (v & 0x3F) - 1;
            p_entry->id_array[i] |= 1 << index;
            obj_id = index + i * 32 + OTS_OBJ_ID_BASE;
            //PROTOCOL_PRINT_TRACE2("alloc_obj_id index: %d, obj_id: 0x%x", index, obj_id);
            break;
        }
    }

    return obj_id;
}

static void free_obj_id(T_OTS_SERV_CB *p_entry, uint64_t id)
{
    int array_num = (id - OTS_OBJ_ID_BASE) / 32;
    int index = (id - OTS_OBJ_ID_BASE) % 32;
    p_entry->id_array[array_num] &= (~(1 << index));
}

static void create_directory_listing_object(T_OTS_SERV_CB *p_entry)
{
    T_OBJECT_CB *object;

    object = (T_OBJECT_CB *)ble_audio_mem_zalloc(sizeof(T_OBJECT_CB));
    if (object == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]create_directory_listing_object, alloc objec fail ");
        return;
    }

    object->obj_id = 0;
    //FIX TODO for now we don't care first-create and last-modify time
    uint16_t size = 24 + strlen("Directory");

    object->obj_size[0] = size;
    object->obj_size[1] = size;
    object->obj_name.name_len = strlen("Directory");
    memcpy(object->obj_name.name, "Directory", object->obj_name.name_len);
    object->obj_type.uuid_size = UUID_16BIT_SIZE;
    object->obj_type.uu.char_uuid16 = OTS_DIRECT_LIST_OBJ_UUID;

    object->obj_props = 1 << OBJ_PROP_READ_BIT_NUM;
    os_queue_in(&p_entry->obj_queue, (void *)object);

    p_entry->dir_list_obj = object;
    return;
}

static uint32_t update_direct_list_obj_size(T_OTS_SERV_CB *p_entry)
{
    uint32_t cur_size = 0;
    int i;
    T_OBJECT_CB *object;

    for (i = 0; i < p_entry->obj_queue.count; i++)
    {
        object = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
        cur_size += 22 + object->obj_type.uuid_size + object->obj_name.name_len;
    }

    p_entry->dir_list_obj->obj_size[0] = cur_size;
    p_entry->dir_list_obj->obj_size[1] = cur_size;
    return cur_size;
}

static bool generate_direct_lib_obj_record(T_OTS_SERV_CB *p_entry)
{
    uint32_t cur_size = 0;
    int i;
    T_OBJECT_CB *object = NULL;
    uint8_t *p_buf = NULL;

    if (p_entry->dir_list_obj->alloc_buf)
    {
        ble_audio_mem_free(p_entry->dir_list_obj->alloc_buf);
    }

    cur_size = update_direct_list_obj_size(p_entry);

    p_buf = (uint8_t *)ble_audio_mem_zalloc(cur_size);
    if (!p_buf)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]generate_direct_lib_obj_record alloc size: %d fail", cur_size);
        return false;
    }
    p_entry->dir_list_obj->alloc_buf = p_buf;
    for (i = 0; i < p_entry->obj_queue.count; i++)
    {
        object = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
        // exclude length of object record
        uint16_t size = object->obj_type.uuid_size + object->obj_name.name_len + 20;
        PROTOCOL_PRINT_TRACE1("[OTS]generate_direct_lib_obj_record objec size: %d", size);
        memcpy(p_buf, (uint8_t *)&size, 2);
        p_buf += 2;

        memcpy(p_buf, (uint8_t *)&object->obj_id, 6);
        p_buf += 6;

        *p_buf = (uint8_t)(object->obj_name.name_len);
        p_buf++;

        memcpy(p_buf, object->obj_name.name, object->obj_name.name_len);
        p_buf += object->obj_name.name_len;

        *p_buf = 0x27;    //flag
        p_buf++;

        memcpy(p_buf, object->obj_type.uu.char_uuid128, object->obj_type.uuid_size);
        p_buf += object->obj_type.uuid_size;

        memcpy(p_buf, (uint8_t *)object->obj_size, 8);
        p_buf += 8;

        memcpy(p_buf, (uint8_t *)&object->obj_props, 4);
        p_buf += 4;
    }
    return true;
}

static uint8_t oacp_create_ots_object(T_OTS_SERV_CB *p_entry, T_OTS_CL_CB *cl_cb,
                                      uint32_t size, uint16_t uuid_size,
                                      uint8_t *uuid)
{
    T_OBJECT_CB *object;
    uint8_t ind_change[OTS_OBJ_CHG_CHAR_VAL_LEN] = {0};
    uint8_t res = OACP_SUCCESS_RES_CODE;
    if (uuid == NULL)
    {
        return OACP_INV_PARAM_RES_CODE;
    }

    object = (T_OBJECT_CB *)ble_audio_mem_zalloc(sizeof(T_OBJECT_CB));
    if (object == NULL)
    {
        return OACP_INSF_RESC_RES_CODE;
    }

    object->obj_id = alloc_obj_id(p_entry);
    if (object->obj_id == 0)
    {
        res = OACP_OPER_FAIL_RES_CODE;
        goto fail;
    }

    object->alloc_buf = ble_audio_mem_zalloc(size);
    if (object->alloc_buf == NULL)
    {
        res = OACP_INSF_RESC_RES_CODE;
        free_obj_id(p_entry, object->obj_id);
        goto fail;
    }

    object->obj_size[1] = size;
    object->obj_name.name_len = 0;
    object->obj_type.uuid_size = uuid_size;
    memcpy(object->obj_type.uu.char_uuid128, uuid, uuid_size);

    //The write property shall initally be set to True
    object->obj_props = OTS_OACP_CRAETE_DEFAULT_PROPS;
    os_queue_in(&p_entry->obj_queue, (void *)object);

    cl_cb->current_obj = object;

    ind_change[0] |= OTS_SRC_OF_CHG_CLIENT | OTS_OBJ_CREATE;
    memcpy(&ind_change[1], (uint8_t *) & (object->obj_id), OTS_OBJ_ID_LEN);
    ind_change_to_clients(p_entry, cl_cb->conn_handle, ind_change, OTS_OBJ_CHG_CHAR_VAL_LEN);
    return res;

fail:
    ble_audio_mem_free(object);
    return res;

}

uint8_t delete_ots_obj(T_OTS_SERV_CB *p_entry, uint16_t conn_handle, T_OBJECT_CB *object)
{
    int i;
    if (!object)
    {
        return OACP_INV_OBJ_RES_CODE;
    }
    os_queue_delete(&p_entry->obj_queue, (void *)object);

    for (i = 0; i < p_cl_queue->count; i++)
    {
        T_OTS_CL_CB *cl_cb = (T_OTS_CL_CB *)os_queue_peek(p_cl_queue, i);
        if (cl_cb->current_obj == object)
        {
            cl_cb->current_obj = NULL;
        }
    }

    uint8_t ind_change[OTS_OBJ_CHG_CHAR_VAL_LEN] = {0};

    ind_change[0] |= OTS_OBJ_DEL;
    if (conn_handle != 0xFF00)    //not server
    {
        ind_change[0] |= OTS_SRC_OF_CHG_CLIENT;
    }
    memcpy(&ind_change[1], (uint8_t *) & (object->obj_id), 6);

    ble_audio_mem_free(object->alloc_buf);
    ble_audio_mem_free(object);
    ind_change_to_clients(p_entry, conn_handle, ind_change, OTS_OBJ_CHG_CHAR_VAL_LEN);
    return OACP_SUCCESS_RES_CODE;
}

static bool check_obj_name_exist(T_OTS_SERV_CB *p_entry, uint8_t *name, uint16_t len)
{
    int index = 0;
    T_OBJECT_CB *object;
    while ((object = os_queue_peek(&p_entry->obj_queue, index)) != NULL)
    {
        if (object->obj_name.name_len == len && !memcmp(object->obj_name.name, name, len))
        {
            return true;
        }
        index++;
    }
    return false;
}

static void handle_oacp_request(T_OTS_SERV_CB *p_entry, T_OTS_CL_CB *cl_cb, uint8_t *value,
                                uint16_t len)
{
    uint8_t *p = value;
    uint8_t opcode = *p++;
    uint8_t *rsp;
    len--;
    uint8_t rsp_value[3] = {OTS_OACP_RSP_CODE_OP, opcode, OACP_SUCCESS_RES_CODE};
    rsp = rsp_value;
    uint16_t rsp_len = 3;
    uint32_t cur_size;
    uint32_t alloc_size;
    uint8_t conn_id;

    if (cl_cb == NULL)
    {
        return;
    }

    le_get_conn_id_by_handle(cl_cb->conn_handle, &conn_id);
    T_OBJECT_CB *current_obj = cl_cb->current_obj;
    uint32_t oacp_feature = p_entry->ots_features[0];
    uint32_t mask = opcode == OTS_OACP_ABORT_OP ? OTS_OACP_ABORT_FEAT_MSK : (opcode - 1);
    if ((oacp_feature & (1 << mask)) == 0)
    {
        rsp_value[2] = OACP_OPCODE_NOTSUP_RES_CODE;
        goto send_ind;
    }

    if ((!current_obj || check_obj_exc_filter_list(cl_cb, current_obj)) &&
        (opcode != OTS_OACP_CREATE_OP))
    {
        rsp_value[2] = OACP_INV_OBJ_RES_CODE;
        goto send_ind;
    }

    if (current_obj)
    {
        //If the current obj is reading or write, create should be not allowed
        if (current_obj->obj_lock_state)
        {
            if (opcode == OTS_OACP_CREATE_OP)
            {
                rsp_value[2] = OACP_OPER_FAIL_RES_CODE;
                goto send_ind;
            }
            else if (opcode != OTS_OACP_ABORT_OP)
            {
                rsp_value[2] = OACP_OBJ_LOCKD_RES_CODE;
                goto send_ind;
            }
        }

        cur_size = current_obj->obj_size[0];
        alloc_size = current_obj->obj_size[1];
    }

    PROTOCOL_PRINT_TRACE1("[OTS]handle_oacp_request opcode: 0x%x", opcode);
    switch (opcode)
    {
    case OTS_OACP_CREATE_OP:
        {
            if (len < 4)
            {
                rsp_value[2] = OACP_INV_PARAM_RES_CODE;
                break;
            }
            uint32_t size = 0;
            memcpy((uint8_t *)&size, p, 4);
            p += 4;
            len -= 4;

            if (len == 16)
            {
                rsp_value[2] = oacp_create_ots_object(p_entry, cl_cb, size, UUID_128BIT_SIZE, p);
            }
            else if (len == 2)
            {
                rsp_value[2] = oacp_create_ots_object(p_entry, cl_cb, size, UUID_16BIT_SIZE, p);
            }
            else
            {
                rsp_value[2] = OACP_INV_PARAM_RES_CODE;
            }

            if (OACP_SUCCESS_RES_CODE == rsp_value[2])
            {
                for (int i = 0; i < 3; i++)
                {
                    cl_cb->olf_cb[i].filt_type = OTS_NO_FILTER;
                    if (cl_cb->olf_cb[i].param_len > 0)
                    {
                        ble_audio_mem_free(cl_cb->olf_cb[i].p_param);
                        cl_cb->olf_cb[i].param_len = 0;
                        cl_cb->olf_cb[i].p_param = NULL;
                    }
                }
            }
        }
        break;
    case OTS_OACP_DELETE_OP:
        {
            if (current_obj->obj_props & (1 << OBJ_PROP_DELETE_BIT_NUM))
            {
                rsp_value[2] = delete_ots_obj(p_entry, cl_cb->conn_handle, current_obj);
                cl_cb->current_obj = NULL;
            }
            else
            {
                rsp_value[2] = OACP_PROCE_NOT_PERM_RES_CODE;
            }
        }
        break;
    case OTS_OACP_CAL_CHECKSUM_OP:
        //FIX TODO when 32bit crc checksum is ready
        break;
    case OTS_OACP_EXECUTE_OP:
        {
            if (current_obj->obj_props & (1 << OBJ_PROP_EXECUTE_BIT_NUM))
            {
                //FIX TODO this depends on app
                rsp_value[2] = OACP_OPER_FAIL_RES_CODE;
            }
            else
            {
                rsp_value[2] = OACP_PROCE_NOT_PERM_RES_CODE;
            }
        }
        break;
    case OTS_OACP_READ_OP:
        {
            uint32_t offset = 0;
            uint32_t length = 0;

            if (len != 8)
            {
                rsp_value[2] = OACP_INV_PARAM_RES_CODE;
                goto send_ind;
            }
            memcpy((uint8_t *)&offset, p, 4);
            memcpy((uint8_t *)&length, (p + 4), 4);
            if (cl_cb->cid == 0)
            {
                le_coc_register_cback(conn_id, ots_serv_coc_callback, (uint8_t)cl_cb->service_id, true);
            }

            if ((current_obj->obj_props & (1 << OBJ_PROP_READ_BIT_NUM)) == 0)
            {
                rsp_value[2] = OACP_PROCE_NOT_PERM_RES_CODE;
            }
            else if (cl_cb->cid == 0)
            {
                rsp_value[2] = OACP_CHAN_UNAV_RES_CODE;
            }
            else if (offset > cur_size || (length + offset) > cur_size)
            {
                rsp_value[2] = OACP_INV_PARAM_RES_CODE;
            }
            else
            {
                if (current_obj->obj_type.uu.char_uuid16 == OTS_DIRECT_LIST_OBJ_UUID &&
                    !generate_direct_lib_obj_record(p_entry))
                {
                    rsp_value[2] = OACP_INSF_RESC_RES_CODE;
                    gatt_svc_send_data(cl_cb->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                       p_entry->oacp_cb.char_index,
                                       rsp, rsp_len, GATT_PDU_TYPE_INDICATION);
                    return;
                }
                //For now send indication first and then send data
                //we may send indication, and send data asynchronization
                gatt_svc_send_data(cl_cb->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                   p_entry->oacp_cb.char_index,
                                   rsp, rsp_len, GATT_PDU_TYPE_INDICATION);

                if (length == 0)
                {
                    PROTOCOL_PRINT_ERROR0("[OTS]handle_oacp_request read length is 0");
                    return;
                }
                uint32_t transfer_len = length > ((uint32_t)cl_cb->mtu) ? (uint32_t)cl_cb->mtu : length;
                T_GAP_CAUSE res = le_coc_send_data(cl_cb->cid, &current_obj->alloc_buf[offset],
                                                   (uint16_t)transfer_len);
                if (res != GAP_CAUSE_SUCCESS)
                {
                    PROTOCOL_PRINT_ERROR1("[OTS]handle_oacp_request coc_send data fail: 0x%x", res);
                    return;
                }

                current_obj->wr_len = length - transfer_len;
                current_obj->wr_offset = offset + transfer_len;
                current_obj->obj_lock_state = OTS_OBJ_LOCK_READ_STATE;
                return;
            }
        }
        break;
    case OTS_OACP_WRITE_OP:
        {
            le_coc_register_cback(conn_id, ots_serv_coc_callback, (uint8_t)cl_cb->service_id, true);
            if (!(current_obj->obj_props & (1 << OBJ_PROP_WRITE_BIT_NUM)))
            {
                rsp_value[2] = OACP_PROCE_NOT_PERM_RES_CODE;
            }
            else if (cl_cb->cid == 0)
            {
                rsp_value[2] = OACP_CHAN_UNAV_RES_CODE;
            }
            else
            {
                //FIX TODO should we set a flag to receive data?
                //So that if flag is not, we may ignore the write data
                uint32_t write_offset;
                memcpy((uint8_t *)&write_offset, p, 4);
                uint32_t write_len;
                memcpy((uint8_t *)&write_len, (p + 4), 4);
                uint8_t mode = *(p + 8);

                //If not support patch
                rsp_value[2] = OACP_PROCE_NOT_PERM_RES_CODE;
                if (write_offset < cur_size &&
                    ((p_entry->ots_features[0] & (1 << OTS_OACP_PATCH_FEAT_MSK)) == 0 ||
                     (current_obj->obj_props & (1 << OBJ_PROP_PATCH_BIT_NUM)) == 0))
                {
                    goto send_ind;
                }

                if (current_obj->mode == OTS_OACP_TRUNCATE_MODE)
                {
                    //If not support truncate
                    if ((p_entry->ots_features[0] & (1 << OTS_OACP_TRUNCATE_FEAT_MSK)) == 0 ||
                        (current_obj->obj_props & (1 << OBJ_PROP_TRUNCATE_BIT_NUM)) == 0)
                    {
                        goto send_ind;
                    }
                }

                if ((write_offset + write_len) > alloc_size)
                {
                    //If not support append
                    if ((p_entry->ots_features[0] & (1 << OTS_OACP_APPEND_FEAT_MSK)) == 0 ||
                        (current_obj->obj_props & (1 << OBJ_PROP_APPEDND_BIT_NUM)) == 0)
                    {
                        goto send_ind;
                    }


                    if ((p_entry->ots_features[0] & (1 << OTS_OACP_APPEND_FEAT_MSK)) == 0 ||
                        (current_obj->obj_props & (1 << OBJ_PROP_APPEDND_BIT_NUM)) == 0)
                    {
                        goto send_ind;
                    }

                    uint8_t *new_buf = (uint8_t *)ble_audio_mem_zalloc((write_offset + write_len));
                    if (!new_buf)
                    {
                        rsp_value[2] = OACP_INSF_RESC_RES_CODE;
                        goto send_ind;
                    }
                    memcpy(new_buf, current_obj->alloc_buf, cur_size);
                    current_obj->obj_size[0] = 0;
                    current_obj->obj_size[1] = write_offset + write_len;

                    //We can re-use the old buffer
                    current_obj->temp_write_buf = current_obj->alloc_buf;
                    current_obj->alloc_buf = new_buf;
                }
                else
                {
                    current_obj->temp_write_buf = (uint8_t *)ble_audio_mem_zalloc(write_len);
                    if (!current_obj->temp_write_buf)
                    {
                        rsp_value[2] = OACP_INSF_RESC_RES_CODE;
                        goto send_ind;
                    }
                }

                current_obj->obj_lock_state = (write_offset + write_len) > cur_size ?
                                              OTS_OBJ_LOCK_WRITE_APPEND_STATE : OTS_OBJ_LOCK_WRITE_PATCH_STATE;
                rsp_value[2] = OACP_SUCCESS_RES_CODE;
                current_obj->wr_offset = write_offset;
                current_obj->wr_len = write_len;
                current_obj->transferd_len = 0;
                current_obj->mode = mode;
            }
        }
        break;

    case OTS_OACP_ABORT_OP:
        {
            if (current_obj->obj_lock_state != OTS_OBJ_LOCK_READ_STATE)
            {
                rsp_value[2] = OACP_OPER_FAIL_RES_CODE;
            }
            else
            {
                current_obj->wr_len = 0;
                rsp_value[2] = OACP_SUCCESS_RES_CODE;
            }
        }
        break;

    default:
        {
            rsp_value[2] = OACP_OPCODE_NOTSUP_RES_CODE;
        }
        break;
    }

send_ind:
    gatt_svc_send_data(cl_cb->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                       p_entry->oacp_cb.char_index,
                       rsp, rsp_len, GATT_PDU_TYPE_INDICATION);
}


static void handle_olcp_request(T_OTS_SERV_CB *p_entry, T_OTS_CL_CB *cl_cb, uint8_t *value,
                                uint16_t len)
{
    uint8_t *p = value;
    uint8_t opcode = *p++;
    len--;
    int i;
    T_OBJECT_CB *obj;
    uint8_t rsp_value[3] = {OTS_OLCP_RSP_CODE_OP, opcode, OLCP_SUCCESS_RES_CODE};
    PROTOCOL_PRINT_TRACE1("handle_olcp_request, opcode : 0x%x", opcode);
    T_OBJECT_CB *current_obj = cl_cb->current_obj;
    uint32_t olcp_feature = p_entry->ots_features[1];

    if (current_obj && current_obj->obj_lock_state)
    {
        rsp_value[2] = OLCP_OPER_FAIL_RES_CODE;
        PROTOCOL_PRINT_ERROR1("handle_olcp_request, obj_lock_state : 0x%x", current_obj->obj_lock_state);
        goto send_ind;
    }

    switch (opcode)
    {
    case OTS_OLCP_FIRST_OP:
        {
            for (i = 0; i < p_entry->obj_queue.count; i++)
            {
                obj = os_queue_peek(&p_entry->obj_queue, i);
                if (obj && !check_obj_exc_filter_list(cl_cb, obj))
                {
                    break;
                }
            }
            if (i == p_entry->obj_queue.count)
            {
                rsp_value[2] = OLCP_NO_OBJ_RES_CODE;
            }
            else
            {
                cl_cb->current_obj = obj;
            }
        }
        break;
    case OTS_OLCP_LAST_OP:
        {
            for (i = p_entry->obj_queue.count - 1; i < 0; i--)
            {
                obj = os_queue_peek(&p_entry->obj_queue, i);
                if (obj && !check_obj_exc_filter_list(cl_cb, obj))
                {
                    break;
                }
            }
            if (i == p_entry->obj_queue.count)
            {
                rsp_value[2] = OLCP_NO_OBJ_RES_CODE;
            }
            else
            {
                cl_cb->current_obj = obj;
            }
        }
        break;
    case OTS_OLCP_PREVIOUS_OP:
        {
            T_OBJECT_CB *obj;
            T_OBJECT_CB *prev_obj = NULL;
            int i;
            if (!cl_cb->current_obj || check_obj_exc_filter_list(cl_cb, cl_cb->current_obj))
            {
                rsp_value[2] = OLCP_OPER_FAIL_RES_CODE;
            }
            else
            {
                for (i = 0; i < p_entry->obj_queue.count; i++)
                {
                    obj = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
                    if (obj == cl_cb->current_obj)
                    {
                        break;
                    }
                    if (!check_obj_exc_filter_list(cl_cb, obj))
                    {
                        prev_obj = obj;
                    }
                }

                if (!prev_obj)
                {
                    rsp_value[2] = OLCP_OOB_RES_CODE;
                }
                else
                {
                    cl_cb->current_obj = prev_obj;
                }
            }
        }
        break;
    case OTS_OLCP_NEXT_OP:
        {
            T_OBJECT_CB *obj = NULL;
            T_OBJECT_CB *next_obj = NULL;
            int i;
            if (!cl_cb->current_obj || check_obj_exc_filter_list(cl_cb, cl_cb->current_obj))
            {
                rsp_value[2] = OLCP_OPER_FAIL_RES_CODE;
            }
            else
            {
                for (i = p_entry->obj_queue.count - 1; i < 0; i--)
                {
                    obj = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
                    if (obj == cl_cb->current_obj)
                    {
                        break;
                    }
                }
                if (!check_obj_exc_filter_list(cl_cb, obj))
                {
                    next_obj = obj;
                }

                if (!next_obj)
                {
                    rsp_value[2] = OLCP_OOB_RES_CODE;
                }
                else
                {
                    cl_cb->current_obj = next_obj;
                }
            }
        }

        break;
    case OTS_OLCP_GOTO_OP:
        {
            T_OBJECT_CB *obj;
            int i;
            uint64_t objId = 0;
            if ((olcp_feature & (1 << OTS_OLCP_GOTO_FEAT_MSK)) == 0)
            {
                rsp_value[2] = OLCP_OPCODE_NOTSUP_RES_CODE;
                PROTOCOL_PRINT_ERROR0("[OTS]handle_olcp_request, not support goto opcode");
                goto send_ind;
            }
            memcpy((uint8_t *)&objId, p, 6);
            if (len != 6 || (objId < OTS_OBJ_ID_BASE && objId != 0))
            {
                rsp_value[2] = OLCP_INV_PARAM_RES_CODE;
                break;
            }
            for (i = 0; i < p_entry->obj_queue.count; i++)
            {
                obj = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
                if (obj && obj->obj_id == objId)
                {
                    break;
                }
            }
            if (i == p_entry->obj_queue.count)
            {
                rsp_value[2] = OLCP_NO_OBJ_RES_CODE;
            }
            else
            {
                cl_cb->current_obj = obj;
            }

            //set OLF filter type to no filter
            for (i = 0; i < 3; i++)
            {
                if (cl_cb->olf_cb[i].p_param != NULL)
                {
                    ble_audio_mem_free(cl_cb->olf_cb[i].p_param);
                }
                cl_cb->olf_cb[i].param_len = 0;
                cl_cb->olf_cb[i].p_param = NULL;
                cl_cb->olf_cb[i].filt_type = OTS_NO_FILTER;
            }
        }
        break;
    case OTS_OLCP_ORDER_OP:
        {
            if ((olcp_feature & (1 << OTS_OLCP_ORDER_FEAT_MSK)) == 0)
            {
                rsp_value[2] = OLCP_OPCODE_NOTSUP_RES_CODE;
                PROTOCOL_PRINT_ERROR0("[OTS]handle_olcp_request, not support order opcode");
            }
            else if (!order_obj_list_by_cur_size(p_entry, p[0]))
            {
                rsp_value[2] = OLCP_OPCODE_NOTSUP_RES_CODE;
            }
        }
        break;
    case OTS_OLCP_REQ_NUM_OBJS_OP:
        {
            uint32_t total_num = 0;
            if ((olcp_feature & (1 << OTS_OLCP_REQ_NUM_OBJ_FEAT_MSK)) == 0)
            {
                rsp_value[2] = OLCP_OPCODE_NOTSUP_RES_CODE;
                PROTOCOL_PRINT_ERROR0("[OTS]handle_olcp_request, not support requst obj num opcode");
                goto send_ind;
            }
            if (p_entry->obj_queue.count == 0)
            {
                rsp_value[2] = OLCP_NO_OBJ_RES_CODE;
            }
            else
            {
                for (i = 0; i < p_entry->obj_queue.count; i++)
                {
                    obj = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
                    if (obj && !check_obj_exc_filter_list(cl_cb, obj))
                    {
                        total_num++;
                    }
                }
                uint8_t value[7] = {OTS_OLCP_RSP_CODE_OP, opcode, OLCP_SUCCESS_RES_CODE, 0, 0, 0, 0};
                memcpy(&value[3], (uint8_t *)&total_num,  4);
                gatt_svc_send_data(cl_cb->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                   p_entry->olcp_cb.char_index,
                                   value, 7, GATT_PDU_TYPE_INDICATION);
                return;
            }
        }
        break;
    case OTS_OLCP_CLR_MARK_OP:
        {
            T_OBJECT_CB *obj;
            int i;
            if ((olcp_feature & (1 << OTS_OLCP_CLR_MARK_FEAT_MSK)) == 0)
            {
                rsp_value[2] = OLCP_OPCODE_NOTSUP_RES_CODE;
                PROTOCOL_PRINT_ERROR0("[OTS]handle_olcp_request, not support clear mark opcode");
                goto send_ind;
            }
            if (p_entry->obj_queue.count == 0)
            {
                rsp_value[2] = OLCP_NO_OBJ_RES_CODE;
                break;
            }
            for (i = 0; i < p_entry->obj_queue.count; i++)
            {
                obj = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
                if (check_obj_exc_filter_list(cl_cb, obj))
                {
                    continue;
                }
                obj->obj_props &= (~(1 << OBJ_PROP_MARK_BIT_NUM));
            }
        }
        break;
    default:
        rsp_value[2] = OLCP_OPCODE_NOTSUP_RES_CODE;
        break;
    }

send_ind:
    gatt_svc_send_data(cl_cb->conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                       p_entry->olcp_cb.char_index,
                       rsp_value, 3, GATT_PDU_TYPE_INDICATION);

}


static void ind_change_to_clients(T_OTS_SERV_CB *p_entry, uint16_t conn_handle, uint8_t *value,
                                  uint16_t len)
{
    int i;
    if (p_entry->obj_change_index)
    {
        for (i = 0; i < ble_audio_db.acl_link_num; i++)
        {
            if ((ble_audio_db.le_link[i].used == true) &&
                (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
            {
                T_LE_SRV_CFG *p_srv = ble_srv_find_by_srv_id(ble_audio_db.le_link[i].conn_handle,
                                                             p_entry->service_id);
                if (p_srv != NULL && (p_srv->cccd_cfg[0] & OTS_OBJ_CHG_CCCD_FLAG))
                {
                    gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                                       p_entry->obj_change_index, value, len, GATT_PDU_TYPE_INDICATION);
                }
            }
        }
    }
}


/**
 * @brief read characteristic data from service.
 *
 * @param conn_id           Connection ID.
 * @param service_id        ServiceID to be read.
 * @param attrib_index      Attribute index of getting characteristic data.
 * @param offset            Offset of characteritic to be read.
 * @param p_length          Length of getting characteristic data.
 * @param pp_value          Pointer to pointer of characteristic value to be read.
 * @return T_APP_RESULT
*/
T_APP_RESULT ots_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                              uint16_t attrib_index,
                              uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{

    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    *p_length = 0;
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl, attrib_index,
                                                             p_entry->attr_num);

    PROTOCOL_PRINT_INFO2("[OTS]ots_attr_read_cb attrib_index = %d offset %x", attrib_index, offset);

    T_OTS_CL_CB *cl_cb = find_cl_by_conn_handle(conn_handle, service_id);
    if (cl_cb == NULL)
    {
        cl_cb = alloc_cl_cb_by_conn_handle(conn_handle);
        if (cl_cb == NULL)
        {
            return APP_RESULT_REJECT;
        }
        cl_cb->service_id = service_id;
    }

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]ots_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    if (char_uuid.uu.char_uuid16 != OTS_FEATURE_CHAR_UUID &&
        (!cl_cb->current_obj || check_obj_exc_filter_list(cl_cb, cl_cb->current_obj)))
    {
        return (T_APP_RESULT)ATT_ERR_OTS_OBJ_NOT_SELECTED;
    }

    T_OBJECT_CB *current_obj = cl_cb->current_obj;
    switch (char_uuid.uu.char_uuid16)
    {
    case OTS_FEATURE_CHAR_UUID:
        *pp_value = (uint8_t *)&p_entry->ots_features;
        *p_length = 8;
        break;
    case OTS_OBJ_NAME_CHAR_UUID:
        *pp_value = current_obj->obj_name.name;
        *p_length = current_obj->obj_name.name_len;

        break;
    case OTS_OBJ_TYPE_CHAR_UUID:
        if (current_obj->obj_type.uuid_size == UUID_16BIT_SIZE)
        {
            *pp_value = (uint8_t *)&current_obj->obj_type.uu.char_uuid16;
            *p_length = UUID_16BIT_SIZE;
        }
        else
        {
            *pp_value = current_obj->obj_type.uu.char_uuid128;
            *p_length = UUID_128BIT_SIZE;
        }
        break;
    case OTS_OBJ_SIZE_CHAR_UUID:
        if (current_obj->obj_type.uu.char_uuid16 == OTS_DIRECT_LIST_OBJ_UUID)
        {
            update_direct_list_obj_size(p_entry);
        }
        *pp_value = (uint8_t *)current_obj->obj_size;
        *p_length = 8;

        break;
    case OTS_OBJ_FIRST_CR_CHAR_UUID:
        *pp_value = (uint8_t *)&current_obj->obj_first_created;
        *p_length = 8;
        break;
    case OTS_OBJ_LAST_MOD_CHAR_UUID:
        *pp_value = (uint8_t *)&current_obj->obj_last_modify;
        *p_length = 8;
        break;
    case OTS_OBJ_ID_CHAR_UUID:
        *pp_value = (uint8_t *)&current_obj->obj_id;
        *p_length = 6;
        break;
    case OTS_OBJ_PROPS_CHAR_UUID:
        *pp_value = (uint8_t *)&current_obj->obj_props;
        *p_length = 4;
        break;

    case OTS_OLF_CHAR_UUID:
        cause = APP_RESULT_INSUFFICIENT_RESOURCES;
        uint8_t *p_value;
        uint8_t olf_idx;
        if (p_entry->olf_attr_idx[0] == attrib_index)
        {
            olf_idx = 0;
        }
        else if (p_entry->olf_attr_idx[1] == attrib_index)
        {
            olf_idx = 1;
        }
        else
        {
            olf_idx = 2;
        }
        if (offset > (cl_cb->olf_cb[olf_idx].param_len + 1))
        {
            cause = APP_RESULT_INVALID_OFFSET;
            break;
        }
        p_value = ble_audio_mem_zalloc(cl_cb->olf_cb[olf_idx].param_len + 1);
        if (p_value == NULL)
        {
            break;
        }
        p_value[0] = cl_cb->olf_cb[olf_idx].filt_type;
        memcpy(&p_value[1], cl_cb->olf_cb[olf_idx].p_param, cl_cb->olf_cb[olf_idx].param_len);
        if (gatt_svc_read_confirm(conn_handle, cid, service_id, attrib_index, p_value + offset,
                                  cl_cb->olf_cb[olf_idx].param_len + 1 - offset,
                                  cl_cb->olf_cb[olf_idx].param_len + 1, APP_RESULT_SUCCESS))
        {
            cause = APP_RESULT_PENDING;
        }
        ble_audio_mem_free(p_value);
        break;
    default:
        {
            PROTOCOL_PRINT_ERROR1("[OTS]ots_attr_read_cb attrib_index = %d not found", attrib_index);
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }
    return (cause);
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_id           Connection ID.
 * @param service_id        Service ID.
 * @param index             Attribute index of characteristic data.
 * @param ccc_bits          CCCD bits from stack.
 * @return None
*/
void ots_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id, uint16_t index,
                        uint16_t ccc_bits)
{
    PROTOCOL_PRINT_INFO2("[OTS]ots_cccd_update_cb index = %d ccc_bits %x", index, ccc_bits);
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return;
    }
    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl, index,
                                                             p_entry->attr_num);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]ots_cccd_update_cb Error: attrib_index 0x%x", index);
        return;
    }

    T_LE_SRV_CFG *p_srv = ble_srv_alloc_by_srv_id(conn_handle, service_id);

    switch (char_uuid.uu.char_uuid16)
    {
    case OTS_OACP_CHAR_UUID:
        p_entry->oacp_cb.char_index = char_uuid.index;
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, OTS_SERV_OACP_CCCD_FLAG, true);
        break;

    case OTS_OLCP_CHAR_UUID:
        p_entry->olcp_cb.char_index = char_uuid.index;
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, OTS_SERV_OLCP_CCCD_FLAG, true);
        break;

    case OTS_OBJ_CHANGE_CHAR_UUID:
        p_entry->obj_change_index = char_uuid.index;
        ble_srv_set_cccd_flags(p_srv, ccc_bits, 0, OTS_SERV_OBJ_CHG_CCCD_FLAG, true);
        break;

    default:
        {
            break;
        }

    }


    return;
}

void ots_write_post_callback(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                             uint16_t attrib_index,
                             uint16_t length, uint8_t *p_value)
{
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return;
    }

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl, attrib_index,
                                                             p_entry->attr_num);
    bool need_ind = false;
    uint8_t ind_change[OTS_OBJ_CHG_CHAR_VAL_LEN] = {0};
    T_OTS_CL_CB *cl_cb = find_cl_by_conn_handle(conn_handle, service_id);
    if (cl_cb == NULL)
    {
        return;
    }


    switch (char_uuid.uu.char_uuid16)
    {
    case OTS_OBJ_NAME_CHAR_UUID:
        memcpy(cl_cb->current_obj->obj_name.name, p_value, length);
        need_ind = true;
        break;

    case OTS_OBJ_FIRST_CR_CHAR_UUID:
    case OTS_OBJ_LAST_MOD_CHAR_UUID:

        break;

    case OTS_OBJ_PROPS_CHAR_UUID:
        memcpy((uint8_t *)&cl_cb->current_obj->obj_props, p_value, 4);
        need_ind = true;
        break;

    case OTS_OACP_CHAR_UUID:
        handle_oacp_request(p_entry, cl_cb, p_value, length);
        break;

    case OTS_OLCP_CHAR_UUID:
        handle_olcp_request(p_entry, cl_cb, p_value, length);
        break;

    case OTS_OLF_CHAR_UUID:
        {
        }
        break;
    default:
        {
            PROTOCOL_PRINT_ERROR1("[OTS]ots_post_callback attrib_index = %d not found", attrib_index);
        }
        break;
    }

    if (need_ind)
    {
        ind_change[0] = OTS_OBJ_META_CHG_TRUE | OTS_SRC_OF_CHG_CLIENT;
        memcpy(&ind_change[1], (uint8_t *) & (cl_cb->current_obj->obj_id), OTS_OBJ_ID_LEN);
        ind_change_to_clients(p_entry, conn_handle, ind_change, OTS_OBJ_CHG_CHAR_VAL_LEN);
    }
}


/**
 * @brief write characteristic data from service.
 *
 * @param conn_id
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT ots_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index, T_WRITE_TYPE write_type,
                               uint16_t length, uint8_t *p_value,
                               P_FUN_EXT_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    PROTOCOL_PRINT_INFO1("[OTS]ots_attr_write_cb write_type = 0x%x", write_type);
    *p_write_ind_post_proc = NULL;

    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl, attrib_index,
                                                             p_entry->attr_num);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR2("[OTS]ots_attr_write_cb Error: attrib_index 0x%x, length %d",
                              attrib_index, length);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    T_OTS_CL_CB *cl_cb = find_cl_by_conn_handle(conn_handle, service_id);
    if (cl_cb == NULL)
    {
        cl_cb = alloc_cl_cb_by_conn_handle(conn_handle);
        if (cl_cb == NULL)
        {
            return APP_RESULT_REJECT;
        }
        cl_cb->service_id = service_id;
    }

    if ((char_uuid.uu.char_uuid16 != OTS_OACP_CHAR_UUID &&
         char_uuid.uu.char_uuid16 != OTS_OLCP_CHAR_UUID) &&
        !cl_cb->current_obj)
    {
        return (T_APP_RESULT)ATT_ERR_OTS_OBJ_NOT_SELECTED;
    }

    switch (char_uuid.uu.char_uuid16)
    {
    case OTS_OBJ_NAME_CHAR_UUID:
        if (length > OTS_OBJ_NAME_LEN_MAX)
        {
            cause = (T_APP_RESULT)ATT_ERR_OTS_WRITE_REJECTED;
        }
        else if (check_obj_name_exist(p_entry, p_value, length))
        {
            cause = (T_APP_RESULT)ATT_ERR_OTS_OBJ_NAME_ALREADY_EXIST;
        }
        break;

    case OTS_OBJ_FIRST_CR_CHAR_UUID:
    case OTS_OBJ_LAST_MOD_CHAR_UUID:
        //FIX TODO
        cause = (T_APP_RESULT)ATT_ERR_OTS_WRITE_REJECTED;
        break;

    case OTS_OBJ_PROPS_CHAR_UUID:
        if (length < 4)
        {
            cause = APP_RESULT_INVALID_VALUE_SIZE;
        }
        break;

    case OTS_OACP_CHAR_UUID:
        break;

    case OTS_OLCP_CHAR_UUID:

        break;

    case OTS_OLF_CHAR_UUID:
        {
            uint8_t filt_type = p_value[0];
            if (filt_type == OTS_CREATE_BETWEEN_TMS ||
                filt_type == OTS_MOD_BETWEEN_TMS)
            {
                cause = (T_APP_RESULT)ATT_ERR_OTS_WRITE_REJECTED;
            }
            else if (filt_type == OTS_CUR_SIZE_BETWEEN ||
                     filt_type == OTS_ALLOC_SIZE_BETWEEN)
            {
                if (length < 9)
                {
                    cause = APP_RESULT_INVALID_VALUE_SIZE;
                    break;
                }
                uint32_t size1 = *(uint32_t *)&p_value[1];
                uint32_t size2 = *(uint32_t *)&p_value[5];
                if (size1 > size2)
                {
                    cause = (T_APP_RESULT)ATT_ERR_OTS_WRITE_REJECTED;
                    break;
                }
            }

            uint8_t olf_idx = 0;
            if (p_entry->olf_attr_idx[0] == attrib_index)
            {
                olf_idx = 0;
            }
            else if (p_entry->olf_attr_idx[1] == attrib_index)
            {
                olf_idx = 1;
            }
            else
            {
                olf_idx = 2;
            }
            if (cl_cb->olf_cb[olf_idx].param_len > 0)
            {
                uint8_t *p;
                p = ble_audio_mem_zalloc(length - 1);
                if (p == NULL)
                {
                    cause = APP_RESULT_INSUFFICIENT_RESOURCES;
                    break;
                }
                ble_audio_mem_free(cl_cb->olf_cb[olf_idx].p_param);
                cl_cb->olf_cb[olf_idx].filt_type = p_value[0];
                cl_cb->olf_cb[olf_idx].p_param = p;
                memcpy(cl_cb->olf_cb[olf_idx].p_param, &p_value[1], length - 1);
                cl_cb->olf_cb[olf_idx].param_len = length - 1;
            }
        }
        break;
    default:
        {
            PROTOCOL_PRINT_ERROR1("[OTS]ots_attr_write_cb attrib_index = %d not found", attrib_index);
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }
    if (cause == APP_RESULT_SUCCESS)
    {
        *p_write_ind_post_proc = ots_write_post_callback;
    }
    return cause;
}

/**
 * @brief Media Control Service Callbacks.
*/
static const T_FUN_GATT_EXT_SERVICE_CBS ots_cbs =
{
    ots_attr_read_cb,  // Read callback function pointer
    ots_attr_write_cb, // Write callback function pointer
    ots_cccd_update_cb  // CCCD update callback function pointer
};

static bool set_ots_num_init(uint8_t ots_num)
{
    if (ots_num == 0 || p_ots_mem != NULL)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]set_ots_num_init: invalid param");
        return false;
    }
    p_ots_mem = ble_audio_mem_zalloc(ots_num * sizeof(T_OTS_SERV_CB));
    if (p_ots_mem == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]set_ots_num_init: allocate mem failed");
        return false;
    }
    total_ots_num = ots_num;
    return true;
}

static T_OTS_SERV_CB *ots_allocate_entry(uint16_t char_msk, uint16_t attr_feature)
{
    uint8_t i;
    if (p_ots_mem == NULL)
    {
        return NULL;
    }
    for (i = 0; i < total_ots_num; i++)
    {
        if (p_ots_mem[i].used == false)
        {
            memset(&p_ots_mem[i], 0, sizeof(T_OTS_SERV_CB));
            p_ots_mem[i].used = true;
            p_ots_mem[i].char_msk = char_msk;
            p_ots_mem[i].attr_feature = attr_feature;
            os_queue_init(&p_ots_mem[i].obj_queue);
            create_directory_listing_object(&p_ots_mem[i]);

            return &p_ots_mem[i];
        }
    }
    return NULL;
}

static bool ots_create_table(uint16_t char_msk, uint16_t attr_feature, T_OTS_SERV_CB *p_entry)
{
    uint16_t idx = 19;
    uint16_t attr_tbl_size = 0;

    if (char_msk & OBJ_FIRST_CR_CHAR_MSK)
    {
        idx += 2;
    }
    if (char_msk & OBJ_LAST_MOD_CHAR_MSK)
    {
        idx += 2;
    }
    if (char_msk & OBJ_LIST_FILTER_CHAR_MSK)
    {
        idx += 2 * 3;
    }
    if (char_msk & OBJ_CHG_CHAR_MSK)
    {
        idx += 3;
    }

    PROTOCOL_PRINT_TRACE1("[OTS]ots_create_table: total idx: %d", idx);
    p_entry->attr_num = idx;
    attr_tbl_size = idx * sizeof(T_ATTRIB_APPL);
    p_entry->p_attr_tbl = ble_audio_mem_zalloc(attr_tbl_size);

    if (p_entry->p_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]ots_create_table: allocate p_attr_tbl failed");
        return false;
    }
    idx = 0;
    memcpy(p_entry->p_attr_tbl, (uint8_t *)ots_prim_srv, sizeof(T_ATTRIB_APPL));
    idx++;
    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)ots_feat_mand_attr, 2 * sizeof(T_ATTRIB_APPL));
    idx += 2;

    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_name_mand_opt_attr, 2 * sizeof(T_ATTRIB_APPL));
    if ((attr_feature & OBJ_NAME_WRITE_MSK) == 0)
    {
        p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_WRITE);
    }
    idx += 2;
    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_type_mand_opt_attr, 2 * sizeof(T_ATTRIB_APPL));
    idx += 2;
    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_size_mand_opt_attr, 2 * sizeof(T_ATTRIB_APPL));
    idx += 2;

    if (char_msk & OBJ_FIRST_CR_CHAR_MSK)
    {
        memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_first_cr_opt_opt_attr, 2 * sizeof(T_ATTRIB_APPL));
        if ((attr_feature & OBJ_FIRST_CR_WRITE_MSK) == 0)
        {
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_WRITE);
        }
        idx += 2;
    }
    if (char_msk & OBJ_LAST_MOD_CHAR_MSK)
    {
        memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_last_mod_opt_opt_attr, 2 * sizeof(T_ATTRIB_APPL));
        if ((attr_feature & OBJ_LAST_MOD_WRITE_MSK) == 0)
        {
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_WRITE);
        }
        idx += 2;
    }

    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_id_mand_attr, 2 * sizeof(T_ATTRIB_APPL));
    idx += 2;

    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_props_mand_opt_attr, 2 * sizeof(T_ATTRIB_APPL));
    if ((attr_feature & OBJ_PROPS_WRITE_MSK) == 0)
    {
        p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_WRITE);
    }
    idx += 2;

    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)oacp_mand_attr, 3 * sizeof(T_ATTRIB_APPL));
    idx += 3;
    memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)olcp_mand_attr, 3 * sizeof(T_ATTRIB_APPL));
    idx += 3;
    if (char_msk & OBJ_LIST_FILTER_CHAR_MSK)
    {
        memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_list_filter_opt_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        p_entry->olf_attr_idx[0] = idx + 1;
        idx += 2;
        memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_list_filter_opt_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        p_entry->olf_attr_idx[1] = idx + 1;
        idx += 2;
        memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_list_filter_opt_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        p_entry->olf_attr_idx[2] = idx + 1;
        idx += 2;
    }
    if (char_msk & OBJ_CHG_CHAR_MSK)
    {
        memcpy(&p_entry->p_attr_tbl[idx], (uint8_t *)obj_changed_opt_opt_attr, 3 * sizeof(T_ATTRIB_APPL));
        idx += 3;
    }
    PROTOCOL_PRINT_TRACE2("[OTS]ots_create_table: create idx: %d, attr_tbl_size: %d", idx,
                          attr_tbl_size);
    return true;
}

static T_SERVER_ID ots_add_service(T_OTS_SERV_CB *p_entry, void *p_func)
{
    T_SERVER_ID service_id;
    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_entry->p_attr_tbl,
                              p_entry->attr_num * sizeof(T_ATTRIB_APPL),
                              &ots_cbs, NULL))
    {
        PROTOCOL_PRINT_ERROR0("[OTS]ots_add_service: add service fail");
        service_id = 0xff;
    }
    p_entry->service_id = service_id;
    return service_id;
}


T_OTS_SERV_CB *ots_get_attr_tbl(T_SERVER_ID service_id)
{
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);
    return p_entry;
}

T_SERVER_ID ots_add_server(uint16_t char_msk, uint16_t attr_feature)
{
    T_OTS_SERV_CB *p_entry;
    p_entry = ots_allocate_entry(char_msk, attr_feature);
    if (p_entry == NULL)
    {
        return 0xFF;
    }
    if (ots_create_table(char_msk, attr_feature, p_entry))
    {
        if (ots_add_service(p_entry, NULL) == 0xFF)
        {
            goto error;
        }
    }
    else
    {
        PROTOCOL_PRINT_ERROR0("[OTS]ots_add_server: create service table fail");
        goto error;
    }
    return p_entry->service_id;
error:
    if (p_entry->p_attr_tbl != NULL)
    {
        ble_audio_mem_free(p_entry->p_attr_tbl);
        p_entry->p_attr_tbl = NULL;
    }
    p_entry->used = false;
    return 0xFF;
}


static void ots_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        if (param->conn_state.state == GAP_CONN_STATE_CONNECTED)
        {
            PROTOCOL_PRINT_INFO0("[OTS]ots_dm_cback: GAP_CONN_STATE_CONNECTED");
        }
        else if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            PROTOCOL_PRINT_INFO0("[OTS]ots_dm_cback: GAP_CONN_STATE_DISCONNECTED");
            if (p_link)
            {
                delete_cl_cb_by_id(p_link->conn_handle);
            }
        }

        break;

    default:
        break;
    }
}

bool ots_server_init(uint8_t ots_num)
{
    if (!set_ots_num_init(ots_num))
    {
        return false;
    }

    p_cl_queue = ble_audio_mem_zalloc(sizeof(T_OS_QUEUE));
    if (p_cl_queue == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[OTS]ots_server_init: alloc p_cl_queue fail!");
        ble_audio_mem_free(p_ots_mem);
        return false;
    }
    os_queue_init(p_cl_queue);

    le_coc_enable();
    ble_dm_cback_register(ots_dm_cback);
    return true;
}

T_OS_QUEUE *ots_get_obj_queue(T_SERVER_ID ots_service_id)
{
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(ots_service_id);
    if (p_entry == NULL)
    {
        return NULL;
    }
    return &p_entry->obj_queue;
}

void ots_set_feature(T_SERVER_ID ots_service_id, uint32_t oacp_feature, uint32_t olcp_feature)
{
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(ots_service_id);
    if (p_entry == NULL)
    {
        return;
    }
    p_entry->ots_features[0] = oacp_feature;
    p_entry->ots_features[1] = olcp_feature;
}

void delete_ots_object_internal(T_SERVER_ID service_id, uint64_t obj_id)
{
    int i;
    T_OBJECT_CB *p_obj;
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(service_id);
    if (p_entry == NULL)
    {
        return;
    }

    for (i = 0; i < p_entry->obj_queue.count; i++)
    {
        p_obj = (T_OBJECT_CB *)os_queue_peek(&p_entry->obj_queue, i);
        if (p_obj->obj_id == obj_id)
        {
            delete_ots_obj(p_entry, 0xFF00, p_obj);
            break;
        }
    }
}


T_OBJECT_CB *create_ots_object_internal(T_SERVER_ID ots_service_id, T_OTS_OBJ_PARAM *p_param)
{
    T_OTS_SERV_CB *p_entry = ots_find_by_srv_id(ots_service_id);
    uint8_t ind_change[OTS_OBJ_CHG_CHAR_VAL_LEN] = {0};
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]create_ots_object_internal: entry is NULL, service id: 0x%x",
                              ots_service_id);
        return NULL;
    }
    T_OBJECT_CB *object;
    if (p_param->p_uuid == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]create_ots_object_internal: uuid is NULL, service id: 0x%x",
                              ots_service_id);
        return NULL;
    }

    object = (T_OBJECT_CB *)ble_audio_mem_zalloc(sizeof(T_OBJECT_CB));
    if (object == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]create_ots_object_internal: alloc object fail service id: 0x%x",
                              ots_service_id);
        return NULL;
    }

    object->obj_id = alloc_obj_id(p_entry);
    if (object->obj_id == 0)
    {
        PROTOCOL_PRINT_ERROR1("[OTS]create_ots_object_internal: no free object id, service id: 0x%x",
                              ots_service_id);
        goto fail;
    }

    if (p_param->len > 0)
    {
        object->alloc_buf = ble_audio_mem_zalloc(p_param->len);
        if (object->alloc_buf == NULL)
        {
            PROTOCOL_PRINT_ERROR1("[OTS]create_ots_object_internal: object->alloc_buff fail,service id: 0x%x",
                                  ots_service_id);
            free_obj_id(p_entry, object->obj_id);
            goto fail;
        }
        if (p_param->p_value != NULL)
        {
            memcpy(object->alloc_buf, p_param->p_value, p_param->len);
        }
    }

    object->obj_size[0] = p_param->len;
    object->obj_size[1] = p_param->len;
    //FIX TODO may be we should check the obj name
    object->obj_name.name_len = p_param->name_len;
    memcpy(object->obj_name.name, p_param->p_name, p_param->name_len);
    object->obj_type.uuid_size = p_param->uuid_size;
    memcpy(object->obj_type.uu.char_uuid128, p_param->p_uuid, p_param->uuid_size);

    object->obj_props = p_param->props;
    os_queue_in(&p_entry->obj_queue, (void *)object);

    ind_change[0] |= OTS_OBJ_CREATE;
    memcpy(&ind_change[1], (uint8_t *) & (object->obj_id), OTS_OBJ_ID_LEN);
    ind_change_to_clients(p_entry, 0xFFFF, ind_change, OTS_OBJ_CHG_CHAR_VAL_LEN);
    return object;

fail:
    ble_audio_mem_free(object);
    return NULL;

}

#if LE_AUDIO_DEINIT
void ots_server_deinit(void)
{
    if (p_cl_queue)
    {
        T_OTS_CL_CB *cl_cb;
        while ((cl_cb = os_queue_out(p_cl_queue)) != NULL)
        {
            for (uint8_t j = 0; j < 3; j++)
            {
                if (cl_cb->olf_cb[j].p_param != NULL)
                {
                    ble_audio_mem_free(cl_cb->olf_cb[j].p_param);
                }
            }
            ble_audio_mem_free(cl_cb);
        }
        ble_audio_mem_free(p_cl_queue);
        p_cl_queue = NULL;
    }
    if (p_ots_mem)
    {
        T_OTS_SERV_CB *p_entry;
        T_OBJECT_CB *object;

        for (uint8_t i = 0; i < total_ots_num; i++)
        {
            p_entry = &p_ots_mem[i];
            while ((object = os_queue_out(&p_entry->obj_queue)) != NULL)
            {
                if (object->temp_write_buf)
                {
                    ble_audio_mem_free(object->temp_write_buf);
                }
                if (object->alloc_buf)
                {
                    ble_audio_mem_free(object->alloc_buf);
                }
                ble_audio_mem_free(object);
            }
            if (p_entry->p_attr_tbl)
            {
                ble_audio_mem_free(p_entry->p_attr_tbl);
            }
        }
        ble_audio_mem_free(p_ots_mem);
        p_ots_mem = NULL;
    }
    total_ots_num = 0;
}
#endif
#endif
