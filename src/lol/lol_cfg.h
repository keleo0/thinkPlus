/*
 * lol_cfg.h: LOL Cfg Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-14	1.0 released
 *
 */
#ifndef __LOL_CFG_H__
#define __LOL_CFG_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_CFG_H__ */
