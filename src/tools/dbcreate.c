#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbcreate dbfile conffile";
	LOL_DB *db;
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
	flags=LOL_DB_CREAT|LOL_DB_TRUNC;
	if((db=lol_db_openx(argv[optind],flags,argv[optind+1]))==NULL){
		lol_showerror();
		exit(-1);
	}
	lol_db_close(db);

	return 0;
}
