#include "lol_dbf.h"

/* lol_dbf_open */
LOL_DBF *lol_dbf_open(const char *pathname,int flags,const char *conffile);

/* lol_dbf_fetch */
int lol_dbf_fetch(LOL_DBF *dbf);

/* lol_dbf_getfield */
int lol_dbf_getfield(LOL_DBF *dbf,unsigned int no,void *value,unsigned int size,int flags);

/* lol_dbf_getfieldbyname */
int lol_dbf_getfieldbyname(LOL_DBF *dbf,const char *name,void *value,unsigned int size,int flags);

/* lol_dbf_new */
void lol_dbf_new(LOL_DBF *dbf);

/* lol_dbf_putfield */
int lol_dbf_putfield(LOL_DBF *dbf,unsigned int no,const void *value,unsigned int length,int flags);

/* lol_dbf_putfieldbyname */
int lol_dbf_putfieldbyname(LOL_DBF *dbf,const char *name,const void *value,unsigned int length,int flags);

/* lol_dbf_insert */
int lol_dbf_insert(LOL_DBF *dbf);

/* lol_dbf_seek */
int lol_dbf_seek(LOL_DBF *dbf,int offset,int whence);

/* lol_dbf_sync */
int lol_dbf_sync(LOL_DBF *dbf);

/* lol_dbf_eof */
int lol_dbf_eof(LOL_DBF *dbf);

/* lol_dbf_close */
int lol_dbf_close(LOL_DBF *dbf);

int lol_dbf_seek(LOL_DBF *dbf,int offset,int whence)
{
	int pos;

	switch(whence){
		case LOL_DBF_SEEK_SET:
			if(offset<0){
				offset=0;
			}else if(offset>dbf->record_count){
				offset=dbf->record_count;
			}
			break;
		case LOL_DBF_SEEK_CUR:
			if(dbf->cursor==0){
				if(offset<0){
					offset=0;
				}else if(offset>dbf->record_count){
					offset=dbf->record_count;
				}
			}else if(lol_dbf_eof(dbf) || dbf->cursor==dbf->record_count){
				if(offset>=0){
					offset=dbf->record_count;
				}else{
					if(abs(offset)>dbf->record_count){
						offset=0;
					}else{
						offset=dbf->record_count-offset*(-1)+1;
					}
				}
			}else{
				if(offset>0){
					if(offset>dbf->record_count-dbf->cursor){
						offset=dbf->record_count;
					}else{
						offset=dbf->cursor+offset;
					}
				}else if(offset==0){
					offset=dbf->cursor;
				}else{
					if(offset<dbf->cursor*(-1)){
						offset=0;
					}else{
						offset=dbf->cursor+offset;
					}
				}
			}
			break;
		case LOL_DBF_SEEK_END:
			if(offset>=0){
				offset=dbf->record_count;
			}else{
				if(abs(offset)>dbf->record_count){
					offset=0;
				}else{
					offset=dbf->record_count-offset*(-1)+1;
				}
			}
			break;
		default:
			lol_error(0,"[%s]:illegal whence value.[whence=%d]",__func__,whence);
			return -1;
	};
	pos=offset*dbf->record_length+dbf->head_length;
	if(lol_fileseek(dbf->file,pos,LOL_FILE_SEEK_SET)<0){
		lol_errorerror();
		return -1;
	}
	dbf->cursor=offset;

	return 0;
}

int lol_dbf_close(LOL_DBF *dbf)
{
	if(!(dbf->flags & LOL_DBF_RDONLY) && !(dbf->flags & LOL_DBF_SYNC))
		lol_dbf_sync(dbf);
	lol_fileclose(dbf->file);
	free(dbf->buffer);
	free(dbf->field_array);
	free(dbf);

	return 0;
}
LOL_DBF *lol_dbf_open(const char *pathname,int flags,const char *conffile)
{
	LOL_DBF *dbf;
	LOL_CONF *conf;
	LOL_DBF_FIELD *p;
	lol_time tm;
	int oflags,i,pos,h,t,dbf_create;
	char buffer[32];


	/* dbf handle */
	if((dbf=malloc(sizeof(LOL_DBF)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	bzero(dbf,sizeof(LOL_DBF));

	/* summary info */
	dbf->record_count=0;
	dbf->head_length=32+1;	/* file head,field end */
	dbf->field_count=0;
	dbf->record_length=1;	/* delete flag */

	/* cursor set */
	dbf->cursor=0;

	/* creat & excl & trunc */
	dbf_create=0;	/* file create or not */
	if(flags & LOL_DBF_CREAT){
		if(access(pathname,F_OK)==0){
			if(flags & LOL_DBF_EXCL){
				lol_error(0,"[%s]:file exists.",__func__);
				free(dbf);
				return NULL;
			}
		}else{
			dbf_create=1;	/* create */
		}
	}
	if(flags & LOL_DBF_TRUNC)
		dbf_create=1;	/* create */

	/* open flags */
	dbf->flags=flags;
	if(flags==0)
		dbf->flags=LOL_DBF_RDWR;	/* defalut flags */
	oflags=0;
	if(dbf->flags & LOL_DBF_CREAT)	/* creat */
		oflags|=LOL_FILE_CREAT;
	if(dbf->flags & LOL_DBF_TRUNC)	/* trunc */
		oflags|=LOL_FILE_TRUNC;
	oflags|=LOL_FILE_RDWR|LOL_FILE_BINARY;
	if((dbf->file=lol_fileopen(pathname,oflags,0666))==NULL){
		lol_errorerror();
		free(dbf);
		return NULL;
	}

	if(dbf_create){	/* create */
		/* load conf */
		if((conf=lol_loadconf(conffile,'|'))==NULL){
			lol_errorerror();
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
		if(conf->hcount==0){
			lol_error(0,"[%s]:no fields.",__func__);
			lol_freeconf(conf);
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}


		/* summary info */
		lol_gettime(&tm);
		sprintf(dbf->last_modify_date,"%04d-%02d-%02d",tm.year,tm.month,tm.day);
		for(h=0;h<conf->hcount;h++){
			dbf->field_count++;
			dbf->head_length+=32;
			if(lol_getconf(conf,h,2,&t,sizeof(t),LOL_CONF_INT)<0){
				lol_errorerror();
				lol_freeconf(conf);
				lol_fileclose(dbf->file);
				free(dbf);
				return NULL;
			}
			dbf->record_length+=t;
		}

		/* init field array */
		if((dbf->field_array=malloc(sizeof(LOL_DBF_FIELD)*dbf->field_count))==NULL){
			lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			lol_freeconf(conf);
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
		bzero(dbf->field_array,sizeof(LOL_DBF_FIELD)*dbf->field_count);

		pos=1;	/* 0 used for delete flag */
		for(h=0,p=dbf->field_array;h<conf->hcount;h++,p++){
			if(lol_getconf(conf,h,0,p->name,sizeof(p->name),0)<0){
				lol_errorerror();
				free(dbf->field_array);
				lol_freeconf(conf);
				lol_fileclose(dbf->file);
				free(dbf);
				return NULL;
			}
			if(lol_getconf(conf,h,1,&p->type,1,0)<0){
				lol_errorerror();
				free(dbf->field_array);
				lol_freeconf(conf);
				lol_fileclose(dbf->file);
				free(dbf);
				return NULL;
			}
			switch(p->type){
				case 'C':	/* character */
				case 'N':	/* numeric */
					break;
				default:
					lol_error(0,"[%s]:field type not supported![type=%s]",__func__,p->type);
					free(dbf->field_array);
					lol_freeconf(conf);
					lol_fileclose(dbf->file);
					free(dbf);
					return NULL;
			};
			p->offset=pos;
			if(lol_getconf(conf,h,2,&p->length,sizeof(p->length),LOL_CONF_INT)<0){
				lol_errorerror();
				free(dbf->field_array);
				lol_freeconf(conf);
				lol_fileclose(dbf->file);
				free(dbf);
				return NULL;
			}
			pos+=p->length;
		}

		/* freeconf */
		lol_freeconf(conf);

		/* sync */
		if(dbf->flags & LOL_DBF_SYNC){
			if(lol_dbf_sync(dbf)<0){
				lol_errorerror();
				free(dbf->field_array);
				lol_fileclose(dbf->file);
				free(dbf);
				return NULL;
			}
		}
	}else{	/* open */
		/* summary */
		if(lol_fileseek(dbf->file,0,LOL_FILE_SEEK_SET)<0){
			lol_errorerror();
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
		/* read */
		if(lol_fileread(dbf->file,buffer,32)!=32){
			lol_errorerror();
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}

		/* file type */
		if(buffer[0]!=0x03){
			lol_error(0,"[%s]:file type not supported![filetype=0x%0X]",__func__,buffer[0]);
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
		/*last modify date*/
		strcpy(dbf->last_modify_date,"2000-00-00");
		lol_bintohex(dbf->last_modify_date+2,*(buffer+1));
		lol_bintohex(dbf->last_modify_date+5,*(buffer+2));
		lol_bintohex(dbf->last_modify_date+8,*(buffer+3));

		/*record count*/
		dbf->record_count=lol_ltoh4(*((unsigned int *)(buffer+4)));

		/*head length*/
		dbf->head_length=lol_ltoh2(*((unsigned short *)(buffer+8)));

		/*record lenght*/
		dbf->record_length=lol_ltoh2(*((unsigned short *)(buffer+10)));

		/*field count*/
		dbf->field_count=(dbf->head_length-1-32)/32;

		/* fields */
		if((dbf->field_array=malloc(sizeof(LOL_DBF_FIELD)*dbf->field_count))==NULL){
			lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
		bzero(dbf->field_array,sizeof(LOL_DBF_FIELD)*dbf->field_count);
		if(lol_fileseek(dbf->file,32,LOL_FILE_SEEK_SET)<0){
			lol_errorerror();
			free(dbf->field_array);
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
		pos=1;	/* 0 used for delete flag */
		for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
			if(lol_fileread(dbf->file,buffer,32)!=32){
				lol_errorerror();
				free(dbf->field_array);
				lol_fileclose(dbf->file);
				free(dbf);
				return NULL;
			}

			/* field name */
			snprintf(p->name,sizeof(p->name),"%s",buffer);
			lol_strtrim(p->name);

			/* field type */
			switch(buffer[11]){
				case 'C':
				case 'N':
					p->type=buffer[11];
					break;
				default:
					lol_error(0,"[%s]:field type not supported![type=%c]",__func__,buffer[11]);
					free(dbf->field_array);
					lol_fileclose(dbf->file);
					free(dbf);
					return NULL;
			}

			/*field offset*/
			p->offset=pos;

			/*field length*/
			p->length=(unsigned char)buffer[16];

			pos+=p->length;
		}

		/* dbf head end flag */
		if(lol_fileread(dbf->file,buffer,1)!=1){
			lol_errorerror();
			free(dbf->field_array);
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
		if(buffer[0]!=0x0D){
			lol_error(0,"[%s]:dbf head end flag not match.",__func__);
			free(dbf->field_array);
			lol_fileclose(dbf->file);
			free(dbf);
			return NULL;
		}
	}
	if((dbf->buffer=malloc(dbf->record_length))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(dbf->field_array);
		lol_fileclose(dbf->file);
		free(dbf);
		return NULL;
	}

	return dbf;
}
int lol_dbf_sync(LOL_DBF *dbf)
{
	LOL_DBF_FIELD *p;
	lol_time tm;
	char buffer[32];
	int i,offset,whence;

	/* seek */
	if(lol_fileseek(dbf->file,0,LOL_FILE_SEEK_SET)<0){
		lol_errorerror();
		return -1;
	}
	/* write dbf summary */
	bzero(buffer,sizeof(buffer));

	/* file type: 0 */
	buffer[0]=0x03;

	/* last modify date: 1 */
	lol_gettime(&tm);
	sprintf(dbf->last_modify_date,"%04d-%02d-%02d",tm.year,tm.month,tm.day);
	lol_hextobin(buffer+1,dbf->last_modify_date+2);	/* yy */
	lol_hextobin(buffer+2,dbf->last_modify_date+5);	/* mm */
	lol_hextobin(buffer+3,dbf->last_modify_date+8);	/* dd */

	/* record count: 4 */
	*((unsigned int *)(buffer+4))=lol_htol4(dbf->record_count);

	/* head length: 8 */
	*((unsigned short *)(buffer+8))=lol_htol2(dbf->head_length);

	/* record lenght: 10 */
	*((unsigned short *)(buffer+10))=lol_htol2(dbf->record_length);

	/* write */
	if(lol_filewrite(dbf->file,buffer,32)!=32){
		lol_errorerror();
		return -1;
	}

	/* write dbf fields */
	for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
		bzero(buffer,sizeof(buffer));

		/*field name*/
		snprintf(buffer,11,"%s",p->name);

		/*field type*/
		buffer[11]=p->type;

		/*field offset*/
		*((unsigned int *)(buffer+12))=lol_htol4(p->offset);

		/*field length*/
		*((unsigned char *)buffer+16)=p->length;

		/* write */
		if(lol_filewrite(dbf->file,buffer,32)!=32){
			lol_errorerror();
			return -1;
		}
	}

	/* write dbf head end flag */
	buffer[0]=0x0D;
	if(lol_filewrite(dbf->file,buffer,1)!=1){
		lol_errorerror();
		return -1;
	}

	/* cursor reset */
	if(lol_dbf_eof(dbf)){
		offset=0;
		whence=LOL_DBF_SEEK_END;
	}else{
		offset=dbf->cursor;
		whence=LOL_DBF_SEEK_SET;
	}
	if(lol_dbf_seek(dbf,offset,whence)<0){
		lol_errorerror();
		return -1;
	}

	return 0;
}
int lol_dbf_fetch(LOL_DBF *dbf)
{
	if(dbf->flags & LOL_DBF_WRONLY){
		lol_error(0,"[%s]:write only.",__func__);
		return -1;
	}
	if(lol_dbf_eof(dbf)){
		lol_error(0,"[%s]:record end.",__func__);
		return -1;
	}
	if(dbf->record_count==0 || dbf->cursor==dbf->record_count){
		dbf->cursor=LOL_DBF_EOF;
		return 0;
	}
	if(lol_fileread(dbf->file,dbf->buffer,dbf->record_length)!=dbf->record_length){
		lol_errorerror();
		return -1;
	}
	dbf->cursor++;

	return 1;
}
void lol_dbf_new(LOL_DBF *dbf)
{
	dbf->buffer[0]=0x20;
	bzero(dbf->buffer+1,dbf->record_length-1);
}
int lol_dbf_insert(LOL_DBF *dbf)
{
	if(dbf->flags & LOL_DBF_RDONLY){
		lol_error(0,"[%s]:read only.",__func__);
		return -1;
	}

	/* append */
	if(dbf->flags & LOL_DBF_APPEND){
		if(lol_dbf_seek(dbf,0,LOL_DBF_SEEK_END)<0){
			lol_errorerror();
			return -1;
		}
	}
	if(lol_filewrite(dbf->file,dbf->buffer,dbf->record_length)!=dbf->record_length){
		lol_errorerror();
		return -1;
	}

	/* record count */
	if(lol_dbf_eof(dbf) || dbf->cursor==dbf->record_count)	/* else is update */
		dbf->record_count++;

	/* cursor */
	if(lol_dbf_eof(dbf))
		dbf->cursor=dbf->record_count;
	else
		dbf->cursor++;

	if(dbf->flags & LOL_DBF_SYNC){
		if(lol_dbf_sync(dbf)<0)
			return -1;
	}

	return 0;
}
int lol_dbf_getfield(LOL_DBF *dbf,unsigned int no,void *value,unsigned int size,int flags)
{
	LOL_DBF_FIELD *p;
	char fieldvalue[256];
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if(no>=dbf->field_count){
		lol_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=dbf->field_array+no;
	bzero(fieldvalue,sizeof(fieldvalue));
	memcpy(fieldvalue,dbf->buffer+p->offset,p->length);
	lol_strtrim(fieldvalue);
	if(strlen(fieldvalue)==0 && (flags & LOL_DBF_NOT_EMPTY)){
		lol_error(0,"[%s]:field value is empty.[%d]->[%d:%s]",__func__,dbf->cursor,no,p->name);
		return -1;
	}
	switch(flags & LOL_DBF_MASK_TYPE){
		case 0:				/* default(char) */
		case LOL_DBF_CHAR:		/* char */
			if(strlen(fieldvalue)>=size){
				if(size!=1 || (size==1 && strlen(fieldvalue)>1)){
					lol_error(0,"[%s]:value too long![%d]->[%d:%s]:[%s]",__func__,dbf->cursor,no,p->name,fieldvalue);
					return -1;
				}
			}
			if(size==1)
				memcpy(value,fieldvalue,1);
			else
				strcpy(value,fieldvalue);
			return strlen(value);
		case LOL_DB_SHORT:		/* short */
			if(size!=2){
				lol_error(0,"[%s]:field size not equal to 2 with short type![size=%d][%d:%s]",__func__,no,p->name,size);
				return -1;
			}
			t_short=atol(fieldvalue);
			memcpy(value,&t_short,2);
			break;
		case LOL_DB_INT:		/* int */
			if(size!=4){
				lol_error(0,"[%s]:field size not equal to 4 with int type![size=%d][%d:%s]",__func__,size,no,p->name);
				return -1;
			}
			t_int=atol(fieldvalue);
			memcpy(value,&t_int,4);
			break;
		case LOL_DB_LONG:		/* long */
			if(size!=8){
				lol_error(0,"[%s]:field size not equal to 8 with long type![size=%d][%d:%s]",__func__,size,no,p->name);
				return -1;
			}
			t_long=atoll(fieldvalue);
			memcpy(value,&t_long,8);
			break;
		case LOL_DB_FLOAT:		/* float */
			if(size!=4){
				lol_error(0,"[%s]:field size not equal to 4 with float type![size=%d][%d:%s]",__func__,size,no,p->name);
				return -1;
			}
			t_float=atof(fieldvalue);
			memcpy(value,&t_float,4);
			break;
		case LOL_DB_DOUBLE:		/* double */
			if(size!=8){
				lol_error(0,"[%s]:field size not equal to 8 with double type![size=%d][%d:%s]",__func__,size,no,p->name);
				return -1;
			}
			t_double=atof(fieldvalue);
			memcpy(value,&t_double,8);
			break;
		default:
			lol_error(0,"[%s]:illegal field type.[fieldno=%d][type='%c']",__func__,no,flags&LOL_DBF_MASK_TYPE);
			return -1;
	}

	return 0;
}
int lol_dbf_getfieldbyname(LOL_DBF *dbf,const char *name,void *value,unsigned int size,int flags)
{
	LOL_DBF_FIELD *p;
	int i,r;

	for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==dbf->field_count){
		lol_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if((r=lol_dbf_getfield(dbf,i,value,size,flags))<0){
		lol_errorerror();
		return -1;
	}

	return r;
}
int lol_dbf_putfield(LOL_DBF *dbf,unsigned int no,const void *value,unsigned int length,int flags)
{
	LOL_DBF_FIELD *p;
	char fieldvalue[256];
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if(no>=dbf->field_count){
		lol_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=dbf->field_array+no;
	bzero(fieldvalue,sizeof(fieldvalue));
	switch(flags & LOL_DBF_MASK_TYPE){
		case 0:
		case LOL_DBF_CHAR:
			if(length>p->length){
				lol_error(0,"[%s]:value too long![%d:%s],[type=char],[length=%d],[len=%d]",__func__,no,p->name,p->length,length);
				return -1;
			}
			memcpy(fieldvalue,value,p->length);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_SHORT:
			if(length!=2){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=short],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_short,value,2);
			sprintf(fieldvalue,"%d",t_short);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_INT:
			if(length!=4){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=int],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_int,value,4);
			sprintf(fieldvalue,"%d",t_int);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_LONG:
			if(length!=8){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=long],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_long,value,8);
			sprintf(fieldvalue,"%lld",t_long);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_FLOAT:
			if(length!=4){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=float],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_float,value,4);
			sprintf(fieldvalue,"%f",t_float);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_DOUBLE:
			if(length!=8){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=double],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_double,value,8);
			sprintf(fieldvalue,"%f",t_double);
			memcpy(dbf->buffer+p->offset,fieldvalue,p->length);
			break;
		default:
			lol_error(0,"[%s]:illegal field type.[fieldno=%d][type='%c']",__func__,no,flags&LOL_DBF_MASK_TYPE);
			return -1;
	};

	return 0;
}
int lol_dbf_putfieldbyname(LOL_DBF *dbf,const char *name,const void *value,unsigned int length,int flags)
{
	LOL_DBF_FIELD *p;
	int i;

	for(i=0,p=dbf->field_array;i<dbf->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==dbf->field_count){
		lol_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if(lol_dbf_putfield(dbf,i,value,length,flags)<0){
		lol_errorerror();
		return -1;
	}

	return 0;
}

int lol_dbf_eof(LOL_DBF *dbf)
{
	if(dbf->cursor==LOL_DBF_EOF)
		return 1;

	return 0;
}
