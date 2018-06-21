#include "lol_error.h"
#include "lol_utility.h"
#include "lol_sem.h"

LOL_SEM *lol_semopen(const char *name,int flags)
{
	LOL_SEM *sem;
#ifdef __LOL_UNIX__
	int key,oflags=0;
	LOL_SEMUN su;
#endif

	if((sem=malloc(sizeof(LOL_SEM)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	sem->flags=flags;
#ifdef __LOL_WINDOWS__
	if((sem->handle=CreateSemaphore(NULL,0,65536,name))==NULL){
		lol_error(0,"[%s]:CreateSemaphore error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(sem);
		return NULL;
	}
	if(flags & LOL_SEM_CREAT && flags & LOL_SEM_EXCL){
		if(lol_errno==ERROR_ALREADY_EXISTS){
			lol_error(0,"[%s]:Semaphore exists.",__func__);
			CloseHandle(sem->handle);
			free(sem);
			return NULL;
		}
	}
#else
	if((key=ftok(name,0))<0){
		lol_error(0,"[%s]:ftok error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(sem);
		return NULL;
	}

	if(flags & LOL_SEM_CREAT)
		oflags|=IPC_CREAT;
	if(flags & LOL_SEM_EXCL)
		oflags|=IPC_EXCL;
	if(flags & LOL_SEM_RUSR)
		oflags|=S_IRUSR;
	if(flags & LOL_SEM_WUSR)
		oflags|=S_IWUSR;
	if(flags & LOL_SEM_RGRP)
		oflags|=S_IRGRP;
	if(flags & LOL_SEM_WGRP)
		oflags|=S_IWGRP;
	if(flags & LOL_SEM_ROTH)
		oflags|=S_IROTH;
	if(flags & LOL_SEM_WOTH)
		oflags|=S_IWOTH;

	if((sem->semid=semget(key,1,oflags))<0){
		lol_error(0,"[%s]:semget error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(sem);
		return NULL;
	}
	if(flags & LOL_SEM_TRUNC){
		su.val=0;
		if(semctl(sem->semid,0,SETVAL,su)<0){
			lol_error(0,"[%s]:semctl error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			free(sem);
			return NULL;
		}
	}
#endif

	return sem;
}

int lol_semget(LOL_SEM *sem,int timeout)
{
	int r;
#ifdef __LOL_UNIX__
	struct sembuf sb;
#endif

#ifdef __LOL_WINDOWS__ /* Windows */
	if(timeout<0)
		timeout=INFINITE;
	if((r=WaitForSingleObject(sem->handle,timeout))!=WAIT_OBJECT_0){
		if(r==WAIT_TIMEOUT)
			return 0;
		lol_error(0,"[%s]:WaitForSingleObject error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=0;
	if(sem->flags & LOL_SEM_UNDO)
		sb.sem_flg|=SEM_UNDO;
	if(timeout<0){
		if(semop(sem->semid,&sb,1)<0){
			if(lol_errno==EINTR)
				return 0;
			lol_error(0,"[%s]:semop error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			return -1;
		}
	}else if(timeout==0){
		sb.sem_flg|=IPC_NOWAIT;
		if(semop(sem->semid,&sb,1)<0){
			if(lol_errno==EINTR || lol_errno==EAGAIN)
				return 0;
			lol_error(0,"[%s]:semop error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			return -1;
		}
	}else{
		sb.sem_flg|=IPC_NOWAIT;
		for(;timeout>=0;lol_sleep(1),timeout--){
			if(semop(sem->semid,&sb,1)<0){
				if(lol_errno==EINTR)
					return 0;
				if(lol_errno==EAGAIN){
					if(timeout==0)
						return 0;
					continue;
				}
				lol_error(0,"[%s]:semop error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
				return -1;
			}
			break;
		}
	}
#endif

	return 1;
}
int lol_semput(LOL_SEM *sem)
{
#ifdef __LOL_UNIX__
	struct sembuf sb;
#endif

#ifdef __LOL_WINDOWS__ /* Windows */
	if(ReleaseSemaphore(sem->handle,1,NULL)==0){
		lol_error(0,"[%s]:ReleaseSemaphore error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=1;
	sb.sem_flg=0;
	if(sem->flags & LOL_SEM_UNDO)
		sb.sem_flg|=SEM_UNDO;
	if(semop(sem->semid,&sb,1)<0){
		lol_error(0,"[%s]:semop error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif

	return 0;
}
int lol_semclose(LOL_SEM *sem)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	CloseHandle(sem->handle);
#else /* UNIX */
#endif
	free(sem);

	return 0;
}

