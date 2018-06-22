/*
 * lol_file.h: LOL File Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_FILE_H__
#define __LOL_FILE_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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
#define lollseek _lseeki64
#define loltell _telli64
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
#define LOL_FILE_CREAT 		0x01		/* creat */
#define LOL_FILE_EXCL 		0x02		/* excl */
#define LOL_FILE_TRUNC 		0x04		/* trunc */
#define LOL_FILE_RDONLY 	0x08		/* read only */
#define LOL_FILE_WRONLY 	0x10		/* write only */
#define LOL_FILE_RDWR 		0x20		/* read & write */
#define LOL_FILE_APPEND 	0x40		/* append */
#define LOL_FILE_BINARY		0x80		/* binary */

#define LOL_FILE_SEEK_SET	SEEK_SET
#define LOL_FILE_SEEK_CUR	SEEK_CUR
#define LOL_FILE_SEEK_END	SEEK_END

LOL_FILE *lol_fileopen(const char *pathname,int flags,int mode);
int lol_fileclose(LOL_FILE *file);

int lol_fileread(LOL_FILE *file,void *buffer,unsigned int nbytes);
int lol_filewrite(LOL_FILE *file,const void *buffer,unsigned int nbytes);

LOL_LONG lol_fileseek(LOL_FILE *file, LOL_LONG offset,int whence);
LOL_LONG lol_filesize(LOL_FILE *file);

int lol_access(const char* path, int accessmode);
int lol_fileaccess(const char* filename, int accessmode);
int lol_diraccess(const char* filename, int accessmode);

int lol_remove(const char* path);
int lol_rename(const char* oldname, const char* newname);
int lol_move(const char* oldpath, const char* newpath);

int lol_disk_free_size(const char* pDisk, LOL_LONG* free_size);
int lol_disk_size(const char* pDisk, LOL_LONG* total_size);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_FILE_H__ */
