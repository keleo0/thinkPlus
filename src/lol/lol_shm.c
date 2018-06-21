#include "lol_error.h"
#include "lol_shm.h"

LOL_SHM *lol_shmopen(const char *name,int size,int flags)
{
	LOL_SHM *shm;
	int oflags;
#ifdef __LOL_UNIX__ /* UNIX */
	int key;
#endif

	if((shm=malloc(sizeof(LOL_SHM)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}

#ifdef __LOL_WINDOWS__
	if(flags & LOL_SHM_CREAT){
		if(flags & LOL_SHM_RDONLY)
			oflags=PAGE_READONLY;
		else
			oflags=PAGE_READWRITE;
		if((shm->handle=CreateFileMapping(INVALID_HANDLE_VALUE,NULL,oflags,0,size,name))==NULL){
			lol_error(0,"[%s]:CreateFileMapping error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			free(shm);
			return NULL;
		}
		if(flags & LOL_SHM_CREAT && flags & LOL_SHM_EXCL){
			if(lol_errno==ERROR_ALREADY_EXISTS){
				lol_error(0,"[%s]:ShareMemory exists.",__func__);
				CloseHandle(shm->handle);
				free(shm);
				return NULL;
			}
		}
	}else{
		if(flags & LOL_SHM_RDONLY)
			oflags=FILE_MAP_READ;
		else
			oflags=FILE_MAP_WRITE;
		if((shm->handle=OpenFileMapping(oflags,FALSE,name))==NULL){
			lol_error(0,"[%s]:OpenFileMapping error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			CloseHandle(shm->handle);
			free(shm);
			return NULL;
		}
	}
	if(flags & LOL_SHM_RDONLY)
		oflags=FILE_MAP_READ;
	else
		oflags=FILE_MAP_WRITE;
	if((shm->addr=MapViewOfFile(shm->handle,oflags,0,0,0))==NULL){
		lol_error(0,"[%s]:MapViewOfFile error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		CloseHandle(shm->handle);
		free(shm);
		return NULL;
	}
#else /* UNIX */
	if((key=ftok(name,0))<0){
		lol_error(0,"[%s]:ftok error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(shm);
		return NULL;
	}

	oflags=0;
	if(flags & LOL_SHM_CREAT)
		oflags|=IPC_CREAT;
	if(flags & LOL_SHM_EXCL)
		oflags|=IPC_EXCL;
	if(flags & LOL_SHM_RUSR)
		oflags|=S_IRUSR;
	if(flags & LOL_SHM_WUSR)
		oflags|=S_IWUSR;
	if(flags & LOL_SHM_RGRP)
		oflags|=S_IRGRP;
	if(flags & LOL_SHM_WGRP)
		oflags|=S_IWGRP;
	if(flags & LOL_SHM_ROTH)
		oflags|=S_IROTH;
	if(flags & LOL_SHM_WOTH)
		oflags|=S_IWOTH;

	if(flags & LOL_SHM_TRUNC){
		if((shm->shmid=shmget(key,0,0))>0){
			if(shmctl(shm->shmid,IPC_RMID,NULL)<0){
				lol_error(0,"[%s]:shmctl error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
				free(shm);
				return NULL;
			}
		}
	}
	if((shm->shmid=shmget(key,size,oflags))<0){
		lol_error(0,"[%s]:shmget error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(shm);
		return NULL;
	}
	oflags=0;
	if(flags & LOL_SHM_RDONLY)
		oflags|=SHM_RDONLY;
	if((shm->addr=shmat(shm->shmid,NULL,oflags))==SHM_FAILED){
		lol_error(0,"[%s]:shmat error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(shm);
		return NULL;
	}
#endif

	return shm;
}
int lol_shmclose(LOL_SHM *shm)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	UnmapViewOfFile(shm->addr);
	CloseHandle(shm->handle);
#else /* UNIX */
	shmdt(shm->addr);
#endif
	free(shm);

	return 0;
}
