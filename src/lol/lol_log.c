#include "lol_error.h"
#include "lol_utility.h"
#include "lol_gl.h"
#include "lol_log.h"

LOL_LOG *lol_logopen(const char *addr,const char *name,int flags)
{
	LOL_NET *net;
	char msgbuf[1024],respcode[3],resptext[128];
	char path[256],subpath[256],fullpath[256],filename[256];
	char fullname[256],ip[16],port[6];
	LOL_LOG *lp;
	LOL_DB *db;
	int msglen,r,oflags;
	LOL_DB_FIELD fieldarray[6]={
		{"IP",'C',15,0},
		{"PID",'N',5,0},
		{"Time",'C',19,0},
		{"Level",'N',5,0},
		{"Desc",'C',31,0},
		{"Info",'C',255,0}
	};

	/* lp */
	if((lp=malloc(sizeof(LOL_LOG)))==NULL){
		lol_error(0,"[%s]:malloc lol_error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	bzero(lp,sizeof(LOL_LOG));
	lp->net=NULL;	/* net */
	lp->db=NULL;	/* local */
	snprintf(lp->addr,sizeof(lp->addr),"%s",addr);	/* addr */
	snprintf(lp->name,sizeof(lp->name),"%s",name);	/* name */
	lp->flags=flags;	/* flags */
	lp->pid=getpid();	/* pid */
	if((lp->flags & LOL_LOG_MASK_LEVEL)==0)
		lp->flags|=LOL_LOG_NORMAL;	/* default level */

	if(flags & LOL_LOG_LOCAL){	/* local */
		/* get path */
		strcpy(path,lp->addr);
		if(strlen(path)>1 && path[strlen(path)-1]=='/')
			path[strlen(path)-1]='\0';	/* delete last separate char '/' */
		if(strlen(path)==0)
			strcpy(path,".");	/* current dir */

		/* get subpath & filename */
		bzero(subpath,sizeof(subpath));
		bzero(filename,sizeof(filename));
		if(lp->name[0]=='/'){
			lol_error(0,"[%s]:name illegal.[%s]",__func__,name);
			free(lp);
			return NULL;
		}
		if((r=lol_chrfind(lp->name,'/',-1))<0){
			strcpy(subpath,"");
			strcpy(filename,lp->name);
		}else{
			memcpy(subpath,lp->name,r);
			strcpy(filename,lp->name+r+1);
		}

		/* get fullpath */
		if(strlen(subpath)==0)
			strcpy(fullpath,path);
		else
			sprintf(fullpath,"%s/%s",path,subpath);
		if(automkdir(fullpath,0777)<0){	/* auto mkdir */
			lol_errorerror();
			free(lp);
			return NULL;
		}

		/* get full file */
		sprintf(fullname,"%s/log%s.db",fullpath,filename);

		/* db */
		oflags=LOL_DB_CREAT|LOL_DB_RDWR|LOL_DB_APPEND|LOL_DB_SYNC;
		if(lp->flags & LOL_LOG_TRUNC)
			oflags|=LOL_DB_TRUNC;
		if(lp->flags & LOL_LOG_EXCL)
			oflags|=LOL_DB_EXCL;
		if((db=lol_db_open(fullname,oflags,fieldarray,6))==NULL){
			lol_errorerror();
			free(lp);
			return NULL;
		}
		lp->db=db;
	}else{	/* NET */
		/* init */
		msglen=0;

		/* 1 msgtype */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,1,'C',2,"10",2);

		/* 2 name */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,2,'C',127,name,strlen(name));

		/* 3 flags */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,3,'I',0,&flags,sizeof(flags));

		/* 5 pid */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,5,'I',0,&lp->pid,sizeof(lp->pid));

		/* ip:port */
		bzero(ip,sizeof(ip));
		bzero(port,sizeof(port));
		if(strlen(lp->addr)==0)
			strcpy(lp->addr,"127.0.0.1:10000");
		lol_strgetfield(lp->addr,':',0,ip,sizeof(ip));
		lol_strgetfield(lp->addr,':',1,port,sizeof(port));

		/* connect */
		if((net=lol_netconnect(ip,atol(port)))==NULL){
			lol_errorerror();
			free(lp);
			return NULL;
		}

		/* send package */
		if(lol_netsendmsg(net,msgbuf,msglen)<0){
			lol_errorerror();
			lol_netclose(net);
			free(lp);
			return NULL;
		}
		if((msglen=lol_netrecvmsg(net,msgbuf,sizeof(msgbuf)))<0){
			lol_errorerror();
			lol_netclose(net);
			free(lp);
			return NULL;
		}
		bzero(respcode,sizeof(respcode));
		if(lol_gl_get(msgbuf,msglen,125,'C',2,respcode,sizeof(respcode))<0){
			lol_errorerror();
			lol_netclose(net);
			free(lp);
			return NULL;
		}
		if(lol_gl_get(msgbuf,msglen,126,'C',127,resptext,sizeof(resptext))<0){
			lol_errorerror();
			lol_netclose(net);
			free(lp);
			return NULL;
		}
		if(strcmp(respcode,"00")!=0){
			lol_error(0,"[%s]:[%s:%s]",__func__,respcode,resptext);
			lol_netclose(net);
			free(lp);
			return NULL;
		}
		lp->net=net;
	}

	return lp;
}

int lol_logsend(LOL_LOG *lp,int flags,const char *desc,const char *fmt,...)
{
	char msgbuf[1024];
	int msglen,r;
	va_list args;
	char logtime[20],logdesc[32],loginfo[512];
	int loglevel,pos;
	char prompt[128];
	lol_time tm;

	/* time "yyyy-mm-dd hh:mm:ss" */
	lol_gettime(&tm);
	sprintf(logtime,"%04d-%02d-%02d %02d:%02d:%02d",tm.year,tm.month,tm.day,tm.hour,tm.minute,tm.second);

	/* level */
	loglevel=flags & LOL_LOG_MASK_LEVEL;
	if(loglevel==0)
		loglevel=LOL_LOG_NORMAL;	/* default level */
	if(loglevel<(lp->flags & LOL_LOG_MASK_LEVEL))
		return 0;	/* level lower than settings */

	/* desc */
	bzero(logdesc,sizeof(logdesc));
	snprintf(logdesc,sizeof(logdesc),"%s",desc);

	/* info */
	va_start(args,fmt);
	vsnprintf(loginfo,sizeof(loginfo),fmt,args);
	va_end(args);

	/* print */
	if(!(lp->flags & LOL_LOG_QUIET) && (lp->flags & LOL_LOG_PRINT || flags & LOL_LOG_PRINT)){
		sprintf(prompt,"[%s]-[%d]-[%s]-",logtime,loglevel,logdesc);
		fwrite(prompt,1,strlen(prompt),stdout);
		pos=0;
		while(pos!=strlen(loginfo)){
			r=lol_chrfind(loginfo+pos,'\n',0);
			if(r<0 || (pos+r+1)==strlen(loginfo)){
				fwrite(loginfo+pos,1,strlen(loginfo)-pos,stdout);
				break;
			}
			fwrite(loginfo+pos,1,r+1,stdout);
			fwrite(prompt,1,strlen(prompt),stdout);
			pos+=r+1;
		}
		fwrite("\n",1,1,stdout);
	}

	if(!(lp->flags & LOL_LOG_LOCAL)){	/* NET */
		/* init */
		msglen=0;

		/* 1 msgtype */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,1,'C',2,"20",2);

		/* 7 time */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,7,'C',19,logtime,strlen(logtime));

		/* 8 level */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,8,'I',0,&loglevel,sizeof(loglevel));

		/* 9 desc */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,9,'C',31,logdesc,strlen(logdesc));

		/* 10 info */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,10,'C',511,loginfo,strlen(loginfo));

		/* send package */
		if(lol_netsendmsg(lp->net,msgbuf,msglen)<0){
			lol_errorerror();
			return -1;
		}
	}else{	/* local */
		lol_db_new(lp->db);
		lol_db_putfield(lp->db,1,&lp->pid,sizeof(lp->pid),LOL_DB_INT);
		lol_db_putfield(lp->db,2,logtime,strlen(logtime),0);
		lol_db_putfield(lp->db,3,&loglevel,sizeof(loglevel),LOL_DB_INT);
		lol_db_putfield(lp->db,4,logdesc,strlen(logdesc),0);
		lol_db_putfield(lp->db,5,loginfo,strlen(loginfo),0);
		if(lol_db_insert(lp->db)<0){
			lol_errorerror();
			return -1;
		}
	}

	return 0;
}
int lol_logclose(LOL_LOG *lp)
{
	if(lp->flags & LOL_LOG_LOCAL){
		/* local */
		lol_db_close(lp->db);
	}else{	/* net */
		lol_netclose(lp->net);
	}
	free(lp);

	return 0;
}

int lol_loggetflags(const char *strflags)
{
	int flags;
	
	flags=0;
	if(strstr(strflags,"debug"))
		flags|=LOL_LOG_DEBUG;
	if(strstr(strflags,"normal"))
		flags|=LOL_LOG_NORMAL;
	if(strstr(strflags,"info"))
		flags|=LOL_LOG_INFO;
	if(strstr(strflags,"key"))
		flags|=LOL_LOG_KEY;
	if(strstr(strflags,"warn"))
		flags|=LOL_LOG_WARN;
	if(strstr(strflags,"error"))
		flags|=LOL_LOG_ERROR;
	if(strstr(strflags,"fatal"))
		flags|=LOL_LOG_FATAL;
	if(strstr(strflags,"print"))
		flags|=LOL_LOG_PRINT;
	if(strstr(strflags,"trunc"))
		flags|=LOL_LOG_TRUNC;
	if(strstr(strflags,"excl"))
		flags|=LOL_LOG_EXCL;
	if(strstr(strflags,"quiet"))
		flags|=LOL_LOG_QUIET;
	if(strstr(strflags,"local"))
		flags|=LOL_LOG_LOCAL;
	
	return flags;
}

