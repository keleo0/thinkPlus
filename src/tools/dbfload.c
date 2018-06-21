#include "lol.h"
#include "lol_dbf.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: dbfload [-d delimiter] dbffile txtfile";
	LOL_DBF *dbf;
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
	if((dbf=lol_dbf_open(argv[optind],LOL_DBF_WRONLY|LOL_DBF_APPEND,""))==NULL){
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
		if(lol_chrstat(linebuf,d)!=dbf->field_count-1){
			printf("field count not match.[recordno=%d]\n",n);
			fclose(fp);
			lol_dbf_close(dbf);
			exit(-1);
		}

		/* init record */
		lol_dbf_new(dbf);

		/* put fields */
		for(i=0;i<dbf->field_count;i++){
			/*field value*/
			bzero(value,sizeof(value));
			lol_strgetfield(linebuf,d,i,value,sizeof(value));
			lol_dbf_putfield(dbf,i,value,strlen(value),0);
		}
		if(lol_dbf_insert(dbf)<0){
			lol_showerror();
			fclose(fp);
			lol_dbf_close(dbf);
			exit(-1);
		}
		n++;
	}
	printf("%d records inserted.\n",n);
	fclose(fp);
	lol_dbf_close(dbf);

	return 0;
}
