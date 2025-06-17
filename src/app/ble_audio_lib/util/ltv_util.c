#include <string.h>
#include "trace.h"
#include "ltv_util.h"

#if (LE_AUDIO_BASE_DATA_GENERATE || LE_AUDIO_BASE_DATA_PARSE)
bool ltv_util_check_format_int(const char *p_func_name, uint16_t len, uint8_t *p_data)
{
    uint8_t error_idx = 0;
    uint16_t data_len = 0;
    uint16_t idx = 0;
    uint16_t length;
    if (p_data == NULL || len == 0)
    {
        error_idx = 1;
        goto error;
    }
    for (; data_len < len;)
    {
        length = p_data[idx];
        idx += (length + 1);
        data_len += (length + 1);
    }

    if (data_len != len)
    {
        error_idx = 2;
        goto error;
    }
    return true;
error:
    PROTOCOL_PRINT_ERROR2("ltv_util_check_format: %s failed, error_idx %d",
                          TRACE_STRING(p_func_name), error_idx);
    return false;
}
#endif
