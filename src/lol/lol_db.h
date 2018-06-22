/*
 * lol_db.h: LOL DB Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_DB_H__
#define __LOL_DB_H__

#include "lol_os.h"
#include "lol_file.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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
#define LOL_DB_CHAR			'C'		/* 1 bytes */
#define LOL_DB_SHORT		'S'		/* 2 bytes */
#define LOL_DB_INT			'I'		/* 4 bytes */
#define LOL_DB_LONG			'L'		/* 8 bytes */
#define LOL_DB_FLOAT		'F'		/* 4 bytes */
#define LOL_DB_DOUBLE		'D'		/* 8 bytes */
#define LOL_DB_MASK_TYPE	0xFF		/* field type mask */

/* flags */
#define LOL_DB_NOT_EMPTY	0x0100		/* field value not empty */

#define LOL_DB_EOF		-1		/* eof */

/* db field */
struct __lol_db_field {
	char name[11];			/* field name */
	char type;				/* field type */
	unsigned int length;	/* field length */
	unsigned int offset;	/* field offset */
};
typedef struct __lol_db_field LOL_DB_FIELD;

/* db handle */
struct __lol_db {
	LOL_FILE *file;			/* db file */
	int flags;				/* open flags */
	int cursor;				/* cursor */
	char *buffer;			/* buffer */

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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_DB_H__ */
