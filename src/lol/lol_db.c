#include "lol_error.h"
#include "lol_utility.h"
#include "lol_conf.h"
#include "lol_db.h"

int lol_db_seek(LOL_DB *db,int offset,int whence)
{
	int pos;

	switch(whence){
		case LOL_DB_SEEK_SET:
			if(offset<0){
				offset=0;
			}else if(offset>db->record_count){
				offset=db->record_count;
			}
			break;
		case LOL_DB_SEEK_CUR:
			if(db->cursor==0){
				if(offset<0){
					offset=0;
				}else if(offset>db->record_count){
					offset=db->record_count;
				}
			}else if(lol_db_eof(db) || db->cursor==db->record_count){
				if(offset>=0){
					offset=db->record_count;
				}else{
					if(abs(offset)>db->record_count){
						offset=0;
					}else{
						offset=db->record_count-offset*(-1)+1;
					}
				}
			}else{
				if(offset>0){
					if(offset>db->record_count-db->cursor){
						offset=db->record_count;
					}else{
						offset=db->cursor+offset;
					}
				}else if(offset==0){
					offset=db->cursor;
				}else{
					if(offset<db->cursor*(-1)){
						offset=0;
					}else{
						offset=db->cursor+offset;
					}
				}
			}
			break;
		case LOL_DB_SEEK_END:
			if(offset>=0){
				offset=db->record_count;
			}else{
				if(abs(offset)>db->record_count){
					offset=0;
				}else{
					offset=db->record_count-offset*(-1)+1;
				}
			}
			break;
		default:
			lol_error(0,"[%s]:illegal whence value.[whence=%d]",__func__,whence);
			return -1;
	};
	pos=offset*db->record_length+db->head_length;
	if(lol_fileseek(db->file,pos,LOL_FILE_SEEK_SET)<0){
		lol_errorerror();
		return -1;
	}
	db->cursor=offset;

	return 0;
}

int lol_db_close(LOL_DB *db)
{
	if(!(db->flags & LOL_DB_RDONLY) && !(db->flags & LOL_DB_SYNC))
		lol_db_sync(db);
	lol_fileclose(db->file);
	free(db->buffer);
	free(db->field_array);
	free(db);

	return 0;
}
LOL_DB *lol_db_open(const char *pathname,int flags,LOL_DB_FIELD *fieldarray,unsigned int fieldcount)
{
	LOL_DB *db;
	LOL_DB_FIELD *p;
	int oflags,i,pos,db_create;
	char buffer[32];
	lol_time tm;


	/* db handle */
	if((db=malloc(sizeof(LOL_DB)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	bzero(db,sizeof(LOL_DB));

	/* summary info */
	db->record_count=0;
	db->head_length=32+1;	/* file head,field end */
	db->field_count=0;
	db->record_length=1;	/* delete flag */

	/* cursor set */
	db->cursor=0;

	/* creat & excl & trunc */
	db_create=0;	/* file create or not */
	if(flags & LOL_DB_CREAT){
		if(access(pathname,F_OK)==0){
			if(flags & LOL_DB_EXCL){
				lol_error(0,"[%s]:file exists.",__func__);
				free(db);
				return NULL;
			}
		}else{
			db_create=1;	/* create */
		}
	}
	if(flags & LOL_DB_TRUNC)
		db_create=1;	/* create */

	/* open flags */
	db->flags=flags;
	if(flags==0)
		db->flags=LOL_DB_RDWR;	/* defalut flags */
	oflags=0;
	if(db->flags & LOL_DB_CREAT)	/* creat */
		oflags|=LOL_FILE_CREAT;
	if(db->flags & LOL_DB_TRUNC)	/* trunc */
		oflags|=LOL_FILE_TRUNC;
	oflags|=LOL_FILE_RDWR|LOL_FILE_BINARY;
	if((db->file=lol_fileopen(pathname,oflags,0666))==NULL){
		lol_errorerror();
		free(db);
		return NULL;
	}

	if(db_create){	/* create */
		if(fieldcount==0){
			lol_error(0,"[%s]:fieldcount is 0.",__func__);
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}

		/* summary info */
		lol_gettime(&tm);
		sprintf(db->last_modify_date,"%04d-%02d-%02d",tm.year,tm.month,tm.day);
		db->field_count=fieldcount;
		db->head_length+=32*fieldcount;
		for(i=0,p=fieldarray;i<fieldcount;i++,p++){
			db->record_length+=p->length;
		}

		/* init field array */
		if((db->field_array=malloc(sizeof(LOL_DB_FIELD)*db->field_count))==NULL){
			lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}
		memcpy(db->field_array,fieldarray,sizeof(LOL_DB_FIELD)*db->field_count);
		pos=1;	/* 0 used for delete flag */
		for(i=0,p=db->field_array;i<fieldcount;i++,p++){
			switch(p->type){
				case 'C':	/* character */
				case 'N':	/* numeric */
					break;
				default:
					lol_error(0,"[%s]:field type not supported![type=%s]",__func__,p->type);
					free(db->field_array);
					lol_fileclose(db->file);
					free(db);
					return NULL;
			};
			p->offset=pos;
			pos+=p->length;
		}

		/* sync */
		if(db->flags & LOL_DB_SYNC){
			if(lol_db_sync(db)<0){
				lol_errorerror();
				free(db->field_array);
				lol_fileclose(db->file);
				free(db);
				return NULL;
			}
		}
	}else{	/* open */
		/* summary */
		if(lol_fileseek(db->file,0,LOL_FILE_SEEK_SET)<0){
			lol_errorerror();
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}
		/* read */
		if(lol_fileread(db->file,buffer,32)!=32){
			lol_errorerror();
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}

		/* file type */
		if(buffer[0]!=0x03){
			lol_error(0,"[%s]:file type not supported![filetype=0x%0X]",__func__,buffer[0]);
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}
		/*last modify date*/
		strcpy(db->last_modify_date,"2000-00-00");
		lol_bintohex(db->last_modify_date+2,*(buffer+1));
		lol_bintohex(db->last_modify_date+5,*(buffer+2));
		lol_bintohex(db->last_modify_date+8,*(buffer+3));

		/*record count*/
		db->record_count=lol_ltoh4(*((unsigned int *)(buffer+4)));

		/*head length*/
		db->head_length=lol_ltoh2(*((unsigned short *)(buffer+8)));

		/*record lenght*/
		db->record_length=lol_ltoh2(*((unsigned short *)(buffer+10)));

		/*field count*/
		db->field_count=(db->head_length-1-32)/32;

		/* fields */
		if((db->field_array=malloc(sizeof(LOL_DB_FIELD)*db->field_count))==NULL){
			lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}
		bzero(db->field_array,sizeof(LOL_DB_FIELD)*db->field_count);
		if(lol_fileseek(db->file,32,LOL_FILE_SEEK_SET)<0){
			lol_errorerror();
			free(db->field_array);
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}
		pos=1;	/* 0 used for delete flag */
		for(i=0,p=db->field_array;i<db->field_count;i++,p++){
			if(lol_fileread(db->file,buffer,32)!=32){
				lol_errorerror();
				free(db->field_array);
				lol_fileclose(db->file);
				free(db);
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
					free(db->field_array);
					lol_fileclose(db->file);
					free(db);
					return NULL;
			}

			/*field offset*/
			p->offset=pos;

			/*field length*/
			p->length=(unsigned char)buffer[16];

			pos+=p->length;
		}

		/* db head end flag */
		if(lol_fileread(db->file,buffer,1)!=1){
			lol_errorerror();
			free(db->field_array);
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}
		if(buffer[0]!=0x0D){
			lol_error(0,"[%s]:db head end flag not match.",__func__);
			free(db->field_array);
			lol_fileclose(db->file);
			free(db);
			return NULL;
		}
	}
	if((db->buffer=malloc(db->record_length))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(db->field_array);
		lol_fileclose(db->file);
		free(db);
		return NULL;
	}

	return db;
}

LOL_DB *lol_db_openx(const char *pathname,int flags,const char *conffile)
{
	LOL_DB_FIELD *p,*fieldarray;
	int pos,h,db_create,fieldcount;
	LOL_CONF *conf;
	
	/* creat & excl & trunc */
	db_create=0;	/* file create or not */
	if(flags & LOL_DB_CREAT){
		if(access(pathname,F_OK)==0){
			if(flags & LOL_DB_EXCL){
				lol_error(0,"[%s]:file exists.",__func__);
				return NULL;
			}
		}else{
			db_create=1;	/* create */
		}
	}
	if(flags & LOL_DB_TRUNC)
		db_create=1;	/* create */
	
	if(db_create){	/* create */
		/* load conf */
		if((conf=lol_loadconf(conffile,'|'))==NULL){
			lol_errorerror();
			return NULL;
		}
		if(conf->hcount==0){
			lol_error(0,"[%s]:no fields.",__func__);
			lol_freeconf(conf);
			return NULL;
		}
		fieldcount=conf->hcount;
		
		/* init field array */
		if((fieldarray=malloc(sizeof(LOL_DB_FIELD)*fieldcount))==NULL){
			lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			lol_freeconf(conf);
			return NULL;
		}
		bzero(fieldarray,sizeof(LOL_DB_FIELD)*fieldcount);
		
		pos=1;	/* 0 used for delete flag */
		for(h=0,p=fieldarray;h<fieldcount;h++,p++){
			if(lol_getconf(conf,h,0,p->name,sizeof(p->name),0)<0){
				lol_errorerror();
				free(fieldarray);
				lol_freeconf(conf);
				return NULL;
			}
			if(lol_getconf(conf,h,1,&p->type,1,0)<0){
				lol_errorerror();
				free(fieldarray);
				lol_freeconf(conf);
				return NULL;
			}
			switch(p->type){
			case 'C':	/* character */
			case 'N':	/* numeric */
				break;
			default:
				lol_error(0,"[%s]:field type not supported![type=%s]",__func__,p->type);
				free(fieldarray);
				lol_freeconf(conf);
				return NULL;
			};
			p->offset=pos;
			if(lol_getconf(conf,h,2,&p->length,sizeof(p->length),LOL_CONF_INT)<0){
				lol_errorerror();
				free(fieldarray);
				lol_freeconf(conf);
				return NULL;
			}
			pos+=p->length;
		}
		
		/* freeconf */
		lol_freeconf(conf);
	}
	return lol_db_open(pathname,flags,fieldarray,fieldcount);
}

int lol_db_sync(LOL_DB *db)
{
	LOL_DB_FIELD *p;
	lol_time tm;
	char buffer[32];
	int i,offset,whence;

	/* seek */
	if(lol_fileseek(db->file,0,LOL_FILE_SEEK_SET)<0){
		lol_errorerror();
		return -1;
	}
	/* write db summary */
	bzero(buffer,sizeof(buffer));

	/* file type: 0 */
	buffer[0]=0x03;

	/* last modify date: 1 */
	lol_gettime(&tm);
	sprintf(db->last_modify_date,"%04d-%02d-%02d",tm.year,tm.month,tm.day);
	lol_hextobin(buffer+1,db->last_modify_date+2);	/* yy */
	lol_hextobin(buffer+2,db->last_modify_date+5);	/* mm */
	lol_hextobin(buffer+3,db->last_modify_date+8);	/* dd */

	/* record count: 4 */
	*((unsigned int *)(buffer+4))=lol_htol4(db->record_count);

	/* head length: 8 */
	*((unsigned short *)(buffer+8))=lol_htol2(db->head_length);

	/* record lenght: 10 */
	*((unsigned short *)(buffer+10))=lol_htol2(db->record_length);

	/* write */
	if(lol_filewrite(db->file,buffer,32)!=32){
		lol_errorerror();
		return -1;
	}

	/* write db fields */
	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
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
		if(lol_filewrite(db->file,buffer,32)!=32){
			lol_errorerror();
			return -1;
		}
	}

	/* write db head end flag */
	buffer[0]=0x0D;
	if(lol_filewrite(db->file,buffer,1)!=1){
		lol_errorerror();
		return -1;
	}

	/* cursor reset */
	if(lol_db_eof(db)){
		offset=0;
		whence=LOL_DB_SEEK_END;
	}else{
		offset=db->cursor;
		whence=LOL_DB_SEEK_SET;
	}
	if(lol_db_seek(db,offset,whence)<0){
		lol_errorerror();
		return -1;
	}

	return 0;
}
int lol_db_fetch(LOL_DB *db)
{
	if(db->flags & LOL_DB_WRONLY){
		lol_error(0,"[%s]:write only.",__func__);
		return -1;
	}
	if(lol_db_eof(db)){
		lol_error(0,"[%s]:record end.",__func__);
		return -1;
	}
	if(db->record_count==0 || db->cursor==db->record_count){
		db->cursor=LOL_DB_EOF;
		return 0;
	}
	if(lol_fileread(db->file,db->buffer,db->record_length)!=db->record_length){
		lol_errorerror();
		return -1;
	}
	db->cursor++;

	return 1;
}
void lol_db_new(LOL_DB *db)
{
	db->buffer[0]=0x20;
	bzero(db->buffer+1,db->record_length-1);
}
int lol_db_insert(LOL_DB *db)
{
	if(db->flags & LOL_DB_RDONLY){
		lol_error(0,"[%s]:read only.",__func__);
		return -1;
	}

	/* append */
	if(db->flags & LOL_DB_APPEND){
		if(lol_db_seek(db,0,LOL_DB_SEEK_END)<0){
			lol_errorerror();
			return -1;
		}
	}
	if(lol_filewrite(db->file,db->buffer,db->record_length)!=db->record_length){
		lol_errorerror();
		return -1;
	}

	/* record count */
	if(lol_db_eof(db) || db->cursor==db->record_count)	/* else is update */
		db->record_count++;

	/* cursor */
	if(lol_db_eof(db))
		db->cursor=db->record_count;
	else
		db->cursor++;

	if(db->flags & LOL_DB_SYNC){
		if(lol_db_sync(db)<0)
			return -1;
	}

	return 0;
}
int lol_db_getfield(LOL_DB *db,unsigned int no,void *value,unsigned int size,int flags)
{
	LOL_DB_FIELD *p;
	char fieldvalue[256];
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if(no>=db->field_count){
		lol_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=db->field_array+no;
	bzero(fieldvalue,sizeof(fieldvalue));
	memcpy(fieldvalue,db->buffer+p->offset,p->length);
	lol_strtrim(fieldvalue);
	if(strlen(fieldvalue)==0 && (flags & LOL_DB_NOT_EMPTY)){
		lol_error(0,"[%s]:field value is empty.[%d]->[%d:%s]",__func__,db->cursor,no,p->name);
		return -1;
	}
	switch(flags & LOL_DB_MASK_TYPE){
		case 0:				/* default(char) */
		case LOL_DB_CHAR:		/* char */
			if(strlen(fieldvalue)>=size){
				if(size!=1 || (size==1 && strlen(fieldvalue)>1)){
					lol_error(0,"[%s]:value too long![%d]->[%d:%s]:[%s]",__func__,db->cursor,no,p->name,fieldvalue);
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
			lol_error(0,"[%s]:illegal field type.[fieldno=%d][type='%c']",__func__,no,flags&LOL_DB_MASK_TYPE);
			return -1;
	}

	return 0;
}
int lol_db_getfieldbyname(LOL_DB *db,const char *name,void *value,unsigned int size,int flags)
{
	LOL_DB_FIELD *p;
	int i,r;

	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==db->field_count){
		lol_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if((r=lol_db_getfield(db,i,value,size,flags))<0){
		lol_errorerror();
		return -1;
	}

	return r;
}
int lol_db_putfield(LOL_DB *db,unsigned int no,const void *value,unsigned int length,int flags)
{
	LOL_DB_FIELD *p;
	char fieldvalue[256];
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if(no>=db->field_count){
		lol_error(0,"[%s]:field no out of range.[no=%d]",__func__);
		return -1;
	}
	p=db->field_array+no;
	bzero(fieldvalue,sizeof(fieldvalue));
	switch(flags & LOL_DB_MASK_TYPE){
		case 0:
		case LOL_DB_CHAR:
			if(length>p->length){
				lol_error(0,"[%s]:value too long![%d:%s],[type=char],[length=%d],[len=%d]",__func__,no,p->name,p->length,length);
				return -1;
			}
			memcpy(fieldvalue,value,p->length);
			memcpy(db->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_SHORT:
			if(length!=2){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=short],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_short,value,2);
			sprintf(fieldvalue,"%d",t_short);
			memcpy(db->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_INT:
			if(length!=4){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=int],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_int,value,4);
			sprintf(fieldvalue,"%d",t_int);
			memcpy(db->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_LONG:
			if(length!=8){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=long],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_long,value,8);
			sprintf(fieldvalue,"%lld",t_long);
			memcpy(db->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_FLOAT:
			if(length!=4){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=float],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_float,value,4);
			sprintf(fieldvalue,"%f",t_float);
			memcpy(db->buffer+p->offset,fieldvalue,p->length);
			break;
		case LOL_DB_DOUBLE:
			if(length!=8){
				lol_error(0,"[%s]:data len not match![%d:%s],[type=double],[len=%d]",__func__,no,p->name,length);
				return -1;
			}
			memcpy(&t_double,value,8);
			sprintf(fieldvalue,"%f",t_double);
			memcpy(db->buffer+p->offset,fieldvalue,p->length);
			break;
		default:
			lol_error(0,"[%s]:illegal field type.[fieldno=%d][type='%c']",__func__,no,flags&LOL_DB_MASK_TYPE);
			return -1;
	};

	return 0;
}
int lol_db_putfieldbyname(LOL_DB *db,const char *name,const void *value,unsigned int length,int flags)
{
	LOL_DB_FIELD *p;
	int i;

	for(i=0,p=db->field_array;i<db->field_count;i++,p++){
		if(strcmp(name,p->name)==0)
			break;
	}
	if(i==db->field_count){
		lol_error(0,"[%s]:field not exists.[name=%s]",__func__,name);
		return -1;
	}
	if(lol_db_putfield(db,i,value,length,flags)<0){
		lol_errorerror();
		return -1;
	}

	return 0;
}

int lol_db_eof(LOL_DB *db)
{
	if(db->cursor==LOL_DB_EOF)
		return 1;

	return 0;
}
