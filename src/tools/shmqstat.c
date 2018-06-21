#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: shmqsend [-n name]";
	LOL_SHMQ *shmq;
	int flags;
	char shmqname[128];
	LOL_OPTARRAY optarray[1]={
		{'n',shmqname,sizeof(shmqname),LOL_OPT_ARG|LOL_OPT_NOT_EMPTY,NULL}
	};
	LOL_ENVARRAY envarray[1]={
		{"SHMQ_NAME",shmqname,sizeof(shmqname),LOL_ENV_NOT_EMPTY,NULL}
	};

	if(lol_getenvs(envarray,1)<0){
		lol_showerror();
		return -1;
	}
	if(lol_getopts(argc,argv,optarray,1)<0){
		lol_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind!=0){
		puts(usage);
		return -1;
	}
	flags=0777;
	if((shmq=lol_shmqopen(shmqname,0,flags))==NULL){
		lol_showerror();
		return -1;
	}
	lol_mutexlock(shmq->mutex,-1);
	printf("size:%d\n",shmq->head->size);
	printf("totalsize:%d\n",shmq->head->totalsize);
	printf("freesize:%d\n",shmq->head->freesize);
	printf("head:%d\n",shmq->head->head);
	printf("tail:%d\n",shmq->head->tail);
	printf("msgnum:%d\n",shmq->head->msgnum);
	lol_mutexunlock(shmq->mutex);
	lol_shmqclose(shmq);

	return 0;
}
