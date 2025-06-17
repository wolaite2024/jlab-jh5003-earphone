#if BISTO_FEATURE_SUPPORT

#include "app_bisto_info.h"
#include "bisto_api.h"

#include <string.h>


#define SERIAL_NUM_STRING "1234"
#define PORT_VERSION_STRING "5678"
#define APP_VERSION_STRING "5678"


void app_bisto_info_init(void)
{
    /* information module init */
    BISTO_INFO_STRS bisto_info_strs = {NULL};

    memcpy(bisto_info_strs.serial_num_info.serial_num, SERIAL_NUM_STRING, sizeof(SERIAL_NUM_STRING));
    bisto_info_strs.serial_num_info.length = sizeof(SERIAL_NUM_STRING);


    memcpy(bisto_info_strs.port_version_info.port_version, PORT_VERSION_STRING,
           sizeof(PORT_VERSION_STRING));
    bisto_info_strs.port_version_info.length = sizeof(PORT_VERSION_STRING);


    memcpy(bisto_info_strs.app_version_info.app_version, APP_VERSION_STRING,
           sizeof(APP_VERSION_STRING));
    bisto_info_strs.app_version_info.length = sizeof(APP_VERSION_STRING);


    bisto_info_init(&bisto_info_strs);
}




#endif



