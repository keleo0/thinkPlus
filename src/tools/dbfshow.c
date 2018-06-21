#include "lol.h"
#include "lol_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfshow dbffile";
	LOL_DBF *dbf;
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
	if((dbf=lol_dbf_open(argv[optind],LOL_DBF_RDONLY,""))==NULL){
		lol_showerror();
		exit(-1);
	}
	/* show title */
	for(i=0;i<dbf->field_count;i++){
		if(i==0){
			if((dbf->field_array+i)->type=='C'){
				printf("%-*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}else{
				printf("%*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}
		}else{
			if((dbf->field_array+i)->type=='C'){
				printf("|%-*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}else{
				printf("|%*s",(dbf->field_array+i)->length
				>strlen((dbf->field_array+i)->name)
				?(dbf->field_array+i)->length
				:strlen((dbf->field_array+i)->name)
				,(dbf->field_array+i)->name);
			}
		}
	}
	printf("\n");
	while(lol_dbf_fetch(dbf)==1){
		for(i=0;i<dbf->field_count;i++){
			lol_dbf_getfield(dbf,i,value,sizeof(value),0);
			if(i==0){
				if((dbf->field_array+i)->type=='C'){
					printf("%-*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}else{
					printf("%*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}
			}else{
				if((dbf->field_array+i)->type=='C'){
					printf("|%-*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}else{
					printf("|%*s",(dbf->field_array+i)->length
					>strlen((dbf->field_array+i)->name)
					?(dbf->field_array+i)->length
					:strlen((dbf->field_array+i)->name)
					,value);
				}
			}
		}
		printf("\n");
	}
	printf("record count: %d\n",dbf->record_count);
	lol_dbf_close(dbf);

	return 0;
}
