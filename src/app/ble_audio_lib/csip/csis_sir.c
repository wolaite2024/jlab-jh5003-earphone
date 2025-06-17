#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "aes_cmac_api.h"
#include "string.h"
#include "csis_sir.h"
#include "ble_audio_ual.h"

#if (LE_AUDIO_CSIS_CLIENT_SUPPORT|LE_AUDIO_CSIS_SUPPORT)
typedef struct
{
    uint64_t a, b;
} csis_u128;

static inline void csis_u128_xor(const uint8_t p[16], const uint8_t q[16],
                                 uint8_t r[16])
{
    csis_u128 pp, qq, rr;
    memcpy(&pp, p, 16);
    memcpy(&qq, q, 16);
    rr.a = pp.a ^ qq.a;
    rr.b = pp.b ^ qq.b;
    memcpy(r, &rr, 16);
}

void csis_crypto_s1(uint8_t *M, uint8_t len, uint8_t res[16])
{
    uint8_t zero[16] = {0};

    aes_cmac128_lsb(zero, M, len, res);
}

void csis_crypto_k1(uint8_t *N, uint8_t N_len, uint8_t salt[16],
                    uint8_t *P, uint8_t p_len, uint8_t res[16])
{
    uint8_t T[16];
    aes_cmac128_lsb(salt, N, N_len, T);
    aes_cmac128_lsb(T, P, p_len, res);
}

#if LE_AUDIO_CSIS_SUPPORT
bool csis_server_generate_le_k(uint8_t *remote_bd, uint8_t remote_bd_type, uint16_t conn_handle,
                               uint8_t res[16])
{
    uint8_t key_len;
    if (ble_audio_ual_get_le_ltk(remote_bd, remote_bd_type, conn_handle, false, &key_len, res))
    {
        return true;
    }
    return false;
}

void csis_crypto_sef(uint8_t K[16], uint8_t *p_sirk, uint8_t res[16])
{
    uint8_t val[16];
    uint8_t salt[16];
    uint8_t m[7] = {'c', 'n', 'e', 'K', 'R', 'I', 'S'};
    csis_crypto_s1(m, 7, salt);
    m[0] = 's';
    m[1] = 'i';
    m[2] = 's';
    m[3] = 'c';
    csis_crypto_k1(K, 16, salt, m, 4, val);
    csis_u128_xor(val, p_sirk, res);
}
#endif

#if LE_AUDIO_CSIS_CLIENT_SUPPORT
bool csis_client_generate_le_k(uint8_t *remote_bd, uint8_t remote_bd_type, uint16_t conn_handle,
                               uint8_t res[16])
{
    uint8_t key_len;
    if (ble_audio_ual_get_le_ltk(remote_bd, remote_bd_type, conn_handle, true, &key_len, res))
    {
        return true;
    }
    return false;
}

void csis_crypto_sdf(uint8_t K[16], uint8_t *p_enc_sirk, uint8_t res[16])
{
    uint8_t val[16];
    uint8_t salt[16];
    uint8_t m[7] = {'c', 'n', 'e', 'K', 'R', 'I', 'S'};
    csis_crypto_s1(m, 7, salt);
    m[0] = 's';
    m[1] = 'i';
    m[2] = 's';
    m[3] = 'c';
    csis_crypto_k1(K, 16, salt, m, 4, val);
    csis_u128_xor(val, p_enc_sirk, res);
}
#endif
#endif

