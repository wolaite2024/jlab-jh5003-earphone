
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if XM_XIAOAI_FEATURE_SUPPORT

#include <string.h>
#include <trace.h>

#include <xiaoai_mem.h>
#include <trace.h>

#include <xiaoai_mem.h>
#include <app_xiaoai_rcsp.h>
#include "app_xiaoai_transport.h"
#include <xm_auth.h>


bool xm_set_access_auth_state(T_XM_CMD_COMM_WAY_TYPE comm_way, uint8_t *bd_addr, uint8_t conn_id,
                              T_XM_RCSP_ACCESS_AUTH_STATE state)
{
    bool ret = false;

    T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_link(comm_way, bd_addr);
    if (p_link)
    {
        p_link->auth_state = state;
        ret = true;
    }
    return ret;
}

bool xm_set_access_auth_result(T_XM_CMD_COMM_WAY_TYPE comm_way, uint8_t *bd_addr, uint8_t conn_id,
                               bool result)
{
    bool ret = false;

    T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_link(comm_way, bd_addr);
    if (p_link)
    {
        p_link->auth_result = result;
        ret = true;
    }
    return ret;
}

bool xm_xiaoai_get_opcode_sn(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                             uint8_t *opcode_sn)
{
    T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_link(type, bd_addr);
    if (p_link)
    {
        *opcode_sn = p_link->cmd_opcode_sn;
        return true;
    }
    else
    {
        APP_PRINT_ERROR0("xm_xiaoai_get_opcode_sn: app_xiaoai_find_link return NULL");
    }
    return false;
}

bool xm_xiaoai_inc_opcode_sn(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id)
{
    T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_link(type, bd_addr);
    if (p_link)
    {
        p_link->cmd_opcode_sn++;
        return true;
    }
    else
    {
        APP_PRINT_ERROR0("xm_xiaoai_inc_opcode_sn: app_xiaoai_find_link return NULL");
    }
    return false;
}

uint8_t *xm_xiaoai_get_rand(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id)
{
    T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_link(type, bd_addr);
    if (p_link)
    {
        return (uint8_t *) & (p_link->rand);

    }
    return NULL;
}

bool xm_xiaoai_set_rand(T_XM_CMD_COMM_WAY_TYPE type, uint8_t *bd_addr, uint8_t conn_id,
                        uint8_t rand[17])
{
    bool ret = true;
    T_XM_XIAOAI_LINK *p_link = app_xiaoai_find_link(type, bd_addr);
    if (p_link)
    {
        memcpy(&p_link->rand, rand, sizeof(p_link->rand));
    }
    else
    {
        ret = false;
    }
    return ret;
}

void xm_rx_access_auth_pkt(T_XM_CMD_COMM_WAY_TYPE comm_way, uint8_t *bd_addr, uint8_t conn_id,
                           uint8_t *p_pkt,
                           uint16_t len)
{
    uint8_t error = 0x00;

    if (len > 0)
    {
        switch (p_pkt[0])
        {
        case XM_ACCESS_AUTH_PKT_TYPE_RAND:
            {
                T_XM_AUTH_PKT_RAND *p_rand = (T_XM_AUTH_PKT_RAND *)p_pkt;
                T_XM_AUTH_PKT_RAND_RESULT rand_result;

                if (len != sizeof(T_XM_AUTH_PKT_RAND))
                {
                    error = 0x01;
                    break;
                }

                xm_set_access_auth_state(comm_way, bd_addr, conn_id, T_XM_RCSP_ACCESS_AUTH_RX_RAND);

                if (0 == get_encrypted_auth_data((uint8_t *)p_rand, (uint8_t *)&rand_result))
                {
                    if (xm_tx_pkt(bd_addr, conn_id, comm_way, (uint8_t *)&rand_result, sizeof(rand_result)))
                    {
                        xm_set_access_auth_state(comm_way, bd_addr, conn_id, T_XM_RCSP_ACCESS_AUTH_TX_RAND_RESULT);
                    }
                    else
                    {
                        error = 0x02;
                    }
                }
                else
                {
                    error = 0x03;
                }
            }
            break;

        case XM_ACCESS_AUTH_PKT_TYPE_RAND_RESULT:
            {
                bool result = true;
                T_XM_AUTH_PKT_RAND_RESULT *p_rand_result = (T_XM_AUTH_PKT_RAND_RESULT *)p_pkt;
                T_XM_AUTH_PKT_RAND_RESULT rand_result;
                T_XM_AUTH_PKT_AUTH_RESULT auth_result;
                uint8_t *rand = xm_xiaoai_get_rand(comm_way, bd_addr, conn_id);

                if (len != sizeof(T_XM_AUTH_PKT_RAND_RESULT))
                {
                    error = 0x11;
                    break;
                }

                auth_result.pkt_type = XM_ACCESS_AUTH_PKT_TYPE_AUTH_RESULT;

                xm_set_access_auth_state(comm_way, bd_addr, conn_id, T_XM_RCSP_ACCESS_AUTH_RX_RAND_RESULT);

                if (0 == get_encrypted_auth_data(rand, (uint8_t *)&rand_result))
                {
                    if (0 == memcmp((void *)&rand_result, p_rand_result, sizeof(T_XM_AUTH_PKT_RAND_RESULT)))
                    {
                        memcpy(auth_result.result, "pass", sizeof(auth_result.result));
                        result = true;
                    }
                    else
                    {
                        memcpy(auth_result.result, "fail", sizeof(auth_result.result));
                        result = false;
                    }

                    if (xm_tx_pkt(bd_addr, conn_id, comm_way, (uint8_t *)&auth_result, sizeof(auth_result)))
                    {
                        xm_set_access_auth_state(comm_way, bd_addr, conn_id, T_XM_RCSP_ACCESS_AUTH_TX_AUTH_RESULT);
                        if (result == true)
                        {
                            xm_set_access_auth_result(comm_way, bd_addr, conn_id, true);
                            xm_set_access_auth_state(comm_way, bd_addr, conn_id, T_XM_RCSP_ACCESS_AUTH_COMPLETE);

                        }
                    }
                    else
                    {
                        error = 0x12;
                    }
                }
                else
                {
                    error = 0x13;
                }
            }
            break;

        case XM_ACCESS_AUTH_PKT_TYPE_AUTH_RESULT:
            {
                T_XM_AUTH_PKT_AUTH_RESULT *p_auth_result = (T_XM_AUTH_PKT_AUTH_RESULT *)p_pkt;
                T_XM_AUTH_PKT_RAND rand;

                if (len != sizeof(T_XM_AUTH_PKT_AUTH_RESULT))
                {
                    error = 0x21;
                    break;
                }

                xm_set_access_auth_state(comm_way, bd_addr, conn_id, T_XM_RCSP_ACCESS_AUTH_RX_AUTH_RESULT);

                if (0 == memcmp(p_auth_result->result, "pass", 4))
                {
                    if (0 == get_random_auth_data((uint8_t *)&rand))
                    {
                        if (xm_tx_pkt(bd_addr, conn_id, comm_way, (uint8_t *)&rand, sizeof(rand)))
                        {
                            xm_xiaoai_set_rand(comm_way, bd_addr, conn_id, (uint8_t *)&rand);
                            xm_set_access_auth_state(comm_way, bd_addr, conn_id, T_XM_RCSP_ACCESS_AUTH_TX_RAND);
                        }
                        else
                        {
                            error = 0x22;
                        }
                    }
                    else
                    {
                        error = 0x23;
                    }
                }
                else
                {
                    error = 0x24;
                }
            }
            break;

        default:
            {
                error = 0x30;
            }
            break;
        }
    }
    else
    {
        error = 0x40;
    }

    if (0 != error)
    {
        APP_PRINT_ERROR1("xm_rx_access_auth_pkt: error 0x%02x", error);
    }
}

#endif
