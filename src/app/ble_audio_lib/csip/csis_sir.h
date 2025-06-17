#ifndef _CSIS_SIR_H_
#define _CSIS_SIR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include <stdint.h>
#include <stdbool.h>

#if (LE_AUDIO_CSIS_CLIENT_SUPPORT|LE_AUDIO_CSIS_SUPPORT)
void csis_crypto_s1(uint8_t *M, uint8_t len, uint8_t res[16]);
void csis_crypto_sef(uint8_t K[16], uint8_t *p_sirk, uint8_t res[16]);
void csis_crypto_k1(uint8_t *N, uint8_t N_len, uint8_t salt[16],
                    uint8_t *P, uint8_t p_len, uint8_t res[16]);
void csis_crypto_sdf(uint8_t K[16], uint8_t *p_enc_sirk, uint8_t res[16]);
bool csis_client_generate_le_k(uint8_t *remote_bd, uint8_t remote_bd_type, uint16_t conn_handle,
                               uint8_t res[16]);
bool csis_server_generate_le_k(uint8_t *remote_bd, uint8_t remote_bd_type, uint16_t conn_handle,
                               uint8_t res[16]);
//bool csis_generate_bredr_k(uint8_t *remote_bd, uint8_t res[16]);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
