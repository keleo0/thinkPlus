/*
 * lol_dbf.h: DBF Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(enigma1983@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_DBF_H__
#define __LOL_DBF_H__

#include "lol.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* open flags */
#define LOL_DBF_CREAT 	0x01		/* creat */
#define LOL_DBF_EXCL 		0x02		/* excl */
#define LOL_DBF_TRUNC 	0x04		/* trunc */
#define LOL_DBF_RDONLY 	0x08		/* read only */
#define LOL_DBF_WRONLY 	0x10		/* write only */
#define LOL_DBF_RDWR 		0x20		/* read & write */
#define LOL_DBF_APPEND 	0x40		/* append */
#define LOL_DBF_SYNC		0x80		/* sync */

/* seek */
#define LOL_DBF_SEEK_SET	0		/* from first record */
#define LOL_DBF_SEEK_CUR	1		/* from current record */
#define LOL_DBF_SEEK_END	2		/* from last record */

/* field type */
#define LOL_DBF_CHAR		'C'		/* 1 bytes */
#define LOL_DBF_SHORT		'S'		/* 2 bytes */
#define LOL_DBF_INT		'I'		/* 4 bytes */
#define LOL_DBF_LONG		'L'		/* 8 bytes */
#define LOL_DBF_FLOAT		'F'		/* 4 bytes */
#define LOL_DBF_DOUBLE	'D'		/* 8 bytes */
#define LOL_DBF_MASK_TYPE	0xFF		/* field type mask */

/* flags */
#define LOL_DBF_NOT_EMPTY	0x0100		/* field value not empty */

#define LOL_DBF_EOF		-1		/* eof */

/* dbf field */
struct __lol_dbf_field {
	char name[11];				/* field name */
	char type;				/* field type */
	unsigned int offset;			/* field offset */
	unsigned int length;			/* field length */
};
typedef struct __lol_dbf_field LOL_DBF_FIELD;

/* dbf handle */
struct __lol_dbf {
	LOL_FILE *file;			/* dbf file */
	int flags;				/* open flags */
	int cursor;				/* cursor */
	char *buffer;				/* buffer */

	char last_modify_date[11];		/* last modify date: yyyy-mm-dd */
	unsigned int record_count;		/* record count */
	unsigned short head_length;		/* head length */
	unsigned short record_length;		/* record length */
	unsigned int field_count;		/* field count */
	LOL_DBF_FIELD *field_array;		/* field array */
};
typedef struct __lol_dbf LOL_DBF;

/* lol_dbf_open */
extern LOL_DBF *lol_dbf_open(const char *pathname,int flags,const char *conffile);

/* lol_dbf_fetch */
extern int lol_dbf_fetch(LOL_DBF *dbf);

/* lol_dbf_getfield */
extern int lol_dbf_getfield(LOL_DBF *dbf,unsigned int no,void *value,unsigned int size,int flags);

/* lol_dbf_getfieldbyname */
extern int lol_dbf_getfieldbyname(LOL_DBF *dbf,const char *name,void *value,unsigned int size,int flags);

/* lol_dbf_new */
extern void lol_dbf_new(LOL_DBF *dbf);

/* lol_dbf_putfield */
extern int lol_dbf_putfield(LOL_DBF *dbf,unsigned int no,const void *value,unsigned int length,int flags);

/* lol_dbf_putfieldbyname */
extern int lol_dbf_putfieldbyname(LOL_DBF *dbf,const char *name,const void *value,unsigned int length,int flags);

/* lol_dbf_insert */
extern int lol_dbf_insert(LOL_DBF *dbf);

/* lol_dbf_seek */
extern int lol_dbf_seek(LOL_DBF *dbf,int offset,int whence);

/* lol_dbf_sync */
extern int lol_dbf_sync(LOL_DBF *dbf);

/* lol_dbf_eof */
extern int lol_dbf_eof(LOL_DBF *dbf);

/* lol_dbf_close */
extern int lol_dbf_close(LOL_DBF *dbf);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_DBF_H__ */
