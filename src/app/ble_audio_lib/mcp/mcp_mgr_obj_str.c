/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     mcp_mgr_obj_str.c
* @brief    Media Control service object structure.
* @details  Interfaces to create object structure
* @author   cheng_cai
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include "string.h"
#include "trace.h"
#include "bt_gatt_svc.h"
#include "mcp_mgr_int.h"
#include "mcp_mgr_obj_str.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_MCP_SERVER_SUPPORT
#if LE_AUDIO_OTS_SERV_SUPPORT
uint64_t mcs_find_parent_id_by_gp_id(T_MCS_CB *p_entry, uint64_t gp_obj_id)
{
    int i;
    T_GP_ELEM *p_gp;

    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_gp->obj_id == gp_obj_id)
        {
            if (p_gp->parent_gp != NULL)
            {
                return p_gp->parent_gp->obj_id;
            }
            else
            {
                return gp_obj_id;    //if the parent gp is null, the gp may be the root gp
            }
        }
    }

    return 0;
}

T_TK_ELEM *mcs_find_tk_by_tk_id(T_MCS_CB *p_entry, uint64_t tk_obj_id)
{
    int i, j;
    T_GP_ELEM *p_gp;
    T_TK_ELEM *p_tk;

    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_gp == NULL)
        {
            PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_find_tk_by_tk_id, group is NULL");
            continue;
        }
        for (j = 0; j < p_gp->tr_queue.count; j++)
        {
            p_tk = (T_TK_ELEM *)os_queue_peek(&p_gp->tr_queue, j);
            if (p_tk && p_tk->obj_id == tk_obj_id)
            {
                return p_tk;
            }
        }
    }
    return NULL;
}


void mcs_create_objs_complt(T_MCS_CB *p_entry)
{
    int i, j, idx = 0;
    T_GP_ELEM *p_gp, *p_temp_gp;
    T_TK_ELEM *p_tk;
    T_MCS_GP_OBJ *p_value = NULL;

    if (p_entry->ots_service_id == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_objs_complt, ots_service_id is 0xFF!");
        return;
    }
    PROTOCOL_PRINT_TRACE1("[MCS_OBJ]mcs_create_objs_complt, mcs_gp_tk_num: %d", p_entry->mcs_gp_tk_num);

    if (p_entry->mcs_gp_tk_num > 0)
    {
        p_value = ble_audio_mem_zalloc(p_entry->mcs_gp_tk_num * sizeof(T_MCS_GP_OBJ));
        if (p_value == NULL)
        {
            PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_objs_complt, alloc p_value fail");
            return;
        }
    }
    else
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_objs_complt, no group or track");
        return;
    }
    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        idx = 0;
        p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_gp == NULL)
        {
            continue;
        }

        for (j = 0; j < p_gp->tr_queue.count; j++)
        {
            p_tk = (T_TK_ELEM *)os_queue_peek(&p_gp->tr_queue, j);
            if (p_tk)
            {
                p_value[idx].type = MCS_TRACK_OBJ_TYPE;
                memcpy(p_value[idx].obj_id, (uint8_t *)&p_tk->obj_id, OTS_OBJ_ID_LEN);
                idx++;
            }
        }

        //find group which parent group is p_gp
        for (j = 0; j < p_entry->gp_obj_queue.count; j++)
        {
            p_temp_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, j);
            if (p_temp_gp &&
                p_temp_gp->parent_gp == p_gp)
            {
                p_value[idx].type = MCS_GROUP_OBJ_TYPE;
                memcpy(p_value[idx].obj_id, (uint8_t *)&p_temp_gp->obj_id, OTS_OBJ_ID_LEN);
                idx++;
            }
        }

        update_obj_value_by_id(p_entry->ots_service_id, p_gp->obj_id, (uint8_t *)p_value,
                               idx * sizeof(T_MCS_GP_OBJ));
    }

    ble_audio_mem_free(p_value);
}

void mcs_update_gp_value(T_MCS_CB *p_entry, uint64_t gp_obj_id)
{
    int i = 0, idx = 0;
    T_GP_ELEM *p_gp, *p_temp_gp;
    T_TK_ELEM *p_tk;
    T_MCS_GP_OBJ *p_value = NULL;

    if (p_entry->ots_service_id == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_update_gp_value, ots_service_id is 0xFF!");
        return;
    }

    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_gp == NULL)
        {
            continue;
        }
        else if (p_gp->obj_id == gp_obj_id)
        {
            break;
        }
    }
    if ((i == p_entry->gp_obj_queue.count) || (p_gp == NULL))
    {
        PROTOCOL_PRINT_ERROR1("[MCS_OBJ]mcs_update_gp_value, can't find group : %d", (uint32_t)gp_obj_id);
        return;
    }

    if (p_entry->mcs_gp_tk_num > 0)
    {
        p_value = ble_audio_mem_zalloc(p_entry->mcs_gp_tk_num * sizeof(T_MCS_GP_OBJ));
        if (p_value == NULL)
        {
            PROTOCOL_PRINT_ERROR1("[MCS_OBJ]mcs_update_gp_value, alloc p_value fail, mcs_gp_tk_num = %d",
                                  p_entry->mcs_gp_tk_num);
            return;
        }
    }
    else
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_update_gp_value, no group or track");
        return;
    }
    //find group which parent group is p_gp
    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_temp_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_temp_gp &&
            p_temp_gp->parent_gp == p_gp)
        {
            p_value[idx].type = MCS_GROUP_OBJ_TYPE;
            memcpy(p_value[idx].obj_id, (uint8_t *)&p_temp_gp->obj_id, OTS_OBJ_ID_LEN);
            idx++;
        }
    }

    for (i = 0; i < p_gp->tr_queue.count; i++)
    {
        p_tk = (T_TK_ELEM *)os_queue_peek(&p_gp->tr_queue, i);
        if (p_tk)
        {
            p_value[idx].type = MCS_TRACK_OBJ_TYPE;
            memcpy(p_value[idx].obj_id, (uint8_t *)&p_tk->obj_id, OTS_OBJ_ID_LEN);
            idx++;
        }
    }
    update_obj_value_by_id(p_entry->ots_service_id, gp_obj_id, (uint8_t *)p_value,
                           idx * sizeof(T_MCS_GP_OBJ));

    ble_audio_mem_free(p_value);
}

void mcs_delete_track(T_MCS_CB *p_entry, uint64_t gp_obj_id, uint64_t tk_obj_id)
{
    int i;
    T_GP_ELEM *p_gp;
    if (p_entry->ots_service_id == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_delete_track, ots_service_id is 0xFF!");
        return;
    }

    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_gp->obj_id == gp_obj_id)
        {
            break;
        }
    }

    if (i == p_entry->gp_obj_queue.count)
    {
        return;
    }

    T_TK_ELEM *p_tk;
    for (i = 0; i < p_gp->tr_queue.count; i++)
    {
        p_tk = (T_TK_ELEM *)os_queue_peek(&p_gp->tr_queue, i);
        if (p_tk->obj_id == tk_obj_id)
        {
            break;
        }
    }

    if (i == p_gp->tr_queue.count)
    {
        return;
    }
    os_queue_delete(&p_gp->tr_queue, p_tk);

    ble_audio_mem_free(p_tk);
    delete_ots_object_internal(p_entry->ots_service_id, tk_obj_id);
    p_entry->mcs_gp_tk_num--;
    mcs_update_gp_value(p_entry, gp_obj_id);
}

//when the group is removed , the all tracks in this group should be removed too
void mcs_delete_gp(T_MCS_CB *p_entry, uint64_t gp_obj_id)
{
    int i;
    T_GP_ELEM *p_gp, *p_temp_gp;
    T_TK_ELEM *p_tk;
    T_OS_QUEUE temp_queue;
    T_OS_QUEUE del_queue;
    bool found = false;

    if (p_entry->ots_service_id == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_delete_gp, ots_service_id is 0xFF!");
        return;
    }

    uint64_t parent_obj_id = mcs_find_parent_id_by_gp_id(p_entry, gp_obj_id);
    //remove relevant tracks
    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_gp == NULL)
        {
            PROTOCOL_PRINT_ERROR1("[MCS_OBJ]mcs_delete_gp, can not find group %d", i);
            continue;
        }
        if (p_gp->obj_id == gp_obj_id)
        {
            while (p_gp->tr_queue.count > 0)
            {
                p_tk = (T_TK_ELEM *)os_queue_out(&p_gp->tr_queue);
                if (p_tk != NULL)
                {
                    delete_ots_object_internal(p_entry->ots_service_id, p_tk->obj_id);
                    ble_audio_mem_free(p_tk);
                    p_entry->mcs_gp_tk_num--;
                }
            }
        }
        else
        {
            p_temp_gp = p_gp;
            while (p_gp->parent_gp != NULL)
            {
                p_gp = p_gp->parent_gp;
                if (p_gp->obj_id == gp_obj_id)
                {
                    while (p_temp_gp->tr_queue.count > 0)
                    {
                        p_tk = (T_TK_ELEM *)os_queue_out(&p_temp_gp->tr_queue);
                        if (p_tk != NULL)
                        {
                            delete_ots_object_internal(p_entry->ots_service_id, p_tk->obj_id);
                            ble_audio_mem_free(p_tk);
                            p_entry->mcs_gp_tk_num--;
                        }
                    }
                    break;
                }
            }
        }
    }

    //remove group and sub group
    os_queue_init(&temp_queue);
    os_queue_init(&del_queue);
    while (p_entry->gp_obj_queue.count > 0)
    {
        found = false;
        p_gp = (T_GP_ELEM *)os_queue_out(&p_entry->gp_obj_queue);
        p_temp_gp = p_gp;
        if (p_gp == NULL)
        {
            PROTOCOL_PRINT_ERROR1("[MCS_OBJ]mcs_delete_gp, can not find group, count: %d",
                                  p_entry->gp_obj_queue.count);
            continue;
        }

        if (p_gp->obj_id == gp_obj_id)
        {
            os_queue_in(&del_queue, p_gp);
            continue;
        }
        else
        {

            while (p_gp->parent_gp != NULL)
            {
                p_gp = p_gp->parent_gp;
                if (p_gp->obj_id == gp_obj_id)
                {
                    found = true;
                    os_queue_in(&del_queue, p_temp_gp);
                    break;
                }
            }
        }
        if (found)
        {
            continue;
        }
        os_queue_in(&temp_queue, p_temp_gp);
    }

    temp_queue.flags = 0;
    p_entry->gp_obj_queue = temp_queue;


    while (del_queue.count > 0)
    {
        p_gp = (T_GP_ELEM *)os_queue_out(&del_queue);
        delete_ots_object_internal(p_entry->ots_service_id, p_gp->obj_id);
        ble_audio_mem_free(p_gp);
        p_entry->mcs_gp_tk_num--;
    }

    if (parent_obj_id != 0)
    {
        mcs_update_gp_value(p_entry, parent_obj_id);
    }
}

uint64_t mcs_create_track(T_MCS_CB *p_entry, T_MCS_TK_ELEM *p_tk_elem,
                          uint64_t gp_obj_id, uint8_t *p_id3v2_fmt, uint16_t fmt_len)
{
    int i;
    T_GP_ELEM *p_gp;
    if (p_entry->ots_service_id == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_track, ots_service_id is 0xFF!");
        return 0;
    }
    uint16_t elem_len = p_tk_elem->tk_name_len + p_tk_elem->artist_name_len +
                        p_tk_elem->alb_name_len + p_tk_elem->elst_year_len +
                        p_tk_elem->latest_year_len + p_tk_elem->genre_len;
    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_gp && p_gp->obj_id == gp_obj_id)
        {
            break;
        }
    }

    if (i == p_entry->gp_obj_queue.count)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_track, can't find group object id!");
        return 0;
    }

    T_TK_ELEM *p_spec_elem = ble_audio_mem_zalloc(sizeof(T_TK_ELEM) + elem_len);
    if (p_spec_elem == NULL)
    {
        return 0;
    }

    if (p_tk_elem->tk_name_len > 0)
    {
        p_spec_elem->tk_elem.p_tk_name = (uint8_t *)(p_spec_elem + 1);
        memcpy(p_spec_elem->tk_elem.p_tk_name, p_tk_elem->p_tk_name, p_tk_elem->tk_name_len);
        p_spec_elem->tk_elem.tk_name_len = p_tk_elem->tk_name_len;
    }

    if (p_tk_elem->artist_name_len > 0)
    {
        p_spec_elem->tk_elem.p_artist_name = p_spec_elem->tk_elem.p_tk_name + p_tk_elem->tk_name_len;
        memcpy(p_spec_elem->tk_elem.p_artist_name, p_tk_elem->p_artist_name, p_tk_elem->artist_name_len);
        p_spec_elem->tk_elem.artist_name_len = p_tk_elem->artist_name_len;
    }

    if (p_tk_elem->alb_name_len > 0)
    {
        p_spec_elem->tk_elem.p_alb_name = p_spec_elem->tk_elem.p_artist_name + p_tk_elem->artist_name_len;
        memcpy(p_spec_elem->tk_elem.p_alb_name, p_tk_elem->p_alb_name, p_tk_elem->alb_name_len);
        p_spec_elem->tk_elem.alb_name_len = p_tk_elem->alb_name_len;
    }

    if (p_tk_elem->elst_year_len > 0)
    {
        p_spec_elem->tk_elem.p_elst_year = p_spec_elem->tk_elem.p_alb_name + p_tk_elem->alb_name_len;
        memcpy(p_spec_elem->tk_elem.p_elst_year, p_tk_elem->p_elst_year, p_tk_elem->elst_year_len);
        p_spec_elem->tk_elem.elst_year_len = p_tk_elem->elst_year_len;
    }

    if (p_tk_elem->latest_year_len > 0)
    {
        p_spec_elem->tk_elem.p_latest_year = p_spec_elem->tk_elem.p_elst_year + p_tk_elem->elst_year_len;
        memcpy(p_spec_elem->tk_elem.p_latest_year, p_tk_elem->p_latest_year, p_tk_elem->latest_year_len);
        p_spec_elem->tk_elem.latest_year_len = p_tk_elem->latest_year_len;
    }

    if (p_tk_elem->genre_len > 0)
    {
        p_spec_elem->tk_elem.p_genre = p_spec_elem->tk_elem.p_latest_year + p_tk_elem->latest_year_len;
        memcpy(p_spec_elem->tk_elem.p_genre, p_tk_elem->p_genre, p_tk_elem->genre_len);
        p_spec_elem->tk_elem.genre_len = p_tk_elem->genre_len;
    }

    T_OTS_OBJ_PARAM param;
    uint16_t uuid = TRACK_OBJ_TYPE;
    param.uuid_size = UUID_16BIT_SIZE;
    param.p_uuid = (uint8_t *)&uuid;
    param.p_name = p_tk_elem->p_tk_name;
    param.name_len = p_tk_elem->tk_name_len;
    param.p_value = p_id3v2_fmt;
    param.len = fmt_len;
    param.props = (1 << OBJ_PROP_READ_BIT_NUM);

    T_OBJECT_CB *p_obj = create_ots_object_internal(p_entry->ots_service_id, &param);
    if (p_obj == NULL)
    {
        goto error;
    }

    p_spec_elem->obj_id = p_obj->obj_id;
    p_spec_elem->gp_obj_id = gp_obj_id;

    if (p_tk_elem->tk_seg_num > 0)
    {
        uint16_t seg_total_len = 0;
        for (i = 0; i < p_tk_elem->tk_seg_num; i++)
        {
            seg_total_len += (5 + p_tk_elem->p_tk_seg[i].seg_name_len);
        }
        uint8_t data[seg_total_len];
        seg_total_len = 0;
        for (i = 0; i < p_tk_elem->tk_seg_num; i++)
        {
            data[seg_total_len++] = p_tk_elem->p_tk_seg[i].seg_name_len;
            memcpy(&data[seg_total_len], p_tk_elem->p_tk_seg[i].p_seg_name,
                   p_tk_elem->p_tk_seg[i].seg_name_len);
            seg_total_len += p_tk_elem->p_tk_seg[i].seg_name_len;
            memcpy(&data[seg_total_len], (uint8_t *)&p_tk_elem->p_tk_seg[i].seg_pos, 4);
            seg_total_len += 4;
        }
        uint16_t uuid = TRACK_SEGMENGS_OBJ_TYPE;
        //FIX TODO segment name
        uint8_t seg_name[20] = {0};
        uint8_t seg_name_len = 0;
        memcpy(seg_name, "SGM_", 4);
        if (p_tk_elem->tk_name_len > 0)
        {
            seg_name_len = p_tk_elem->tk_name_len > 16 ? 16 : p_tk_elem->tk_name_len;
            memcpy(seg_name + 4, p_tk_elem->p_tk_name, seg_name_len);
        }

        seg_name_len += 4;
        param.uuid_size = UUID_16BIT_SIZE;
        param.p_uuid = (uint8_t *)&uuid;
        param.p_name = seg_name;
        param.name_len = seg_name_len;
        param.p_value = data;
        param.len = seg_total_len;
        param.props = (1 << OBJ_PROP_READ_BIT_NUM);

        p_obj = create_ots_object_internal(p_entry->ots_service_id, &param);
        if (p_obj != NULL)
        {
            p_spec_elem->seg_obj_id = p_obj->obj_id;
            PROTOCOL_PRINT_TRACE2("[MCS_OBJ]mcs_create_track, obj_id 0x%x, seg_len: %d", (int)p_obj->obj_id,
                                  seg_total_len);
        }
    }
    os_queue_in(&p_gp->tr_queue, p_spec_elem);
    p_entry->mcs_gp_tk_num++;

    return p_spec_elem->obj_id;
error:
    ble_audio_mem_free(p_spec_elem);
    return 0;
}


uint64_t mcs_create_spec_gp(T_MCS_CB *p_entry, T_MCS_GP_ELEM *p_gp_elem, uint64_t parent_obj_id)
{
    int i;
    T_GP_ELEM *p_parent_gp;
    if (p_entry->ots_service_id == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_spec_gp, ots_service_id is 0xFF!");
        return 0;
    }
    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        p_parent_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (p_parent_gp && p_parent_gp->obj_id == parent_obj_id)
        {
            break;
        }
    }

    if (i == p_entry->gp_obj_queue.count)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_spec_gp, can't find parent object id!");
        return 0;
    }

    T_GP_ELEM *p_spec_elem = ble_audio_mem_zalloc(sizeof(T_GP_ELEM) + p_gp_elem->gp_name_len +
                                                  p_gp_elem->artist_name_len);
    if (p_spec_elem == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_spec_gp, alloc p_spec_elem fail!");
        return 0;
    }
    os_queue_init(&p_spec_elem->tr_queue);

    T_OTS_OBJ_PARAM param;
    uint16_t uuid = GROUP_OBJ_TYPE;
    param.uuid_size = UUID_16BIT_SIZE;
    param.p_uuid = (uint8_t *)&uuid;
    param.p_name = p_gp_elem->p_gp_name;
    param.name_len = p_gp_elem->gp_name_len;
    param.p_value = p_gp_elem->p_gp_value;
    param.len = p_gp_elem->gp_value_len;
    param.props = (1 << OBJ_PROP_READ_BIT_NUM);

    T_OBJECT_CB *p_obj = create_ots_object_internal(p_entry->ots_service_id, &param);
    if (p_obj == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_spec_gp, create object fail!");
        goto error;
    }

    p_spec_elem->gp_elem.p_gp_name = (uint8_t *)(p_spec_elem + 1);
    memcpy(p_spec_elem->gp_elem.p_gp_name, p_gp_elem->p_gp_name, p_gp_elem->gp_name_len);
    p_spec_elem->gp_elem.gp_name_len = p_gp_elem->gp_name_len;
    if (p_gp_elem->artist_name_len > 0)
    {
        p_spec_elem->gp_elem.p_artist_name = ((uint8_t *)(p_spec_elem + 1)) + p_gp_elem->gp_name_len;
        memcpy(p_spec_elem->gp_elem.p_artist_name, p_gp_elem->p_artist_name, p_gp_elem->artist_name_len);
    }
    else
    {
        p_spec_elem->gp_elem.p_artist_name = NULL;
    }
    p_spec_elem->gp_elem.artist_name_len = p_gp_elem->artist_name_len;
    p_spec_elem->obj_id = p_obj->obj_id;
    p_spec_elem->parent_gp = p_parent_gp;
    os_queue_in(&p_entry->gp_obj_queue, p_spec_elem);
    p_entry->mcs_gp_tk_num++;

    return p_spec_elem->obj_id;

error:
    ble_audio_mem_free(p_spec_elem);
    return 0;
}

uint64_t mcs_create_root_gp(T_MCS_CB *p_entry, T_MCS_GP_ELEM *p_gp_elem)
{
    if (p_entry->ots_service_id == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_root_gp, ots_service_id is 0xFF!");
        return 0;
    }

    T_GP_ELEM *p_root_elem = ble_audio_mem_zalloc(sizeof(T_GP_ELEM) + p_gp_elem->gp_name_len);
    if (p_root_elem == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_root_gp, alloc p_root_elem fail!");
        return 0;
    }

    os_queue_init(&p_root_elem->tr_queue);

    T_OTS_OBJ_PARAM param;
    uint16_t uuid = GROUP_OBJ_TYPE;
    param.uuid_size = UUID_16BIT_SIZE;
    param.p_uuid = (uint8_t *)&uuid;
    param.p_name = p_gp_elem->p_gp_name;
    param.name_len = p_gp_elem->gp_name_len;
    param.p_value = NULL;
    param.len = 0;
    param.props = (1 << OBJ_PROP_READ_BIT_NUM);

    T_OBJECT_CB *p_obj = create_ots_object_internal(p_entry->ots_service_id, &param);
    if (p_obj == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[MCS_OBJ]mcs_create_root_gp, create object fail!");
        goto error;
    }

    p_root_elem->gp_elem.p_gp_name = (uint8_t *)(p_root_elem + 1);
    memcpy(p_root_elem->gp_elem.p_gp_name, p_gp_elem->p_gp_name, p_gp_elem->gp_name_len);
    p_root_elem->gp_elem.gp_name_len = p_gp_elem->gp_name_len;
    p_root_elem->gp_elem.artist_name_len = 0;
    p_root_elem->gp_elem.p_artist_name = NULL;

    p_root_elem->obj_id = p_obj->obj_id;
    p_root_elem->parent_gp = NULL;

    os_queue_in(&p_entry->gp_obj_queue, p_root_elem);
    p_entry->mcs_gp_tk_num++;
    return p_root_elem->obj_id;
error:
    ble_audio_mem_free(p_root_elem);
    return 0;
}
#endif
#endif


