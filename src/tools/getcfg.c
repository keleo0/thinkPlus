#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: getcfg [-p cfgpath] [-f field] cfgfile name";
	char cfgpath[256],field[128],value[1024];
	LOL_CFG *cfg;
	LOL_OPTARRAY optarray[2]={
		{'p',cfgpath,sizeof(cfgpath),LOL_OPT_ARG,NULL},
		{'f',field,sizeof(field),LOL_OPT_ARG,NULL}
	};

	memset(cfgpath,0x00,sizeof(cfgpath));
	memset(field,0x00,sizeof(field));
	if(lol_getopts(argc,argv,optarray,2)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=2){
		puts(usage);
		exit(-1);
	}
	if((cfg=lol_loadcfg(argv[optind],cfgpath))==NULL){
		lol_showerror();
		exit(-1);
	}
	if(lol_getcfg(cfg,field,argv[optind+1],value,sizeof(value),0)<0){
		lol_showerror();
		exit(-1);
	}
	printf("%s",value);
	lol_freecfg(cfg);

	return 0;
}
