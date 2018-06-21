#include "lol.h"
#include "lol_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfstat dbffile";
	LOL_DBF *dbf;
	LOL_DBF_FIELD *p;
	int i;

	if(lol_getopts(argc,argv,NULL,0)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	if((dbf=lol_dbf_open(argv[optind],LOL_DBF_RDONLY,""))==NULL){
		lol_showerror();
		exit(-1);
	}

	/* summary info */
	printf("last_modify_date=[%s]\n",dbf->last_modify_date);
	printf("record_count=[%d]\n",dbf->record_count);
	printf("head_length=[%d]\n",dbf->head_length);
	printf("record_length=[%d]\n",dbf->record_length);
	printf("field_count=[%d]\n",dbf->field_count);

	/* field info */
	puts("------------------------------------");
	printf("%-12s|%-7s|%s\n","Name","Type","Length");
	puts("------------------------------------");
	for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
		printf("%-12s|%-7c|%d\n",p->name,p->type,p->length);
	}

	return 0;
}
