/*
 * lol.h: LOL Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(enigma1983@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_H__
#define __LOL_H__

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* lol_os */

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

typedef char LOL_CHAR;
typedef unsigned char LOL_UCHAR;
typedef short LOL_SHORT;
typedef unsigned short LOL_USHORT;
typedef int LOL_INT;
typedef unsigned int LOL_UINT;
typedef __int64 LOL_LONG;
typedef unsigned __int64 LOL_ULONG;
typedef float LOL_FLOAT;
typedef double LOL_DOUBLE;

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

typedef char LOL_CHAR;
typedef unsigned char LOL_UCHAR;
typedef short LOL_SHORT;
typedef unsigned short LOL_USHORT;
typedef int LOL_INT;
typedef unsigned int LOL_UINT;
typedef long long LOL_LONG;
typedef unsigned long long LOL_ULONG;
typedef float LOL_FLOAT;
typedef double LOL_DOUBLE;

#else	/* Unknown OS */
#error Operation System not supported!
#endif	/* __LOL_WINDOWS__ */


/* byte order */

#if defined(__LOL_SPARC__) || defined(__LOL_AIX__) || defined(__LOL_HPUX__) || defined(__LOL_POWERPC__)
#define __LOL_BIG_ENDIAN__
#else
#define __LOL_LITTLE_ENDIAN__
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


/* lol_error */

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
extern unsigned int __lol_errno__;
extern char __lol_errmsg__[LOL_ERROR_ERRMSG_SIZE];

void lol_error(unsigned int e,const char *fmt,...);
void lol_showerror(void);

#define lol_errorerror() lol_error(__lol_errno__,"[%s]:%s",__func__,__lol_errmsg__)


/* lol_utility */

#ifdef __LOL_WINDOWS__
extern int opterr,optind,optopt;
extern char *optarg;
int getopt(int argc,char *argv[],const char *ostr);
#define R_OK        4       /* Test for read permission */
#define W_OK        2       /* Test for write permission */
#define X_OK        1       /* Test for execute (search) permission */
#define F_OK        0       /* Test for existence of file */
#define atoll atol
#define lol_mkdir(path,mode) mkdir(path)
#define SIGPIPE 0
#else
#define atoll(s) strtoll(s,(char **)NULL,10)
#define lol_mkdir(path,mode) mkdir(path,mode)
#endif	/* __LOL_WINDOWS__ */

int lol_bin_to_hex(char *dest,const char *src,int len);
int lol_bin_to_bcd(char *dest,const char *src,int len);
int lol_hex_to_bin(char *dest,const char *src,int len);
int lol_bcd_to_bin(char *dest,const char *src,int len);
int lol_hextobin(char *dest,const char *src);
int lol_bcdtobin(char *dest,const char *src);
int lol_bintohex(char *dest,char src);
int lol_bintobcd(char *dest,char src);
int lol_inttohex(int bin);
int lol_inttobcd(int bin);
int lol_hextoint(int hex);
int lol_bcdtoint(int hex);
int lol_printbin(const char *buf,const int len,int linesize);

char *lol_strtrim(char *str);
char *lol_strinsert(char *srcstr,int pos,char *insstr);
char *lol_chrinsert(char *srcstr,int pos,char insstr);
char *lol_strreplace(char *srcstr,char *pattern,char *repstr);
char *lol_chrreplace(char *srcstr,char pattern,char repstr);
char *lol_strdelete(char *srcstr,const char *pattern);
char *lol_chrdelete(char *srcstr,char delchr);
int lol_strfind(const char *str,const char *pattern,int index);
int lol_chrfind(const char *str,char chr,int index);
int lol_chrstat(const char *str,char ch);
int lol_strstat(const char *str,const char *pattern);

int lol_strgetfield(const char *str,char sepchr,int index,char *value,unsigned int size);

int lol_bitset(char *bitmap,int bit);
int lol_bittest(const char *bitmap,int bit);
int lol_bitclear(char *bitmap,int bit);

#define LOL_TIME_SUNDAY	0
#define LOL_TIME_MONDAY	1
#define LOL_TIME_TUESDAY	2
#define LOL_TIME_WEDNESDAY	3
#define LOL_TIME_THURSDAY	4
#define LOL_TIME_FRIDAY	5
#define LOL_TIME_SATURDAY	6

struct _lol_time {
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int week;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int msecond;
};
typedef struct _lol_time lol_time;
int lol_gettime(lol_time *t);
int lol_sleep(unsigned int t); /* mil sleep */

#define LOL_OPT_CHAR		'C'		/* 1 bytes */
#define LOL_OPT_SHORT		'S'		/* 2 bytes */
#define LOL_OPT_INT		'I'		/* 4 bytes */
#define LOL_OPT_LONG		'L'		/* 8 bytes */
#define LOL_OPT_FLOAT		'F'		/* 4 bytes */
#define LOL_OPT_DOUBLE	'D'		/* 8 bytes */
#define LOL_OPT_MASK_TYPE	0xFF		/* type mask */

#define LOL_OPT_ARG			0x0100
#define LOL_OPT_NOT_IGNORE		0x0200
#define LOL_OPT_NOT_EMPTY		0x0400

struct __lol_optarray {
	char opt;
	void *value;
	unsigned int size;
	int flags;
	int *exists;			/* 0:not exists,1:exists */
};
typedef struct __lol_optarray LOL_OPTARRAY;
int lol_getopt(int argc,char *argv[],char opt,void *value,unsigned int size,int flags);
int lol_getopts(int argc,char *argv[],LOL_OPTARRAY *optarray,unsigned int optcount);

#define LOL_ENV_CHAR		'C'		/* 1 bytes */
#define LOL_ENV_SHORT		'S'		/* 2 bytes */
#define LOL_ENV_INT		'I'		/* 4 bytes */
#define LOL_ENV_LONG		'L'		/* 8 bytes */
#define LOL_ENV_FLOAT		'F'		/* 4 bytes */
#define LOL_ENV_DOUBLE	'D'		/* 8 bytes */
#define LOL_ENV_MASK_TYPE	0xFF		/* type mask */

#define LOL_ENV_NOT_IGNORE		0x0100
#define LOL_ENV_NOT_EMPTY		0x0200

struct __lol_envarray {
	const char *name;
	void *value;
	unsigned int size;
	int flags;
	int *exists;
};
typedef struct __lol_envarray LOL_ENVARRAY;
int lol_getenv(const char *name,void *value,int size,int flags);
int lol_getenvs(LOL_ENVARRAY *envarray,unsigned int envcount);

int automkdir(const char *path,int mode);


/* lol_file */

#ifdef __LOL_WINDOWS__
#define O_RDONLY        _O_RDONLY
#define O_WRONLY        _O_WRONLY
#define O_RDWR          _O_RDWR
#define O_APPEND        _O_APPEND
#define O_CREAT         _O_CREAT
#define O_TRUNC         _O_TRUNC
#define O_EXCL          _O_EXCL
#define O_BINARY        _O_BINARY

#define lolopen _open
#define lolread _read
#define lolwrite _write
#define lollseek _lseek
#define lolclose _close
#define lolaccess _access
#else
#define O_BINARY 0
#endif	/* __LOL_WINDOWS__ */

struct __lol_file {
	int fd;	/* file descriptor */
};
typedef struct __lol_file LOL_FILE;

/* open flags */
#define LOL_FILE_CREAT 	0x01		/* creat */
#define LOL_FILE_EXCL 	0x02		/* excl */
#define LOL_FILE_TRUNC 	0x04		/* trunc */
#define LOL_FILE_RDONLY 	0x08		/* read only */
#define LOL_FILE_WRONLY 	0x10		/* write only */
#define LOL_FILE_RDWR 	0x20		/* read & write */
#define LOL_FILE_APPEND 	0x40		/* append */
#define LOL_FILE_BINARY	0x80		/* binary */

#define LOL_FILE_SEEK_SET	0
#define LOL_FILE_SEEK_CUR	1
#define LOL_FILE_SEEK_END	2

LOL_FILE *lol_fileopen(const char *pathname,int flags,int mode);
int lol_fileread(LOL_FILE *file,void *buffer,unsigned int nbytes);
int lol_filewrite(LOL_FILE *file,const void *buffer,unsigned int nbytes);
int lol_fileseek(LOL_FILE *file,int offset,int whence);
int lol_fileclose(LOL_FILE *file);


/* lol_net */

#ifdef __LOL_WINDOWS__
#define socketclose closesocket
#define socklen_t int
#else
#define socketclose close
#endif

#ifndef INADDR_NONE
#define INADDR_NONE -1
#endif

#define LOL_NET_LISTENER	0x00000001		/* listener */

#define LOL_NET_WAIT		0x00000001		/* wait */
#define LOL_NET_READ		0x00000002		/* read */
#define LOL_NET_WRITE		0x00000004		/* write */
#define LOL_NET_ERROR		0x00000008		/* error */

struct __lol_net {
	int sockfd;
	char ip[16];
	unsigned short port;
	int flags;
};
typedef struct __lol_net LOL_NET;

struct __lol_netlist {
	LOL_NET *net;
	int flags;
	struct __lol_netlist *prior;
	struct __lol_netlist *next;
};
typedef struct __lol_netlist LOL_NETLIST;

int lol_netstart(void);
int lol_netstop(void);

LOL_NET *lol_netconnect(const char *ip,unsigned short port);
LOL_NET *lol_netlisten(const char *ip,unsigned short port);
LOL_NET *lol_netaccept(LOL_NET *net);
int lol_netrecv(LOL_NET *net,void *buf,unsigned int siz,int flags);
int lol_netsend(LOL_NET *net,const void *buf,unsigned int len,int flags);
int lol_netclose(LOL_NET *net);

/* select & poll */
int lol_netlist_add(LOL_NETLIST **netlist,LOL_NET *net);
int lol_netlist_del(LOL_NETLIST **netlist,LOL_NET *net);
LOL_NETLIST *lol_netlist_find(LOL_NETLIST *netlist,LOL_NET *net);
int lol_netlist_clean(LOL_NETLIST **netlist);
int lol_netselect(LOL_NETLIST **netlist,int timeout);
#ifdef __LOL_UNIX__
int lol_netpoll(LOL_NETLIST **netlist,int timeout);
#else	/* windows */
#define lol_netpoll lol_netselect
#endif
int lol_netlist_free(LOL_NETLIST **netlist);

int lol_netsendmsg(LOL_NET *net,const void *buf,unsigned int len);
int lol_netrecvmsg(LOL_NET *net,void *buf,unsigned int siz);


/* lol_netcenter */

#define LOL_NETCENTER_BUFSIZE	8192

#define LOL_NETCENTER_ACCEPT 			0x00000001	/* accept */
#define LOL_NETCENTER_DISCONNECTED	0x00000002	/* disconnected */
#define LOL_NETCENTER_ERROR			0x00000004	/* error */
#define LOL_NETCENTER_DATARECVED		0x00000008	/* datarecved */

struct __lol_netcenter_buflist {
	char *buf;
	unsigned int pos;
	unsigned int len;
	struct __lol_netcenter_buflist *prior;
	struct __lol_netcenter_buflist *next;
};
typedef struct __lol_netcenter_buflist LOL_NETCENTER_BUFLIST;

struct __lol_netcenter_net;
typedef struct __lol_netcenter_net LOL_NETCENTER_NET;
typedef void (*LOL_NETCENTER_NETHANDLE)(LOL_NETCENTER_NET *,int);
struct __lol_netcenter_net {
	LOL_NET *net;
	LOL_NETCENTER_BUFLIST *rlist;
	LOL_NETCENTER_BUFLIST *wlist;
	unsigned int nrbytes;
	unsigned int nwbytes;
	LOL_NETCENTER_NETHANDLE nethandle;
};

struct __lol_netcenter_netlist{
	LOL_NETCENTER_NET *net;
	struct __lol_netcenter_netlist *prior;
	struct __lol_netcenter_netlist *next;
};
typedef struct __lol_netcenter_netlist LOL_NETCENTER_NETLIST;

struct __lol_netcenter{
	LOL_NETCENTER_NETLIST *netlist;
	LOL_NETLIST *nlist;
};
typedef struct __lol_netcenter LOL_NETCENTER;

int lol_netcenter_recvto(LOL_NETCENTER_NET *net);
int lol_netcenter_send(LOL_NETCENTER_NET *net,const void *buf,unsigned int len);
int lol_netcenter_recv(LOL_NETCENTER_NET *net,void *buf,unsigned int siz);
int lol_netcenter_sendfrom(LOL_NETCENTER_NET *net);
int lol_netcenter_peek(LOL_NETCENTER_NET *net,void *buf,unsigned int siz);
int lol_netcenter_select(LOL_NETCENTER *netcenter,int timeout);

LOL_NETCENTER *lol_netcenter_new();
int lol_netcenter_idle(LOL_NETCENTER *netcenter,int timeout);
int lol_netcenter_free(LOL_NETCENTER *netcenter);

LOL_NETCENTER_NETLIST *lol_netcenter_netadd(LOL_NETCENTER *netcenter,LOL_NET *net,LOL_NETCENTER_NETHANDLE nethandle);
int lol_netcenter_netdel(LOL_NETCENTER *netcenter,LOL_NET *net);
LOL_NETCENTER_NETLIST *lol_netcenter_netfind(LOL_NETCENTER *netcenter,LOL_NET *net);
int lol_netcenter_netclean(LOL_NETCENTER *netcenter);

int lol_netcenter_ismsgok(LOL_NETCENTER_NET *net);
int lol_netcenter_recvmsg(LOL_NETCENTER_NET *net,void *buf,unsigned int siz);
int lol_netcenter_sendmsg(LOL_NETCENTER_NET *net,const void *buf,unsigned int len);


/* lol_timer */

#define LOL_TIMER_TIMEOUT 0x00000001

struct __lol_timer;
typedef struct __lol_timer LOL_TIMER;
typedef void (*LOL_TIMERHANDLE)(LOL_TIMER *);
struct __lol_timer {
	unsigned int timeout;
	unsigned int lasttime;
	LOL_TIMERHANDLE timerhandle;
};

struct __lol_timerlist {
	LOL_TIMER *timer;
	struct __lol_timerlist *prior;
	struct __lol_timerlist *next;
};
typedef struct __lol_timerlist LOL_TIMERLIST;

LOL_TIMER *lol_timernew(LOL_TIMERHANDLE timerhandle,unsigned int timeout);
int lol_timerfree(LOL_TIMER *timer);
int lol_timerrefresh(LOL_TIMER *timer);
int lol_timerreset(LOL_TIMER *timer);

int lol_timerlist_add(LOL_TIMERLIST **timerlist,LOL_TIMER *timer);
int lol_timerlist_del(LOL_TIMERLIST **timerlist,LOL_TIMER *timer);
LOL_TIMERLIST *lol_timerlist_find(LOL_TIMERLIST *timerlist,LOL_TIMER *timer);
int lol_timerlist_clean(LOL_TIMERLIST **timerlist);
int lol_timerlist_free(LOL_TIMERLIST **timerlist);
int lol_timerlist_idle(LOL_TIMERLIST **timerlist);


/* lol_cfg */

#define LOL_CFG_FIELD_SIZE	128	/* field size */
#define LOL_CFG_NAME_SIZE	128	/* name size */
#define LOL_CFG_VALUE_SIZE	1024	/* value size */
#define LOL_CFG_FILE_SIZE	256	/* file size */

struct __lol_cfg_cfglist {
	char field[LOL_CFG_FIELD_SIZE];	/* field */
	char name[LOL_CFG_NAME_SIZE];		/* name */
	char value[LOL_CFG_VALUE_SIZE];	/* value */
	char file[LOL_CFG_FILE_SIZE];		/* file */
	unsigned int lineno;
	struct __lol_cfg_cfglist *prior;
	struct __lol_cfg_cfglist *next;
};			/* cfg list */
typedef struct __lol_cfg_cfglist LOL_CFG_CFGLIST;

struct __lol_cfg{
	unsigned int cfgcount;		/* cfg count */
	LOL_CFG_CFGLIST *cfglist; 	/* cfg list */
};
typedef struct __lol_cfg LOL_CFG;

#define LOL_CFG_CHAR			'C'		/* 1 bytes */
#define LOL_CFG_SHORT			'S'		/* 2 bytes */
#define LOL_CFG_INT			'I'		/* 4 bytes */
#define LOL_CFG_LONG			'L'		/* 8 bytes */
#define LOL_CFG_FLOAT			'F'		/* 4 bytes */
#define LOL_CFG_DOUBLE		'D'		/* 8 bytes */

#define LOL_CFG_MASK_TYPE		0xFF		/* mask */

#define LOL_CFG_NOT_IGNORE		0x0100		/* not ignore */
#define LOL_CFG_NOT_EMPTY		0x0200		/* not empty */

struct __lol_cfgarray {
	const char *domain;
	const char *name;
	void *value;
	unsigned int size;
	int flags;
	int *exists;
};		/* cfg array */
typedef struct __lol_cfgarray LOL_CFGARRAY;

/* load cfg */
LOL_CFG *lol_loadcfg(const char *file,const char *path);

/* get cfg */
int lol_getcfg(LOL_CFG *cfg,const char *field,const char *name,void *value,unsigned int size,int flags);

/* show cfg */
int lol_showcfg(LOL_CFG *cfg);

/* free cfg */
int lol_freecfg(LOL_CFG *cfg);

/* get cfgs */
int lol_getcfgs(LOL_CFG *cfg,LOL_CFGARRAY *cfgarray,unsigned int cfgcount);


/* lol_conf */

/* field type */
#define LOL_CONF_CHAR		'C'		/* 1 bytes */
#define LOL_CONF_SHORT	'S'		/* 2 bytes */
#define LOL_CONF_INT		'I'		/* 4 bytes */
#define LOL_CONF_LONG		'L'		/* 8 bytes */
#define LOL_CONF_FLOAT	'F'		/* 4 bytes */
#define LOL_CONF_DOUBLE	'D'		/* 8 bytes */
#define LOL_CONF_MASK_TYPE	0xFF		/* field type mask */

#define LOL_CONF_NOT_EMPTY	0x0100		/* field value not empty */

#define LOL_CONF_VALUE_SIZE	1024
#define LOL_CONF_FILE_SIZE	256

struct __lol_conf_vlist {
	char value[LOL_CONF_VALUE_SIZE];	/* column value */
	struct __lol_conf_vlist *prior;
	struct __lol_conf_vlist *next;
};	/* column list */
typedef struct __lol_conf_vlist LOL_CONF_VLIST;

struct __lol_conf_hlist {
	struct __lol_conf_vlist *vlist;		/* column list */
	struct __lol_conf_hlist *prior;
	struct __lol_conf_hlist *next;
};	/* line list */
typedef struct __lol_conf_hlist LOL_CONF_HLIST;

struct __lol_conf {
	char d;					/* column delimiter */
	unsigned int hcount;			/* line count */
	unsigned int vcount;			/* column count */
	char file[LOL_CONF_FILE_SIZE];	/* file */
	LOL_CONF_HLIST *hlist;		/* line list */
};
typedef struct __lol_conf LOL_CONF;

LOL_CONF *lol_loadconf(const char *file,char d);
int lol_getconf(LOL_CONF *conf,int h,int v,void *value,unsigned int size,int flags);               
int lol_showconf(LOL_CONF *conf);
int lol_freeconf(LOL_CONF *conf);


/* lol_db */

/* open flags */
#define LOL_DB_CREAT 		0x01		/* creat */
#define LOL_DB_EXCL 		0x02		/* excl */
#define LOL_DB_TRUNC 		0x04		/* trunc */
#define LOL_DB_RDONLY 	0x08		/* read only */
#define LOL_DB_WRONLY 	0x10		/* write only */
#define LOL_DB_RDWR 		0x20		/* read & write */
#define LOL_DB_APPEND 	0x40		/* append */
#define LOL_DB_SYNC		0x80		/* sync */

/* seek */
#define LOL_DB_SEEK_SET	0		/* from first record */
#define LOL_DB_SEEK_CUR	1		/* from current record */
#define LOL_DB_SEEK_END	2		/* from last record */

/* field type */
#define LOL_DB_CHAR		'C'		/* 1 bytes */
#define LOL_DB_SHORT		'S'		/* 2 bytes */
#define LOL_DB_INT		'I'		/* 4 bytes */
#define LOL_DB_LONG		'L'		/* 8 bytes */
#define LOL_DB_FLOAT		'F'		/* 4 bytes */
#define LOL_DB_DOUBLE		'D'		/* 8 bytes */
#define LOL_DB_MASK_TYPE	0xFF		/* field type mask */

/* flags */
#define LOL_DB_NOT_EMPTY	0x0100		/* field value not empty */

#define LOL_DB_EOF		-1		/* eof */

/* db field */
struct __lol_db_field {
	char name[11];				/* field name */
	char type;				/* field type */
	unsigned int offset;			/* field offset */
	unsigned int length;			/* field length */
};
typedef struct __lol_db_field LOL_DB_FIELD;

/* db handle */
struct __lol_db {
	LOL_FILE *file;			/* db file */
	int flags;				/* open flags */
	int cursor;				/* cursor */
	char *buffer;				/* buffer */

	char last_modify_date[11];		/* last modify date: yyyy-mm-dd */
	unsigned int record_count;		/* record count */
	unsigned short head_length;		/* head length */
	unsigned short record_length;		/* record length */
	unsigned int field_count;		/* field count */
	LOL_DB_FIELD *field_array;		/* field array */
};
typedef struct __lol_db LOL_DB;

/* lol_db_open */
LOL_DB *lol_db_open(const char *pathname,int flags,LOL_DB_FIELD *fieldarray,unsigned int fieldcount);
LOL_DB *lol_db_openx(const char *pathname,int flags,const char *conffile);

/* lol_db_fetch */
int lol_db_fetch(LOL_DB *db);

/* lol_db_getfield */
int lol_db_getfield(LOL_DB *db,unsigned int no,void *value,unsigned int size,int flags);

/* lol_db_getfieldbyname */
int lol_db_getfieldbyname(LOL_DB *db,const char *name,void *value,unsigned int size,int flags);

/* lol_db_new */
void lol_db_new(LOL_DB *db);

/* lol_db_putfield */
int lol_db_putfield(LOL_DB *db,unsigned int no,const void *value,unsigned int length,int flags);

/* lol_db_putfieldbyname */
int lol_db_putfieldbyname(LOL_DB *db,const char *name,const void *value,unsigned int length,int flags);

/* lol_db_insert */
int lol_db_insert(LOL_DB *db);

/* lol_db_seek */
int lol_db_seek(LOL_DB *db,int offset,int whence);

/* lol_db_sync */
int lol_db_sync(LOL_DB *db);

/* lol_db_eof */
int lol_db_eof(LOL_DB *db);

/* lol_db_close */
int lol_db_close(LOL_DB *db);


/* lol_mdb */


/* lol_gl */

#define LOL_GL_CHAR		'C'		/* 1 bytes */
#define LOL_GL_SHORT		'S'		/* 2 bytes */
#define LOL_GL_INT		'I'		/* 4 bytes */
#define LOL_GL_LONG		'L'		/* 8 bytes */
#define LOL_GL_FLOAT		'F'		/* 4 bytes */
#define LOL_GL_DOUBLE		'D'		/* 8 bytes */

/* rule */
typedef struct {
	unsigned int no;	/* no */
	char name[32];		/* name */
	char sname[16];		/* short name */
	char type;		/* type */
	unsigned int maxlen;	/* max length */
	char note[128];		/* note */
} LOL_GL_RULE;

/* field */
typedef struct {
	unsigned int no;	/* field no */
	void *data;		/* data address */
	unsigned int size;	/* data size */
	unsigned int *len;	/* data length */
} LOL_GL_FIELD;

/* lol_gl_get */
int lol_gl_get(const char *msgbuf,unsigned int msglen,unsigned int no,char type,unsigned int maxlen,void *data,unsigned int size);

/* lol_gl_put */
int lol_gl_put(char *msgbuf,unsigned int msgsiz,unsigned int no,char type,unsigned int maxlen,const void *data,unsigned int len);

/* lol_gl_del */
int lol_gl_del(char *msgbuf,unsigned int msglen,unsigned int no);

/* lol_gl_exist */
int lol_gl_exist(const char *msgbuf,unsigned int msglen,unsigned int no);

/* lol_gl_gets */
int lol_gl_gets(const char *msgbuf,unsigned int msglen,const LOL_GL_FIELD *fieldarray,unsigned int fieldcount,const LOL_GL_RULE *rulearray,unsigned int rulecount);

/* lol_gl_puts */
int lol_gl_puts(char *msgbuf,unsigned int msgsiz,const LOL_GL_FIELD *fieldarray,unsigned int fieldcount,const LOL_GL_RULE *rulearray,unsigned int rulecount);

/* lol_gl_loadrules */
int lol_gl_loadrules(const char *file,LOL_GL_RULE **rulearray);


/* lol_log */

#define LOL_LOG_DEBUG		1	/* debug */
#define LOL_LOG_NORMAL	2	/* normal(default) */
#define LOL_LOG_INFO		3	/* info */
#define LOL_LOG_KEY		4	/* key */
#define LOL_LOG_WARN		5	/* warn */
#define LOL_LOG_ERROR		6	/* error */
#define LOL_LOG_FATAL		7	/* fatal */
#define LOL_LOG_MASK_LEVEL	0x07	/* log level mask */

#define LOL_LOG_PRINT		0x08	/* print to screen */

#define LOL_LOG_TRUNC		0x80	/* trunc */
#define LOL_LOG_EXCL		0x40	/* exclusive */
#define LOL_LOG_LOCAL		0x20	/* local */
#define LOL_LOG_QUIET		0x10	/* not print to screen */

struct __lol_log {
	LOL_NET *net;			/* net */
	LOL_DB *db;			/* local */
	char addr[128];			/* address */
	char name[32];			/* name */
	int flags;			/* open flags */
	int pid;			/* process id */
};
typedef struct __lol_log LOL_LOG;

LOL_LOG *lol_logopen(const char *addr,const char *name,int flags);
int lol_logsend(LOL_LOG *lp,int flags,const char *desc,const char *fmt,...);
int lol_logclose(LOL_LOG *lp);
int lol_loggetflags(const char *strflags);


/* lol_shm */

#define LOL_SHM_CREAT		0x80000000
#define LOL_SHM_EXCL		0x40000000
#define LOL_SHM_TRUNC		0x20000000
#define LOL_SHM_RDONLY	0x10000000
#define LOL_SHM_RDWR		0x08000000
#define LOL_SHM_RUSR		0x00000100		/*0400(r)*/
#define LOL_SHM_WUSR		0x00000080		/*0200(w)*/
#define LOL_SHM_XUSR		0x00000040		/*0100(x)*/
#define LOL_SHM_RGRP		0x00000020		/*0040(r)*/
#define LOL_SHM_WGRP		0x00000010		/*0020(w)*/
#define LOL_SHM_XGRP		0x00000008		/*0010(x)*/
#define LOL_SHM_ROTH		0x00000004		/*0004(r)*/
#define LOL_SHM_WOTH		0x00000002		/*0002(w)*/
#define LOL_SHM_XOTH		0x00000001		/*0001(x)*/

#define LOL_SHM_MASK_SECURITY	0x000001FF	/*0777(rwxrwxrwx)*/

#ifndef SHM_FAILED
#define SHM_FAILED (void *)-1L
#endif

struct __lol_shm{
#ifdef __LOL_WINDOWS__ /* Windows */
	HANDLE handle;
#else /* UNIX */
	int shmid;
#endif
	void *addr;
};
typedef struct __lol_shm LOL_SHM;
LOL_SHM *lol_shmopen(const char *name,int size,int flags);
int lol_shmclose(LOL_SHM *shm);


/* lol_sem.h */

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


/* lol_mutex */

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


/* lol_shmq */

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


/* lol_thread */

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


/* lol_threadmutex */

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

#endif	/* __LOL_H__ */
