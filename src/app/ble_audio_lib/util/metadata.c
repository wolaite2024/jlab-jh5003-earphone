#include <string.h>
#include "trace.h"
#include "metadata_def.h"
#include "ascs_def.h"
#include "bt_types.h"

#if (LE_AUDIO_ASCS_SUPPORT | LE_AUDIO_ASCS_CLIENT_SUPPORT)
bool metadata_check_format(uint16_t len, uint8_t *p_data, uint8_t *p_resp_code, uint8_t *p_reason,
                           uint16_t *p_audio_contexts)
{
    uint16_t idx = 0;
    uint16_t length;
    uint8_t type;

    if (p_resp_code == NULL || p_reason == NULL)
    {
        goto error;
    }
    if (p_data == NULL || len == 0)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_METADATA;
        goto error;
    }
    *p_resp_code = 0;
    *p_reason = 0;
    for (; idx < len;)
    {
        length = p_data[idx];
        idx++;
        type = p_data[idx];

        switch (type)
        {
        case METADATA_TYPE_PREFERRED_AUDIO_CONTEXTS:
            {
                if (length != 3)
                {
                    *p_resp_code = ASE_CP_RESP_INVALID_METADATA;
                    *p_reason = type;
                    goto error;
                }

            }
            break;

        case METADATA_TYPE_STREAMING_AUDIO_CONTEXTS:
            {
                uint16_t audio_contexts;
                if (length != 3)
                {
                    *p_resp_code = ASE_CP_RESP_INVALID_METADATA;
                    *p_reason = type;
                    goto error;
                }
                LE_ARRAY_TO_UINT16(audio_contexts, &p_data[idx + 1]);
                if (((audio_contexts | AUDIO_CONTEXT_MASK) != AUDIO_CONTEXT_MASK) ||
                    ((audio_contexts & AUDIO_CONTEXT_MASK) == AUDIO_CONTEXT_PROHIBITED))
                {
                    *p_resp_code = ASE_CP_RESP_REJECTED_METADATA;
                    *p_reason = type;
                    goto error;
                }
                *p_audio_contexts = audio_contexts;
            }
            break;

        case METADATA_TYPE_VENDOR_SPECIFIC:
            {
                if (length < 4)
                {
                    *p_resp_code = ASE_CP_RESP_INVALID_METADATA;
                    *p_reason = type;
                    goto error;
                }
            }
            break;

        case METADATA_TYPE_CCID_LIST:
            {
                if (length < 1)
                {
                    *p_resp_code = ASE_CP_RESP_INVALID_METADATA;
                    *p_reason = type;
                    goto error;
                }
            }
            break;

        default:
            {
                *p_resp_code = ASE_CP_RESP_UNSUPPORTED_METADATA;
                *p_reason = type;
                goto error;
            }
        }
        idx += length;
    }

    if (idx != len)
    {
        *p_resp_code = ASE_CP_RESP_INVALID_METADATA;
        goto error;
    }

    return true;
error:
    if (p_resp_code != NULL && p_reason != NULL)
    {
        PROTOCOL_PRINT_ERROR2("metadata_check_format: failed, resp_code 0x%x, reason 0x%x",
                              *p_resp_code, *p_reason);
    }

    return false;
}
#endif
