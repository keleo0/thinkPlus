#include "lol.h"
#include "lol_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfcreate dbffile conffile";
	LOL_DBF *dbf;
	int flags;

	if(lol_getopts(argc,argv,NULL,0)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=2){
		puts(usage);
		exit(-1);
	}
	flags=LOL_DBF_CREAT|LOL_DBF_TRUNC;
	if((dbf=lol_dbf_open(argv[optind],flags,argv[optind+1]))==NULL){
		lol_showerror();
		exit(-1);
	}
	lol_dbf_close(dbf);

	return 0;
}
