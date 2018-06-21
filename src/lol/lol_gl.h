/*
 * lol_gl.h: LOL General Language Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(enigma1983@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */
#ifndef __LOL_GL_H__
#define __LOL_GL_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_GL_H__ */
