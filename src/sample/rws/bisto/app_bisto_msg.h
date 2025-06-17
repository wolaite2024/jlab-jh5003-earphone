#ifndef APP_BISTO_MSG_H
#define APP_BISTO_MSG_H


#include <stdbool.h>
#include "app_io_msg.h"
#include "app_main.h"
#include "os_msg.h"


typedef enum
{
    IO_BISTO_MIC_START_MSG,
    IO_BISTO_MIC_STOP_MSG,
    IO_BISTO_OTA_BEGIN_MSG,
    IO_BISTO_OTA_DATA_UPDATE_MSG,
    IO_BISTO_OTA_STATE_ENTER_MSG,
} IO_BISTO_MSG_TYPE;





bool app_bisto_msg_send(IO_BISTO_MSG_TYPE subtype, void *param_buf);


bool app_bisto_msg_handle(T_IO_MSG io_msg);




#endif

