/*
 * lol_threadmutex.h: LOL Thread Mutex Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_THREADMUTEX_H__
#define __LOL_THREADMUTEX_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

struct __lol_threadmutex{
#ifdef __LOL_WINDOWS__ /* Windows */
		HANDLE handle;
#else /* UNIX */
		pthread_mutex_t *threadmutex;
#endif
};
typedef struct __lol_threadmutex LOL_THREADMUTEX;


LOL_THREADMUTEX *lol_threadmutex_open();
int lol_threadmutex_lock(LOL_THREADMUTEX *threadmutex,int timeout);
int lol_threadmutex_unlock(LOL_THREADMUTEX *threadmutex);
int lol_threadmutex_close(LOL_THREADMUTEX *threadmutex);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_THREADMUTEX_H__ */
