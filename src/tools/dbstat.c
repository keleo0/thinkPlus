#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbstat dbfile";
	LOL_DB *db;
	LOL_DB_FIELD *p;
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
	if((db=lol_db_openx(argv[optind],LOL_DB_RDONLY,""))==NULL){
		lol_showerror();
		exit(-1);
	}

	/* summary info */
	printf("last_modify_date=[%s]\n",db->last_modify_date);
	printf("record_count=[%d]\n",db->record_count);
	printf("head_length=[%d]\n",db->head_length);
	printf("record_length=[%d]\n",db->record_length);
	printf("field_count=[%d]\n",db->field_count);

	/* field info */
	puts("------------------------------------");
	printf("%-12s|%-7s|%s\n","Name","Type","Length");
	puts("------------------------------------");
	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
		printf("%-12s|%-7c|%d\n",p->name,p->type,p->length);
	}

	return 0;
}
