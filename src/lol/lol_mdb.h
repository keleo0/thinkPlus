/*
 * lol_mdb.h: LOL Memory DB Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_MDB_H__
#define __LOL_MDB_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

/* open flags */
#define LOL_MDB_APPEND 	0x40		/* append */

/* seek */
#define LOL_MDB_SEEK_SET	0		/* from first record */
#define LOL_MDB_SEEK_CUR	1		/* from current record */
#define LOL_MDB_SEEK_END	2		/* from last record */

/* field type */
#define LOL_MDB_CHAR		'C'		/* 1 bytes */
#define LOL_MDB_SHORT		'S'		/* 2 bytes */
#define LOL_MDB_INT		'I'		/* 4 bytes */
#define LOL_MDB_LONG		'L'		/* 8 bytes */
#define LOL_MDB_FLOAT		'F'		/* 4 bytes */
#define LOL_MDB_DOUBLE	'D'		/* 8 bytes */
#define LOL_MDB_MASK_TYPE	0xFF		/* field type mask */

/* flags */
#define LOL_MDB_NOT_EMPTY	0x0100		/* field value not empty */

#define LOL_MDB_EOF		-1		/* eof */

/* mdb field */
struct __lol_mdb_field {
	char name[11];				/* field name */
	char type;				/* field type */
	unsigned int length;			/* field length */
	unsigned int offset;			/* field offset */
};
typedef struct __lol_mdb_field LOL_MDB_FIELD;

/* mdb handle */
struct __lol_mdb {
	int flags;				/* open flags */
	int cursor;				/* cursor */
	char *buffer;				/* buffer */

	unsigned int record_count;		/* record count */
	unsigned short head_length;		/* head length */
	unsigned short record_length;		/* record length */
	unsigned int field_count;		/* field count */
	LOL_MDB_FIELD *field_array;		/* field array */
};
typedef struct __lol_mdb LOL_MDB;

/* lol_mdb_open */
LOL_MDB *lol_mdb_open(const char *name,int flags,LOL_MDB_FIELD *fieldarray,unsigned int fieldcount);

/* lol_mdb_fetch */
int lol_mdb_fetch(LOL_MDB *mdb);

/* lol_mdb_getfield */
int lol_mdb_getfield(LOL_MDB *mdb,unsigned int no,void *value,unsigned int size,int flags);

/* lol_mdb_getfieldbyname */
int lol_mdb_getfieldbyname(LOL_MDB *mdb,const char *name,void *value,unsigned int size,int flags);

/* lol_mdb_new */
void lol_mdb_new(LOL_MDB *mdb);

/* lol_mdb_putfield */
int lol_mdb_putfield(LOL_MDB *mdb,unsigned int no,const void *value,unsigned int length,int flags);

/* lol_mdb_putfieldbyname */
int lol_mdb_putfieldbyname(LOL_MDB *mdb,const char *name,const void *value,unsigned int length,int flags);

/* lol_mdb_insert */
int lol_mdb_insert(LOL_MDB *mdb);

/* lol_mdb_seek */
int lol_mdb_seek(LOL_MDB *mdb,int offset,int whence);

/* lol_mdb_eof */
int lol_mdb_eof(LOL_MDB *mdb);

/* lol_mdb_close */
int lol_mdb_close(LOL_MDB *mdb);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_MDB_H__ */
