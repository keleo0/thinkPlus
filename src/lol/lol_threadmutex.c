#include "lol_error.h"
#include "lol_utility.h"
#include "lol_threadmutex.h"

LOL_THREADMUTEX *lol_threadmutex_open()
{
	LOL_THREADMUTEX *threadmutex;

	if((threadmutex=malloc(sizeof(LOL_THREADMUTEX)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
#ifdef __LOL_WINDOWS__ /* Windows */
	if((threadmutex->handle=CreateSemaphore(NULL,1,65536,NULL))==NULL){
		lol_error(0,"[%s]:CreateSemaphore error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(threadmutex);
		return NULL;
	}
#else /* UNIX */
	if((threadmutex->threadmutex=malloc(sizeof(pthread_mutex_t)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(threadmutex);
		return NULL;
	}
	if(pthread_mutex_init(threadmutex->threadmutex,NULL)<0){
		lol_error(0,"[%s]:pthread_mutex_init error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(threadmutex);
		return NULL;
	}
#endif

	return threadmutex;
}
int lol_threadmutex_lock(LOL_THREADMUTEX *threadmutex,int timeout)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	int r;

	if(timeout<0)
		timeout=INFINITE;
	if((r=WaitForSingleObject(threadmutex->handle,timeout))!=WAIT_OBJECT_0){
		if(r==WAIT_TIMEOUT)
			return 0;
		lol_error(0,"[%s]:WaitForSingleObject error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
	if(timeout<0){
		if(pthread_mutex_lock(threadmutex->threadmutex)<0){
			lol_error(0,"[%s]:pthread_mutex_lock error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			return -1;
		}
	}else{
		for(;timeout>=0;timeout--){
			if(pthread_mutex_trylock(threadmutex->threadmutex)<0){
				if(lol_errno==EBUSY){
					lol_sleep(1);
					continue;
				}
				lol_error(0,"[%s]:pthread_mutex_lock error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
				return -1;
			}
			break;
		}
		if(timeout<0)
			return 0;
	}
#endif

	return 1;
}
int lol_threadmutex_unlock(LOL_THREADMUTEX *threadmutex)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	if(ReleaseSemaphore(threadmutex->handle,1,NULL)==0){
		lol_error(0,"[%s]:ReleaseSemaphore error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
	if(pthread_mutex_unlock(threadmutex->threadmutex)<0){
		lol_error(0,"[%s]:pthread_mutex_unlock error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif
	
	return 0;
}
int lol_threadmutex_close(LOL_THREADMUTEX *threadmutex)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	CloseHandle(threadmutex->handle);
#else /* UNIX */
	pthread_mutex_destroy(threadmutex->threadmutex);
	free(threadmutex->threadmutex);
#endif
	free(threadmutex);

	return 0;
}

