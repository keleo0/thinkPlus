#ifndef __LOL_HASH_H__
#define __LOL_HASH_H__

#include "lol_os.h"

#ifdef    __cplusplus
extern "C" {
#endif

uint32_t lol_hash(const void *key, size_t length, const uint32_t initval);

#ifdef    __cplusplus
}
#endif

#endif    /* __LOL_HASH_H__ */

