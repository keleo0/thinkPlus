/*
 * lol_shmq.h: LOL Share Memory Queue Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_SHMQ_H__
#define __LOL_SHMQ_H__

#include "lol_os.h"
#include "lol_shm.h"
#include "lol_sem.h"
#include "lol_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* open mode */
#define LOL_SHMQ_CREAT		0x80000000
#define LOL_SHMQ_EXCL			0x40000000
#define LOL_SHMQ_TRUNC		0x20000000
#define LOL_SHMQ_RDONLY		0x10000000
#define LOL_SHMQ_RUSR			0x00000100		/*0400(r)*/
#define LOL_SHMQ_WUSR			0x00000080		/*0200(w)*/
#define LOL_SHMQ_XUSR			0x00000040		/*0100(x)*/
#define LOL_SHMQ_RGRP			0x00000020		/*0040(r)*/
#define LOL_SHMQ_WGRP			0x00000010		/*0020(w)*/
#define LOL_SHMQ_XGRP			0x00000008		/*0010(x)*/
#define LOL_SHMQ_ROTH			0x00000004		/*0004(r)*/
#define LOL_SHMQ_WOTH			0x00000002		/*0002(w)*/
#define LOL_SHMQ_XOTH			0x00000001		/*0001(x)*/

#define LOL_SHMQ_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

typedef struct {
	int size;
	int totalsize;
	int freesize;
	int head;
	int tail;
	int msgnum;
}LOL_SHMQHEAD;

typedef struct {
	LOL_SHM *shm;
	LOL_SEM *sem;
	LOL_MUTEX *mutex;
	int flags;
	LOL_SHMQHEAD *head;
	void *data;
}LOL_SHMQ;

LOL_SHMQ *lol_shmqopen(const char *name,int size,int flags);
int lol_shmqget(LOL_SHMQ *shmq,void *buf,int size,int timeout);
int lol_shmqput(LOL_SHMQ *shmq,const void *buf,int len,int timeout);
int lol_shmqclose(LOL_SHMQ *shmq);

#ifdef __cplusplus
}
#endif

#endif /* __LOL_SHMQ_H__ */
