#include "lol_error.h"
#include "lol_utility.h"
#include "lol_sem.h"
#include "lol_mutex.h"

LOL_MUTEX *lol_mutexopen(const char *name,int flags)
{
	LOL_MUTEX *mutex;
#ifdef __LOL_UNIX__
	int key,semexist,oflags=0;
	LOL_SEMUN su;
#endif

	if((mutex=malloc(sizeof(LOL_MUTEX)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	mutex->flags=flags;
#ifdef __LOL_WINDOWS__
	if((mutex->handle=CreateSemaphore(NULL,1,65536,name))==NULL){
		lol_error(0,"[%s]:CreateSemaphore error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(mutex);
		return NULL;
	}
	if(flags & LOL_MUTEX_CREAT && flags & LOL_MUTEX_EXCL){
		if(lol_errno==ERROR_ALREADY_EXISTS){
			lol_error(0,"[%s]:mutex exists.",__func__);
			CloseHandle(mutex->handle);
			free(mutex);
			return NULL;
		}
	}
#else
	if((key=ftok(name,0))<0){
		lol_error(0,"[%s]:ftok error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(mutex);
		return NULL;
	}

	if(flags & LOL_MUTEX_CREAT)
		oflags|=IPC_CREAT;
	if(flags & LOL_MUTEX_EXCL)
		oflags|=IPC_EXCL;
	if(flags & LOL_MUTEX_RUSR)
		oflags|=S_IRUSR;
	if(flags & LOL_MUTEX_WUSR)
		oflags|=S_IWUSR;
	if(flags & LOL_MUTEX_RGRP)
		oflags|=S_IRGRP;
	if(flags & LOL_MUTEX_WGRP)
		oflags|=S_IWGRP;
	if(flags & LOL_MUTEX_ROTH)
		oflags|=S_IROTH;
	if(flags & LOL_MUTEX_WOTH)
		oflags|=S_IWOTH;

	if(flags & LOL_MUTEX_CREAT){
		if(semget(key,1,0)<0)
			semexist=0;
		else
			semexist=1;
	}
	if((mutex->semid=semget(key,1,oflags))<0){
		lol_error(0,"[%s]:semget error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(mutex);
		return NULL;
	}
	if(((flags & LOL_MUTEX_CREAT) && !semexist) || (flags & LOL_SEM_TRUNC)){
		su.val=1;
		if(semctl(mutex->semid,0,SETVAL,su)<0){
			lol_error(0,"[%s]:semctl error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			free(mutex);
			return NULL;
		}
	}
#endif

	return mutex;
}

int lol_mutexlock(LOL_MUTEX *mutex,int timeout)
{
	int r;
#ifdef __LOL_UNIX__
	struct sembuf sb;
#endif

#ifdef __LOL_WINDOWS__ /* Windows */
	if(timeout<0)
		timeout=INFINITE;
	if((r=WaitForSingleObject(mutex->handle,timeout))!=WAIT_OBJECT_0){
		if(r==WAIT_TIMEOUT)
			return 0;
		lol_error(0,"[%s]:WaitForSingleObject error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=-1;
	sb.sem_flg=SEM_UNDO;
	if(timeout<0){
		if(semop(mutex->semid,&sb,1)<0){
			if(lol_errno==EINTR)
				return 0;
			lol_error(0,"[%s]:semop error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			return -1;
		}
	}else if(timeout==0){
		sb.sem_flg|=IPC_NOWAIT;
		if(semop(mutex->semid,&sb,1)<0){
			if(lol_errno==EINTR || lol_errno==EAGAIN)
				return 0;
			lol_error(0,"[%s]:semop error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			return -1;
		}
	}else{
		sb.sem_flg|=IPC_NOWAIT;
		for(;timeout>=0;lol_sleep(1),timeout--){
			if(semop(mutex->semid,&sb,1)<0){
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
int lol_mutexunlock(LOL_MUTEX *mutex)
{
#ifdef __LOL_UNIX__
	struct sembuf sb;
#endif

#ifdef __LOL_WINDOWS__ /* Windows */
	if(ReleaseSemaphore(mutex->handle,1,NULL)==0){
		lol_error(0,"[%s]:ReleaseSemaphore error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
	sb.sem_num=0;
	sb.sem_op=1;
	sb.sem_flg=SEM_UNDO;
	if(semop(mutex->semid,&sb,1)<0){
		lol_error(0,"[%s]:semop error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif

	return 0;
}
int lol_mutexclose(LOL_MUTEX *mutex)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	CloseHandle(mutex->handle);
#else /* UNIX */
#endif
	free(mutex);

	return 0;
}

