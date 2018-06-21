#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbshow dbfile";
	LOL_DB *db;
	int i;
	char value[256];

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
	/* show title */
	for(i=0;i<db->field_count;i++){
		if(i==0){
			if((db->field_array+i)->type=='C'){
				printf("%-*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}else{
				printf("%*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}
		}else{
			if((db->field_array+i)->type=='C'){
				printf("|%-*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}else{
				printf("|%*s",(db->field_array+i)->length
				>strlen((db->field_array+i)->name)
				?(db->field_array+i)->length
				:strlen((db->field_array+i)->name)
				,(db->field_array+i)->name);
			}
		}
	}
	printf("\n");
	while(lol_db_fetch(db)==1){
		for(i=0;i<db->field_count;i++){
			lol_db_getfield(db,i,value,sizeof(value),0);
			if(i==0){
				if((db->field_array+i)->type=='C'){
					printf("%-*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}else{
					printf("%*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}
			}else{
				if((db->field_array+i)->type=='C'){
					printf("|%-*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}else{
					printf("|%*s",(db->field_array+i)->length
					>strlen((db->field_array+i)->name)
					?(db->field_array+i)->length
					:strlen((db->field_array+i)->name)
					,value);
				}
			}
		}
		printf("\n");
	}
	printf("record count: %d\n",db->record_count);
	lol_db_close(db);

	return 0;
}
