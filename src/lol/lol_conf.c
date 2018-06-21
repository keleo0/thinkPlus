#include "lol_error.h"
#include "lol_utility.h"
#include "lol_conf.h"

LOL_CONF *lol_loadconf(const char *file,char d)
{
	char line[1024],value[LOL_CONF_VALUE_SIZE];
	FILE *fp;
	int r,h=0,v=0;
	LOL_CONF *conf;
	LOL_CONF_HLIST *ph,*firsth,*lasth;
	LOL_CONF_VLIST *pv,*firstv,*lastv;

	if((conf=malloc(sizeof(LOL_CONF)))==NULL){
		lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));

		return NULL;
	}
	bzero(conf,sizeof(LOL_CONF));
	conf->d=d;
	conf->hcount=0;
	conf->vcount=0;
	conf->hlist=NULL;
	snprintf(conf->file,LOL_CONF_FILE_SIZE,"%s",file);

	if((fp=fopen(file,"r"))==NULL){
		lol_error(0,"[%s]:file open failed.[%d:%s][file=%s]",__func__,lol_errno,lol_strerror(lol_errno),file);
		lol_freeconf(conf);
		return NULL;
	}
	while(!feof(fp)){
		if(fgets(line,sizeof(line),fp)==NULL)
			break;
		lol_strtrim(line);
		if(line[0]=='#' || strlen(line)==0)
			continue;

		r=lol_chrstat(line,d);	/* r will be used later */
		if(conf->hcount!=0 && r!=conf->vcount-1){
			lol_error(0,"[%s]:column count not match![file=%s][h=%d]",__func__,file,h);
			fclose(fp);
			lol_freeconf(conf);
			return NULL;
		}

		/* put line */
		if((ph=malloc(sizeof(LOL_CONF_HLIST)))==NULL){
			lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			fclose(fp);
			lol_freeconf(conf);
			return NULL;
		}
		ph->vlist=NULL;
		ph->prior=NULL;
		ph->next=NULL;
		if(conf->hlist==NULL){
			ph->prior=ph;
			ph->next=ph;
			conf->hlist=ph;
		}else{
			firsth=conf->hlist;
			lasth=conf->hlist->prior;
			ph->next=firsth;
			ph->prior=lasth;
			firsth->prior=ph;
			lasth->next=ph;
		}

		/* put columns */
		for(v=0;v<=r;v++){
			if(lol_strgetfield(line,d,v,value,sizeof(value))<0){
				lol_error(0,"[%s]:value too long![file=%s][h=%d][v=%d]",__func__,file,h,v);
				fclose(fp);
				lol_freeconf(conf);
				return NULL;
			}
			if((pv=malloc(sizeof(LOL_CONF_VLIST)))==NULL){
				lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
				fclose(fp);
				lol_freeconf(conf);
				return NULL;
			}
			strcpy(pv->value,value);
			pv->prior=NULL;
			pv->next=NULL;
			if(ph->vlist==NULL){
				pv->prior=pv;
				pv->next=pv;
				ph->vlist=pv;
			}else{
				firstv=ph->vlist;
				lastv=ph->vlist->prior;
				pv->next=firstv;
				pv->prior=lastv;
				firstv->prior=pv;
				lastv->next=pv;
			}
		}
		h++;	/* line plus */
		conf->hcount=h;
		if(conf->hcount==1)
			conf->vcount=r+1;
	}
	fclose(fp);

	return conf;
}
int lol_getconf(LOL_CONF *conf,int h,int v,void *value,unsigned int size,int flags)
{
	LOL_CONF_HLIST *ph;
	LOL_CONF_VLIST *pv;
	int i;
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if(h>=conf->hcount || v>=conf->vcount){
		lol_error(0,"[%s]:h or v out of range.[h=%d,v=%d]",__func__,h,v);
		return -1;
	}
	ph=conf->hlist;
	for(i=0;i<h;i++)
		ph=ph->next;
	pv=ph->vlist;
	for(i=0;i<v;i++)
		pv=pv->next;
	if(strlen(pv->value)==0 && (flags & LOL_CONF_NOT_EMPTY)){
		lol_error(0,"[%s]:field value is empty.[file=%s][h=%d][v=%d]",__func__,conf->file,h,v);
		return -1;
	}
	switch(flags & LOL_CONF_MASK_TYPE){
		case 0:
		case LOL_CONF_CHAR:
			if(strlen(pv->value)>=size){
				if(size!=1 || (size==1 && strlen(pv->value)>1)){
					lol_error(0,"[%s]:value too long![file=%s][h=%d][v=%d][value=%s]",__func__,conf->file,h,v,pv->value);
					return -1;
				}
			}
			if(size==1)
				memcpy(value,pv->value,1);
			else
				strcpy(value,pv->value);
			return strlen(pv->value);
		case LOL_CONF_SHORT:
			if(size!=2){
				lol_error(0,"[%s]:field size not equal to 2 with short type![size=%d][h=%d][v=%d]",__func__,size,h,v);
				return -1;
			}
			t_short=atol(pv->value);
			memcpy(value,&t_short,2);
			break;
		case LOL_CONF_INT:
			if(size!=4){
				lol_error(0,"[%s]:field size not equal to 4 with int type![size=%d][h=%d][v=%d]",__func__,size,h,v);
				return -1;
			}
			t_int=atol(pv->value);
			memcpy(value,&t_int,4);
			break;
		case LOL_CONF_LONG:
			if(size!=8){
				lol_error(0,"[%s]:field size not equal to 8 with long type![size=%d][h=%d][v=%d]",__func__,size,h,v);
				return -1;
			}
			t_long=atoll(pv->value);
			memcpy(value,&t_long,8);
			break;
		case LOL_CONF_FLOAT:
			if(size!=4){
				lol_error(0,"[%s]:field size not equal to 4 with float type![size=%d][h=%d][v=%d]",__func__,size,h,v);
				return -1;
			}
			t_float=atof(pv->value);
			memcpy(value,&t_float,4);
			break;
		case LOL_CONF_DOUBLE:
			if(size!=8){
				lol_error(0,"[%s]:field size not equal to 8 with double type![size=%d][h=%d][v=%d]",__func__,size,h,v);
				return -1;
			}
			t_double=atof(pv->value);
			memcpy(value,&t_double,8);
			break;
		default:
			lol_error(0,"[%s]:illegal field type.[type='%c'][h=%d][v=%d]",__func__,flags&LOL_CONF_MASK_TYPE,h,v);
			return -1;
	}

	return 0;
}
int lol_showconf(LOL_CONF *conf)
{
	LOL_CONF_HLIST *ph;
	LOL_CONF_VLIST *pv;
	int i,j;

	if(conf->hcount==0){
		printf("conflist is empty.");
		return 0;
	}
	ph=conf->hlist;
	for(i=0;i<conf->hcount;i++){
		pv=ph->vlist;
		for(j=0;j<conf->vcount;j++){
			if(j==0)
				printf("%s",pv->value);
			else
				printf("|%s",pv->value);
			pv=pv->next;
		}
		printf("\n");
		ph=ph->next;
	}

	return 0;
}
int lol_freeconf(LOL_CONF *conf)
{
	LOL_CONF_HLIST *ph;
	LOL_CONF_VLIST *pv;

	while(conf->hlist){
		ph=conf->hlist;
		while(ph->vlist){
			pv=ph->vlist;
			if(pv==pv->next){
				free(pv);
				break;
			}
			pv->next->prior=pv->prior;
			pv->prior->next=pv->next;
			ph->vlist=pv->next;
			free(pv);
		}
		if(ph==ph->next){
			free(ph);
			break;
		}
		ph->next->prior=ph->prior;
		ph->prior->next=ph->next;
		conf->hlist=ph->next;
		free(ph);
	}
	free(conf);

	return 0;
}
