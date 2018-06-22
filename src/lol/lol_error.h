/*
 * lol_error.h: LOL Error Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_ERROR_H__
#define __LOL_ERROR_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __LOL_UNIX__
#define lol_strerror strerror
#define lol_errno errno
#define lol_socketerrno errno
#else
const char *lol_strerror(int e);
#define lol_errno GetLastError()
#define lol_socketerrno WSAGetLastError()
#endif	/* __LOL_UNIX__ */
#include <errno.h>

#define LOL_ERROR_ERRMSG_SIZE	1024
extern int __lol_errno__;
extern char __lol_errmsg__[LOL_ERROR_ERRMSG_SIZE];

void lol_error(unsigned int e,const char *fmt,...);
void lol_showerror(void);

#define lol_errorerror() lol_error(__lol_errno__,"[%s]:%s",__func__,__lol_errmsg__)

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_ERROR_H__ */
