/*
 * lol_thread.h: LOL Thread Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(enigma1983@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_THREAD_H__
#define __LOL_THREAD_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define LOL_THREAD_SUSPEND	0x10000000 /* suspend when created */

struct __lol_thread{
#ifdef __LOL_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	pthread_t thread;
#endif
	int threadid;
};
typedef struct __lol_thread LOL_THREAD;
	
#ifdef __LOL_WINDOWS__ /* Windows */
typedef int (WINAPI *LOL_OS_THREADROUTINE)(void *);
int WINAPI lol_os_threadroutine(void *);
#else /* UNIX */
typedef void *(*LOL_OS_THREADROUTINE)(void *);
void *lol_os_threadroutine(void *);
#endif

/* lol thread routine and arg */
typedef void (*LOL_THREADROUTINE)(void *);
struct __lol_threadarg{
	LOL_THREADROUTINE routine;
	void *arg;
	int flags;
};
typedef struct __lol_threadarg LOL_THREADARG;
	
LOL_THREAD *lol_threadcreate(LOL_THREADROUTINE routine,void *arg,int flags);
int lol_threadid(void); /* get current thread id */
int lol_threadexit(int exitcode);
int lol_threadsuspend(LOL_THREAD *thread);
int lol_threadcontinue(LOL_THREAD *thread);
int lol_threadcancel(LOL_THREAD *thread);
int lol_threadjoin(LOL_THREAD *thread,int *exitcode,int timeout);
int lol_thread_detach(LOL_THREAD *thread);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_THREAD_H__ */
