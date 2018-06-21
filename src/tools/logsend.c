#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: logsend [-a addr] [-n name] [-TELP] [-l level] [-d desc] info";
	LOL_LOG *lp;
	char addr[256],name[256],desc[32],info[1024];
	int T_Exists,E_Exists,L_Exists,P_Exists;
	int flags,level;
	LOL_OPTARRAY optarray[8]={
		{'a',addr,sizeof(addr),LOL_OPT_ARG,NULL},
		{'n',name,sizeof(name),LOL_OPT_ARG,NULL},
		{'T',NULL,0,0,&T_Exists},
		{'E',NULL,0,0,&E_Exists},
		{'L',NULL,0,0,&L_Exists},
		{'P',NULL,0,0,&P_Exists},
		{'l',&level,sizeof(level),LOL_OPT_ARG|LOL_OPT_INT,NULL},
		{'d',desc,sizeof(desc),LOL_OPT_ARG,NULL}
	};
	LOL_ENVARRAY envarray[2]={
		{"LOG_ADDR",addr,sizeof(addr),0,NULL},
		{"LOG_NAME",name,sizeof(name),0,NULL}
	};

	bzero(addr,sizeof(addr));
	bzero(name,sizeof(name));
	level=0;
	bzero(desc,sizeof(desc));
	if(lol_getenvs(envarray,2)<0){
		lol_showerror();
		exit(-1);
	}
	if(lol_getopts(argc,argv,optarray,8)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	flags=0;
	if(T_Exists)
		flags|=LOL_LOG_TRUNC;
	if(E_Exists)
		flags|=LOL_LOG_EXCL;
	if(L_Exists)
		flags|=LOL_LOG_LOCAL;
	flags|=LOL_LOG_DEBUG;
	/* net start */
	lol_netstart();

	if((lp=lol_logopen(addr,name,flags))==NULL){
		lol_showerror();
		exit(-1);
	}
	flags=0;
	if(P_Exists)
		flags|=LOL_LOG_PRINT;
	flags|=level;
	snprintf(info,sizeof(info),"%s",argv[optind]);
	if(lol_logsend(lp,flags,desc,info)<0){
		lol_showerror();
		lol_logclose(lp);
		exit(-1);
	}
	lol_logclose(lp);

	/* net close */
	lol_netstop();

	return 0;
}
