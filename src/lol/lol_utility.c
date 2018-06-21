#include "lol_error.h"
#include "lol_utility.h"

#ifdef __LOL_WINDOWS__
int opterr=1,optind=1,optopt,optreset;
char *optarg;
#endif	/* __LOL_WINDOWS__ */

#ifdef __LOL_WINDOWS__
int getopt(int argc, char *argv[], const char *ostr)
{
	static char *place = "";		/* option letter processing */
	char *oli;				/* option letter list index */

	if (optreset || !*place) {		/* update scanning pointer */
		optreset = 0;
		if (optind >= argc || *(place = argv[optind]) != '-') {
			place = "";
			return (EOF);
		}
		if (place[1] && *++place == '-') {	/* found "--" */
			++optind;
			place = "";
			return (EOF);
		}
	}					/* option letter okay? */
	if ((optopt = (int)*place++) == (int)':' ||
	    !(oli = strchr(ostr, optopt))) {
		/*
		 * if the user didn't specify '-' as an option,
		 * assume it means EOF.
		 */
		if (optopt == (int)'-')
			return (EOF);
		if (!*place)
			++optind;
		return ('?');
	}
	if (*++oli != ':') {			/* don't need argument */
		optarg = NULL;
		if (!*place)
			++optind;
	}
	else {					/* need an argument */
		if (*place)			/* no white space */
			optarg = place;
		else if (argc <= ++optind) {	/* no arg */
			place = "";
			if (*ostr == ':')
				return (':');
			return ('?');
		}
		else				/* white space */
			optarg = argv[optind];
		place = "";
		++optind;
	}
	return (optopt);			/* dump back option letter */
}
#endif	/* __LOL_WINDOWS__ */

char *lol_strtrim(char *str)
{
	char *p,*t;

	t=str;
	if(strlen(str)==0)/* already no spaces */
		return str;

	/* trim left spaces */
	p=str;/* use a move pointer */
	while(*p!='\0'){
		if(!isspace(*p)){
			break;
		}
		p++;
	}
	if(*p=='\0')/* every char in str is space */
		*str='\0';
	else{
		if(p!=str){/* has spaces */
			for(;*p!='\0';)
				*str++=*p++;
			*str='\0';
		}/* else has none spaces */
	}
	str=t;

	if(strlen(str)==0)/* already no spaces */
		return str;

	/* trim right spaces */
	p=str+(strlen(str)-1);/* point to the last char */
	while(p>=str){
		if(!isspace(*p)){
			break;
		}
		p--;
	}
	*(p+1)='\0';

	return str;
}
char *lol_strreplace(char *srcstr,char *pattern,char *repstr)
{
	char *p,*q;

	p=srcstr;
	while(*p!='\0'){
		if((q=strstr(p,pattern))==NULL)
			break;
		memmove(q+strlen(repstr),q+strlen(pattern),strlen(q+strlen(pattern))+1);
		memcpy(q,repstr,strlen(repstr));
		p=q+strlen(repstr);
	}

	return srcstr;
}
char *lol_chrreplace(char *srcstr,char pattern,char repstr)
{
	char tmp1[2],tmp2[2];

	tmp1[0]=pattern;
	tmp1[1]='\0';
	tmp2[0]=repstr;
	tmp2[1]='\0';

	return lol_strreplace(srcstr,tmp1,tmp2);
}
char *lol_strinsert(char *srcstr,int pos,char *insstr)
{
	memmove(srcstr+pos+strlen(insstr),srcstr+pos,strlen(srcstr+pos)+1);
	memcpy(srcstr+pos,insstr,strlen(insstr));

	return srcstr;
}
char *lol_chrinsert(char *srcstr,int pos,char insstr)
{
	memmove(srcstr+pos+1,srcstr+pos,strlen(srcstr+pos)+1);
	*(srcstr+pos)=insstr;

	return srcstr;
}
int lol_chrfind(const char *str,char chr,int index)
{
	const char *p,*start,*end;
	int pos,shift,flag;

	if(index>=0){
		start=str;
		end=str+strlen(str);
		shift=1;
	}else{
		start=str+strlen(str);
		end=str;
		shift=-1;
		index=-1*index;
	}
	flag=0;
	p=start;
	while(p!=end){
		if(*p!=chr){
			p+=shift;
			continue;
		}
		index--;
		if(index!=-1){
			p+=shift;
			continue;
		}
		flag=1;
		break;
	}
	if(flag!=1){
		return -1;
	}
	pos=p-str;

	return pos;
}
/*
 * strgetfield()
 * format:---|-----|----|--------|---
 */

int lol_strgetfield(const char *str,char sepchr,int index,char *value,unsigned int size)
{
	int ppos,qpos,len=0;

	if(index>=0){
		if(index==0){
			ppos=0;
			if((qpos=lol_chrfind(str,sepchr,0))<0)
				len=strlen(str);
			else
				len=qpos-ppos;
		}else{
			if((ppos=lol_chrfind(str,sepchr,index-1))<0)
				len=0;
			else{
				ppos++;
				if((qpos=lol_chrfind(str+ppos,sepchr,0))<0)
					len=strlen(str+ppos);
				else
					len=qpos;
			}
		}
	}else{
		if(index==-1){
			if((ppos=lol_chrfind(str,sepchr,-1))<0){
				ppos=0;
				len=strlen(str);
			}else{
				ppos++;
				len=strlen(str+ppos);
			}
		}else{
			if((qpos=lol_chrfind(str,sepchr,index+1))<0){
				len=0;
			}else{
				if((ppos=lol_chrfind(str,sepchr,index))<0){
					ppos=0;
					len=qpos;
				}else{
					ppos++;
					len=qpos-ppos;
				}
			}
		}
	}
	if(len==0){
		*value='\0';
	}else{
		if(len>=size){
			lol_error(0,"[%s]:field too long![index=%d][field=%#.#s]",__func__,index,len,len,str+ppos);
			return -1;
		}
		memcpy(value,str+ppos,len);
		*(value+len)='\0';
	}
	lol_strtrim(value);

	return strlen(value);
}
/**********************************************
 *int lol_chrstat(const char *str,char ch);
 **********************************************/
int lol_chrstat(const char *str,char ch)
{
    int n=0;

    while(*str!='\0'){
        if(*str==ch)
            n++;
        str++;
    }

    return n;
}
int lol_strstat(const char *str,const char *pattern)
{
	int n=0;

	while(str=strstr(str,pattern)){
		n++;
		str+=strlen(pattern);
	}
	return n;
}
int lol_bin_to_hex(char *dest,const char *src,int len)
{
	int i;

	for(i=0;i<len;i++){
		if(lol_bintohex(dest,*src)<0)
			return -1;
		src++;
		dest+=2;
	}
	*dest='\0';

	return 0;
}
int lol_bin_to_bcd(char *dest,const char *src,int len)
{
	int i;

	for(i=0;i<len;i++){
		if(lol_bintobcd(dest,*src)<0)
			return -1;
		src++;
		dest+=2;
	}
	*dest='\0';

	return 0;
}
int lol_hex_to_bin(char *dest,const char *src,int len)
{
	int i;

	if(len%2 != 0){
		lol_error(0,"hex string length not equal to 2x![len=%d]",len);
		return -1;
	}
	for(i=0;i<len/2;i++){
		if(lol_hextobin(dest,src)<0)
			return -1;
		dest++;
		src+=2;
	}

	return 0;
}
int lol_bcd_to_bin(char *dest,const char *src,int len)
{
	int i;

	if(len%2 != 0){
		lol_error(0,"bcd string length not equal to 2x![len=%d]",len);
		return -1;
	}
	for(i=0;i<len/2;i++){
		if(lol_bcdtobin(dest,src)<0)
			return -1;
		dest++;
		src+=2;
	}

	return 0;
}
int lol_hextobin(char *dest,const char *src)
{
	int firstbyte,lastbyte;
	
	if((firstbyte=lol_hextoint(src[0]))<0)
		return -1;
	if((lastbyte=lol_hextoint(src[1]))<0)
		return -1;
	*dest=(firstbyte << 4) | (lastbyte & 15);

	return 0;
}
int lol_bcdtobin(char *dest,const char *src)
{
	int firstbyte,lastbyte;
	
	if((firstbyte=lol_bcdtoint(src[0]))<0)
		return -1;
	if((lastbyte=lol_bcdtoint(src[1]))<0)
		return -1;
	*dest=(firstbyte << 4) | (lastbyte & 15);

	return 0;
}
int lol_bintohex(char *dest,char src)
{
	int firstbyte,lastbyte,t;

	firstbyte=src>>4;
	firstbyte&=15;
	if((t=lol_inttohex(firstbyte))<0)
		return -1;
	*dest=t;
	
	dest++;
	lastbyte=src&15;
	if((t=lol_inttohex(lastbyte))<0)
		return -1;
	*dest=t;

	return 0;
}
int lol_bintobcd(char *dest,char src)
{
	int firstbyte,lastbyte,t;

	firstbyte=src>>4;
	firstbyte&=15;
	if((t=lol_inttobcd(firstbyte))<0)
		return -1;
	*dest=t;

	dest++;
	lastbyte=src&15;
	if((t=lol_inttobcd(lastbyte))<0)
		return -1;
	*dest=t;

	return 0;
}
int lol_inttohex(int bin)
{
	int result;

	if(bin>=0 && bin<=9){
		result='0'+bin;
	}else if(bin>=10 && bin<=15){
		result='A'+(bin-10);
	}else{
		lol_error(0,"[%s]:wrong hex value![%d]",__func__,bin);
		return -1;
	}

	return result;
}
int lol_inttobcd(int bin)
{
	int result;

	if(bin>=0 && bin<=9){
		result='0'+bin;
	}else{
		lol_error(0,"[%s]:wrong bcd value![%d]",__func__,bin);
		return -1;
	}

	return result;
}
int lol_hextoint(int hex)
{
	int result;

	if(hex>=48 && hex<=57){
		result=hex-'0';
	}else if(hex>=97 && hex<=102){
		result=hex-'a'+10;
	}else if(hex>=65 && hex<=90){
		result=hex-'A'+10;
	}else{
		lol_error(0,"[%s]:invalid hex character![%c]",__func__,hex);
		return -1;
	}

	return result;
}
int lol_bcdtoint(int hex)
{
	int result;

	if(hex>=48 && hex<=57){
		result=hex-'0';
	}else{
		lol_error(0,"[%s]:invalid bcd character![%c]",__func__,hex);
		return -1;
	}

	return result;
}

int lol_printbin(const char *buf,const int len,int linesize)
{
	int i;
	int linepos;
	int linechangeflag;
	int printway;
	const char *p;
	char tmpbuf[3];
	int r;

	if(len==0)
		return 0;
	if(linesize==0)
		linechangeflag=0;
	else
		linechangeflag=1;
	
	if(linesize>=0){
		printway=0;
		p=buf;
	}else{
		printway=1;
		p=buf+len-1;
	}
	linepos=0;
	linesize=abs(linesize);
	for(i=0;i<len;i++){
		if(linechangeflag==1){
			if(linepos==linesize){
				printf("\n");
				linepos=0;
			}
		}
		linepos++;
		bzero(tmpbuf,sizeof(tmpbuf));
		r=lol_bintohex(tmpbuf,*p);
		if(r<0){
			return -1;
		}
		printf("%s ",tmpbuf);
		if(printway==1)
			p--;
		else
			p++;
	}
	printf("\n");
	return 0;
}
/*****************************************************
 *int lol_bitset(char *bitmap,int bit);
 *****************************************************/
int lol_bitset(char *bitmap,int bit)
{
	bit--;
	bitmap[bit/8] |= (0x01<<(7-bit%8));
	return 0;
}

/*****************************************************
 *int lol_bittest(const char *bitmap,int bit);
 *****************************************************/
int lol_bittest(const char *bitmap,int bit)
{
	int existflag;
	bit--;
	existflag=bitmap[bit/8] & (0x01<<(7-bit%8));
	return existflag;
}

/*****************************************************
 *int lol_bitclear(char *bitmap,int bit);
 *****************************************************/
int lol_bitclear(char *bitmap,int bit)
{
	bit--;
	bitmap[bit/8]&=~(0x01<<(7-bit%8));
	return 0;
}
int lol_getopt(int argc,char *argv[],char opt,void *value,unsigned int size,int flags)
{
	char optstring[32];
	int c;
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if(flags & LOL_OPT_ARG)
		snprintf(optstring,sizeof(optstring),":%c:",opt);
	else
		snprintf(optstring,sizeof(optstring),":%c",opt);
	optind=1;
	while((c=getopt(argc,argv,optstring))!=EOF){
		if(c==opt){
			if(!(flags & LOL_OPT_ARG))
				return 0;
			if(strlen(optarg)==0){
				if(flags & LOL_OPT_NOT_EMPTY){
					lol_error(0,"[%s]:option is be empty. -- [-%c]",__func__,opt);
					return -1;
				}
			}
			switch(flags & LOL_OPT_MASK_TYPE){
				case 0:			/* char(default) */
				case LOL_OPT_CHAR:	/* char */
					if(strlen(optarg)>=size){
						if(size!=1 || (size==1 && strlen(optarg)>1)){
							lol_error(0,"[%s]:option value too long. -- [-%c]",__func__,opt);
							return -1;
						}
					}
					if(size==1)
						memcpy(value,optarg,1);
					else
						strcpy(value,optarg);
					return strlen(optarg);
				case LOL_OPT_SHORT:	/* short */
					if(size!=2){
						lol_error(0,"[%s]:size not equal to 2 with short type. -- [-%c]",__func__,opt);
						return -1;
					}
					t_short=atol(optarg);
					memcpy(value,&t_short,2);
					break;
				case LOL_OPT_INT:	/* int */
					if(size!=4){
						lol_error(0,"[%s]:size not equal to 4 with int type. -- [-%c]",__func__,opt);
						return -1;
					}
					t_int=atol(optarg);
					memcpy(value,&t_int,4);
					break;
				case LOL_OPT_LONG:	/* long */
					if(size!=8){
						lol_error(0,"[%s]:size not equal to 8 with long type. -- [-%c]",__func__,opt);
						return -1;
					}
					t_long=atoll(optarg);
					memcpy(value,&t_long,8);
					break;
				case LOL_OPT_FLOAT:	/* float */
					if(size!=4){
						lol_error(0,"[%s]:size not equal to 4 with float type. -- [-%c]",__func__,opt);
						return -1;
					}
					t_float=atof(optarg);
					memcpy(value,&t_float,4);
					break;
				case LOL_OPT_DOUBLE:	/* double */
					if(size!=8){
						lol_error(0,"[%s]:size not equal to 8 with double type. -- [-%c]",__func__,opt);
						return -1;
					}
					t_double=atof(optarg);
					memcpy(value,&t_double,8);
					break;
				default:
					lol_error(0,"[%s]:illegal flags on value type. -- [-%c]",__func__,opt);
					return -1;
			}
			return 0;
		}
		switch(c){
			case ':':
				lol_error(0,"[%s]:option need an argument. -- [-%c]",__func__,optopt);
				return -1;
			case '?':
				lol_error(0,"[%s]:illegal option -- [-%c]",__func__,optopt);
				return -1;
			default:
				lol_error(0,"[%s]:getopt error.",__func__);
				return -1;
		}
	}
	if(flags & LOL_OPT_NOT_IGNORE){
		lol_error(0,"[%s]:option not exists -- [-%c]",__func__,opt);
		return -1;
	}

	return 0;
}
int lol_getopts(int argc,char *argv[],LOL_OPTARRAY *optarray,unsigned int optcount)
{
	LOL_OPTARRAY *p;
	char optstring[1024],*sp;
	int c,i;
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	bzero(optstring,sizeof(optstring));
	sp=optstring;
	*sp++=':';
	for(i=0,p=optarray;i<optcount;i++,p++){
		*sp++=p->opt;
		if(p->flags & LOL_OPT_ARG)
			*sp++=':';
		if(p->exists)
			*(p->exists)=0;	/* reset to not exists */
	}
	optind=1;
	while((c=getopt(argc,argv,optstring))!=EOF){
		for(i=0,p=optarray;i<optcount;i++,p++){
			if(p->opt==c){
				if(p->exists)
					*(p->exists)=1;	/* exists */
				if(p->flags & LOL_OPT_ARG){
					if(strlen(optarg)==0){
						if(p->flags & LOL_OPT_NOT_EMPTY){
							lol_error(0,"[%s]:option can't be empty. -- [-%c]",__func__,c);
							return -1;
						}
					}
					switch(p->flags & LOL_OPT_MASK_TYPE){
						case 0:			/* char(default) */
						case LOL_OPT_CHAR:	/* char */
							if(strlen(optarg)>=p->size){
								if(p->size!=1 || (p->size==1 && strlen(optarg)>1)){
									lol_error(0,"[%s]:option value too long. -- [-%c]",__func__,c);
									return -1;
								}
							}
							if(p->size==1)
								memcpy(p->value,optarg,1);
							else
								strcpy(p->value,optarg);
							break;
						case LOL_OPT_SHORT:	/* short */
							if(p->size!=2){
								lol_error(0,"[%s]:size not equal to 2 with short type. -- [-%c]",__func__,c);
								return -1;
							}
							t_short=atol(optarg);
							memcpy(p->value,&t_short,2);
							break;
						case LOL_OPT_INT:	/* int */
							if(p->size!=4){
								lol_error(0,"[%s]:size not equal to 4 with int type. -- [-%c]",__func__,c);
								return -1;
							}
							t_int=atol(optarg);
							memcpy(p->value,&t_int,4);
							break;
						case LOL_OPT_LONG:	/* long */
							if(p->size!=8){
								lol_error(0,"[%s]:size not equal to 8 with long type. -- [-%c]",__func__,c);
								return -1;
							}
							t_long=atoll(optarg);
							memcpy(p->value,&t_long,8);
							break;
						case LOL_OPT_FLOAT:	/* float */
							if(p->size!=4){
								lol_error(0,"[%s]:size not equal to 4 with float type. -- [-%c]",__func__,c);
								return -1;
							}
							t_float=atof(optarg);
							memcpy(p->value,&t_float,4);
							break;
						case LOL_OPT_DOUBLE:	/* double */
							if(p->size!=8){
								lol_error(0,"[%s]:size not equal to 8 with double type. -- [-%c]",__func__,c);
								return -1;
							}
							t_double=atof(optarg);
							memcpy(p->value,&t_double,8);
							break;
						default:
							lol_error(0,"[%s]:illegal flags on value type. -- [-%c]",__func__,c);
							return -1;
					}
				}
				break;
			}
		}
		if(i==optcount){
			switch(c){
				case ':':
					lol_error(0,"[%s]:option need an argument. -- [-%c]",__func__,optopt);
					return -1;
				case '?':
					lol_error(0,"[%s]:illegal option -- [-%c]",__func__,optopt);
					return -1;
				default:
					lol_error(0,"[%s]:getopt error.",__func__);
					return -1;
			}
		}
	}
	for(i=0,p=optarray;i<optcount;i++,p++){
		if((p->flags & LOL_OPT_NOT_IGNORE) && (!*p->exists)){
			lol_error(0,"[%s]:option not exists -- [-%c]",__func__,p->opt);
			return -1;
		}
	}

	return 0;	/* count of args */
}
int lol_getenv(const char *name,void *value,int size,int flags)
{
	const char *p;
	LOL_SHORT t_short;
	LOL_INT t_int;
	LOL_LONG t_long;
	float t_float;
	double t_double;

	if((p=getenv(name))==NULL){
		if(flags & LOL_ENV_NOT_IGNORE){
			lol_error(0,"[%s]:env not set.[name=%s]",__func__,name);
			return -1;
		}
		return 0;
	}
	if((flags & LOL_ENV_NOT_EMPTY) && (strlen(p)==0)){
		lol_error(0,"[%s]:env is empty.[name=%s]",__func__,name);
		return -1;
	}
	switch(flags & LOL_ENV_MASK_TYPE){
		case 0:			/* char(default) */
		case LOL_ENV_CHAR:	/* char */
			if(strlen(p)>=size){
				if(size!=1 || (size==1 && strlen(p)>1)){
					lol_error(0,"[%s]:env too long.[name=%s][value=%s]",__func__,name,p);
					return -1;
				}
			}
			if(size==1)
				memcpy(value,p,1);
			else
				strcpy(value,p);
			return strlen(p);
		case LOL_ENV_SHORT:	/* short */
			if(size!=2){
				lol_error(0,"[%s]:size not equal to 2 with short type![name=%s]",__func__,name);
				return -1;
			}
			t_short=atol(p);
			memcpy(value,&t_short,2);
			break;
		case LOL_ENV_INT:	/* int */
			if(size!=4){
				lol_error(0,"[%s]:size not equal to 4 with int type![name=%s]",__func__,name);
				return -1;
			}
			t_int=atol(p);
			memcpy(value,&t_int,4);
			break;
		case LOL_ENV_LONG:	/* long */
			if(size!=8){
				lol_error(0,"[%s]:size not equal to 8 with long type![name=%s]",__func__,name);
				return -1;
			}
			t_long=atoll(p);
			memcpy(value,&t_long,8);
			break;
		case LOL_ENV_FLOAT:	/* float */
			if(size!=4){
				lol_error(0,"[%s]:size not equal to 4 with float type![name=%s]",__func__,name);
				return -1;
			}
			t_float=atof(p);
			memcpy(value,&t_float,4);
			break;
		case LOL_ENV_DOUBLE:	/* double */
			if(size!=8){
				lol_error(0,"[%s]:size not equal to 8 with double type![name=%s]",__func__,name);
				return -1;
			}
			t_double=atof(p);
			memcpy(value,&t_double,8);
			break;
		default:
			lol_error(0,"[%s]:illegal flags on value type![name=%s]",__func__,name);
			return -1;
	}

	return 0;
}
int lol_getenvs(LOL_ENVARRAY *envarray,unsigned int envcount)
{
	LOL_ENVARRAY *p;
	int i;
	const char *value;

	/* init */
	for(i=0,p=envarray;i<envcount;i++,p++){
		if(p->exists)
			*(p->exists)=0;	/* reset to not exists */
	}

	p=envarray;
	for(i=0;i<envcount;i++,p++){
		if((value=getenv(p->name))==NULL){
			if(p->flags & LOL_ENV_NOT_IGNORE){
				lol_error(0,"[%s]:env not set.[name=%s]",__func__,p->name);
				return -1;
			}
			continue;
		}
		if(p->exists)
			*(p->exists)=1;
		if(lol_getenv(p->name,p->value,p->size,p->flags)<0){
			lol_errorerror();
			return -1;
		}
	}

	return 0;
}

int automkdir(const char *path,int mode)
{
	char *p,*q,dir[1024],pdir[1024],subdir[1024],pathseparatechar='/';
	int pos,dirlevel;

	if(mode==0)
		mode=0777;
	if(path==NULL){
		lol_error(0,"path is NULL!");
		return -1;
	}
	strcpy(dir,path);
	lol_strtrim(dir);
	if(dir[strlen(dir)-1]==pathseparatechar)
		dir[strlen(dir)-1]='\0';
	p=dir;
	q=pdir;
	if(lol_chrfind(dir,' ',0)>=0){
		lol_error(0,"directory do not support space between pathname![path=%s]",dir);
		return -1;
	}
	bzero(pdir,sizeof(pdir));
	if(*p=='/'){
		*q++='/';
		p++;
	}
	dirlevel=0;
	while(*p!='\0'){
		if((pos=lol_chrfind(p,pathseparatechar,0))<0){
			strcpy(subdir,p);
			if(dirlevel==0)
				strcpy(q,subdir);
			else
				sprintf(q,"/%s",subdir);
			if(access(pdir,F_OK)==0)
				return 0;
			if(lol_mkdir(pdir,mode)<0){
				lol_error(0,"mkdir error![lol_errno=%d][path=%s]",lol_errno,pdir);
				return -1;
			}
			return 0;
		}
		if(pos==0){
			lol_error(0,"invalid path![dir=%s]",dir);
			return -1;
		}
		if(dirlevel==0){
			snprintf(q,pos+1,"%s",p);
			q+=pos;
		}else{
			snprintf(q,pos+2,"/%s",p);
			q+=pos+1;
		}
		p+=pos+1;
		dirlevel++;
		if(access(pdir,F_OK)==0)
			continue;
		if(lol_mkdir(pdir,mode)<0){
			lol_error(0,"mkdir error![lol_errno=%d][path=%s]",lol_errno,pdir);
			return -1;
		}
	}
	return 0;
}
int lol_gettime(lol_time *t)
{
	struct tm *tm;
	time_t tt;
#ifdef __LOL_WINDOWS__
	SYSTEMTIME st;
#else
	struct timeval tv;
#endif

	tt=time(NULL);
	tm=localtime(&tt);
	t->year=tm->tm_year+1900;
	t->month=tm->tm_mon+1;
	t->day=tm->tm_mday;
	t->week=tm->tm_wday;
	t->hour=tm->tm_hour;
	t->minute=tm->tm_min;
	t->second=tm->tm_sec;
#ifdef __LOL_WINDOWS__
	GetLocalTime(&st);
	t->msecond=st.wMilliseconds;
#else
	gettimeofday(&tv,NULL);
	t->msecond=tv.tv_usec/1000;
#endif

	return 0;
}
int lol_sleep(unsigned int t)
{
#ifdef __LOL_WINDOWS__
	Sleep(t);
#else
	int secs,msecs;

	secs=t/1000;
	msecs=t%1000;
	if(secs>0)
		sleep(secs);
	if(msecs>0)
		usleep(msecs*1000);
#endif

	return 0;
}
