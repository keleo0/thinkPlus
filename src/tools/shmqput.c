#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: shmqput [-n name] message";
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
	if(argc-optind!=1){
		puts(usage);
		return -1;
	}
	flags=0777;
	if((shmq=lol_shmqopen(shmqname,0,flags))==NULL){
		lol_showerror();
		return -1;
	}
	if(lol_shmqput(shmq,argv[optind],strlen(argv[optind]),-1)<0){
		lol_showerror();
		return -1;
	}
	lol_shmqclose(shmq);

	return 0;
}
