#include "lol_error.h"
#include "lol_utility.h"
#include "lol_cfg.h"

int __lol_loadcfg(LOL_CFG *cfg,const char *file,const char *path);
int __lol_putcfg(LOL_CFG *cfg,const char *field,const char *name,const char *value,const char *file,unsigned int lineno);
LOL_CFG_CFGLIST *__lol_findcfg(LOL_CFG *cfg,const char *field,const char *name);

LOL_CFG *lol_loadcfg(const char *file,const char *path)
{
	LOL_CFG *cfg;

	if((cfg=(LOL_CFG *)malloc(sizeof(LOL_CFG)))==NULL){
		lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	memset(cfg,0x00,sizeof(LOL_CFG));
	if(__lol_loadcfg(cfg,file,path)<0){	/* recursion */
		lol_errorerror();
		lol_freecfg(cfg);
		return NULL;
	}

	return cfg;
}
int __lol_loadcfg(LOL_CFG *cfg,const char *file,const char *path)
{
	char field[LOL_CFG_FIELD_SIZE],name[LOL_CFG_NAME_SIZE],value[LOL_CFG_VALUE_SIZE];
	char cfgfile[LOL_CFG_FILE_SIZE],cfgpath[LOL_CFG_FILE_SIZE];
	char line[1024];
	FILE *fp=NULL;
	int r,i,icount,includeflag=1;
	unsigned int lineno=0;

	if(*file=='/' || strlen(path)==0){	/* abs path or needn't search */
		if((fp=fopen(file,"r"))==NULL){
			lol_error(0,"[%s]:file open failed![%d:%s][file=%s]",__func__,lol_errno,lol_strerror(lol_errno),file);
			return -1;
		}
		snprintf(cfgfile,sizeof(cfgfile),"%s",file);
	}else{
		icount=lol_chrstat(path,':');
		for(i=0;i<=icount;i++){
			if((r=lol_strgetfield(path,':',i,cfgpath,sizeof(cfgpath)))<0){	/* r will be used later */
				lol_errorerror();
				return -1;
			}
			if(r>0){
				if(strcmp(cfgpath,"/")!=0){
					if(cfgpath[r-1]=='/')
						cfgpath[r-1]='\0';
				}
				if(strcmp(cfgpath,"/")!=0)
					snprintf(cfgfile,sizeof(cfgfile),"%s/%s",cfgpath,file);
				else
					snprintf(cfgfile,sizeof(cfgfile),"/%s",file);
			}else{
				snprintf(cfgfile,sizeof(cfgfile),"%s",file);
			}

			if(lolaccess(cfgfile,F_OK)<0){
				continue;
			}
			if((fp=fopen(cfgfile,"r"))==NULL){
				lol_error(0,"[%s]:file open failed![%d:%s][file=%s]",__func__,lol_errno,lol_strerror(lol_errno),cfgfile);
				return -1;
			}
			break;
		}
		if(fp==NULL){
			lol_error(0,"[%s]:file not found![path=%s]",__func__,path);
			return -1;
		}
	}

	bzero(field,sizeof(field));
	while(!feof(fp)){
		if(fgets(line,sizeof(line),fp)==NULL)
			break;
		lineno++;	/* line no */
		lol_strtrim(line);
		if(strlen(line)==0)	/* empty line */
			continue;
		if(line[0]=='#'){	/* include or note */
			line[0]=' ';	/* space to trim */
			if((r=lol_chrfind(line,'#',0))>0)	/* rm note */
				line[r]='\0';
			lol_strtrim(line);
			if(strncmp(line,"include",7)==0){	/* include */
				if(includeflag==0){
					lol_error(0,"[%s]:include must be ahead of cfgs![%s:%d]",__func__,cfgfile,lineno);
					fclose(fp);
					return -1;
				}
				memset(line,' ',7);
				lol_strtrim(line);
				if(*line!='<' || lol_chrstat(line,'>')!=1 || line[strlen(line)-1]!='>'){
					lol_error(0,"[%s]:illegal format![%s:%d]",__func__,cfgfile,lineno);
					fclose(fp);
					return -1;
				}
				line[0]=' ';
				line[strlen(line)-1]=' ';
				lol_strtrim(line);	/* include file */
				if(__lol_loadcfg(cfg,line,path)<0){	/* recursion load */
					fclose(fp);
					return -1;
				}
				continue;
			}else{	/* note */
				continue;
			}
		}
		if((r=lol_chrfind(line,'#',0))>0)	/* rm note */
			line[r]='\0';
		lol_strtrim(line);
		if(*line=='['){	/* field */
			if(lol_chrstat(line,']')!=1 || line[strlen(line)-1]!=']'){
				lol_error(0,"[%s]:illegal format![%s:%d]",__func__,cfgfile,lineno);
				fclose(fp);
				return -1;
			}
			line[0]=' ';
			line[strlen(line)-1]=' ';
			lol_strtrim(line);
			if(strlen(line)>=LOL_CFG_FIELD_SIZE){
				lol_error(0,"[%s]:field name too long![%s:%d][field=%s]",__func__,cfgfile,lineno,line);
				fclose(fp);
				return -1;
			}
			strcpy(field,line);
			includeflag=0;
			continue;
		}
		/* name = value */
		if(lol_chrstat(line,'=')!=1){
			lol_error(0,"[%s]:illegal format![%s:%d][field=%s]",__func__,cfgfile,lineno,field);
			fclose(fp);
			return -1;
		}
		r=lol_chrfind(line,'=',0);	/* r will be used later */
		/* name */
		bzero(name,sizeof(name));
		line[r]='\0';	/* for trim name */
		lol_strtrim(line);
		if(strlen(line)>=LOL_CFG_NAME_SIZE){
			lol_error(0,"[%s]:name too long![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,line);
			fclose(fp);
			return -1;
		}
		if(strlen(line)==0){
			lol_error(0,"[%s]:name is empty![%s:%d][field=%s]",__func__,cfgfile,lineno,field);
			fclose(fp);
			return -1;
		}
		strcpy(name,line);
		/* value */
		bzero(value,sizeof(value));
		memset(line,' ',r+1);	/* r used here */
		lol_strtrim(line);
		if(*line=='$'){
			if(lol_chrstat(line,'$')!=1){
				lol_error(0,"[%s]:too many '$' in value![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,name);
				fclose(fp);
				return -1;
			}
			line[0]=' ';
			lol_strtrim(line);
			if(strlen(line)>=LOL_CFG_VALUE_SIZE){
				lol_error(0,"[%s]:value too long![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,name);
				fclose(fp);
				return -1;
			}
			if(lol_getcfg(cfg,field,line,value,sizeof(value),0)<0){
				lol_errorerror();
				fclose(fp);
				return -1;
			}
		}else{
			if(strlen(line)>=LOL_CFG_VALUE_SIZE){
				lol_error(0,"[%s]:value too long![%s:%d][field=%s][name=%s]",__func__,cfgfile,lineno,field,name);
				fclose(fp);
				return -1;
			}
			strcpy(value,line);
		}
		/* putcfg */
		if(__lol_putcfg(cfg,field,name,value,cfgfile,lineno)<0){
			lol_errorerror();
			fclose(fp);
			return -1;
		}
		cfg->cfgcount++;
		includeflag=0;
	}
	fclose(fp);

	return 0;
}
int __lol_putcfg(LOL_CFG *cfg,const char *field,const char *name,const char *value,const char *file,unsigned int lineno)
{
	LOL_CFG_CFGLIST *p,*first,*last,*cfglist;

	cfglist=cfg->cfglist;
	if((p=__lol_findcfg(cfg,field,name))!=NULL){
		strcpy(p->value,value);
		return 0;
	}
	if((p=malloc(sizeof(LOL_CFG_CFGLIST)))==NULL){
		lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
	memset(p,0x00,sizeof(LOL_CFG_CFGLIST));
	strcpy(p->field,field);
	strcpy(p->name,name);
	strcpy(p->value,value);
	strcpy(p->file,file);
	p->lineno=lineno;
	p->prior=NULL;
	p->next=NULL;
	if(cfglist==NULL){
		p->prior=p;
		p->next=p;
		cfg->cfglist=p;	/* cfg->cfglist */
	}else{
		first=cfglist;
		last=cfglist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}

	return 0;
}
LOL_CFG_CFGLIST *__lol_findcfg(LOL_CFG *cfg,const char *field,const char *name)
{
	LOL_CFG_CFGLIST *p;

	p=cfg->cfglist;
	if(p==NULL){
		lol_error(0,"[%s]:cfglist is empty.",__func__);
		return NULL;
	}
	do{
		if(strcmp(p->field,field)==0 && strcmp(p->name,name)==0)
			return p;
		p=p->next;
	}while(p!=cfg->cfglist);

	lol_error(0,"[%s]:config not exists.[field=%s][name=%s]",__func__,field,name);
	return NULL;
}
int lol_getcfg(LOL_CFG *cfg,const char *field,const char *name,void *value,unsigned int size,int flags)
{
	LOL_CFG_CFGLIST *p;
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if((p=__lol_findcfg(cfg,field,name))==NULL){
		if(strlen(field)>0){
			/* continue to find global cfg */
			if((p=__lol_findcfg(cfg,"",name))==NULL){
				if(flags & LOL_CFG_NOT_IGNORE){
					lol_error(0,"[%s]:config not exists![%s]->[%s]",__func__,field,name);
					return -1;
				}
				return 0;	/* ignore */
			}
		}else{
			if(flags & LOL_CFG_NOT_IGNORE){
				lol_error(0,"[%s]:config not exists![%s]->[%s]",__func__,field,name);
				return -1;
			}
			return 0;	/* ignore */
		}
	}
	if(strlen(p->value)==0 && (flags & LOL_CFG_NOT_EMPTY)){
		lol_error(0,"[%s]:[%s]->[%s] is empty.[%s:%d]",__func__,p->field,p->name,p->file,p->lineno);
		return -1;
	}
	switch(flags & LOL_CFG_MASK_TYPE){
		case 0:			/* char(default) */
		case LOL_CFG_CHAR:	/* char */
			if(strlen(p->value)>=size){
				if(size!=1 || (size==1 && strlen(p->value)>1)){
					lol_error(0,"[%s]:value too long![%s]->[%s]:[%s][%s:%d]",__func__,p->field,p->name,p->value,p->file,p->lineno);
					return -1;
				}
			}
			if(size==1)
				memcpy(value,p->value,1);
			else
				strcpy(value,p->value);
			return strlen(value);
		case LOL_CFG_SHORT:	/* short */
			if(size!=2){
				lol_error(0,"[%s]:size not equal to 2 with short type![%s]->[%s]",__func__,field,name);
				return -1;
			}
			t_short=atol(p->value);
			memcpy(value,&t_short,2);
			break;
		case LOL_CFG_INT:	/* int */
			if(size!=4){
				lol_error(0,"[%s]:size not equal to 4 with int type![%s]->[%s]",__func__,field,name);
				return -1;
			}
			t_int=atol(p->value);
			memcpy(value,&t_int,4);
			break;
		case LOL_CFG_LONG:	/* long */
			if(size!=8){
				lol_error(0,"[%s]:size not equal to 8 with long type![%s]->[%s]",__func__,field,name);
				return -1;
			}
			t_long=atoll(p->value);
			memcpy(value,&t_long,8);
			break;
		case LOL_CFG_FLOAT:	/* float */
			if(size!=4){
				lol_error(0,"[%s]:size not equal to 4 with float type![%s]->[%s]",__func__,field,name);
				return -1;
			}
			t_float=atof(p->value);
			memcpy(value,&t_float,4);
			break;
		case LOL_CFG_DOUBLE:	/* double */
			if(size!=8){
				lol_error(0,"[%s]:size not equal to 8 with double type![%s]->[%s]",__func__,field,name);
				return -1;
			}
			t_double=atof(p->value);
			memcpy(value,&t_double,8);
			break;
		default:
			lol_error(0,"[%s]:illegal flags on value type![%s]->[%s]",__func__,field,name);
			return -1;
	}

	return 0;
}
int lol_showcfg(LOL_CFG *cfg)
{
	LOL_CFG_CFGLIST *p;

	if(cfg->cfgcount==0){
		printf("cfglist is empty!\n");
		return 0;
	}
	p=cfg->cfglist;
	do{
		printf("[%s]:[%s=%s]\n",p->field,p->name,p->value);
		p=p->next;
	}while(p!=cfg->cfglist);

	return 0;
}
int lol_freecfg(LOL_CFG *cfg)
{
	LOL_CFG_CFGLIST *p;

	while(cfg->cfglist){
		p=cfg->cfglist;
		if(p==p->next){
			free(p);
			break;
		}
		p->next->prior=p->prior;
		p->prior->next=p->next;
		cfg->cfglist=p->next;
		free(p);
	}
	free(cfg);

	return 0;
}
int lol_getcfgs(LOL_CFG *cfg,LOL_CFGARRAY *cfgarray,unsigned int cfgcount)
{
	LOL_CFGARRAY *p;
	int i;
	LOL_CFG_CFGLIST *c;

	/* init */
	p=cfgarray;
	for(i=0;i<cfgcount;i++,p++){
		if(p->exists)
			*p->exists=0;
	}

	p=cfgarray;
	for(i=0;i<cfgcount;i++,p++){
		if((c=__lol_findcfg(cfg,p->domain,p->name))==NULL){
			if(strlen(p->domain)>0){
				/* continue to find global cfg */
				if((c=__lol_findcfg(cfg,"",p->name))==NULL){
					if(p->flags & LOL_CFG_NOT_IGNORE){
						lol_error(0,"[%s]:config not exists![%s]->[%s]",__func__,p->domain,p->name);
						return -1;
					}
				}
			}else{
				if(p->flags & LOL_CFG_NOT_IGNORE){
					lol_error(0,"[%s]:config not exists![%s]->[%s]",__func__,p->domain,p->name);
					return -1;
				}
				return 0;	/* ignore */
			}
		}
		if(lol_getcfg(cfg,p->domain,p->name,p->value,p->size,p->flags)<0){
			lol_errorerror();
			return -1;
		}
	}

	return 0;
}
