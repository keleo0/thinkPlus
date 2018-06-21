#include "lol.h"

LOL_THREADMUTEX *threadmutex;

void mprintf(const char *fmt,...)
{
	va_list args;
	
	lol_threadmutex_lock(threadmutex,-1);
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	lol_threadmutex_unlock(threadmutex);
}

void func1(void *s)
{
	const char *str;
	int i;

	str=s;
	for(i=0;i<15;i++){
		lol_sleep(1000);
		mprintf("threadid=%d str=%s\n",lol_threadid(),str);
	}
	lol_threadexit(1);
}
void func2(void *s)
{
	const char *str;
	int i;

	str=s;
	for(i=0;i<20;i++){
		lol_sleep(1000);
		mprintf("threadid=%d str=%s\n",lol_threadid(),str);
	}
	lol_threadexit(9);
}

int main(int argc,char *argv[])
{
	LOL_THREAD *thread1,*thread2;
	int exitcode,r;

	if((threadmutex=lol_threadmutex_open())==NULL){
		lol_showerror();
		return -1;
	}
	if((thread1=lol_threadcreate(func1,"thread1",0))==NULL){
		lol_showerror();
		return -1;
	}
	if((thread2=lol_threadcreate(func2,"thread2",0))==NULL){
		lol_showerror();
		return -1;
	}
	lol_sleep(3000);
	lol_threadsuspend(thread1);
	lol_sleep(3000);
	lol_threadcontinue(thread1);

	lol_sleep(3000);
	lol_threadsuspend(thread2);
	lol_sleep(3000);
	lol_threadcontinue(thread2);

	mprintf("waiting for thread1 to exit ...\n");
	if((r=lol_threadjoin(thread1,&exitcode,10*1000))<0){
		lol_showerror();
		return -1;
	}
	if(r==0)
		mprintf("wait thread1 timeout.");
	else
		mprintf("thread1 exit %d\n",exitcode);

	mprintf("waiting for thread2 to exit ...\n");
	if((r=lol_threadjoin(thread2,&exitcode,10*1000))<0){
		lol_showerror();
		return -1;
	}
	if(r==0)
		mprintf("wait thread2 timeout.");
	else
		mprintf("thread2 exit %d\n",exitcode);

	lol_threadmutex_close(threadmutex);

	return 0;
}
