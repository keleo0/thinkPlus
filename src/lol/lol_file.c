#include "lol_error.h"
#include "lol_file.h"

LOL_FILE *lol_fileopen(const char *pathname,int flags,int mode)
{
	LOL_FILE *file;
	int oflags;

	/* malloc file descriptor */
	if((file=malloc(sizeof(LOL_FILE)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	bzero(file,sizeof(LOL_FILE));

	/* open flags */
	oflags=0;
	if(flags & LOL_FILE_CREAT)
		oflags|=O_CREAT;
	if(flags & LOL_FILE_EXCL)
		oflags|=O_EXCL;
	if(flags & LOL_FILE_TRUNC)
		oflags|=O_TRUNC;
	if(flags & LOL_FILE_RDONLY)
		oflags|=O_RDONLY;
	if(flags & LOL_FILE_WRONLY)
		oflags|=O_WRONLY;
	if(flags & LOL_FILE_RDWR)
		oflags|=O_RDWR;
	if(flags & LOL_FILE_APPEND)
		oflags|=O_APPEND;
	if(flags & LOL_FILE_BINARY)
		oflags|=O_BINARY;
	
	/* open file */
	if((file->fd=lolopen(pathname,oflags,mode))<0){
		lol_error(0,"[%s]:open failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(file);
		return NULL;
	}

	return file;
}
int lol_fileread(LOL_FILE *file,void *buffer,unsigned int nbytes)
{
	int n;

	if((n=lolread(file->fd,buffer,nbytes))<0){
		lol_error(0,"[%s]:read failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}

	return n;
}
int lol_filewrite(LOL_FILE *file,const void *buffer,unsigned int nbytes)
{
	int n;

	if((n=lolwrite(file->fd,buffer,nbytes))<0){
		lol_error(0,"[%s]:write failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}

	return n;
}
LOL_LONG lol_fileseek(LOL_FILE *file, LOL_LONG offset,int whence)
{
	LOL_LONG pos;

	if((pos=lollseek(file->fd,offset,whence))<0){
		lol_error(0,"[%s]:lseek failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}

	return pos;
}

LOL_LONG lol_filesize(LOL_FILE *file)
{
	int ret = -1;
	struct stat fileStat = { 0 };

	ret = fstat(file->fd, &fileStat);
	if (ret != 0)
	{
		lol_error(0, "[%s]:fstat failed.[%d:%s]", __func__, lol_errno, lol_strerror(lol_errno));
		return ret;
	}
	return fileStat.st_size;
}

int lol_fileclose(LOL_FILE *file)
{
	lolclose(file->fd);
	free(file);

	return 0;
}

int lol_access(const char* path, int accessmode)
{
	return lolaccess(path, accessmode);
}

int lol_fileaccess(const char* filename, int accessmode)
{
	INT32 ret = -1;
	struct stat info = { 0 };

	if (stat(filename, &info))
		return ret;
	if (S_ISREG(info.st_mode))
	{
		ret = 0;
	}
	return ret;
}

int lol_diraccess(const char* dirname, int accessmode)
{
	INT32 ret = -1;
	struct stat info = { 0 };

	if (stat(dirname, &info))
		return ret;
	if (S_ISDIR(info.st_mode))
	{
		ret = 0;
	}
	return ret;
}

int lol_remove(const char* path)
{
	return remove(path);
}

int lol_rename(const char* oldname, const char* newname)
{
	return rename(oldname, newname);
}

int lol_move(const char* oldpath, const char* newpath)
{
	return rename(oldpath, newpath);
}

int lol_disk_free_size(const char* pDisk, LOL_LONG* free_size)
{
#if defined(__LOL_WINDOWS__)
	BOOL fResult;
	LOL_ULONG i64FreeBytesToCaller;
	LOL_ULONG i64TotalBytes;
	LOL_ULONG i64FreeBytes;

	fResult = GetDiskFreeSpaceEx(
		TEXT(""),
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	if (fResult)
	{
		*free_size = i64FreeBytesToCaller;
	}
	else
	{
		return -1;
	}
#else
	struct statfs disk_statfs;

	if (statfs(pDisk, &disk_statfs) >= 0)
	{
		*free_size = ((UINT64)disk_statfs.f_bsize  * (UINT64)disk_statfs.f_bfree);
	}
	else
	{
		FILESYSTEM_DBG_ERR("[sdm]:stor_fs_free_size fail\n");
		return -1;
	}
#endif
	return 0;
}

int lol_disk_size(const char* pDisk, LOL_LONG* total_size)
{
#if defined(__LOL_WINDOWS__)
	BOOL fResult;
	unsigned _int64 i64FreeBytesToCaller;
	unsigned _int64 i64TotalBytes;
	unsigned _int64 i64FreeBytes;

	fResult = GetDiskFreeSpaceEx(
		TEXT(""),
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);
	if (fResult)
	{
		*total_size = i64TotalBytes;
}
	else
	{
		return -1;
	}
#else
	struct statfs disk_statfs;

	if (statfs(pDisk, &disk_statfs) >= 0)
	{
		*total_size = ((UINT64)disk_statfs.f_bsize * (UINT64)disk_statfs.f_blocks);
	}
	else
	{
		return -1;
	}
#endif
	return 0;
}


