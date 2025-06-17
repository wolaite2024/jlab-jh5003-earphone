#ifndef _XM_AUTH_H_
#define _XM_AUTH_H_


#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <stdlib.h>

#define AUTH_SUCCESS 0
#define AUTH_FAIL 1


int get_random_auth_data(unsigned char *result);

int set_link_key(const unsigned char *key);

int get_encrypted_auth_data(unsigned char *random, unsigned char *result);

void *xm_malloc(size_t __size);

void  xm_free(void *);

int xm_rand(void);

void   xm_srand(void);

void   *xm_memcpy(void *str1, const void *str2, size_t n);

void *xm_memset(void *str, int c, size_t n);

int xm_log(const char *format, ...);


#ifdef __cplusplus
}
#endif

#endif  /* _XM_AUTH_H_ */

