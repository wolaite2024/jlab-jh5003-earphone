/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_findmy_task.c
   * @brief     Routines to create App findmy task and handle events & messages
   * @author    cen
   * @date      2022-11-07
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#if F_APP_FINDMY_FEATURE_SUPPORT
#include <stdio.h>
#include <string.h>
#include "platform_os.h"
#include "os_msg.h"
#include "os_task.h"
#include "gap.h"
#include "gap_le.h"
#include "app_findmy_task.h"
#include "app_msg.h"
#include "app_findmy_task.h"
#include "app_findmy.h"
#include "trace.h"
#include "fmna_state_machine.h"
#include "app_io_msg.h"


/** @defgroup  PERIPH_APP_TASK Peripheral App Task
    * @brief This file handles the implementation of application task related functions.
    *
    * Create App task and handle events & messages
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
#define APP_TASK_PRIORITY             1         //!< Task priorities
#define APP_TASK_STACK_SIZE           256 * 32  //!<  Task stack size
#define MAX_NUMBER_OF_GAP_MESSAGE     0x20      //!<  GAP message queue size
#define MAX_NUMBER_OF_IO_MESSAGE      0x20      //!<  IO message queue size
#define MAX_NUMBER_OF_EVENT_MESSAGE   (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE)    //!< Event message queue size

/*============================================================================*
 *                              Variables
 *============================================================================*/


extern void *audio_evt_queue_handle;
extern void *audio_io_queue_handle;

void *app_findmy_queue_handle;


/*============================================================================*
 *                              Functions
 *============================================================================*/

uint32_t app_findmy_sched_event_put(void const *p_event_data,
                                    uint16_t event_size,
                                    app_sched_event_handler_t handler)
{
    uint8_t event = EVENT_IO_TO_APP;
    T_IO_MSG msg;

    msg.type = IO_MSG_TYPE_FINDMY;
    msg.subtype = IO_MSG_FINDMY_EVENT;

    T_APP_SCHED_EVT *evt_context = os_mem_alloc(OS_MEM_TYPE_DATA, sizeof(T_APP_SCHED_EVT));
    evt_context->event_size = event_size;

    if (event_size != 0)
    {
        evt_context->p_event_data = os_mem_alloc(OS_MEM_TYPE_DATA, event_size);
        memcpy(evt_context->p_event_data, p_event_data, event_size);
    }
    else
    {
        evt_context->p_event_data = NULL;
    }

    if (handler != NULL)
    {
        evt_context->handler = handler;
    }
    else
    {
        evt_context->handler = NULL;
    }
    msg.u.buf = (void *)evt_context;
    FMNA_LOG_INFO("app_findmy_sched_event_put: 0x%08x, 0x%08x, audio_io_queue_handle %p, audio_evt_queue_handle %p",
                  msg.u.param, (uint32_t)&evt_context, audio_io_queue_handle, audio_evt_queue_handle);

    return app_io_msg_send(&msg);
}
#endif

/** @} */ /* End of group PERIPH_APP_TASK */


