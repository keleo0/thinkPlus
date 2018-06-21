#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: shmqcreate [-te] name size";
	LOL_SHMQ *shmq;
	int flags,T_Exists,E_Exists;
	LOL_OPTARRAY optarray[2]={
		{'t',NULL,0,0,&T_Exists},
		{'e',NULL,0,0,&E_Exists}
	};

	if(lol_getopts(argc,argv,optarray,2)<0){
		lol_showerror();
		puts(usage);
		return -1;
	}
	if(argc-optind!=2){
		puts(usage);
		return -1;
	}
	flags=LOL_SHMQ_CREAT|0777;
	if(T_Exists)
		flags|=LOL_SHMQ_TRUNC;
	if(E_Exists)
		flags|=LOL_SHMQ_EXCL;
	if((shmq=lol_shmqopen(argv[optind],atol(argv[optind+1]),flags))==NULL){
		lol_showerror();
		return -1;
	}
#ifdef __LOL_WINDOWS__
	getchar();
#else
	lol_shmqclose(shmq);
#endif

	return 0;
}
