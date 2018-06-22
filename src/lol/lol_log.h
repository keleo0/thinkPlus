/*
 * lol_log.h: LOL Log Interface (use LOL DB )
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 *	logtest.db
 *	ip,pid,time,level,desc,info	(this order is reasonable)
 *
 *	default format:(time,level,desc,info)
 *	[2008-03-31 09:30:00]-[0]-[program]-recordno=100
 *	[2008-03-31 11:30:00]-[1]-[net]-connected by 192.168.1.102
 *	[2008-03-31 11:30:01]-[2]-[user]-login ... [0001]
 *	[2008-03-31 11:30:02]-[1]-[user]-login succeeded!
 *	[2008-03-31 11:31:59]-[3]-[trans]-accountno=1121341200002112
 *	[2008-03-31 11:31:59]-[3]-[trans]-amount=10000000.00
 *	[2008-03-31 14:03:20]-[4]-[program]-file too big!
 *	[2008-03-31 14:03:20]-[5]-[program]-file open failed![errno=2]
 *	[2008-03-31 14:03:20]-[6]-[program]-shmget failed![errno=2]
 *
 *	examples:
 *	lol_logopen("127.0.0.1:10000","ATM/trade",LOL_LOG_DEBUG);
 *	lol_logopen("../log","trade",LOL_LOG_PRINT);
 *	lol_logopen("../log","POS/trade",LOL_LOG_DBF|LOL_LOG_QUIET|LOL_LOG_NORMAL);
 *	lol_logsend(lp,LOL_LOG_NORMAL|LOL_LOG_PRINT,"file","%s","/home/luojian/book.txt");
 *	lol_logsend(lp,LOL_LOG_INFO,"connect","connected by %s:%d",ip,port);
 *	lol_logsend(lp,LOL_LOG_KEY,"business","AccountNo:%s",AccountNo);
 *	lol_logsend(lp,LOL_LOG_DEBUG|LOL_LOG_PRINT,"","len=%d",len);
 *	lol_logsend(lp,0,"","hello,world!");
 *	lol_logclose(lp);
 */

#ifndef __LOL_LOG_H__
#define __LOL_LOG_H__

#include "lol_os.h"
#include "lol_db.h"
#include "lol_net.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_LOG_H__ */
