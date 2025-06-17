#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "bass_brs_mgr.h"
#include "sys_timer.h"
#include "ble_audio_sync_int.h"
#include "bass_mgr.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_BASS_SUPPORT

T_BASS_CB *p_bass = NULL;

T_BASS_BRS_DB *bass_brs_find_by_id(uint8_t source_id)
{
    for (uint8_t i = 0; i < p_bass->brs_num; i++)
    {
        if (p_bass->brs_tbl[i].brs_data.brs_is_used && p_bass->brs_tbl[i].brs_data.source_id == source_id)
        {
            return &p_bass->brs_tbl[i];
        }
    }
    return NULL;
}

T_BASS_BRS_DB *bass_brs_find_by_handle(T_BLE_AUDIO_SYNC_HANDLE handle)
{
    for (uint8_t i = 0; i < p_bass->brs_num; i++)
    {
        if (handle != NULL && p_bass->brs_tbl[i].handle == handle)
        {
            return &p_bass->brs_tbl[i];
        }
    }
    return NULL;
}

T_BASS_BRS_DB *bass_brs_find_by_src_info(uint8_t source_address_type,
                                         uint8_t source_adv_sid, uint8_t *broadcast_id)
{
    for (uint8_t i = 0; i < p_bass->brs_num; i++)
    {
        if (p_bass->brs_tbl[i].brs_data.source_address_type == source_address_type &&
            p_bass->brs_tbl[i].brs_data.source_adv_sid == source_adv_sid &&
            memcmp(p_bass->brs_tbl[i].brs_data.broadcast_id, broadcast_id, BROADCAST_ID_LEN) == 0)
        {
            return &p_bass->brs_tbl[i];
        }
    }
    return NULL;
}

uint8_t bass_brs_get_empty_char_num(void)
{
    uint8_t num = 0;
    for (uint8_t i = 0; i < p_bass->brs_num; i++)
    {
        if (p_bass->brs_tbl[i].brs_data.brs_is_used == false)
        {
            num++;
        }
    }
    return num;
}

T_BASS_BRS_DB *bass_brs_get_empty_char(void)
{
    for (uint8_t i = 0; i < p_bass->brs_num; i++)
    {
        if (p_bass->brs_tbl[i].brs_data.brs_is_used == false)
        {
            return &p_bass->brs_tbl[i];
        }
    }
    return NULL;
}

T_BASS_BRS_DB *bass_brs_add(uint8_t source_address_type,
                            uint8_t *source_address, uint8_t source_adv_sid,
                            uint8_t *broadcast_id)
{
    if (source_address == NULL || broadcast_id == NULL)
    {
        PROTOCOL_PRINT_ERROR0("bass_brs_add: failed, source_address or broadcast_id is NULL");
        return NULL;
    }
    T_BASS_BRS_DB *p_db = bass_brs_find_by_src_info(source_address_type, source_adv_sid, broadcast_id);
    if (p_db)
    {
        PROTOCOL_PRINT_ERROR0("bass_brs_add: failed, already exist");
        return NULL;
    }
    p_db = bass_brs_get_empty_char();
    if (p_db == NULL)
    {
        PROTOCOL_PRINT_ERROR0("bass_brs_add: failed, no empty brs char");
        return NULL;
    }
    p_db->brs_data.source_address_type = source_address_type;
    p_db->brs_data.source_adv_sid = source_adv_sid;
    memcpy(p_db->brs_data.source_address, source_address, GAP_BD_ADDR_LEN);
    memcpy(p_db->brs_data.broadcast_id, broadcast_id, 3);
    p_db->brs_data.brs_is_used = true;
    p_db->pa_sync_options = 0;
    p_db->pa_sync_skip = 0;
    p_db->pa_sync_timeout = 1000;//default 10s
    p_db->pa_interval = BASS_PA_INTERVAL_UNKNOWN;
    p_db->past_timeout = 200;//default 2s
    p_db->big_sync_param.mse = 0;
    p_db->big_sync_param.big_sync_timeout = 1000;//default 10s
    PROTOCOL_PRINT_INFO1("bass_brs_add: source_id %d", p_db->brs_data.source_id);
    return p_db;
}

bool bass_brs_gen_char_data(uint8_t source_id, uint8_t  **pp_data,
                            uint16_t *p_data_len)
{
    uint8_t *p_data = NULL;
    uint8_t i;
    T_BASS_BRS_DB *p_db = NULL;
    for (uint8_t i = 0; i < p_bass->brs_num; i++)
    {
        if (p_bass->brs_tbl[i].brs_data.source_id == source_id)
        {
            p_db = &p_bass->brs_tbl[i];
            break;
        }
    }
    if (p_db == NULL)
    {
        goto error;
    }
    if (p_db->brs_data.brs_is_used == false)
    {
        *p_data_len = 0;
        *pp_data = NULL;
        return true;
    }
    p_data = ble_audio_mem_zalloc(BASS_BRA_INC_BST_CODE_LEN + p_db->brs_data.bis_info_size);

    if (p_data != NULL)
    {
        uint16_t idx = 0;
        p_data[idx] = p_db->brs_data.source_id;
        idx++;
        p_data[idx] = p_db->brs_data.source_address_type;
        idx++;
        memcpy(p_data + idx, p_db->brs_data.source_address, GAP_BD_ADDR_LEN);
        idx += GAP_BD_ADDR_LEN;
        p_data[idx] = p_db->brs_data.source_adv_sid;
        idx++;
        memcpy(p_data + idx, p_db->brs_data.broadcast_id, 3);
        idx += 3;
        p_data[idx] = p_db->brs_data.pa_sync_state;
        idx++;
        p_data[idx] = p_db->brs_data.big_encryption;
        idx++;
        if (p_db->brs_data.big_encryption == BIG_BAD_CODE)
        {
            memcpy(p_data + idx, p_db->brs_data.bad_code, BROADCAST_CODE_LEN);
            idx += BROADCAST_CODE_LEN;
        }
        p_data[idx] = p_db->brs_data.num_subgroups;
        idx++;
        for (i = 0; i < p_db->brs_data.num_subgroups; i++)
        {
            LE_UINT32_TO_ARRAY(&p_data[idx], p_db->brs_data.p_cp_bis_info[i].bis_sync);
            idx += 4;

            p_data[idx] = p_db->brs_data.p_cp_bis_info[i].metadata_len;
            idx++;
            if (p_db->brs_data.p_cp_bis_info[i].metadata_len > 0)
            {
                memcpy(&p_data[idx], p_db->brs_data.p_cp_bis_info[i].p_metadata,
                       p_db->brs_data.p_cp_bis_info[i].metadata_len);
                idx += p_db->brs_data.p_cp_bis_info[i].metadata_len;
            }
        }
        *p_data_len = idx;
    }
    else
    {
        goto error;
    }
    *pp_data = p_data;
    return true;
error:
    PROTOCOL_PRINT_ERROR0("bass_brs_gen_char_data: failed");
    return false;
}

void bass_stop_past_recipient(void)
{
    T_GAP_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_PARAM param = {(T_GAP_PAST_RECIPIENT_PERIODIC_ADV_SYNC_TRANSFER_MODE)0};

    param.sync_timeout = 0x3E8;

    for (uint8_t i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED) &&
            ble_audio_db.le_link[i].past_mode != 0)
        {
            if (ble_audio_set_past_recipient_param(ble_audio_db.le_link[i].conn_handle, &param))
            {
                ble_audio_db.le_link[i].past_mode = 0;
            }
        }
    }
}

bool bass_brs_delete(T_BASS_BRS_DB *p_db, bool send_notify)
{
    uint8_t source_id;
    if (p_db == NULL)
    {
        return false;
    }
    if (p_db->brs_data.brs_is_used == false)
    {
        return true;
    }
    if (p_db->handle != NULL)
    {
        if (ble_audio_sync_release_int(&p_db->handle, BLE_AUDIO_ACTION_ROLE_BASS) == false)
        {
            return false;
        }
    }
    if (p_db->brs_data.pa_sync_state == PA_SYNC_STATE_SYNC_INFO_REQ)
    {
        bass_stop_past_recipient();
    }
    source_id = p_db->brs_data.source_id;
    if (p_db->brs_data.p_cp_bis_info)
    {
        ble_audio_mem_free(p_db->brs_data.p_cp_bis_info);
    }
    if (p_db->p_past_timer_handle != NULL)
    {
        sys_timer_delete(p_db->p_past_timer_handle);
    }
    if (p_db->p_pending_cp_data)
    {
        ble_audio_mem_free(p_db->p_pending_cp_data);
    }
    memset(p_db, 0, sizeof(T_BASS_BRS_DB));
    p_db->brs_data.source_id = source_id;
    p_db->brs_modify = send_notify;
    p_db->send_notify = send_notify;
    PROTOCOL_PRINT_INFO1("bass_brs_delete: source_id %d", source_id);
    return true;
}

/* type for action functions */
typedef bool (*T_BASS_ACTION)(T_BASS_BRS_DB *p_db, uint8_t *p_data);

/* state machine action enumeration list */
enum
{
    BASS_PA_TERMINATE_ACT = 0,
    BASS_PA_SYNC_PAST_ACT = 1,
    BASS_PA_SYNC_NO_PAST_ACT = 2,
    BASS_PA_NUM_ACTIONS
} T_BASS_PA_SM_ACTION;

#define BASS_PA_ACT_IGNORE BASS_PA_NUM_ACTIONS

/* state table for BASS_PA_IDLE state */
const uint8_t bass_pa_sm_idle[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_PA_CP_NO_SYNC_EVT      */   {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_CP_SYNC_PAST_EVT  */     {BASS_PA_SYNC_PAST_ACT,     BASS_PA_WAIT_PAST},
    /* BASS_PA_CP_SYNC_NO_PAST_EVT  */  {BASS_PA_SYNC_NO_PAST_ACT,  BASS_PA_SYNC_REQ},
    /* BASS_PA_PAST_TIMEOUT_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_STATE_IDLE  */           {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_STATE_SYNCHRONIZING  */  {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_STATE_SYNCHRONIZED  */   {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_STATE_TERMINATING  */    {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
};

/* state table for BASS_PA_SYNC_REQ state */
const uint8_t bass_pa_sm_sync_req[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_PA_CP_NO_SYNC_EVT      */   {BASS_PA_TERMINATE_ACT,     BASS_PA_TERMINATE_REQ},
    /* BASS_PA_CP_SYNC_PAST_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_CP_SYNC_NO_PAST_EVT  */  {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_PAST_TIMEOUT_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_STATE_IDLE  */           {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_STATE_SYNCHRONIZING  */  {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_STATE_SYNCHRONIZED  */   {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_STATE_TERMINATING  */    {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
};

/* state table for BASS_PA_WAIT_PAST state */
const uint8_t bass_pa_sm_wait_past[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_PA_CP_NO_SYNC_EVT      */   {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_CP_SYNC_PAST_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_WAIT_PAST},
    /* BASS_PA_CP_SYNC_NO_PAST_EVT  */  {BASS_PA_SYNC_NO_PAST_ACT,        BASS_PA_SYNC_REQ},
    /* BASS_PA_PAST_TIMEOUT_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_STATE_IDLE  */           {BASS_PA_ACT_IGNORE,        BASS_PA_WAIT_PAST},
    /* BASS_PA_STATE_SYNCHRONIZING  */  {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_STATE_SYNCHRONIZED  */   {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_STATE_TERMINATING  */    {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
};

/* state table for BASS_PA_SYNCED state */
const uint8_t bass_pa_sm_synced[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_PA_CP_NO_SYNC_EVT      */   {BASS_PA_TERMINATE_ACT,     BASS_PA_TERMINATE_REQ},
    /* BASS_PA_CP_SYNC_PAST_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_CP_SYNC_NO_PAST_EVT  */  {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_PAST_TIMEOUT_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_STATE_IDLE  */           {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_STATE_SYNCHRONIZING  */  {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_STATE_SYNCHRONIZED  */   {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_STATE_TERMINATING  */    {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
};

/* state table for BASS_PA_TERMINATE_REQ state */
const uint8_t bass_pa_sm_terminate_req[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_PA_CP_NO_SYNC_EVT      */   {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
    /* BASS_PA_CP_SYNC_PAST_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
    /* BASS_PA_CP_SYNC_NO_PAST_EVT  */  {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
    /* BASS_PA_PAST_TIMEOUT_EVT  */     {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
    /* BASS_PA_STATE_IDLE  */           {BASS_PA_ACT_IGNORE,        BASS_PA_IDLE},
    /* BASS_PA_STATE_SYNCHRONIZING  */  {BASS_PA_ACT_IGNORE,        BASS_PA_SYNC_REQ},
    /* BASS_PA_STATE_SYNCHRONIZED  */   {BASS_PA_ACT_IGNORE,        BASS_PA_SYNCED},
    /* BASS_PA_STATE_TERMINATING  */    {BASS_PA_ACT_IGNORE,        BASS_PA_TERMINATE_REQ},
};
/* type for state table */
typedef const uint8_t (*T_BASS_PROC_SM_TBL)[2];

/* state table */
const T_BASS_PROC_SM_TBL bass_pa_sm_tbl[] =
{
    bass_pa_sm_idle,
    bass_pa_sm_sync_req,
    bass_pa_sm_wait_past,
    bass_pa_sm_synced,
    bass_pa_sm_terminate_req,
};

static bool bass_pa_terminate_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);
static bool bass_pa_sync_past_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);
static bool bass_pa_sync_no_past_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);

/* action functions */
const T_BASS_ACTION bass_pa_proc_action[] =
{
    bass_pa_terminate_proc,
    bass_pa_sync_past_proc,
    bass_pa_sync_no_past_proc,
    NULL
};

void bass_past_timer_callback(T_SYS_TIMER_HANDLE handle)
{
    uint32_t source_id = sys_timer_id_get(handle);
    PROTOCOL_PRINT_INFO0("bass_past_timer_callback");
    T_BASS_BRS_DB *p_db = NULL;
    p_db = bass_brs_find_by_id(source_id);
    if (p_db)
    {
        bass_pa_proc_sm_execute(p_db, BASS_PA_PAST_TIMEOUT_EVT, NULL, true);
    }
}

bool bass_start_past_timer(T_BASS_BRS_DB *p_db)
{
    if (p_db && p_db->p_past_timer_handle == NULL)
    {
        p_db->p_past_timer_handle = sys_timer_create("past_sync_timer",
                                                     SYS_TIMER_TYPE_LOW_PRECISION,
                                                     p_db->brs_data.source_id,
                                                     p_db->past_timeout * 10000, false, bass_past_timer_callback);
        if (p_db->p_past_timer_handle != NULL)
        {
            if (sys_timer_start(p_db->p_past_timer_handle))
            {
                return true;
            }
        }
    }
    PROTOCOL_PRINT_ERROR0("bass_start_past_timer: failed");
    return false;
}

bool bass_stop_past_timer(T_BASS_BRS_DB *p_db)
{
    if (p_db && p_db->p_past_timer_handle != NULL)
    {
        if (sys_timer_delete(p_db->p_past_timer_handle))
        {
            p_db->p_past_timer_handle = NULL;
            return true;
        }
    }
    return false;
}

void bass_pa_update_state(T_BASS_BRS_DB *p_db, T_BASS_PA_SM_STATE old_state)
{
    if (p_db->pa_sm_state != old_state)
    {
        T_PA_SYNC_STATE old_pa_state = p_db->brs_data.pa_sync_state;
        switch (p_db->pa_sm_state)
        {
        case BASS_PA_IDLE:
            {
                if (old_state == BASS_PA_WAIT_PAST)
                {
                    bass_stop_past_timer(p_db);
                    p_db->brs_data.pa_sync_state = PA_SYNC_STATE_NO_PAST;
                }
                else if (old_state == BASS_PA_SYNC_REQ)
                {
                    p_db->brs_data.pa_sync_state = PA_SYNC_STATE_FAILED;
                }
                else
                {
                    p_db->brs_data.pa_sync_state = PA_SYNC_STATE_NOT_SYNC;
                }
            }
            break;

        case BASS_PA_WAIT_PAST:
            {
                p_db->brs_data.pa_sync_state = PA_SYNC_STATE_SYNC_INFO_REQ;
            }
            break;

        case BASS_PA_SYNCED:
            {
                if (old_state == BASS_PA_WAIT_PAST)
                {
                    bass_stop_past_timer(p_db);
                }
                p_db->brs_data.pa_sync_state = PA_SYNC_STATE_SYNC;
            }
            break;

        default:
            break;
        }
        if (old_pa_state != p_db->brs_data.pa_sync_state)
        {
            PROTOCOL_PRINT_INFO2("bass_pa_update_state: pa state [%d] -> [%d]",
                                 old_pa_state, p_db->brs_data.pa_sync_state);
            p_db->send_notify = true;
            p_db->brs_modify = true;
            if (p_db->brs_data.big_encryption == BIG_BAD_CODE)
            {
                p_db->brs_data.big_encryption = BIG_NOT_ENCRYPTED;
            }
        }
    }
}

void bass_pa_proc_sm_execute(T_BASS_BRS_DB *p_db, T_BASS_PA_SM_EVENT event,
                             uint8_t *p_data, bool send_notify)
{
    T_BASS_PROC_SM_TBL state_table;
    uint8_t action;
    bool res = true;
    T_BASS_PA_SM_STATE old_state = p_db->pa_sm_state;
    state_table = bass_pa_sm_tbl[p_db->pa_sm_state];

    action = state_table[event][0];
    if (action != BASS_PA_ACT_IGNORE)
    {
        res = (*bass_pa_proc_action[action])(p_db, p_data);
    }

    if (res)
    {
        p_db->pa_sm_state = (T_BASS_PA_SM_STATE)state_table[event][1];
    }
    bass_pa_update_state(p_db, old_state);
    if (p_db->send_notify && send_notify)
    {
        bass_send_all_notify(p_db);
    }
    PROTOCOL_PRINT_INFO4("bass_pa_proc_sm_execute: action %d, state [%d] -> [%d] after Event [0x%x]",
                         action, old_state, p_db->pa_sm_state, event);
}

static bool bass_pa_terminate_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    return ble_audio_pa_terminate_int(p_db->handle, BLE_AUDIO_ACTION_ROLE_BASS);
}

static bool bass_pa_sync_past_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    return bass_start_past_timer(p_db);
}

bool bass_cfg_pa_sync_param(uint8_t source_id, uint8_t pa_sync_options, uint16_t pa_sync_skip,
                            uint16_t pa_sync_timeout_10ms, uint16_t past_timeout_10ms)
{
    T_BASS_BRS_DB *p_db = NULL;
    uint8_t err_idx = 0;
    p_db = bass_brs_find_by_id(source_id);
    if (p_db)
    {
        if ((pa_sync_options & 0x7) != 0)
        {
            err_idx = 1;
            goto failed;
        }
        if (pa_sync_skip > 0x1F3)
        {
            err_idx = 2;
            goto failed;
        }

        if (pa_sync_timeout_10ms < 0x0A || pa_sync_timeout_10ms > 0x4000)
        {
            err_idx = 3;
            goto failed;
        }
        p_db->pa_sync_options = pa_sync_options;
        p_db->pa_sync_skip = pa_sync_skip;
        p_db->pa_sync_timeout = pa_sync_timeout_10ms;
        p_db->past_timeout = past_timeout_10ms;
    }
    else
    {
        err_idx = 4;
        goto failed;
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bass_cfg_pa_sync_param: failed, err_idx %d", err_idx);
    return false;
}

static bool bass_pa_sync_no_past_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    T_BASS_GET_PA_SYNC_PARAM sync_param;
    sync_param.source_id = p_db->brs_data.source_id;
    sync_param.handle = p_db->handle;
    sync_param.pa_interval = p_db->pa_interval;
    sync_param.is_past = false;
    ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_GET_PA_SYNC_PARAM, &sync_param);
    return ble_audio_pa_sync_establish_int(p_db->handle, p_db->pa_sync_options, 0,
                                           p_db->pa_sync_skip, p_db->pa_sync_timeout,
                                           BLE_AUDIO_ACTION_ROLE_BASS);
}

/* state machine action enumeration list */
typedef enum
{
    BASS_BIG_TERMINATE_REQ_ACT = 0,
    BASS_BIG_SYNCHRONIZED_ACT = 1,
    BASS_BIG_TERMINATED_ACT = 2,
    BASS_BIG_CHECK_INFO = 3,
    BASS_BIG_SYNC_FAILED = 4,
    BASS_BIG_NUM_ACTIONS
} T_BASS_BIG_SM_ACTION;

#define BASS_BIG_ACT_IGNORE BASS_BIG_NUM_ACTIONS

/* state table for BASS_BIG_IDLE state */
const uint8_t bass_big_sm_idle[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_BIG_CP_NO_SYNC_EVT      */   {BASS_BIG_ACT_IGNORE,     BASS_BIG_IDLE},
    /* BASS_BIG_CP_SYNC_EVT  */          {BASS_BIG_ACT_IGNORE,     BASS_BIG_WAIT_SYNC},
    /* BASS_BIG_PA_SYNC_EVT      */      {BASS_BIG_ACT_IGNORE,     BASS_BIG_IDLE},
    /* BASS_BIG_BIG_INFO_EVT  */         {BASS_BIG_ACT_IGNORE,     BASS_BIG_IDLE},
    /* BASS_BIG_SET_BROADCODE_EVT  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_IDLE},
    /* BASS_BIG_STATE_IDLE  */           {BASS_BIG_TERMINATED_ACT,     BASS_BIG_IDLE},
    /* BASS_BIG_STATE_SYNCHRONIZING  */  {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_STATE_SYNCHRONIZED  */   {BASS_BIG_SYNCHRONIZED_ACT,     BASS_BIG_SYNCED},
    /* BASS_BIG_STATE_TERMINATING  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
};

/* state table for BASS_BIG_SYNC_REQ state */
const uint8_t bass_big_sm_sync_req[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_BIG_CP_NO_SYNC_EVT      */   {BASS_BIG_TERMINATE_REQ_ACT,  BASS_BIG_TERMINATE_REQ},
    /* BASS_BIG_CP_SYNC_EVT  */          {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_PA_SYNC_EVT      */      {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_BIG_INFO_EVT  */         {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_SET_BROADCODE_EVT  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_STATE_IDLE  */           {BASS_BIG_SYNC_FAILED,     BASS_BIG_IDLE},
    /* BASS_BIG_STATE_SYNCHRONIZING  */  {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_STATE_SYNCHRONIZED  */   {BASS_BIG_SYNCHRONIZED_ACT,     BASS_BIG_SYNCED},
    /* BASS_BIG_STATE_TERMINATING  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
};

/* state table for BASS_BIG_WAIT_SYNC state */
const uint8_t bass_big_sm_wait_sync[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_BIG_CP_NO_SYNC_EVT      */   {BASS_BIG_ACT_IGNORE,     BASS_BIG_IDLE},
    /* BASS_BIG_CP_SYNC_EVT  */          {BASS_BIG_CHECK_INFO,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_PA_SYNC_EVT      */      {BASS_BIG_CHECK_INFO,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_BIG_INFO_EVT  */         {BASS_BIG_CHECK_INFO,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_SET_BROADCODE_EVT  */    {BASS_BIG_CHECK_INFO,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_STATE_IDLE  */           {BASS_BIG_TERMINATED_ACT,     BASS_BIG_WAIT_SYNC},
    /* BASS_BIG_STATE_SYNCHRONIZING  */  {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_STATE_SYNCHRONIZED  */   {BASS_BIG_SYNCHRONIZED_ACT,     BASS_BIG_SYNCED},
    /* BASS_BIG_STATE_TERMINATING  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
};

/* state table for BASS_BIG_SYNCED state */
const uint8_t bass_big_sm_synced[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_BIG_CP_NO_SYNC_EVT      */   {BASS_BIG_TERMINATE_REQ_ACT,     BASS_BIG_TERMINATE_REQ},
    /* BASS_BIG_CP_SYNC_EVT  */          {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNCED},
    /* BASS_BIG_PA_SYNC_EVT      */      {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNCED},
    /* BASS_BIG_BIG_INFO_EVT  */         {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNCED},
    /* BASS_BIG_SET_BROADCODE_EVT  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNCED},
    /* BASS_BIG_STATE_IDLE  */           {BASS_BIG_TERMINATED_ACT,     BASS_BIG_IDLE},
    /* BASS_BIG_STATE_SYNCHRONIZING  */  {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_STATE_SYNCHRONIZED  */   {BASS_BIG_SYNCHRONIZED_ACT,     BASS_BIG_SYNCED},
    /* BASS_BIG_STATE_TERMINATING  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
};

/* state table for BASS_BIG_TERMINATE_REQ state */
const uint8_t bass_big_sm_terminate_req[][2] =
{
    /* Event                             Action                    Next state */
    /* BASS_BIG_CP_NO_SYNC_EVT      */   {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
    /* BASS_BIG_CP_SYNC_EVT  */          {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
    /* BASS_BIG_PA_SYNC_EVT      */      {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
    /* BASS_BIG_BIG_INFO_EVT  */         {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
    /* BASS_BIG_SET_BROADCODE_EVT  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
    /* BASS_BIG_STATE_IDLE  */           {BASS_BIG_TERMINATED_ACT,     BASS_BIG_IDLE},
    /* BASS_BIG_STATE_SYNCHRONIZING  */  {BASS_BIG_ACT_IGNORE,     BASS_BIG_SYNC_REQ},
    /* BASS_BIG_STATE_SYNCHRONIZED  */   {BASS_BIG_SYNCHRONIZED_ACT,     BASS_BIG_SYNCED},
    /* BASS_BIG_STATE_TERMINATING  */    {BASS_BIG_ACT_IGNORE,     BASS_BIG_TERMINATE_REQ},
};

/* state table */
const T_BASS_PROC_SM_TBL bass_big_sm_tbl[] =
{
    bass_big_sm_idle,
    bass_big_sm_sync_req,
    bass_big_sm_wait_sync,
    bass_big_sm_synced,
    bass_big_sm_terminate_req,
};

static bool bass_big_terminate_req_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);
static bool bass_big_synchronized_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);
static bool bass_big_terminated_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);
static bool bass_big_check_info_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);
static bool bass_big_sync_failed_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data);

/* action functions */
const T_BASS_ACTION bass_big_proc_action[] =
{
    bass_big_terminate_req_proc,
    bass_big_synchronized_proc,
    bass_big_terminated_proc,
    bass_big_check_info_proc,
    bass_big_sync_failed_proc,
    NULL
};

bool bass_big_update_bis_state(T_BASS_BRS_DB *p_db, uint32_t bis_state,
                               T_BLE_AUDIO_ACTION_ROLE action_role)
{
    if (p_db->brs_data.bis_sync_state != bis_state)
    {
        p_db->brs_data.bis_sync_state = bis_state;
        for (uint8_t i = 0; i < p_db->brs_data.num_subgroups; i++)
        {
            if (p_db->brs_data.bis_sync_state == BASS_FAILED_TO_SYNC_TO_BIG)
            {
                p_db->brs_data.p_cp_bis_info[i].bis_sync = BASS_FAILED_TO_SYNC_TO_BIG;
            }
            else if (p_db->brs_data.bis_sync_state == 0)
            {
                p_db->brs_data.p_cp_bis_info[i].bis_sync = 0;
            }
            else
            {
                if (action_role == BLE_AUDIO_ACTION_ROLE_BASS)
                {
                    if (i < p_db->bis_sync_info.cp_subgroup_num)
                    {
                        p_db->brs_data.p_cp_bis_info[i].bis_sync = (p_db->brs_data.bis_sync_state &
                                                                    p_db->bis_sync_info.cp_subgroup_bis[i]);
                    }
                    else
                    {
                        PROTOCOL_PRINT_WARN1("bass_big_update_bis_state: cp sub group mismatch %d", i);
                    }
                }
                else
                {
                    if (i < p_db->bis_sync_info.base_data_subgroup_num)
                    {
                        p_db->brs_data.p_cp_bis_info[i].bis_sync = (p_db->brs_data.bis_sync_state &
                                                                    p_db->bis_sync_info.base_data_subgroup_bis[i]);
                    }
                    else
                    {
                        PROTOCOL_PRINT_WARN1("bass_big_update_bis_state: base sub group mismatch %d", i);
                    }
                }
            }
        }
        if (p_db->brs_data.bis_sync_state == BASS_FAILED_TO_SYNC_TO_BIG)
        {
            p_db->brs_data.bis_sync_state = 0;
        }
        p_db->brs_modify = true;
        return true;
    }
    return false;
}

void bass_big_proc_sm_execute(T_BASS_BRS_DB *p_db, T_BASS_BIG_SM_EVENT event,
                              uint8_t *p_data, bool send_notify)
{
    T_BASS_PROC_SM_TBL state_table;
    uint8_t action;
    bool res = true;
    T_BASS_BIG_SM_STATE old_state = p_db->big_sm_state;
    state_table = bass_big_sm_tbl[p_db->big_sm_state];

    action = state_table[event][0];
    if (action != BASS_BIG_ACT_IGNORE)
    {
        res = (*bass_big_proc_action[action])(p_db, p_data);
    }

    if (res)
    {
        p_db->big_sm_state = (T_BASS_BIG_SM_STATE)state_table[event][1];
    }
    if (p_db->send_notify && send_notify)
    {
        bass_send_all_notify(p_db);
    }
    PROTOCOL_PRINT_INFO4("bass_big_proc_sm_execute: action %d, state [%d] -> [%d] after Event [0x%x]",
                         action, old_state, p_db->big_sm_state, event);
}

static bool bass_big_terminate_req_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    return ble_audio_big_terminate_int(p_db->handle, BLE_AUDIO_ACTION_ROLE_BASS);
}

static bool bass_big_synchronized_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    T_BLE_AUDIO_BIG_SYNC_STATE *p_big_sync_state = (T_BLE_AUDIO_BIG_SYNC_STATE *)p_data;
    T_BIG_ENCRYPTION_STATE old_big_encryption = p_db->brs_data.big_encryption;
    uint32_t sync_state;
    if (ble_audio_get_bis_sync_state(p_db->handle, &sync_state))
    {
        p_db->send_notify = bass_big_update_bis_state(p_db, sync_state, p_big_sync_state->action_role);
    }
    if (p_big_sync_state->encryption)
    {
        p_db->brs_data.big_encryption = BIG_DECRYPTING;
    }
    else
    {
        p_db->brs_data.big_encryption = BIG_NOT_ENCRYPTED;
    }
    if (old_big_encryption != p_db->brs_data.big_encryption)
    {
        p_db->brs_modify = true;
        p_db->send_notify = true;
    }
    return true;
}

static bool bass_big_terminated_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    T_BLE_AUDIO_BIG_SYNC_STATE *p_big_sync_state = (T_BLE_AUDIO_BIG_SYNC_STATE *)p_data;
    T_BIG_ENCRYPTION_STATE old_big_encryption = p_db->brs_data.big_encryption;

    p_db->send_notify = bass_big_update_bis_state(p_db, 0, p_big_sync_state->action_role);

    if (p_big_sync_state->encryption &&
        (p_big_sync_state->cause == (HCI_ERR | HCI_ERR_ENCRYPT_MODE_NOT_ACCEPTABLE) ||
         p_big_sync_state->cause == (HCI_ERR |
                                     HCI_ERR_MIC_FAILURE))) //FIX TODO mic failure may be mis-match
    {
        T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM sync_param;
        if (ble_audio_big_get_sync_info(p_db->handle, &sync_param))
        {
            memcpy(p_db->brs_data.bad_code, sync_param.broadcast_code, BROADCAST_CODE_LEN);
        }
        p_db->brs_data.big_encryption = BIG_BAD_CODE;
    }
    else
    {
        p_db->brs_data.big_encryption = BIG_NOT_ENCRYPTED;
    }
    if (old_big_encryption != p_db->brs_data.big_encryption)
    {
        p_db->send_notify = true;
        p_db->brs_modify = true;
    }
    return true;
}

static bool bass_big_sync_failed_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    p_db->send_notify = bass_big_update_bis_state(p_db, BASS_FAILED_TO_SYNC_TO_BIG,
                                                  BLE_AUDIO_ACTION_ROLE_IDLE);
    return true;
}

bool bass_update_bis_idx(T_BIG_MGR_SYNC_RECEIVER_BIG_CREATE_SYNC_PARAM *p_sync_param,
                         uint32_t bis_sync)
{
    uint8_t bis_num = 0;
    uint8_t bis_idx = 0;

    while (bis_sync > 0)
    {
        bis_idx++;
        if (bis_sync & 0x01)
        {
            p_sync_param->bis[bis_num] = bis_idx;
            bis_num++;
        }
        bis_sync >>= 1;
        if (bis_num > GAP_BIG_MGR_MAX_BIS_NUM)
        {
            return false;
        }
    }
    p_sync_param->num_bis = bis_num;
    return true;
}

bool bass_cfg_big_sync_param(uint8_t source_id, uint8_t big_mse, uint16_t big_sync_timeout_10ms)
{
    T_BASS_BRS_DB *p_db = NULL;
    p_db = bass_brs_find_by_id(source_id);
    if (p_db)
    {
        if (big_mse > 0x1F)
        {
            goto failed;
        }
        if (big_sync_timeout_10ms < 0x0A || big_sync_timeout_10ms > 0x4000)
        {
            goto failed;
        }
        p_db->big_sync_param.mse = big_mse;
        p_db->big_sync_param.big_sync_timeout = big_sync_timeout_10ms;
    }
    else
    {
        goto failed;
    }
    return true;
failed:
    PROTOCOL_PRINT_ERROR2("bass_cfg_big_sync_param: failed, big_mse %d, big_sync_timeout_10ms %d",
                          big_mse, big_sync_timeout_10ms);
    return false;
}

bool bass_cfg_prefer_bis_sync(uint8_t source_id, uint8_t subgroups_idx, uint32_t bis_sync)
{
    T_BASS_BRS_DB *p_db = NULL;
    uint8_t err_idx = 0;
    p_db = bass_brs_find_by_id(source_id);
    if (p_db && p_db->p_pending_cp_data)
    {
        uint8_t num_subgroups;
        T_BASS_CP_BIS_INFO  *p_cp_bis_info;
        uint32_t cp_bis_sync = 0;
        bool no_prefer = false;
        if (p_db->p_pending_cp_data->cp_op == BASS_CP_OP_ADD_SOURCE)
        {
            num_subgroups = p_db->p_pending_cp_data->param.add_source.num_subgroups;
            p_cp_bis_info = p_db->p_pending_cp_data->param.add_source.p_cp_bis_info;
        }
        else if (p_db->p_pending_cp_data->cp_op == BASS_CP_OP_MODIFY_SOURCE)
        {
            num_subgroups = p_db->p_pending_cp_data->param.modify_source.num_subgroups;
            p_cp_bis_info = p_db->p_pending_cp_data->param.modify_source.p_cp_bis_info;
        }
        else
        {
            err_idx = 1;
            goto failed;
        }
        if (subgroups_idx >= num_subgroups)
        {
            err_idx = 2;
            goto failed;
        }
        if (p_cp_bis_info[subgroups_idx].bis_sync != BASS_CP_BIS_SYNC_NO_PREFER)
        {
            err_idx = 3;
            goto failed;
        }
        p_cp_bis_info[subgroups_idx].bis_sync = bis_sync;
        for (uint8_t i = 0; i < num_subgroups; i++)
        {
            if (p_cp_bis_info[i].bis_sync != BASS_CP_BIS_SYNC_NO_PREFER)
            {
                if (cp_bis_sync & p_cp_bis_info[i].bis_sync)
                {
                    p_cp_bis_info[subgroups_idx].bis_sync = BASS_CP_BIS_SYNC_NO_PREFER;
                    err_idx = 4;
                    goto failed;
                }
                cp_bis_sync |= p_cp_bis_info[i].bis_sync;
            }
            else
            {
                no_prefer = true;
            }
        }
        if (cp_bis_sync >= (0x01 << p_db->biginfo_bis_num))
        {
            p_cp_bis_info[subgroups_idx].bis_sync = BASS_CP_BIS_SYNC_NO_PREFER;
            err_idx = 5;
            goto failed;
        }
        if (no_prefer == false)
        {
            p_db->bis_sync_info.cp_subgroup_bis[subgroups_idx] = bis_sync;
            p_db->bis_sync_info.cp_big_sync = cp_bis_sync;
        }
    }
    else
    {
        err_idx = 6;
        goto failed;
    }
    PROTOCOL_PRINT_INFO3("bass_cfg_prefer_bis_sync: subgroups_idx %d, bis_sync 0x%x, cp_big_sync 0x%x",
                         subgroups_idx, bis_sync, p_db->bis_sync_info.cp_big_sync);
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("bass_cfg_prefer_bis_sync: failed, err_idx %d", err_idx);
    return false;
}

static bool bass_big_check_info_proc(T_BASS_BRS_DB *p_db, uint8_t *p_data)
{
    if (p_db->pa_sync_state == GAP_PA_SYNC_STATE_SYNCHRONIZED)
    {
        PROTOCOL_PRINT_INFO2("bass_big_check_info_proc: source_id %d, big_info_flags 0x%x",
                             p_db->brs_data.source_id, p_db->big_info_flags);
        if (p_db->big_info_flags & BASS_BIG_INFO_RECEIVE_FLAG)
        {
            if (p_db->big_info_flags == (BASS_BIG_INFO_RECEIVE_FLAG | BASS_BIG_BROADCAST_REQUIRED_FLAG))
            {
                if (p_db->brs_data.big_encryption != BIG_BROADCAST_CODE_REQUIRED)
                {
                    T_BASS_GET_BROADCAST_CODE get_info;
                    get_info.source_id = p_db->brs_data.source_id;
                    get_info.handle = p_db->handle;
                    ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_GET_BROADCAST_CODE, &get_info);
                    if (p_db->big_info_flags == (BASS_BIG_INFO_RECEIVE_FLAG | BASS_BIG_BROADCAST_REQUIRED_FLAG))
                    {
                        p_db->brs_data.big_encryption = BIG_BROADCAST_CODE_REQUIRED;
                        p_db->send_notify = true;
                        p_db->brs_modify = true;
                    }
                }
            }
            if (p_db->big_info_flags == BASS_BIG_ENCRYPTION_FLAGS ||
                p_db->big_info_flags == BASS_BIG_NO_ENCRYPTION_FLAGS)
            {
                if (p_db->bis_sync_info.cp_big_sync == BASS_CP_BIS_SYNC_NO_PREFER)
                {
                    if (p_db->p_pending_cp_data)
                    {
                        T_BASS_SET_PREFER_BIS_SYNC get_prefer;
                        get_prefer.source_id = p_db->brs_data.source_id;
                        get_prefer.handle = p_db->handle;
                        if (p_db->p_pending_cp_data->cp_op == BASS_CP_OP_ADD_SOURCE)
                        {
                            get_prefer.num_subgroups = p_db->p_pending_cp_data->param.add_source.num_subgroups;
                            get_prefer.p_cp_bis_info = p_db->p_pending_cp_data->param.add_source.p_cp_bis_info;
                        }
                        else if (p_db->p_pending_cp_data->cp_op == BASS_CP_OP_MODIFY_SOURCE)
                        {
                            get_prefer.num_subgroups = p_db->p_pending_cp_data->param.modify_source.num_subgroups;
                            get_prefer.p_cp_bis_info = p_db->p_pending_cp_data->param.modify_source.p_cp_bis_info;
                        }
                        ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_GET_PREFER_BIS_SYNC, &get_prefer);
                    }
                    if (p_db->bis_sync_info.cp_big_sync != BASS_CP_BIS_SYNC_NO_PREFER)
                    {
                        if (p_db->p_pending_cp_data)
                        {
                            ble_audio_mem_free(p_db->p_pending_cp_data);
                            p_db->p_pending_cp_data = NULL;
                        }
                    }

                    if (p_db->bis_sync_info.cp_big_sync == BASS_CP_BIS_SYNC_NO_PREFER)
                    {
                        return false;
                    }
                    else if (p_db->bis_sync_info.cp_big_sync == 0)
                    {
                        bass_big_proc_sm_execute(p_db, BASS_BIG_CP_NO_SYNC_EVT, NULL, true);
                        return false;
                    }
                }
                T_BASS_GET_BIG_SYNC_PARAM sync_param;
                sync_param.source_id = p_db->brs_data.source_id;
                sync_param.handle = p_db->handle;
                ble_audio_mgr_dispatch(LE_AUDIO_MSG_BASS_GET_BIG_SYNC_PARAM, &sync_param);
                bass_update_bis_idx(&p_db->big_sync_param, p_db->bis_sync_info.cp_big_sync);
                if (ble_audio_big_sync_establish_int(p_db->handle, &p_db->big_sync_param,
                                                     BLE_AUDIO_ACTION_ROLE_BASS))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

#endif

