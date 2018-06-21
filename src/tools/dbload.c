#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbload [-d delimiter] dbfile txtfile";
	LOL_DB *db;
	FILE *fp;
	int i,n;
	char d='|',value[256],linebuf[1024];
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
	if((db=lol_db_openx(argv[optind],LOL_DB_WRONLY|LOL_DB_APPEND,""))==NULL){
		lol_showerror();
		exit(-1);
	}
	if((fp=fopen(argv[optind+1],"r"))==NULL){
		printf("file open failed![errno=%d][file=%s]",errno,argv[1]);
		exit(-1);
	}
	n=0;
	while(1){
		bzero(linebuf,sizeof(linebuf));
		if(fgets(linebuf,sizeof(linebuf),fp)==NULL)
			break;
		lol_strtrim(linebuf);
		if(strlen(linebuf)==0)
			continue;

		/*check fields*/
		if(lol_chrstat(linebuf,d)!=db->field_count-1){
			printf("field count not match.[recordno=%d]\n",n);
			fclose(fp);
			lol_db_close(db);
			exit(-1);
		}

		/* init record */
		lol_db_new(db);

		/* put fields */
		for(i=0;i<db->field_count;i++){
			/*field value*/
			bzero(value,sizeof(value));
			lol_strgetfield(linebuf,d,i,value,sizeof(value));
			lol_db_putfield(db,i,value,strlen(value),0);
		}
		if(lol_db_insert(db)<0){
			lol_showerror();
			fclose(fp);
			lol_db_close(db);
			exit(-1);
		}
		n++;
	}
	printf("%d records inserted.\n",n);
	fclose(fp);
	lol_db_close(db);

	return 0;
}
