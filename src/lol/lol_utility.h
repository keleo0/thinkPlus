/*
 * lol_utility.h: LOL Utility Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_UTILITY_H__
#define __LOL_UTILITY_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

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

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_UTILITY_H__ */
