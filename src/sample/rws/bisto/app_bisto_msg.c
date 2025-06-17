#if BISTO_FEATURE_SUPPORT

#include "app_bisto_msg.h"
#include "app_bisto_audio.h"
#include "app_bisto_ota.h"
#include "trace.h"




bool app_bisto_msg_send(IO_BISTO_MSG_TYPE subtype, void *param_buf)
{
    T_IO_MSG msg;

    msg.type = IO_MSG_TYPE_BISTO;
    msg.subtype = subtype;
    msg.u.buf = param_buf;

    APP_PRINT_TRACE1("app_bisto_msg_send: subtype %d", subtype);

    return app_io_msg_send(msg);
}



bool app_bisto_msg_handle(T_IO_MSG io_msg)
{
    IO_BISTO_MSG_TYPE sub_type = (IO_BISTO_MSG_TYPE)io_msg.subtype;

    APP_PRINT_TRACE1("app_bisto_msg_handle: sub_type %d", sub_type);

    switch (sub_type)
    {
    case IO_BISTO_MIC_START_MSG:
        app_bisto_audio_mic_start();
        break;

    case IO_BISTO_MIC_STOP_MSG:
        app_bisto_audio_mic_close();
        break;

    case IO_BISTO_OTA_DATA_UPDATE_MSG:
        app_bisto_ota_data_update(io_msg.u.buf);
        break;

    case IO_BISTO_OTA_BEGIN_MSG:
        app_bisto_ota_begin(NULL, 0);
        break;


    default:
        break;
    }

    return true;
}




#endif
