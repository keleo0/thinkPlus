/*
 * lol_os.h: LOL OS Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_OS_H__
#define __LOL_OS_H__

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* Softwares */

#ifdef _AIX	/* IBM AIX OS */
#define __LOL_AIX__
#endif

#ifdef __hpux	/* HP HP-UX OS */
#define __LOL_HPUX__
#endif

#ifdef __SVR4	/* Sun Solaris OS */
#define __LOL_SOLARIS__
#endif

#ifdef __FreeBSD__	/* Berkeley FreeBSD OS */
#define __LOL_FREEBSD__
#endif

#ifdef __linux	/* GNU Linux OS */
#define __LOL_LINUX__
#endif

#ifdef __APPLE__	/* Apple Mac OS X */
#define __LOL_APPLE__
#endif

#ifdef _WIN32	/* Microsoft Windows OS */
#define __LOL_WINDOWS__
#endif

/* Hardwares */
#ifdef __sparc	/* Sun Sparc Machine */
#define __LOL_SPARC__
#endif

#ifdef __ppc__ /* IBM PowerPC */
#define __LOL_POWERPC__
#endif

#if defined(__LOL_AIX__) || defined(__LOL_HPUX__) || defined(__LOL_SOLARIS__) || defined(__LOL_FREEBSD__) || defined(__LOL_LINUX__) || defined(__LOL_APPLE__)
#define __LOL_UNIX__
#endif

#if defined(__LOL_WINDOWS__)	/* windows */

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef _WIN32
#pragma warning (disable:4786)
#pragma warning (disable:4018)
#pragma warning (disable:4761)
#pragma warning (disable:4244)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#define WIN32_LEAN_AND_MEAN	/* for socket api */
#include <windows.h>
#include <process.h>
#include <direct.h>
#include <io.h>
#include <winsock2.h>

typedef char				LOL_CHAR;
typedef unsigned char		LOL_UCHAR;
typedef short				LOL_SHORT;
typedef unsigned short		LOL_USHORT;
typedef int					LOL_INT;
typedef unsigned int		LOL_UINT;
typedef __int64				LOL_LONG;
typedef unsigned __int64	LOL_ULONG;
typedef float				LOL_FLOAT;
typedef double				LOL_DOUBLE;

#define __func__ "__func__"
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define bzero(address,size) memset(address,0x00,size)

#elif defined(__LOL_UNIX__)	/* unix */

/* hp-ux */
#ifdef __LOL_HPUX__
#define _XOPEN_SOURCE_EXTENDED 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>

#include <strings.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <poll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

typedef char				LOL_CHAR;
typedef unsigned char		LOL_UCHAR;
typedef short				LOL_SHORT;
typedef unsigned short		LOL_USHORT;
typedef int					LOL_INT;
typedef unsigned int		LOL_UINT;
typedef long long			LOL_LONG;
typedef unsigned long long	LOL_ULONG;
typedef float				LOL_FLOAT;
typedef double				LOL_DOUBLE;

#else	/* Unknown OS */
#error Operation System not supported!
#endif	/* __LOL_WINDOWS__ */

/* export api */
#if defined(__LOL_WINDOWS__)
#if LOL_DLLEXPORT
#define LOL_API __declspec(dllexport)
#else
#define LOL_API __declspec(dllimport)
#endif
#else
#if LOL_DLLEXPORT
#define LOL_API __attribute__((visibility("default")))
#else
#define LOL_API
#endif
#endif

#ifdef __GNUC__
#define LOL_DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define LOL_DEPRECATED __declspec(deprecated)
#else
#define LOL_DEPRECATED
#endif



/* byte order */

#if defined(__LOL_SPARC__) || defined(__LOL_AIX__) || defined(__LOL_HPUX__) || defined(__LOL_POWERPC__)
#define __LOL_BIG_ENDIAN__ 1
#else
#define __LOL_LITTLE_ENDIAN__ 1
#endif

#ifdef __LOL_LITTLE_ENDIAN__
#define lol_htol2(s) (s)
#define lol_htol4(i) (i)
#define lol_htol8(l) (l)
#define lol_ltoh2(s) (s)
#define lol_ltoh4(i) (i)
#define lol_ltoh8(l) (l)
#else
#define lol_htol2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define lol_htol4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
		+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define lol_htol8(l) ((((l)&0x00000000000000FFULL)<<56)+(((l)&0xFF00000000000000ULL)>>56)\
		+(((l)&0x000000000000FF00ULL)<<40)+(((l)&0x00FF000000000000ULL)>>40)\
		+(((l)&0x0000000000FF0000ULL)<<24)+(((l)&0x0000FF0000000000ULL)>>24)\
		+(((l)&0x00000000FF000000ULL)<<8)+(((l)&0x0000000FF0000000ULL)>>8))
#define lol_ltoh2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define lol_ltoh4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
		+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define lol_ltoh8(l) ((((l)&0x00000000000000FFULL)<<56)+(((l)&0xFF00000000000000ULL)>>56)\
		+(((l)&0x000000000000FF00ULL)<<40)+(((l)&0x00FF000000000000ULL)>>40)\
		+(((l)&0x0000000000FF0000ULL)<<24)+(((l)&0x0000FF0000000000ULL)>>24)\
		+(((l)&0x00000000FF000000ULL)<<8)+(((l)&0x0000000FF0000000ULL)>>8))
#endif

#ifdef __LOL_BIG_ENDIAN__
#define lol_htob2(s) (s)
#define lol_htob4(i) (i)
#define lol_htob8(l) (l)
#define lol_btoh2(s) (s)
#define lol_btoh4(i) (i)
#define lol_btoh8(l) (l)
#else
#define lol_htob2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define lol_htob4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
		+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define lol_htob8(l) ((((l)&0x00000000000000FFULL)<<56)+(((l)&0xFF00000000000000ULL)>>56)\
		+(((l)&0x000000000000FF00ULL)<<40)+(((l)&0x00FF000000000000ULL)>>40)\
		+(((l)&0x0000000000FF0000ULL)<<24)+(((l)&0x0000FF0000000000ULL)>>24)\
		+(((l)&0x00000000FF000000ULL)<<8)+(((l)&0x0000000FF0000000ULL)>>8))
#define lol_btoh2(s) ((((s)&0x00FFU)<<8)+(((s)&0xFF00U)>>8))
#define lol_btoh4(i) ((((i)&0x000000FFU)<<24)+(((i)&0xFF000000U)>>24)\
		+(((i)&0x0000FF00U)<<8)+(((i)&0x00FF0000U)>>8))
#define lol_btoh8(l) ((((l)&0x00000000000000FFULL)<<56)+(((l)&0xFF00000000000000ULL)>>56)\
		+(((l)&0x000000000000FF00ULL)<<40)+(((l)&0x00FF000000000000ULL)>>40)\
		+(((l)&0x0000000000FF0000ULL)<<24)+(((l)&0x0000FF0000000000ULL)>>24)\
		+(((l)&0x00000000FF000000ULL)<<8)+(((l)&0x0000000FF0000000ULL)>>8))
#endif

/* C++ */
#ifdef __cplusplus
}
#endif

#endif /* __LOL_OS_H__ */
