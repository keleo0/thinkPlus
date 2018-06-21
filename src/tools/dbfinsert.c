#include "lol.h"
#include "lol_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfinsert [-d delimiter] dbffile record";
	LOL_DBF *dbf;
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

	if((dbf=lol_dbf_open(argv[optind],LOL_DBF_WRONLY|LOL_DBF_APPEND|LOL_DBF_SYNC,""))==NULL){
		lol_showerror();
		exit(-1);
	}
	/*check fields*/
	if(lol_chrstat(argv[optind+1],d)!=dbf->field_count-1){
		printf("field num not match.\n");
		lol_dbf_close(dbf);
		exit(-1);
	}
	lol_dbf_new(dbf);
	/*init record*/
	for(i=0;i<dbf->field_count;i++){
		/*field value*/
		bzero(value,sizeof(value));
		lol_strgetfield(argv[optind+1],d,i,value,sizeof(value));
		lol_dbf_putfield(dbf,i,value,strlen(value),0);
	}
	if(lol_dbf_insert(dbf)<0){
		lol_showerror();
		lol_dbf_close(dbf);
		exit(-1);
	}
	lol_dbf_close(dbf);

	return 0;
}
