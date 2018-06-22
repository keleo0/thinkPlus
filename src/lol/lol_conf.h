/*
 * lol_conf.h: LOL Conf Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-14	1.0 released
 *
 * examples:
 * dbf.conf:
 * Name		|Type	|Length	|Desc
 * ID		|int	|3	|ID
 * Name		|char	|15	|Name
 * Email	|char	|30	|Email
 *
 * server.c:
 * CONF *conf=NULL;
 * char value[128];
 * conf=lol_confload("table.conf",'|');
 * confshow(conf);
 * bzero(value,sizeof(value));
 * confget(conf,0,0,value,sizeof(value));
 * conffree(conf);
 *
 */
#ifndef __LOL_CONF_H__
#define __LOL_CONF_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* field type */
#define LOL_CONF_CHAR		'C'		/* 1 bytes */
#define LOL_CONF_SHORT		'S'		/* 2 bytes */
#define LOL_CONF_INT		'I'		/* 4 bytes */
#define LOL_CONF_LONG		'L'		/* 8 bytes */
#define LOL_CONF_FLOAT		'F'		/* 4 bytes */
#define LOL_CONF_DOUBLE		'D'		/* 8 bytes */
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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_CONF_H__ */
