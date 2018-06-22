/*
 * lol_sem.h: LOL Semaphore Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_SEM_H__
#define __LOL_SEM_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define LOL_SEM_CREAT		0x80000000
#define LOL_SEM_TRUNC		0x40000000
#define LOL_SEM_EXCL		0x20000000
#define LOL_SEM_UNDO		0x10000000
#define LOL_SEM_RUSR		0x00000100		/*0400(r)*/
#define LOL_SEM_WUSR		0x00000080		/*0200(w)*/
#define LOL_SEM_XUSR		0x00000040		/*0100(x)*/
#define LOL_SEM_RGRP		0x00000020		/*0040(r)*/
#define LOL_SEM_WGRP		0x00000010		/*0020(w)*/
#define LOL_SEM_XGRP		0x00000008		/*0010(x)*/
#define LOL_SEM_ROTH		0x00000004		/*0004(r)*/
#define LOL_SEM_WOTH		0x00000002		/*0002(w)*/
#define LOL_SEM_XOTH		0x00000001		/*0001(x)*/

#define LOL_SEM_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

struct __lol_sem{
#ifdef __LOL_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int semid;
#endif
	int flags;
};
typedef struct __lol_sem LOL_SEM;

#ifdef __LOL_UNIX__ /* UNIX */
struct __lol_semun{
	int val;
	struct semid_ds *buf;
	ushort *array;
#ifdef __LOL_LINUX__ /* linux */
	struct seminfo *__buf;
#endif /* __LOL_LINUX__ */
};
typedef struct __lol_semun LOL_SEMUN;
#endif /* __LOL_UNIX__ */

LOL_SEM *lol_semopen(const char *name,int flags);
int lol_semget(LOL_SEM *sem,int timeout);
int lol_semput(LOL_SEM *sem);
int lol_semclose(LOL_SEM *sem);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_SEM_H__ */
