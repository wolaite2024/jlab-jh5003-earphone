#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
#include <string.h>
#include "trace.h"
#include "stdlib.h"
#include "bt_gattc_storage.h"
#include "gap_conn_le.h"
#include "gatt.h"

uint8_t gattc_storage_get_srv_instance_id(T_GATTC_STORAGE_CB *p_gattc_cb, bool is_uuid16,
                                          uint16_t uuid16, uint8_t *p_uuid128)
{
    uint8_t instance_id = 0;
    T_ATTR_DIS_SRV *p_srv_dis_cb = NULL;

    for (uint8_t i = 0; i < p_gattc_cb->gattc_dis_list.count; i++)
    {
        p_srv_dis_cb = (T_ATTR_DIS_SRV *)os_queue_peek(&p_gattc_cb->gattc_dis_list, i);
        if (p_srv_dis_cb)
        {
            if (is_uuid16)
            {
                if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                    p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
                {
                    if (uuid16 == p_srv_dis_cb->srv_data.srv_uuid16.uuid16)
                    {
                        instance_id++;
                    }
                }
            }
            else
            {
                if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                    p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
                {
                    if (memcmp(p_uuid128, p_srv_dis_cb->srv_data.srv_uuid128.uuid128, 16) == 0)
                    {
                        instance_id++;
                    }
                }
            }
        }
    }
    return instance_id;
}

uint8_t gattc_storage_get_char_instance_id(T_ATTR_DIS_SRV *p_srv_dis_cb, bool is_uuid16,
                                           uint16_t uuid16,
                                           uint8_t *p_uuid128)
{
    uint8_t instance_id = 0;
    T_ATTR_DIS_CHAR *p_char_data;

    for (uint8_t i = 0; i < p_srv_dis_cb->char_queue.count; i++)
    {
        p_char_data = (T_ATTR_DIS_CHAR *)os_queue_peek(&p_srv_dis_cb->char_queue, i);
        if (p_char_data)
        {
            if (is_uuid16)
            {
                if (p_char_data->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID16)
                {
                    if (uuid16 == p_char_data->char_data.char_uuid16.uuid16)
                    {
                        instance_id++;
                    }
                }
            }
            else
            {
                if (p_char_data->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID128)
                {
                    if (memcmp(p_uuid128, p_char_data->char_data.char_uuid128.uuid128, 16) == 0)
                    {
                        instance_id++;
                    }
                }
            }
        }
    }
    return instance_id;
}

bool gattc_storage_add_primary_service(T_GATTC_STORAGE_CB *p_gattc_cb, bool uuid16,
                                       T_DISCOVERY_RESULT_DATA *p_result_data)
{
    T_ATTR_DIS_SRV *p_srv_dis_cb = NULL;
    T_ATTR_UUID srv_uuid;
    T_SPEC_GATTC_CB *p_spec_cb;

    if (uuid16)
    {
        srv_uuid.is_uuid16 = true;
        srv_uuid.p.uuid16 = p_result_data->p_srv_uuid16_disc_data->uuid16;
        p_spec_cb = gatt_spec_client_find_by_uuid(&srv_uuid);
        if (p_spec_cb == NULL && ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_REG_SVC_BIT) != 0))
        {
            BTM_PRINT_TRACE1("gattc_storage_add_primary_service: skip uuid16 0x%x",
                             p_result_data->p_srv_uuid16_disc_data->uuid16);
            return true;
        }
        p_srv_dis_cb = calloc(1,
                              sizeof(T_ATTR_DIS_SRV) - ATTR_DATA_STRUCT_LEN + sizeof(T_ATTR_SRV_UUID16));
        if (p_srv_dis_cb == NULL)
        {
            goto error;
        }
        p_srv_dis_cb->srv_data.srv_uuid16.hdr.attr_type = ATTR_TYPE_PRIMARY_SRV_UUID16;
        p_srv_dis_cb->srv_data.srv_uuid16.hdr.instance_id = gattc_storage_get_srv_instance_id(p_gattc_cb,
                                                            true,
                                                            p_result_data->p_srv_uuid16_disc_data->uuid16, NULL);
        p_srv_dis_cb->srv_data.srv_uuid16.hdr.att_handle =
            p_result_data->p_srv_uuid16_disc_data->att_handle;
        p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle =
            p_result_data->p_srv_uuid16_disc_data->end_group_handle;
        p_srv_dis_cb->srv_data.srv_uuid16.uuid16 = p_result_data->p_srv_uuid16_disc_data->uuid16;
        p_srv_dis_cb->p_spec_cb = p_spec_cb;
    }
    else
    {
        srv_uuid.is_uuid16 = false;
        memcpy(srv_uuid.p.uuid128, p_result_data->p_srv_uuid128_disc_data->uuid128, 16);
        p_spec_cb = gatt_spec_client_find_by_uuid(&srv_uuid);
        if (p_spec_cb == NULL && ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_REG_SVC_BIT) != 0))
        {
            BTM_PRINT_TRACE0("gattc_storage_add_primary_service: skip uuid128");
            return true;
        }
        p_srv_dis_cb = calloc(1,
                              sizeof(T_ATTR_DIS_SRV) - ATTR_DATA_STRUCT_LEN + sizeof(T_ATTR_SRV_UUID128));
        if (p_srv_dis_cb == NULL)
        {
            goto error;
        }
        p_srv_dis_cb->srv_data.srv_uuid128.hdr.attr_type = ATTR_TYPE_PRIMARY_SRV_UUID128;
        p_srv_dis_cb->srv_data.srv_uuid128.hdr.instance_id = gattc_storage_get_srv_instance_id(p_gattc_cb,
                                                             false, 0, p_result_data->p_srv_uuid128_disc_data->uuid128);;
        p_srv_dis_cb->srv_data.srv_uuid128.hdr.att_handle =
            p_result_data->p_srv_uuid128_disc_data->att_handle;
        p_srv_dis_cb->srv_data.srv_uuid128.end_group_handle =
            p_result_data->p_srv_uuid128_disc_data->end_group_handle;
        memcpy(p_srv_dis_cb->srv_data.srv_uuid128.uuid128, p_result_data->p_srv_uuid128_disc_data->uuid128,
               16);
        p_srv_dis_cb->p_spec_cb = p_spec_cb;
    }
    os_queue_in(&p_gattc_cb->gattc_dis_list, p_srv_dis_cb);
    return true;
error:
    return false;
}

bool gattc_storage_add_secondary_service(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    bool is_exist = false;
    T_ATTR_UUID srv_uuid;
    T_SPEC_GATTC_CB *p_spec_cb;
    T_ATTR_DIS_SRV *p_srv_dis_cb = NULL;
    uint8_t primary_srv_num = p_gattc_cb->gattc_dis_list.count;

    for (uint8_t i = 0; i < primary_srv_num; i++)
    {
        p_srv_dis_cb = (T_ATTR_DIS_SRV *)os_queue_peek(&p_gattc_cb->gattc_dis_list, i);
        if (p_srv_dis_cb)
        {
            T_ATTR_DIS_INC *p_disc_inc;
            for (uint8_t j = 0; j < p_srv_dis_cb->include_srv_queue.count; j++)
            {
                p_disc_inc = (T_ATTR_DIS_INC *)os_queue_peek(&p_srv_dis_cb->include_srv_queue, j);
                if (p_disc_inc)
                {
                    bool is_new = true;
                    T_ATTR_DIS_SRV *p_srv_temp = NULL;
                    for (uint8_t k = 0; k < p_gattc_cb->gattc_dis_list.count; k++)
                    {
                        p_srv_temp = (T_ATTR_DIS_SRV *)os_queue_peek(&p_gattc_cb->gattc_dis_list, k);
                        if (p_srv_temp == NULL)
                        {
                            break;
                        }

                        if (p_disc_inc->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID16 &&
                            (p_srv_temp->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                             p_srv_temp->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16))
                        {
                            if (p_disc_inc->inc_srv.include_uuid16.uuid16 == p_srv_temp->srv_data.srv_uuid16.uuid16 &&
                                p_disc_inc->inc_srv.include_uuid16.start_handle == p_srv_temp->srv_data.srv_uuid16.hdr.att_handle &&
                                p_disc_inc->inc_srv.include_uuid16.end_handle == p_srv_temp->srv_data.srv_uuid16.end_group_handle)
                            {
                                is_new = false;
                                break;
                            }
                        }

                        if (p_disc_inc->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID128 &&
                            (p_srv_temp->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                             p_srv_temp->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128))
                        {
                            if (p_disc_inc->inc_srv.include_uuid128.start_handle ==
                                p_srv_temp->srv_data.srv_uuid128.hdr.att_handle
                                &&
                                p_disc_inc->inc_srv.include_uuid128.end_handle == p_srv_temp->srv_data.srv_uuid128.end_group_handle
                                &&
                                memcmp(p_disc_inc->inc_srv.include_uuid128.uuid128, p_srv_temp->srv_data.srv_uuid128.uuid128,
                                       16) == 0)
                            {
                                is_new = false;
                                break;
                            }
                        }
                    }

                    if (is_new)
                    {
                        if (p_disc_inc->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID16)
                        {
                            srv_uuid.is_uuid16 = true;
                            srv_uuid.p.uuid16 = p_disc_inc->inc_srv.include_uuid16.uuid16;
                            p_spec_cb = gatt_spec_client_find_by_uuid(&srv_uuid);
                            if (p_spec_cb == NULL  && ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_REG_SVC_BIT) != 0))
                            {
                                BTM_PRINT_TRACE1("gattc_storage_add_secondary_service: skip uuid16 0x%x",
                                                 p_disc_inc->inc_srv.include_uuid16.uuid16);
                                break;
                            }
                            p_srv_temp = calloc(1,
                                                sizeof(T_ATTR_DIS_SRV) - ATTR_DATA_STRUCT_LEN + sizeof(T_ATTR_SRV_UUID16));
                            if (p_srv_temp == NULL)
                            {
                                break;
                            }
                            p_srv_temp->srv_data.srv_uuid16.hdr.attr_type = ATTR_TYPE_SECONDARY_SRV_UUID16;
                            p_srv_temp->srv_data.srv_uuid16.hdr.instance_id = gattc_storage_get_srv_instance_id(p_gattc_cb,
                                                                              true, p_disc_inc->inc_srv.include_uuid16.uuid16, NULL);
                            p_srv_temp->srv_data.srv_uuid16.hdr.att_handle = p_disc_inc->inc_srv.include_uuid16.start_handle;
                            p_srv_temp->srv_data.srv_uuid16.end_group_handle =
                                p_disc_inc->inc_srv.include_uuid16.end_handle;
                            p_srv_temp->srv_data.srv_uuid16.uuid16 = p_disc_inc->inc_srv.include_uuid16.uuid16;
                            p_srv_temp->p_spec_cb = p_spec_cb;
                        }
                        else if (p_disc_inc->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID128)
                        {
                            srv_uuid.is_uuid16 = false;
                            memcpy(srv_uuid.p.uuid128, p_disc_inc->inc_srv.include_uuid128.uuid128, 16);
                            p_spec_cb = gatt_spec_client_find_by_uuid(&srv_uuid);
                            if (p_spec_cb == NULL  && ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_REG_SVC_BIT) != 0))
                            {
                                BTM_PRINT_TRACE0("gattc_storage_add_secondary_service: skip uuid128");
                                break;
                            }
                            p_srv_temp = calloc(1,
                                                sizeof(T_ATTR_DIS_SRV) - ATTR_DATA_STRUCT_LEN + sizeof(T_ATTR_SRV_UUID128));
                            if (p_srv_temp == NULL)
                            {
                                break;
                            }
                            p_srv_temp->srv_data.srv_uuid128.hdr.attr_type = ATTR_TYPE_SECONDARY_SRV_UUID128;
                            p_srv_temp->srv_data.srv_uuid128.hdr.instance_id = gattc_storage_get_srv_instance_id(p_gattc_cb,
                                                                               false, 0, p_disc_inc->inc_srv.include_uuid128.uuid128);
                            p_srv_temp->srv_data.srv_uuid128.hdr.att_handle = p_disc_inc->inc_srv.include_uuid128.start_handle;
                            p_srv_temp->srv_data.srv_uuid128.end_group_handle =
                                p_disc_inc->inc_srv.include_uuid128.end_handle;
                            memcpy(p_srv_temp->srv_data.srv_uuid128.uuid128, p_disc_inc->inc_srv.include_uuid128.uuid128, 16);
                            p_srv_temp->p_spec_cb = p_spec_cb;
                        }
                        os_queue_in(&p_gattc_cb->gattc_dis_list, p_srv_temp);
                        is_exist = true;
                    }
                }
            }
        }
    }
    return is_exist;
}

bool gattc_storage_add_include(T_ATTR_DIS_SRV *p_srv_dis_cb, bool uuid16,
                               T_DISCOVERY_RESULT_DATA *p_result_data)
{
    if (p_srv_dis_cb == NULL)
    {
        goto error;
    }

    if (uuid16)
    {
        T_ATTR_DIS_INC *p_include16 = calloc(1,
                                             sizeof(T_ATTR_DIS_INC));

        if (p_include16 == NULL)
        {
            goto error;
        }
        p_include16->inc_srv.include_uuid16.hdr.attr_type = ATTR_TYPE_INCLUDE_UUID16;
        p_include16->inc_srv.include_uuid16.hdr.instance_id = 0;
        p_include16->inc_srv.include_uuid16.hdr.att_handle =
            p_result_data->p_relation_uuid16_disc_data->decl_handle;
        p_include16->inc_srv.include_uuid16.start_handle =
            p_result_data->p_relation_uuid16_disc_data->att_handle;
        p_include16->inc_srv.include_uuid16.end_handle =
            p_result_data->p_relation_uuid16_disc_data->end_group_handle;
        p_include16->inc_srv.include_uuid16.uuid16 = p_result_data->p_relation_uuid16_disc_data->uuid16;
        p_srv_dis_cb->inc16_srv_num++;
        os_queue_in(&p_srv_dis_cb->include_srv_queue, p_include16);
    }
    else
    {
        T_ATTR_DIS_INC *p_include128 = calloc(1,
                                              sizeof(T_ATTR_DIS_INC));

        if (p_include128 == NULL)
        {
            goto error;
        }
        p_include128->inc_srv.include_uuid128.hdr.attr_type = ATTR_TYPE_INCLUDE_UUID128;
        p_include128->inc_srv.include_uuid128.hdr.instance_id = 0;
        p_include128->inc_srv.include_uuid128.hdr.att_handle =
            p_result_data->p_relation_uuid128_disc_data->decl_handle;
        p_include128->inc_srv.include_uuid128.start_handle =
            p_result_data->p_relation_uuid128_disc_data->att_handle;
        p_include128->inc_srv.include_uuid128.end_handle =
            p_result_data->p_relation_uuid128_disc_data->end_group_handle;
        memcpy(p_include128->inc_srv.include_uuid128.uuid128,
               p_result_data->p_relation_uuid128_disc_data->uuid128, 16);
        p_srv_dis_cb->inc128_srv_num++;
        os_queue_in(&p_srv_dis_cb->include_srv_queue, p_include128);
    }
    return true;
error:
    BTM_PRINT_ERROR0("gattc_storage_add_include: failed");
    return false;
}

bool gattc_storage_add_char(T_ATTR_DIS_SRV *p_srv_dis_cb, bool uuid16,
                            T_DISCOVERY_RESULT_DATA *p_result_data)
{
    T_ATTR_DIS_CHAR *p_char_cb = NULL;

    if (p_srv_dis_cb == NULL)
    {
        goto error;
    }

    if (uuid16)
    {
        p_char_cb = calloc(1,
                           sizeof(T_ATTR_DIS_CHAR) - ATTR_DATA_STRUCT_LEN + sizeof(T_ATTR_CHAR_UUID16));
        if (p_char_cb == NULL)
        {
            goto error;
        }
        p_char_cb->char_data.char_uuid16.hdr.attr_type = ATTR_TYPE_CHAR_UUID16;
        p_char_cb->char_data.char_uuid16.hdr.instance_id = gattc_storage_get_char_instance_id(p_srv_dis_cb,
                                                           true, p_result_data->p_char_uuid16_disc_data->uuid16, NULL);
        p_char_cb->char_data.char_uuid16.hdr.att_handle =
            p_result_data->p_char_uuid16_disc_data->decl_handle;
        p_char_cb->char_data.char_uuid16.properties = p_result_data->p_char_uuid16_disc_data->properties;
        p_char_cb->char_data.char_uuid16.value_handle =
            p_result_data->p_char_uuid16_disc_data->value_handle;
        p_char_cb->char_data.char_uuid16.uuid16 = p_result_data->p_char_uuid16_disc_data->uuid16;
        p_srv_dis_cb->char16_num++;
    }
    else
    {
        p_char_cb = calloc(1,
                           sizeof(T_ATTR_DIS_CHAR) - ATTR_DATA_STRUCT_LEN + sizeof(T_ATTR_CHAR_UUID128));
        if (p_char_cb == NULL)
        {
            goto error;
        }
        p_char_cb->char_data.char_uuid128.hdr.attr_type = ATTR_TYPE_CHAR_UUID128;
        p_char_cb->char_data.char_uuid128.hdr.instance_id = gattc_storage_get_char_instance_id(p_srv_dis_cb,
                                                            false, 0, p_result_data->p_char_uuid128_disc_data->uuid128);
        p_char_cb->char_data.char_uuid128.hdr.att_handle =
            p_result_data->p_char_uuid128_disc_data->decl_handle;
        p_char_cb->char_data.char_uuid128.properties = p_result_data->p_char_uuid128_disc_data->properties;
        p_char_cb->char_data.char_uuid128.value_handle =
            p_result_data->p_char_uuid128_disc_data->value_handle;
        memcpy(p_char_cb->char_data.char_uuid128.uuid128, p_result_data->p_char_uuid128_disc_data->uuid128,
               16);
        p_srv_dis_cb->char128_num++;
    }
    os_queue_in(&p_srv_dis_cb->char_queue, p_char_cb);
    return true;
error:
    BTM_PRINT_ERROR0("gattc_storage_add_char: failed");
    return false;
}

void gattc_storage_srv_list_release(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    if (p_gattc_cb->srv_list.count != 0)
    {
        T_ATTR_SRV_CB *p_srv_cb;

        while ((p_srv_cb = os_queue_out(&p_gattc_cb->srv_list)) != NULL)
        {
            free(p_srv_cb);
        }
        os_queue_init(&p_gattc_cb->srv_list);
    }
    if (p_gattc_cb->p_srv_table)
    {
        free(p_gattc_cb->p_srv_table);
        p_gattc_cb->p_srv_table = NULL;
    }
    if (p_gattc_cb->p_cccd_table)
    {
        free(p_gattc_cb->p_cccd_table);
        p_gattc_cb->p_cccd_table = NULL;
        p_gattc_cb->cccd_num = 0;
    }
    p_gattc_cb->srv_table_len = 0;
}

void gattc_storage_dis_list_release(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    p_gattc_cb->p_curr_srv = NULL;
    if (p_gattc_cb->gattc_dis_list.count != 0)
    {
        T_ATTR_DIS_SRV *p_srv_dis_cb;

        while ((p_srv_dis_cb = os_queue_out(&p_gattc_cb->gattc_dis_list)) != NULL)
        {
            if (p_srv_dis_cb->include_srv_queue.count != 0)
            {
                void *p_inc_data;
                while ((p_inc_data = os_queue_out(&p_srv_dis_cb->include_srv_queue)) != NULL)
                {
                    free(p_inc_data);
                }
            }

            if (p_srv_dis_cb->char_queue.count != 0)
            {
                void *p_char_data;
                while ((p_char_data = os_queue_out(&p_srv_dis_cb->char_queue)) != NULL)
                {
                    free(p_char_data);
                }
            }
            free(p_srv_dis_cb);
        }
        os_queue_init(&p_gattc_cb->gattc_dis_list);
    }
}

bool gattc_storage_srv_list_gen(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    uint8_t err_idx = 0;
    uint16_t idx = 0;
    uint8_t cccd_num = 0;
    T_ATTR_DIS_SRV *p_srv_dis_cb = NULL;

    if (p_gattc_cb->p_srv_table != NULL)
    {
        gattc_storage_srv_list_release(p_gattc_cb);
    }

    if (p_gattc_cb->gattc_dis_list.count != 0)
    {
        T_ATTR_SRV_CB *p_srv_cb;

        for (uint8_t i = 0; i < p_gattc_cb->gattc_dis_list.count; i++)
        {
            T_ATTR_DIS_SRV *p_srv_temp_cb = (T_ATTR_DIS_SRV *)os_queue_peek(&p_gattc_cb->gattc_dis_list, i);

            if (p_srv_temp_cb)
            {
                if (p_srv_temp_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                    p_srv_temp_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
                {
                    idx += sizeof(T_ATTR_SRV_UUID16);
                }
                else if (p_srv_temp_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                         p_srv_temp_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
                {
                    idx += sizeof(T_ATTR_SRV_UUID128);
                }
                else
                {
                    err_idx = 1;
                    goto failed;
                }
                idx += p_srv_temp_cb->inc16_srv_num * sizeof(T_ATTR_INCLUDE_UUID16);
                idx += p_srv_temp_cb->inc128_srv_num * sizeof(T_ATTR_INCLUDE_UUID128);
                idx += p_srv_temp_cb->char16_num * sizeof(T_ATTR_CHAR16_CB);
                idx += p_srv_temp_cb->char128_num * sizeof(T_ATTR_CHAR128_CB);
                idx += 4;
            }
        }
        p_gattc_cb->p_srv_table = calloc(1, idx);
        if (p_gattc_cb->p_srv_table == NULL)
        {
            err_idx = 2;
            goto failed;
        }
        p_gattc_cb->srv_table_len = idx;
        idx = 0;
        while ((p_srv_dis_cb = os_queue_out(&p_gattc_cb->gattc_dis_list)) != NULL)
        {
            p_srv_cb = calloc(1, sizeof(T_ATTR_SRV_CB));
            if (p_srv_cb == NULL)
            {
                err_idx = 3;
                goto failed;
            }
            p_srv_cb->p_srv_data = (T_ATTR_DATA *)&p_gattc_cb->p_srv_table[idx];
            if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
            {
                memcpy(p_srv_cb->p_srv_data, &p_srv_dis_cb->srv_data.srv_uuid16, sizeof(T_ATTR_SRV_UUID16));
                idx += sizeof(T_ATTR_SRV_UUID16);
            }
            else
            {
                memcpy(p_srv_cb->p_srv_data, &p_srv_dis_cb->srv_data.srv_uuid128, sizeof(T_ATTR_SRV_UUID128));
                idx += sizeof(T_ATTR_SRV_UUID128);
            }
            os_queue_in(&p_gattc_cb->srv_list, p_srv_cb);
            p_srv_cb->p_spec_cb = p_srv_dis_cb->p_spec_cb;
            p_gattc_cb->p_srv_table[idx] = p_srv_dis_cb->inc16_srv_num;
            idx++;
            p_gattc_cb->p_srv_table[idx] = p_srv_dis_cb->inc128_srv_num;
            idx++;
            p_gattc_cb->p_srv_table[idx] = p_srv_dis_cb->char16_num;
            idx++;
            p_gattc_cb->p_srv_table[idx] = p_srv_dis_cb->char128_num;
            idx++;
            if (p_srv_dis_cb->inc16_srv_num)
            {
                p_srv_cb->p_inc16_table = (T_ATTR_INCLUDE_UUID16 *)&p_gattc_cb->p_srv_table[idx];
                idx += p_srv_dis_cb->inc16_srv_num * sizeof(T_ATTR_INCLUDE_UUID16);
            }

            if (p_srv_dis_cb->inc128_srv_num)
            {
                p_srv_cb->p_inc128_table = (T_ATTR_INCLUDE_UUID128 *)&p_gattc_cb->p_srv_table[idx];
                idx += p_srv_dis_cb->inc128_srv_num * sizeof(T_ATTR_INCLUDE_UUID128);
            }

            if (p_srv_dis_cb->char16_num)
            {
                p_srv_cb->p_char16_table = (T_ATTR_CHAR16_CB *)&p_gattc_cb->p_srv_table[idx];
                idx += p_srv_dis_cb->char16_num * sizeof(T_ATTR_CHAR16_CB);
            }

            if (p_srv_dis_cb->char128_num)
            {
                p_srv_cb->p_char128_table = (T_ATTR_CHAR128_CB *)&p_gattc_cb->p_srv_table[idx];
                idx += p_srv_dis_cb->char128_num * sizeof(T_ATTR_CHAR128_CB);
            }

            if (p_srv_dis_cb->include_srv_queue.count != 0)
            {
                T_ATTR_DIS_INC *p_inc_data;
                while ((p_inc_data = os_queue_out(&p_srv_dis_cb->include_srv_queue)) != NULL)
                {
                    if (p_inc_data->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID16)
                    {
                        memcpy(&p_srv_cb->p_inc16_table[p_srv_cb->inc16_srv_num],
                               &p_inc_data->inc_srv.include_uuid16, sizeof(T_ATTR_INCLUDE_UUID16));
                        p_srv_cb->inc16_srv_num++;
                    }
                    else if (p_inc_data->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID128)
                    {
                        memcpy(&p_srv_cb->p_inc128_table[p_srv_cb->inc128_srv_num],
                               &p_inc_data->inc_srv.include_uuid128, sizeof(T_ATTR_INCLUDE_UUID128));
                        p_srv_cb->inc128_srv_num++;
                    }
                    free(p_inc_data);
                }
            }

            if (p_srv_dis_cb->char_queue.count != 0)
            {
                T_ATTR_DIS_CHAR *p_char_data;
                while ((p_char_data = os_queue_out(&p_srv_dis_cb->char_queue)) != NULL)
                {
                    if (p_char_data->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID16)
                    {
                        if (p_char_data->desc_end_handle != 0)
                        {
                            if (p_char_data->desc_end_handle < p_char_data->char_data.char_uuid16.end_handle)
                            {
                                p_char_data->char_data.char_uuid16.end_handle = p_char_data->desc_end_handle;
                            }
                        }
                        else
                        {
                            p_char_data->char_data.char_uuid16.end_handle = p_char_data->char_data.char_uuid16.value_handle;
                        }
                        memcpy(&p_srv_cb->p_char16_table[p_srv_cb->char16_num].char_data,
                               &p_char_data->char_data.char_uuid16, sizeof(T_ATTR_CHAR_UUID16));
                        memcpy(&p_srv_cb->p_char16_table[p_srv_cb->char16_num].cccd_descriptor,
                               &p_char_data->cccd_descriptor, sizeof(T_ATTR_CCCD_DESC));
                        p_srv_cb->char16_num++;
                        if (p_char_data->cccd_descriptor.hdr.att_handle)
                        {
                            cccd_num++;
                        }
                    }
                    else if (p_char_data->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID128)
                    {
                        if (p_char_data->desc_end_handle != 0)
                        {
                            if (p_char_data->desc_end_handle < p_char_data->char_data.char_uuid128.end_handle)
                            {
                                p_char_data->char_data.char_uuid128.end_handle = p_char_data->desc_end_handle;
                            }
                        }
                        else
                        {
                            p_char_data->char_data.char_uuid128.end_handle = p_char_data->char_data.char_uuid128.value_handle;
                        }
                        memcpy(&p_srv_cb->p_char128_table[p_srv_cb->char128_num].char_data,
                               &p_char_data->char_data.char_uuid128, sizeof(T_ATTR_CHAR_UUID128));
                        memcpy(&p_srv_cb->p_char128_table[p_srv_cb->char128_num].cccd_descriptor,
                               &p_char_data->cccd_descriptor, sizeof(T_ATTR_CCCD_DESC));
                        p_srv_cb->char128_num++;
                        if (p_char_data->cccd_descriptor.hdr.att_handle)
                        {
                            cccd_num++;
                        }
                    }
                    free(p_char_data);
                }
            }

            if (p_srv_dis_cb->inc16_srv_num != p_srv_cb->inc16_srv_num ||
                p_srv_dis_cb->inc128_srv_num != p_srv_cb->inc128_srv_num ||
                p_srv_dis_cb->char16_num != p_srv_cb->char16_num ||
                p_srv_dis_cb->char128_num != p_srv_cb->char128_num)
            {
                err_idx = 4;
                goto failed;
            }

            if (idx > p_gattc_cb->srv_table_len)
            {
                err_idx = 5;
                goto failed;
            }
            free(p_srv_dis_cb);
            p_srv_dis_cb = NULL;
        }
        os_queue_init(&p_gattc_cb->gattc_dis_list);
    }

    if (idx != p_gattc_cb->srv_table_len)
    {
        err_idx = 6;
        goto failed;
    }

    if ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_CCCD_STORAGE_BIT) != 0)
    {
        BTM_PRINT_INFO1("gattc_storage_srv_list_gen: cccd_num %d", cccd_num);
        p_gattc_cb->cccd_num = cccd_num;
        if (p_gattc_cb->cccd_num)
        {
            p_gattc_cb->p_cccd_table = calloc(1,
                                              p_gattc_cb->cccd_num * sizeof(T_GATTC_CCCD));
        }
    }
    gattc_storage_dis_list_release(p_gattc_cb);
    return true;
failed:
    if (p_srv_dis_cb)
    {
        free(p_srv_dis_cb);
    }
    gattc_storage_dis_list_release(p_gattc_cb);
    gattc_storage_srv_list_release(p_gattc_cb);
    BTM_PRINT_ERROR1("gattc_storage_srv_list_gen: failed, err_idx %d", err_idx);
    return false;
}

#if 0
void gattc_storage_dis_print(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    T_ATTR_DIS_SRV *p_srv_dis_cb = NULL;
    T_ATTR_DIS_INC *p_dis_inc_cb = NULL;
    T_ATTR_DIS_CHAR *p_dis_char_cb = NULL;
    for (uint8_t i = 0; i < p_gattc_cb->gattc_dis_list.count; i++)
    {
        p_srv_dis_cb = (T_ATTR_DIS_SRV *)os_queue_peek(&p_gattc_cb->gattc_dis_list, i);
        if (p_srv_dis_cb == NULL)
        {
            return;
        }
        BTM_PRINT_INFO5("Disc Primary Service: inc16_srv_num %d, inc128_srv_num %d, char16_num %d, char128_num %d, p_spec_cb %p",
                        p_srv_dis_cb->inc16_srv_num, p_srv_dis_cb->inc128_srv_num, p_srv_dis_cb->char16_num,
                        p_srv_dis_cb->char128_num, p_srv_dis_cb->p_spec_cb);
        if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16)
        {
            BTM_PRINT_INFO4("Primary Service: handle[0x%x - 0x%x], inst[%d], uuid16[0x%x]",
                            p_srv_dis_cb->srv_data.srv_uuid16.hdr.att_handle,
                            p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle,
                            p_srv_dis_cb->srv_data.srv_uuid16.hdr.instance_id,
                            p_srv_dis_cb->srv_data.srv_uuid16.uuid16);
        }
        else if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128)
        {
            BTM_PRINT_INFO4("Primary Service: handle[0x%x - 0x%x], inst[%d], uuid128[%b]",
                            p_srv_dis_cb->srv_data.srv_uuid128.hdr.att_handle,
                            p_srv_dis_cb->srv_data.srv_uuid128.end_group_handle,
                            p_srv_dis_cb->srv_data.srv_uuid128.hdr.instance_id,
                            TRACE_BINARY(16, p_srv_dis_cb->srv_data.srv_uuid128.uuid128));
        }
        else if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
        {
            BTM_PRINT_INFO4("Secondary Service: handle[0x%x - 0x%x], inst[%d], uuid16[0x%x]",
                            p_srv_dis_cb->srv_data.srv_uuid16.hdr.att_handle,
                            p_srv_dis_cb->srv_data.srv_uuid16.end_group_handle,
                            p_srv_dis_cb->srv_data.srv_uuid16.hdr.instance_id,
                            p_srv_dis_cb->srv_data.srv_uuid16.uuid16);
        }
        else if (p_srv_dis_cb->srv_data.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
        {
            BTM_PRINT_INFO4("Secondary Service: handle[0x%x - 0x%x], inst[%d], uuid128[%b]",
                            p_srv_dis_cb->srv_data.srv_uuid128.hdr.att_handle,
                            p_srv_dis_cb->srv_data.srv_uuid128.end_group_handle,
                            p_srv_dis_cb->srv_data.srv_uuid128.hdr.instance_id,
                            TRACE_BINARY(16, p_srv_dis_cb->srv_data.srv_uuid128.uuid128));
        }
        for (uint8_t j = 0; j < p_srv_dis_cb->include_srv_queue.count; j++)
        {
            p_dis_inc_cb = (T_ATTR_DIS_INC *)os_queue_peek(&p_srv_dis_cb->include_srv_queue, j);
            if (p_dis_inc_cb == NULL)
            {
                return;
            }
            if (p_dis_inc_cb->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID16)
            {
                BTM_PRINT_INFO5("\tInclude: decl handle[0x%x], handle[0x%x - 0x%x], inst[%d], uuid16[0x%x]",
                                p_dis_inc_cb->inc_srv.hdr.att_handle,
                                p_dis_inc_cb->inc_srv.include_uuid16.start_handle,
                                p_dis_inc_cb->inc_srv.include_uuid16.end_handle,
                                p_dis_inc_cb->inc_srv.hdr.instance_id,
                                p_dis_inc_cb->inc_srv.include_uuid16.uuid16);
            }
            if (p_dis_inc_cb->inc_srv.hdr.attr_type == ATTR_TYPE_INCLUDE_UUID128)
            {
                BTM_PRINT_INFO5("\tInclude: decl handle[0x%x], handle[0x%x - 0x%x], inst[%d], uuid128[%b]",
                                p_dis_inc_cb->inc_srv.hdr.att_handle,
                                p_dis_inc_cb->inc_srv.include_uuid128.start_handle,
                                p_dis_inc_cb->inc_srv.include_uuid128.end_handle,
                                p_dis_inc_cb->inc_srv.hdr.instance_id,
                                TRACE_BINARY(16, p_dis_inc_cb->inc_srv.include_uuid128.uuid128));
            }
        }
        for (uint8_t j = 0; j < p_srv_dis_cb->char_queue.count; j++)
        {
            p_dis_char_cb = (T_ATTR_DIS_CHAR *)os_queue_peek(&p_srv_dis_cb->char_queue, j);
            if (p_dis_char_cb == NULL)
            {
                return;
            }
            if (p_dis_char_cb->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID16)
            {
                BTM_PRINT_INFO5("\tChar: decl handle[0x%x], value handle[0x%x], inst[%d], properties[0x%x], uuid16[0x%x]",
                                p_dis_char_cb->char_data.hdr.att_handle,
                                p_dis_char_cb->char_data.char_uuid16.value_handle,
                                p_dis_char_cb->char_data.hdr.instance_id,
                                p_dis_char_cb->char_data.char_uuid16.properties,
                                p_dis_char_cb->char_data.char_uuid16.uuid16);
            }
            if (p_dis_char_cb->char_data.hdr.attr_type == ATTR_TYPE_CHAR_UUID128)
            {
                BTM_PRINT_INFO5("\tChar: decl handle[0x%x], value handle[0x%x], inst[%d], properties[0x%x], uuid128[%b]",
                                p_dis_char_cb->char_data.hdr.att_handle,
                                p_dis_char_cb->char_data.char_uuid128.value_handle,
                                p_dis_char_cb->char_data.hdr.instance_id,
                                p_dis_char_cb->char_data.char_uuid128.properties,
                                TRACE_BINARY(16, p_dis_char_cb->char_data.char_uuid128.uuid128));
            }
            if (p_dis_char_cb->cccd_descriptor.hdr.att_handle != 0)
            {
                BTM_PRINT_INFO1("\t\tCCCD Desc: handle[0x%x]",
                                p_dis_char_cb->cccd_descriptor.hdr.att_handle);
            }
        }
    }
}
#endif
void gattc_storage_print(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    T_ATTR_SRV_CB *p_srv_cb = NULL;

    BTM_PRINT_INFO1("gattc_storage_print: Registered Service Number %d", spec_gattc_queue.count);
#if 0
    for (uint8_t i = 0; i < spec_gattc_queue.count; i++)
    {
        T_SPEC_GATTC_CB *p_db = (T_SPEC_GATTC_CB *)os_queue_peek(&spec_gattc_queue, i);
        if (p_db)
        {
            if (p_db->srv_uuid.is_uuid16)
            {
                BTM_PRINT_INFO2("Registered Service[%d]: uuid16[0x%x]",
                                i, p_db->srv_uuid.p.uuid16);
            }
            else
            {
                BTM_PRINT_INFO2("Registered Service[%d]: uuid128[%b]",
                                i, TRACE_BINARY(16, p_db->srv_uuid.p.uuid128));
            }
        }
    }
#endif
    for (uint8_t i = 0; i < p_gattc_cb->srv_list.count; i++)
    {
        p_srv_cb = (T_ATTR_SRV_CB *)os_queue_peek(&p_gattc_cb->srv_list, i);
        if (p_srv_cb)
        {
            BTM_PRINT_INFO5("Primary Service: inc16_srv_num %d, inc128_srv_num %d, char16_num %d, char128_num %d, p_spec_cb %p",
                            p_srv_cb->inc16_srv_num, p_srv_cb->inc128_srv_num, p_srv_cb->char16_num,
                            p_srv_cb->char128_num, p_srv_cb->p_spec_cb);
            if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16)
            {
                BTM_PRINT_INFO4("Primary Service: handle[0x%x - 0x%x], inst[%d], uuid16[0x%x]",
                                p_srv_cb->p_srv_data->srv_uuid16.hdr.att_handle,
                                p_srv_cb->p_srv_data->srv_uuid16.end_group_handle,
                                p_srv_cb->p_srv_data->srv_uuid16.hdr.instance_id,
                                p_srv_cb->p_srv_data->srv_uuid16.uuid16);
            }
            else if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128)
            {
                BTM_PRINT_INFO4("Primary Service: handle[0x%x - 0x%x], inst[%d], uuid128[%b]",
                                p_srv_cb->p_srv_data->srv_uuid128.hdr.att_handle,
                                p_srv_cb->p_srv_data->srv_uuid128.end_group_handle,
                                p_srv_cb->p_srv_data->srv_uuid128.hdr.instance_id,
                                TRACE_BINARY(16, p_srv_cb->p_srv_data->srv_uuid128.uuid128));
            }
            else if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
            {
                BTM_PRINT_INFO4("Secondary Service: handle[0x%x - 0x%x], inst[%d], uuid16[0x%x]",
                                p_srv_cb->p_srv_data->srv_uuid16.hdr.att_handle,
                                p_srv_cb->p_srv_data->srv_uuid16.end_group_handle,
                                p_srv_cb->p_srv_data->srv_uuid16.hdr.instance_id,
                                p_srv_cb->p_srv_data->srv_uuid16.uuid16);
            }
            else if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
            {
                BTM_PRINT_INFO4("Secondary Service: handle[0x%x - 0x%x], inst[%d], uuid128[%b]",
                                p_srv_cb->p_srv_data->srv_uuid128.hdr.att_handle,
                                p_srv_cb->p_srv_data->srv_uuid128.end_group_handle,
                                p_srv_cb->p_srv_data->srv_uuid128.hdr.instance_id,
                                TRACE_BINARY(16, p_srv_cb->p_srv_data->srv_uuid128.uuid128));
            }

            if (p_srv_cb->inc16_srv_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->inc16_srv_num; j++)
                {
                    BTM_PRINT_INFO5("\tInclude: decl handle[0x%x], handle[0x%x - 0x%x], inst[%d], uuid16[0x%x]",
                                    p_srv_cb->p_inc16_table[j].hdr.att_handle,
                                    p_srv_cb->p_inc16_table[j].start_handle,
                                    p_srv_cb->p_inc16_table[j].end_handle,
                                    p_srv_cb->p_inc16_table[j].hdr.instance_id,
                                    p_srv_cb->p_inc16_table[j].uuid16);
                }
            }

            if (p_srv_cb->inc128_srv_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->inc128_srv_num; j++)
                {
                    BTM_PRINT_INFO5("\tInclude: decl handle[0x%x], handle[0x%x - 0x%x], inst[%d], uuid128[%b]",
                                    p_srv_cb->p_inc128_table[j].hdr.att_handle,
                                    p_srv_cb->p_inc128_table[j].start_handle,
                                    p_srv_cb->p_inc128_table[j].end_handle,
                                    p_srv_cb->p_inc128_table[j].hdr.instance_id,
                                    TRACE_BINARY(16, p_srv_cb->p_inc128_table[j].uuid128));
                }
            }

            if (p_srv_cb->char16_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
                {
                    BTM_PRINT_INFO6("\tChar: decl handle[0x%x], value handle[0x%x], inst[%d], properties[0x%x], uuid16[0x%x], end handle[0x%x]",
                                    p_srv_cb->p_char16_table[j].char_data.hdr.att_handle,
                                    p_srv_cb->p_char16_table[j].char_data.value_handle,
                                    p_srv_cb->p_char16_table[j].char_data.hdr.instance_id,
                                    p_srv_cb->p_char16_table[j].char_data.properties,
                                    p_srv_cb->p_char16_table[j].char_data.uuid16,
                                    p_srv_cb->p_char16_table[j].char_data.end_handle);
                    if (p_srv_cb->p_char16_table[j].cccd_descriptor.hdr.att_handle != 0)
                    {
                        BTM_PRINT_INFO1("\t\tCCCD Desc: handle[0x%x]",
                                        p_srv_cb->p_char16_table[j].cccd_descriptor.hdr.att_handle);
                    }
                }
            }

            if (p_srv_cb->char128_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
                {
                    BTM_PRINT_INFO6("\tChar: decl handle[0x%x], value handle[0x%x], inst[%d], properties[0x%x], uuid128[%b], end handle[0x%x]",
                                    p_srv_cb->p_char128_table[j].char_data.hdr.att_handle,
                                    p_srv_cb->p_char128_table[j].char_data.value_handle,
                                    p_srv_cb->p_char128_table[j].char_data.hdr.instance_id,
                                    p_srv_cb->p_char128_table[j].char_data.properties,
                                    TRACE_BINARY(16, p_srv_cb->p_char128_table[j].char_data.uuid128),
                                    p_srv_cb->p_char128_table[j].char_data.end_handle);
                    if (p_srv_cb->p_char128_table[j].cccd_descriptor.hdr.att_handle != 0)
                    {
                        BTM_PRINT_INFO1("\t\tCCCD Desc: handle[0x%x]",
                                        p_srv_cb->p_char128_table[j].cccd_descriptor.hdr.att_handle);
                    }
                }
            }
        }
    }

    if (p_gattc_cb->p_cccd_table)
    {
        BTM_PRINT_INFO1("\tCCCD Table: cccd_num %d",
                        p_gattc_cb->cccd_num);
        for (uint8_t i = 0; i < p_gattc_cb->cccd_num; i++)
        {
            if (p_gattc_cb->p_cccd_table[i].handle != 0)
            {
                BTM_PRINT_INFO3("\t\tCCCD Table[%d]: handle[0x%x], cccd_bits[0x%x]",
                                i, p_gattc_cb->p_cccd_table[i].handle,
                                p_gattc_cb->p_cccd_table[i].cccd_bits);
            }
        }
    }
}

uint8_t gattc_storage_get_srv_num(T_GATTC_STORAGE_CB *p_gattc_cb, bool is_uuid16,
                                  uint16_t uuid16, uint8_t *p_uuid128)
{
    uint8_t instance_id = 0;
    T_ATTR_SRV_CB *p_srv_cb = NULL;

    for (uint8_t i = 0; i < p_gattc_cb->srv_list.count; i++)
    {
        p_srv_cb = (T_ATTR_SRV_CB *)os_queue_peek(&p_gattc_cb->srv_list, i);
        if (p_srv_cb)
        {
            if (is_uuid16)
            {
                if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                    p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
                {
                    if (uuid16 == p_srv_cb->p_srv_data->srv_uuid16.uuid16)
                    {
                        instance_id++;
                    }
                }
            }
            else
            {
                if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                    p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
                {
                    if (memcmp(p_uuid128, p_srv_cb->p_srv_data->srv_uuid128.uuid128, 16) == 0)
                    {
                        instance_id++;
                    }
                }
            }
        }
    }
    return instance_id;
}

T_ATTR_SRV_CB *gattc_storage_find_srv_by_handle(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle)
{
    T_ATTR_SRV_CB *p_srv_cb = NULL;

    for (uint8_t i = 0; i < p_gattc_cb->srv_list.count; i++)
    {
        p_srv_cb = (T_ATTR_SRV_CB *)os_queue_peek(&p_gattc_cb->srv_list, i);
        if (p_srv_cb)
        {
            if (handle >= p_srv_cb->p_srv_data->srv_uuid16.hdr.att_handle &&
                handle <= p_srv_cb->p_srv_data->srv_uuid16.end_group_handle)
            {
                return p_srv_cb;
            }
        }
    }
    BTM_PRINT_ERROR1("gattc_storage_find_srv_by_handle: failed, handle 0x%x", handle);
    return NULL;
}

bool gattc_storage_find_char_desc(T_ATTR_SRV_CB *p_srv_cb, uint16_t handle,
                                  T_GATT_CHAR_TYPE *p_char_type,
                                  T_ATTR_DATA *p_char_data)
{
    bool is_found = false;
    *p_char_type = GATT_CHAR_UNKNOWN;

    if (p_srv_cb && handle != 0)
    {
        if (p_srv_cb->char16_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
            {
                T_ATTR_CHAR16_CB *p_attr_char = &p_srv_cb->p_char16_table[j];
                if (handle == p_attr_char->char_data.value_handle)
                {
                    *p_char_type = GATT_CHAR_VALUE;
                    is_found = true;
                }
                else if (handle == p_attr_char->cccd_descriptor.hdr.att_handle)
                {
                    *p_char_type = GATT_CHAR_CCCD;
                    is_found = true;
                }
                else if (handle > p_attr_char->char_data.value_handle &&
                         handle <= p_attr_char->char_data.end_handle)
                {
                    *p_char_type = GATT_CHAR_DESCRIPTOR;
                    is_found = true;
                }

                if (is_found)
                {
                    memcpy(p_char_data,  &p_attr_char->char_data, sizeof(T_ATTR_CHAR_UUID16));
                    break;
                }
            }
        }
        if (p_srv_cb->char128_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
            {
                T_ATTR_CHAR128_CB *p_attr_char = &p_srv_cb->p_char128_table[j];
                if (handle == p_attr_char->char_data.value_handle)
                {
                    *p_char_type = GATT_CHAR_VALUE;
                    is_found = true;
                }
                else if (handle == p_attr_char->cccd_descriptor.hdr.att_handle)
                {
                    *p_char_type = GATT_CHAR_CCCD;
                    is_found = true;
                }
                else if (handle > p_attr_char->char_data.value_handle &&
                         handle <= p_attr_char->char_data.end_handle)
                {
                    *p_char_type = GATT_CHAR_DESCRIPTOR;
                    is_found = true;
                }

                if (is_found)
                {
                    memcpy(p_char_data,  &p_attr_char->char_data, sizeof(T_ATTR_CHAR_UUID128));
                    break;
                }
            }
        }
    }

    if (is_found == false)
    {
        BTM_PRINT_ERROR1("gattc_storage_find_char_desc: failed, handle 0x%x", handle);
    }
    return is_found;
}

T_ATTR_SRV_CB *gattc_storage_find_srv_by_uuid(T_GATTC_STORAGE_CB *p_gattc_cb,
                                              T_ATTR_UUID *p_attr_uuid)
{
    T_ATTR_SRV_CB *p_srv_cb = NULL;

    if (p_gattc_cb == NULL || p_attr_uuid == NULL)
    {
        return NULL;
    }

    for (uint8_t i = 0; i < p_gattc_cb->srv_list.count; i++)
    {
        p_srv_cb = (T_ATTR_SRV_CB *)os_queue_peek(&p_gattc_cb->srv_list, i);
        if (p_srv_cb)
        {
            if (p_attr_uuid->is_uuid16)
            {
                if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                    p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
                {
                    if ((p_attr_uuid->instance_id == p_srv_cb->p_srv_data->hdr.instance_id) &&
                        (p_attr_uuid->p.uuid16 == p_srv_cb->p_srv_data->srv_uuid16.uuid16))
                    {
                        return p_srv_cb;
                    }
                }
            }
            else
            {
                if (p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                    p_srv_cb->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
                {
                    if ((p_attr_uuid->instance_id == p_srv_cb->p_srv_data->hdr.instance_id) &&
                        memcmp(p_attr_uuid->p.uuid128, p_srv_cb->p_srv_data->srv_uuid128.uuid128, 16) == 0)
                    {
                        return p_srv_cb;
                    }
                }
            }
        }
    }
    return NULL;
}

T_ATTR_SRV_CB *gattc_storage_find_inc_srv_by_uuid(T_GATTC_STORAGE_CB *p_gattc_cb,
                                                  T_ATTR_SRV_CB *p_inc_srv)
{
    T_ATTR_SRV_CB *p_srv_cb = NULL;

    for (uint8_t i = 0; i < p_gattc_cb->srv_list.count; i++)
    {
        p_srv_cb = (T_ATTR_SRV_CB *)os_queue_peek(&p_gattc_cb->srv_list, i);
        if (p_srv_cb)
        {
            if (p_inc_srv->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
                p_inc_srv->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
            {
                if (p_srv_cb->inc16_srv_num)
                {
                    for (uint8_t j = 0; j < p_srv_cb->inc16_srv_num; j++)
                    {
                        if (p_srv_cb->p_inc16_table[j].uuid16 == p_inc_srv->p_srv_data->srv_uuid16.uuid16 &&
                            p_srv_cb->p_inc16_table[j].start_handle == p_inc_srv->p_srv_data->srv_uuid16.hdr.att_handle &&
                            p_srv_cb->p_inc16_table[j].end_handle == p_inc_srv->p_srv_data->srv_uuid16.end_group_handle)
                        {
                            return p_srv_cb;
                        }
                    }
                }
            }
            if (p_inc_srv->p_srv_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                p_inc_srv->p_srv_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
            {
                if (p_srv_cb->inc128_srv_num)
                {
                    for (uint8_t j = 0; j < p_srv_cb->inc128_srv_num; j++)
                    {
                        if (p_srv_cb->p_inc128_table[j].start_handle == p_inc_srv->p_srv_data->srv_uuid128.hdr.att_handle
                            &&
                            p_srv_cb->p_inc128_table[j].end_handle == p_inc_srv->p_srv_data->srv_uuid128.end_group_handle &&
                            memcmp(p_srv_cb->p_inc128_table[j].uuid128, p_inc_srv->p_srv_data->srv_uuid128.uuid128, 16) == 0)
                        {
                            return p_srv_cb;
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

bool gattc_storage_find_char_by_uuid(T_ATTR_SRV_CB *p_srv_cb, T_ATTR_UUID *p_attr_uuid,
                                     T_ATTR_DATA *p_attr_data)
{
    bool is_found = false;

    if (p_srv_cb && p_attr_uuid != NULL && p_attr_data != NULL)
    {
        if (p_attr_uuid->is_uuid16)
        {
            if (p_srv_cb->char16_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
                {
                    T_ATTR_CHAR16_CB *p_attr_char = &p_srv_cb->p_char16_table[j];

                    if ((p_attr_uuid->instance_id == p_attr_char->char_data.hdr.instance_id) &&
                        (p_attr_uuid->p.uuid16 == p_attr_char->char_data.uuid16))
                    {
                        memcpy(&p_attr_data->char_uuid16, &p_attr_char->char_data, sizeof(T_ATTR_CHAR_UUID16));
                        is_found = true;
                        break;
                    }
                }
            }
        }
        else
        {
            if (p_srv_cb->char128_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
                {
                    T_ATTR_CHAR128_CB *p_attr_char = &p_srv_cb->p_char128_table[j];

                    if ((p_attr_uuid->instance_id == p_attr_char->char_data.hdr.instance_id) &&
                        memcmp(p_attr_uuid->p.uuid128, p_attr_char->char_data.uuid128, 16) == 0)
                    {
                        memcpy(&p_attr_data->char_uuid128, &p_attr_char->char_data, sizeof(T_ATTR_CHAR_UUID128));
                        is_found = true;
                        break;
                    }
                }
            }
        }
    }
    return is_found;
}

bool gattc_storage_find_char(T_ATTR_SRV_CB *p_srv_cb, T_ATTR_UUID *p_attr_uuid,
                             T_ATTR_CHAR16_CB **pp_char16, T_ATTR_CHAR128_CB **pp_char128)
{
    bool is_found = false;
    if (p_srv_cb && p_attr_uuid != NULL)
    {
        if (p_attr_uuid->is_uuid16)
        {
            if (p_srv_cb->char16_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
                {
                    T_ATTR_CHAR16_CB *p_attr_char = &p_srv_cb->p_char16_table[j];

                    if ((p_attr_uuid->instance_id == p_attr_char->char_data.hdr.instance_id) &&
                        (p_attr_uuid->p.uuid16 == p_attr_char->char_data.uuid16))
                    {
                        *pp_char16 = p_attr_char;
                        is_found = true;
                        break;
                    }
                }
            }
        }
        else
        {
            if (p_srv_cb->char128_num)
            {
                for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
                {
                    T_ATTR_CHAR128_CB *p_attr_char = &p_srv_cb->p_char128_table[j];

                    if ((p_attr_uuid->instance_id == p_attr_char->char_data.hdr.instance_id) &&
                        memcmp(p_attr_uuid->p.uuid128, p_attr_char->char_data.uuid128, 16) == 0)
                    {
                        *pp_char128 = p_attr_char;
                        is_found = true;
                        break;
                    }
                }
            }
        }
    }
    return is_found;
}

bool gattc_storage_check_prop(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle,
                              uint16_t properties_bit)
{
    T_ATTR_SRV_CB *p_srv_cb = gattc_storage_find_srv_by_handle(p_gattc_cb, handle);
    T_ATTR_DATA char_data;
    T_GATT_CHAR_TYPE char_type;

    if (p_srv_cb == NULL)
    {
        goto error;
    }

    if (gattc_storage_find_char_desc(p_srv_cb, handle, &char_type, &char_data))
    {
        if (char_type == GATT_CHAR_CCCD)
        {
            if ((GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_READ) & properties_bit)
            {
                return true;
            }
        }
        else if (char_type == GATT_CHAR_VALUE)
        {
            if (char_data.char_uuid16.properties & properties_bit)
            {
                return true;
            }
        }
        else if (char_type == GATT_CHAR_DESCRIPTOR)
        {
            return true;
        }
    }
error:
    BTM_PRINT_ERROR2("gattc_storage_check_prop: failed, handle 0x%x, properties_bit 0x%x", handle,
                     properties_bit);
    return false;
}

void att_data_covert_to_uuid(T_ATTR_DATA *p_attr_data, T_ATTR_UUID *p_attr_uuid)
{
    p_attr_uuid->instance_id = p_attr_data->hdr.instance_id;
    if (p_attr_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
        p_attr_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
    {
        p_attr_uuid->is_uuid16 = true;
        p_attr_uuid->p.uuid16 = p_attr_data->srv_uuid16.uuid16;
    }
    else if (p_attr_data->hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
             p_attr_data->hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
    {
        p_attr_uuid->is_uuid16 = false;
        memcpy(p_attr_uuid->p.uuid128, p_attr_data->srv_uuid128.uuid128, 16);
    }
    else if (p_attr_data->hdr.attr_type == ATTR_TYPE_CHAR_UUID16)
    {
        p_attr_uuid->is_uuid16 = true;
        p_attr_uuid->p.uuid16 = p_attr_data->char_uuid16.uuid16;
    }
    else if (p_attr_data->hdr.attr_type == ATTR_TYPE_CHAR_UUID128)
    {
        p_attr_uuid->is_uuid16 = false;
        memcpy(p_attr_uuid->p.uuid128, p_attr_data->char_uuid128.uuid128, 16);
    }
    else if (p_attr_data->hdr.attr_type == ATTR_TYPE_INCLUDE_UUID16)
    {
        p_attr_uuid->is_uuid16 = true;
        p_attr_uuid->p.uuid16 = p_attr_data->include_uuid16.uuid16;
    }
    else if (p_attr_data->hdr.attr_type == ATTR_TYPE_INCLUDE_UUID128)
    {
        p_attr_uuid->is_uuid16 = false;
        memcpy(p_attr_uuid->p.uuid128, p_attr_data->include_uuid128.uuid128, 16);
    }
    else if (p_attr_data->hdr.attr_type == ATTR_TYPE_CCCD_DESC)
    {
        p_attr_uuid->is_uuid16 = true;
        p_attr_uuid->p.uuid16 = GATT_UUID_CHAR_CLIENT_CONFIG;
    }
}

bool gattc_storage_load(T_GATTC_STORAGE_CB *p_gattc_cb, bool database_hash_check)
{
    uint8_t err_idx = 0;
    uint16_t idx = 0;
    uint8_t remote_bd[GAP_BD_ADDR_LEN];
    uint8_t remote_bd_type;
    T_ATTR_UUID srv_uuid;
    uint8_t cccd_num = 0;
    uint16_t svc_data_len = 0;
    uint8_t  *p_svc_data = NULL;

    if (p_gattc_cb->p_srv_table != NULL)
    {
        err_idx = 1;
        goto error1;
    }

    if (gap_chann_get_addr(p_gattc_cb->conn_handle, remote_bd, &remote_bd_type) == false)
    {
        err_idx = 2;
        goto error1;
    }

    if (gatt_storage_cb)
    {
        T_GATT_STORAGE_SVC_TBL_GET_IND get_ind = {0};
        get_ind.remote_bd_type = remote_bd_type;
        memcpy(get_ind.addr, remote_bd, 6);
        if (gatt_storage_cb(GATT_STORAGE_EVENT_SVC_TBL_GET_IND, &get_ind) != APP_RESULT_SUCCESS)
        {
            if (database_hash_check)
            {
                T_GATT_STORAGE_SVC_TBL_USING_DATABASE_HASH_GET_IND get_ind_using_database_hash = {0};
                get_ind_using_database_hash.remote_bd_type = remote_bd_type;
                memcpy(get_ind_using_database_hash.addr, remote_bd, 6);
                memcpy(get_ind_using_database_hash.database_hash, p_gattc_cb->gatt_svc_data.database_hash,
                       GATT_SVC_DATABASE_HASH_LEN);

                if (gatt_storage_cb(GATT_STORAGE_EVENT_SVC_TBL_USING_DATABASE_HASH_GET_IND,
                                    &get_ind_using_database_hash) == APP_RESULT_SUCCESS)
                {
                    BTM_PRINT_TRACE0("gattc_storage_load: GATT_STORAGE_EVENT_SVC_TBL_USING_DATABASE_HASH_GET_IND");
                    svc_data_len = get_ind_using_database_hash.data_len;
                    p_svc_data = get_ind_using_database_hash.p_data;
                }
            }
        }
        else
        {
            svc_data_len = get_ind.data_len;
            p_svc_data = get_ind.p_data;
        }
    }
    else
    {
        err_idx = 4;
        goto error1;
    }

    if (p_svc_data == NULL)
    {
        err_idx = 5;
        goto error1;
    }
    p_gattc_cb->srv_table_len = svc_data_len;
    p_gattc_cb->p_srv_table = p_svc_data;

    for (idx = 0; idx < svc_data_len;)
    {
        T_ATTR_SRV_CB *p_srv_cb;

        p_srv_cb = calloc(1, sizeof(T_ATTR_SRV_CB));
        if (p_srv_cb == NULL)
        {
            err_idx = 6;
            goto error;
        }
        os_queue_in(&p_gattc_cb->srv_list, p_srv_cb);
        p_srv_cb->p_srv_data = (T_ATTR_DATA *)&p_gattc_cb->p_srv_table[idx];
        if (p_srv_cb->p_srv_data->srv_uuid16.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID16 ||
            p_srv_cb->p_srv_data->srv_uuid16.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID16)
        {
            idx += sizeof(T_ATTR_SRV_UUID16);
            srv_uuid.is_uuid16 = true;
            srv_uuid.p.uuid16 = p_srv_cb->p_srv_data->srv_uuid16.uuid16;
        }
        else if (p_srv_cb->p_srv_data->srv_uuid16.hdr.attr_type == ATTR_TYPE_PRIMARY_SRV_UUID128 ||
                 p_srv_cb->p_srv_data->srv_uuid16.hdr.attr_type == ATTR_TYPE_SECONDARY_SRV_UUID128)
        {
            idx += sizeof(T_ATTR_SRV_UUID128);
            srv_uuid.is_uuid16 = false;
            memcpy(srv_uuid.p.uuid128, p_srv_cb->p_srv_data->srv_uuid128.uuid128, 16);
        }
        else
        {
            err_idx = 7;
            goto error;
        }
        p_srv_cb->p_spec_cb = gatt_spec_client_find_by_uuid(&srv_uuid);
        if (p_srv_cb->p_spec_cb == NULL  && ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_REG_SVC_BIT) != 0))
        {
            err_idx = 8;
            if (srv_uuid.is_uuid16)
            {
                BTM_PRINT_TRACE1("gattc_storage_load: skip uuid16 0x%x",
                                 p_srv_cb->p_srv_data->srv_uuid16.uuid16);
            }
            else
            {
                BTM_PRINT_TRACE0("gattc_storage_load: skip uuid128");
            }
            goto error;
        }
        p_srv_cb->inc16_srv_num = p_gattc_cb->p_srv_table[idx];
        idx++;
        p_srv_cb->inc128_srv_num = p_gattc_cb->p_srv_table[idx];
        idx++;
        p_srv_cb->char16_num = p_gattc_cb->p_srv_table[idx];
        idx++;
        p_srv_cb->char128_num = p_gattc_cb->p_srv_table[idx];
        idx++;
        if (p_srv_cb->inc16_srv_num)
        {
            p_srv_cb->p_inc16_table = (T_ATTR_INCLUDE_UUID16 *)&p_gattc_cb->p_srv_table[idx];
            idx += p_srv_cb->inc16_srv_num * sizeof(T_ATTR_INCLUDE_UUID16);
        }

        if (p_srv_cb->inc128_srv_num)
        {
            p_srv_cb->p_inc128_table = (T_ATTR_INCLUDE_UUID128 *)&p_gattc_cb->p_srv_table[idx];
            idx += p_srv_cb->inc128_srv_num * sizeof(T_ATTR_INCLUDE_UUID128);
        }

        if (p_srv_cb->char16_num)
        {
            p_srv_cb->p_char16_table = (T_ATTR_CHAR16_CB *)&p_gattc_cb->p_srv_table[idx];
            idx += p_srv_cb->char16_num * sizeof(T_ATTR_CHAR16_CB);
        }

        if (p_srv_cb->char128_num)
        {
            p_srv_cb->p_char128_table = (T_ATTR_CHAR128_CB *)&p_gattc_cb->p_srv_table[idx];
            idx += p_srv_cb->char128_num * sizeof(T_ATTR_CHAR128_CB);
        }

        if (p_srv_cb->inc16_srv_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->inc16_srv_num; j++)
            {
                if (p_srv_cb->p_inc16_table[j].hdr.attr_type != ATTR_TYPE_INCLUDE_UUID16)
                {
                    err_idx = 9;
                    goto error;
                }
            }
        }

        if (p_srv_cb->inc128_srv_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->inc128_srv_num; j++)
            {
                if (p_srv_cb->p_inc128_table[j].hdr.attr_type != ATTR_TYPE_INCLUDE_UUID128)
                {
                    err_idx = 10;
                    goto error;
                }
            }
        }

        if (p_srv_cb->char16_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->char16_num; j++)
            {
                if (p_srv_cb->p_char16_table[j].char_data.hdr.attr_type != ATTR_TYPE_CHAR_UUID16)
                {
                    err_idx = 11;
                    goto error;
                }
                if (p_srv_cb->p_char16_table[j].cccd_descriptor.hdr.att_handle)
                {
                    cccd_num++;
                }
            }
        }

        if (p_srv_cb->char128_num)
        {
            for (uint8_t j = 0; j < p_srv_cb->char128_num; j++)
            {
                if (p_srv_cb->p_char128_table[j].char_data.hdr.attr_type != ATTR_TYPE_CHAR_UUID128)
                {
                    err_idx = 12;
                    goto error;
                }
                if (p_srv_cb->p_char128_table[j].cccd_descriptor.hdr.att_handle)
                {
                    cccd_num++;
                }
            }
        }
    }

    if (idx != svc_data_len)
    {
        err_idx = 13;
        goto error;
    }

    if (p_gattc_cb->srv_list.count == 0)
    {
        err_idx = 14;
        goto error;
    }

    if ((gattc_dis_mode & GATT_CLIENT_DISCOV_MODE_CCCD_STORAGE_BIT) != 0)
    {
        p_gattc_cb->cccd_num = cccd_num;
        BTM_PRINT_INFO1("gattc_storage_load: cccd_num %d", cccd_num);
        if (p_gattc_cb->cccd_num)
        {
            p_gattc_cb->p_cccd_table = calloc(1,
                                              p_gattc_cb->cccd_num * sizeof(T_GATTC_CCCD));
            if (gatt_storage_cb)
            {
                T_GATT_STORAGE_SVC_CCCD_GET_IND get_cccd_ind = {0};
                get_cccd_ind.remote_bd_type = remote_bd_type;
                memcpy(get_cccd_ind.addr, remote_bd, 6);
                if (gatt_storage_cb(GATT_STORAGE_EVENT_SVC_CCCD_GET_IND, &get_cccd_ind) == APP_RESULT_SUCCESS)
                {
                    if (get_cccd_ind.data_len != 0 &&
                        get_cccd_ind.data_len <= p_gattc_cb->cccd_num * sizeof(T_GATTC_CCCD))
                    {
                        memcpy(p_gattc_cb->p_cccd_table, get_cccd_ind.p_data, get_cccd_ind.data_len);
                    }

                    if (get_cccd_ind.p_data)
                    {
                        free(get_cccd_ind.p_data);
                    }
                }
            }
        }
    }
    BTM_PRINT_INFO1("gattc_storage_load: success, conn_handle 0x%x", p_gattc_cb->conn_handle);
    return true;
error1:
    BTM_PRINT_ERROR1("gattc_storage_load: failed 1, err_idx %d", err_idx);
    return false;
error:
    BTM_PRINT_ERROR1("gattc_storage_load: failed 2, err_idx %d", err_idx);
    gattc_storage_srv_list_release(p_gattc_cb);
    return false;
}

bool gattc_storage_write(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    bool ret = false;
    uint8_t error_idx = 0;
    T_GATT_STORAGE_SVC_TBL_SET_IND set_ind = {0};
    uint8_t remote_bd[GAP_BD_ADDR_LEN];
    uint8_t remote_bd_type;

    if (p_gattc_cb == NULL || p_gattc_cb->srv_table_len == 0 ||
        p_gattc_cb->p_srv_table == NULL)
    {
        error_idx = 1;
        goto result;
    }

    if (p_gattc_cb->state != GATT_CLIENT_STATE_DONE)
    {
        error_idx = 2;
        goto result;
    }

    if (gatt_storage_cb == NULL)
    {
        error_idx = 3;
        goto result;
    }

    if (gap_chann_get_addr(p_gattc_cb->conn_handle, remote_bd, &remote_bd_type))
    {
        set_ind.remote_bd_type = remote_bd_type;
        memcpy(set_ind.addr, remote_bd, 6);
    }
    else
    {
        error_idx = 4;
        goto result;
    }
    set_ind.data_len = p_gattc_cb->srv_table_len;
    set_ind.p_data = p_gattc_cb->p_srv_table;
    set_ind.cccd_data_len = p_gattc_cb->cccd_num * 4;
    if (gatt_storage_cb(GATT_STORAGE_EVENT_SVC_TBL_SET_IND, &set_ind) != APP_RESULT_SUCCESS)
    {
        error_idx = 5;
        goto result;
    }

    ret = true;
    BTM_PRINT_INFO2("gattc_storage_write: conn_handle 0x%x, write_len %d",
                    p_gattc_cb->conn_handle, set_ind.data_len);
result:
    if (ret == false)
    {
        BTM_PRINT_ERROR1("gattc_storage_write: failed, error_idx %d", error_idx);
    }
    return ret;
}

uint16_t gattc_get_cccd_data(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle)
{
    if (p_gattc_cb->p_cccd_table)
    {
        for (uint8_t i = 0; i < p_gattc_cb->cccd_num; i++)
        {
            if (p_gattc_cb->p_cccd_table[i].handle == handle)
            {
                return p_gattc_cb->p_cccd_table[i].cccd_bits;
            }
        }
    }
    return 0;
}

static void gattc_cccd_timeout(void *p_handle)
{
    uint32_t timer_id;
    os_timer_id_get(&p_handle, &timer_id);

    T_GATTC_STORAGE_CB *p_gattc_cb = (T_GATTC_STORAGE_CB *)timer_id;

    BTM_PRINT_INFO0("gattc_cccd_timeout");
    if (os_queue_search(&bt_gattc_queue, (void *)p_gattc_cb))
    {
        if (gatt_storage_cb)
        {
            uint8_t remote_bd[GAP_BD_ADDR_LEN];
            uint8_t remote_bd_type;

            if (gap_chann_get_addr(p_gattc_cb->conn_handle, remote_bd, &remote_bd_type))
            {
                T_GATT_STORAGE_SVC_CCCD_SET_IND set_ind = {0};
                set_ind.remote_bd_type = remote_bd_type;
                memcpy(set_ind.addr, remote_bd, 6);
                set_ind.data_len = p_gattc_cb->cccd_num * sizeof(T_GATTC_CCCD);
                set_ind.p_data = (uint8_t *)p_gattc_cb->p_cccd_table;
                gatt_storage_cb(GATT_STORAGE_EVENT_SVC_CCCD_SET_IND, &set_ind);
            }
        }
        if (p_gattc_cb->p_cccd_timer_handle)
        {
            if (os_timer_delete(&p_gattc_cb->p_cccd_timer_handle))
            {
                p_gattc_cb->p_cccd_timer_handle = NULL;
            }
        }
    }
}

void gattc_start_cccd_timer(T_GATTC_STORAGE_CB *p_gattc_cb)
{
    if (p_gattc_cb->p_cccd_timer_handle == NULL)
    {
        os_timer_create(&p_gattc_cb->p_cccd_timer_handle,
                        "cccd_timer", (uint32_t)p_gattc_cb,
                        1000000,
                        false, gattc_cccd_timeout);
        if (p_gattc_cb->p_cccd_timer_handle != NULL)
        {
            os_timer_start(&p_gattc_cb->p_cccd_timer_handle);
        }
    }
    else
    {
        os_timer_restart(&p_gattc_cb->p_cccd_timer_handle, 1000000);
    }
}

void gattc_set_cccd_data(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle, uint16_t cccd_bits)
{
    BTM_PRINT_INFO2("gattc_set_cccd_data: handle 0x%x, cccd_bits 0x%x", handle, cccd_bits);
    if (p_gattc_cb->p_cccd_table)
    {
        for (uint8_t i = 0; i < p_gattc_cb->cccd_num; i++)
        {
            if (p_gattc_cb->p_cccd_table[i].handle == handle)
            {
                if (p_gattc_cb->p_cccd_table[i].cccd_bits == cccd_bits)
                {
                    return;
                }
                p_gattc_cb->p_cccd_table[i].cccd_bits = cccd_bits;
                goto MODIFIED;
            }
        }

        for (uint8_t i = 0; i < p_gattc_cb->cccd_num; i++)
        {
            if (p_gattc_cb->p_cccd_table[i].handle == 0)
            {
                p_gattc_cb->p_cccd_table[i].handle = handle;
                p_gattc_cb->p_cccd_table[i].cccd_bits = cccd_bits;
                goto MODIFIED;
            }
        }
    }
    return;
MODIFIED:
    gattc_start_cccd_timer(p_gattc_cb);
    return;
}
#endif


