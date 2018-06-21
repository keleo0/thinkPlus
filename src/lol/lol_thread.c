#include "lol_error.h"
#include "lol_thread.h"

#ifdef __LOL_WINDOWS__ /* Windows */
int WINAPI lol_os_threadroutine(void *arg)
{
	LOL_THREADARG *threadarg;

	threadarg=arg;
	threadarg->routine(threadarg->arg);
	free(arg);

	return 0;
}
#else /* UNIX */
void *lol_os_threadroutine(void *arg)
{
	LOL_THREADARG *threadarg;

	threadarg=arg;
#if defined(__LOL_AIX__) || defined(__LOL_HPUX__)
	if(threadarg->flags & LOL_THREAD_SUSPEND)
		pthread_suspend(pthread_self());
#endif
	threadarg->routine(threadarg->arg);
	free(arg);

	return NULL;
}
#endif

LOL_THREAD *lol_threadcreate(LOL_THREADROUTINE routine,void *arg,int flags)
{
	LOL_THREAD *thread;
	LOL_THREADARG *threadarg;
#ifdef __LOL_WINDOWS__
	int oflags=0;
#endif

	if((thread=malloc(sizeof(LOL_THREAD)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}

	if((threadarg=malloc(sizeof(LOL_THREADARG)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(thread);
		return NULL;
	}
	threadarg->routine=routine;
	threadarg->arg=arg;
	threadarg->flags=flags;
#ifdef __LOL_WINDOWS__ /* Windows */
	if(flags & LOL_THREAD_SUSPEND)
		oflags|=CREATE_SUSPENDED;
	if((thread->handle=CreateThread(NULL,0,lol_os_threadroutine,threadarg,oflags,&thread->threadid))==NULL){
		lol_error(0,"[%s]:CreateThread error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(threadarg);
		free(thread);
		return NULL;
	}
#else /* UNIX */
	if(pthread_create(&thread->thread,NULL,lol_os_threadroutine,threadarg)<0){
		lol_error(0,"[%s]:pthread_create error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(threadarg);
		free(thread);
		return NULL;
	}
	thread->threadid=(int)thread->thread;
#endif

	return thread;
}
int lol_threadid()
{
	int threadid;

#ifdef __LOL_WINDOWS__ /* Windows */
	threadid=GetCurrentThreadId();
#else /* UNIX */
	threadid=(int)pthread_self();
#endif

	return threadid;
}
int lol_threadexit(int exitcode)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	ExitThread(exitcode);
#else /* UNIX */
	pthread_exit((void *)exitcode);
#endif

	return 0;
}
int lol_threadsuspend(LOL_THREAD *thread)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	if(SuspendThread(thread->handle)<0){
		lol_error(0,"[%s]:SuspendThread error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
#if defined(__LOL_AIX__) || defined(__LOL_HPUX__)
	if(pthread_suspend(thread->thread)<0){
		lol_error(0,"[%s]:pthread_suspend error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif
#endif /* Windows */
	
	return 0;
}
int lol_threadcontinue(LOL_THREAD *thread)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	if(ResumeThread(thread->handle)<0){
		lol_error(0,"[%s]:ResumeThread error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
#if defined(__LOL_AIX__) || defined(__LOL_HPUX__)
	if(pthread_continue(thread->thread)<0){
		lol_error(0,"[%s]:pthread_continue error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif
#endif /* Windows */
	
	return 0;
}
int lol_threadcancel(LOL_THREAD *thread)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	if(TerminateThread(thread->handle,0)==0){
		lol_error(0,"[%s]:TerminateThread error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#else /* UNIX */
	if(pthread_cancel(thread->thread)<0){
		lol_error(0,"[%s]:pthread_cancel error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif
	free(thread);

	return 0;
}
int lol_threadjoin(LOL_THREAD *thread,int *exitcode,int timeout)
{
#ifdef __LOL_WINDOWS__ /* Windows */
	int r;

	if(timeout<0)
		timeout=INFINITE;
	if((r=WaitForSingleObject(thread->handle,timeout))!=WAIT_OBJECT_0){
		if(r==WAIT_TIMEOUT)
			return 0;
		lol_error(0,"[%s]:WaitForSingleObject error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
	if(exitcode)
		GetExitCodeThread(thread->handle,exitcode);
#else /* UNIX */
	if(pthread_join(thread->thread,(void **)exitcode)<0){
		lol_error(0,"[%s]:pthread_join error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif
	free(thread);

	return 1;
}
int lol_thread_detach(LOL_THREAD *thread)
{
#ifdef __LOL_WINDOWS__ /* Windows */
#else /* UNIX */
	if(pthread_detach(thread->thread)<0){
		lol_error(0,"[%s]:pthread_detach error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
#endif
	
	return 0;
}
