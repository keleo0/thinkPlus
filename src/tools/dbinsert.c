#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbinsert [-d delimiter] dbfile record";
	LOL_DB *db;
	int i;
	char d='|',value[256];
	LOL_OPTARRAY optarray[1]={
		{'d',&d,1,LOL_OPT_ARG|LOL_OPT_NOT_EMPTY,NULL}
	};

	if(lol_getopts(argc,argv,optarray,1)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=2){
		puts(usage);
		exit(-1);
	}

	if((db=lol_db_openx(argv[optind],LOL_DB_WRONLY|LOL_DB_APPEND|LOL_DB_SYNC,""))==NULL){
		lol_showerror();
		exit(-1);
	}
	/*check fields*/
	if(lol_chrstat(argv[optind+1],d)!=db->field_count-1){
		printf("field num not match.\n");
		lol_db_close(db);
		exit(-1);
	}
	lol_db_new(db);
	/*init record*/
	for(i=0;i<db->field_count;i++){
		/*field value*/
		bzero(value,sizeof(value));
		lol_strgetfield(argv[optind+1],d,i,value,sizeof(value));
		lol_db_putfield(db,i,value,strlen(value),0);
	}
	if(lol_db_insert(db)<0){
		lol_showerror();
		lol_db_close(db);
		exit(-1);
	}
	lol_db_close(db);

	return 0;
}
