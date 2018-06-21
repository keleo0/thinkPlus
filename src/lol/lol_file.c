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
	if((file->fd=open(pathname,oflags,mode))<0){
		lol_error(0,"[%s]:open failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(file);
		return NULL;
	}

	return file;
}
int lol_fileread(LOL_FILE *file,void *buffer,unsigned int nbytes)
{
	int n;

	if((n=read(file->fd,buffer,nbytes))<0){
		lol_error(0,"[%s]:read failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}

	return n;
}
int lol_filewrite(LOL_FILE *file,const void *buffer,unsigned int nbytes)
{
	int n;

	if((n=write(file->fd,buffer,nbytes))<0){
		lol_error(0,"[%s]:write failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}

	return n;
}
int lol_fileseek(LOL_FILE *file,int offset,int whence)
{
	int pos;

	if((pos=lseek(file->fd,offset,whence))<0){
		lol_error(0,"[%s]:lseek failed.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}

	return pos;
}
int lol_fileclose(LOL_FILE *file)
{
	close(file->fd);
	free(file);

	return 0;
}
