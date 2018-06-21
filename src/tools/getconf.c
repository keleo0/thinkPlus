#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: getconf [-d delimiter] conffile h v";
	LOL_CONF *conf;
	char d='|',value[1024];
	LOL_OPTARRAY optarray[1]={
		{'d',&d,1,LOL_OPT_NOT_EMPTY|LOL_OPT_ARG,NULL}
	};

	if(lol_getopts(argc,argv,optarray,1)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=3){
		puts(usage);
		exit(-1);
	}
	if((conf=lol_loadconf(argv[optind],d))==NULL){
		lol_showerror();
		exit(-1);
	}
	if(lol_getconf(conf,atol(argv[optind+1]),atol(argv[optind+2]),value,sizeof(value),0)<0){
		lol_showerror();
		exit(-1);
	}
	printf("%s",value);
	lol_freeconf(conf);

	return 0;
}
