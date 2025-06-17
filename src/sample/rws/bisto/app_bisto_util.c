#if BISTO_FEATURE_SUPPORT
#include "app_bisto_util.h"
#include "bisto_api.h"


void app_bisto_util_init(void)
{
    BISTO_UTIL_CFG bisto_util_cfg = {.ram_type = OS_MEM_TYPE_DATA};
    bisto_utilities_init(&bisto_util_cfg);
}

#endif

