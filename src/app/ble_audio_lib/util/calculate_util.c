#include "calculate_util.h"

#if LE_AUDIO_ASCS_SUPPORT
uint32_t least_common_multiple(uint32_t value1, uint32_t value2)
{
    if (value1 != 0 && value2 != 0)
    {
        uint32_t i = 1;
        while ((value1 * i % value2) != 0)
        {
            i++;
        }
        return (value1 * i);
    }
    return 0;
}
#endif
