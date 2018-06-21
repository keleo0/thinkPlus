#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: showcfg [-p cfgpath] cfgfile";
	LOL_CFG *cfg;
	char cfgpath[256];
	LOL_OPTARRAY optarray[1]={
		{'p',cfgpath,sizeof(cfgpath),LOL_OPT_ARG,NULL}
	};

	memset(cfgpath,0x00,sizeof(cfgpath));
	if(lol_getopts(argc,argv,optarray,1)<0){
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	if((cfg=lol_loadcfg(argv[optind],cfgpath))==NULL){
		lol_showerror();
		exit(-1);
	}
	lol_showcfg(cfg);
	lol_freecfg(cfg);

	return 0;
}
