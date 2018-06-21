/*
 * lol_mutex.h: LOL Mutex Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(enigma1983@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_MUTEX_H__
#define __LOL_MUTEX_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define LOL_MUTEX_CREAT		0x80000000
#define LOL_MUTEX_TRUNC		0x40000000
#define LOL_MUTEX_EXCL		0x20000000
#define LOL_MUTEX_RUSR		0x00000100		/*0400(r)*/
#define LOL_MUTEX_WUSR		0x00000080		/*0200(w)*/
#define LOL_MUTEX_XUSR		0x00000040		/*0100(x)*/
#define LOL_MUTEX_RGRP		0x00000020		/*0040(r)*/
#define LOL_MUTEX_WGRP		0x00000010		/*0020(w)*/
#define LOL_MUTEX_XGRP		0x00000008		/*0010(x)*/
#define LOL_MUTEX_ROTH		0x00000004		/*0004(r)*/
#define LOL_MUTEX_WOTH		0x00000002		/*0002(w)*/
#define LOL_MUTEX_XOTH		0x00000001		/*0001(x)*/

#define LOL_MUTEX_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

struct __lol_mutex{
#ifdef __LOL_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int semid;
#endif
	int flags;
};
typedef struct __lol_mutex LOL_MUTEX;

LOL_MUTEX *lol_mutexopen(const char *name,int flags);
int lol_mutexlock(LOL_MUTEX *mutex,int timeout);
int lol_mutexunlock(LOL_MUTEX *mutex);
int lol_mutexclose(LOL_MUTEX *mutex);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_MUTEX_H__ */
