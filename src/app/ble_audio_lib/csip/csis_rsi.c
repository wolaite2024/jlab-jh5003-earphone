#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "aes_api.h"
#include "csis_rsi.h"
#include "platform_utils.h"
#include "csis_def.h"

#if (LE_AUDIO_CSIS_CLIENT_SUPPORT|LE_AUDIO_CSIS_SUPPORT)

bool csis_gen_rsi(const uint8_t *p_sirk, uint8_t *p_rsi)
{
    uint8_t *p = p_rsi;
    bool     ret = true;
    uint8_t rand[16] = {0};
    uint8_t out[16] = {0};
    uint8_t rand_all_zeros[3] = {0x00, 0x00, 0x40};
    uint8_t rand_all_ones[3] = {0xFF, 0xFF, 0x7F};

    if (p_sirk == NULL || p_rsi == NULL)
    {
        return false;
    }

    LE_UINT32_TO_STREAM(p, platform_random(0xFFFFFFFF));
    LE_UINT16_TO_STREAM(p, platform_random(0xFFFFFFFF));

    p_rsi[5] &= 0x7F;
    p_rsi[5] |= 0x40;

    if ((memcmp(p_rsi, rand_all_zeros, 3) == 0) ||
        (memcmp(p_rsi, rand_all_ones, 3) == 0))
    {
        p_rsi[3] = 0x88;
    }
#if 0
    // ONLY for test purpose, sample data here
    /**
    SIRK    838e6805 53f1415a a265bbaf c6ea03b8
    prand   00000000 00000000 00000000 0069f563
    M       00000000 00000000 00000000 0069f563
    AES_128 ad169cc8 43b9d503 dc07661d 72b3337c
    sih     b3337c

      */
    uint8_t sirk_data[CSIS_SIRK_LEN] = {0xb8, 0x03, 0xea, 0xc6, 0xaf, 0xbb, 0x65, 0xa2, 0x5a, 0x41, 0xf1,
                                        0x53, 0x05, 0x68, 0x8e, 0x83
                                       };
    p_psri[3] = 0x63;
    p_psri[4] = 0xf5;
    p_psri[5] = 0x69;
    memcpy(p_sirk, sirk_data, CSIS_SIRK_LEN);
#endif

    rand[0] = p_rsi[3];
    rand[1] = p_rsi[4];
    rand[2] = p_rsi[5];

    aes128_ecb_encrypt_msb2lsb(rand, p_sirk, out);

    memcpy(p_rsi, out, 3);

    PROTOCOL_PRINT_INFO2("[CSIS] csis_gen_rsi: SIRK %b, PSRI %b",
                         TRACE_BINARY(CSIS_SIRK_LEN, p_sirk), TRACE_BINARY(CSI_RSI_LEN, p_rsi));

    return ret;
}
#endif

#if LE_AUDIO_CSIS_CLIENT_SUPPORT
bool csis_check_rsi(const uint8_t *p_sirk, uint8_t *p_rsi)
{
    uint8_t buffer[CSIS_SIRK_LEN];
    uint8_t encrypt_buffer[CSIS_SIRK_LEN];

    if (p_sirk == NULL || p_rsi == NULL)
    {
        return false;
    }

    PROTOCOL_PRINT_INFO2("[CSIS] csis_check_rsi: SIRK %b, PSRI %b",
                         TRACE_BINARY(CSIS_SIRK_LEN, p_sirk), TRACE_BINARY(CSI_RSI_LEN, p_rsi));

    memset(buffer, 0x00, CSIS_SIRK_LEN);
    buffer[0] = p_rsi[3];
    buffer[1] = p_rsi[4];
    buffer[2] = p_rsi[5];

    aes128_ecb_encrypt_msb2lsb(buffer, p_sirk, encrypt_buffer);

    if ((encrypt_buffer[2] == p_rsi[2])
        && (encrypt_buffer[1] == p_rsi[1])
        && (encrypt_buffer[0] == p_rsi[0]))
    {
        PROTOCOL_PRINT_INFO1("[CSIS] csis_check_rsi: hash %b, match",
                             TRACE_BINARY(3, &encrypt_buffer[0]));
        return true;
    }
    else
    {
        PROTOCOL_PRINT_INFO0("[CSIS] csis_check_rsi: not match");
        return false;
    }
}
#endif

