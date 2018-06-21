#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: showconf [-d delimiter] conffile";
	LOL_CONF *conf;
	char d='|';
	LOL_OPTARRAY optarray[1]={
		{'d',&d,1,LOL_OPT_NOT_EMPTY|LOL_OPT_ARG,NULL}
	};

	if(lol_getopts(argc,argv,optarray,1)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	if((conf=lol_loadconf(argv[optind],d))==NULL){
		lol_showerror();
		exit(-1);
	}
	lol_showconf(conf);
	lol_freeconf(conf);

	return 0;
}
