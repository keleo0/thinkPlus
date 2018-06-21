#include "lol.h"

void logview(	const char *logip,
		int logpid,
		const char *logtime,
		int loglevel,
		const char *logdesc,
		const char *loginfo,
		char format);
int main(int argc,char *argv[])
{
	char usage[]="usage: logview [-a addr] [-Lm] [-I ip] [-P pid] [-t time] [-l level] [-d desc] [-p pattern] [-f format] [name]";
	char msgbuf[1024];
	int msglen,r,lineno;
	char path[256],subpath[256],fullpath[256],filename[256],fullname[256];
	char logip[16],logtime[20],logdesc[32],loginfo[1024];
	int logpid,loglevel;
	char respcode[3],resptext[128],endflag;
	LOL_NET *net;
	LOL_DB *db;
	char addr[256],ip[16],time[20],desc[32],pattern[128],format,name[256];
	int L_Exists,m_Exists;
	int I_Exists,P_Exists,t_Exists,l_Exists,d_Exists,p_Exists;
	int pid;
	int level;
	char servip[16],servport[6];
	LOL_OPTARRAY optarray[10]={
		{'a',addr,sizeof(addr),LOL_OPT_ARG,NULL},
		{'L',NULL,0,0,&L_Exists},
		{'m',NULL,0,0,&m_Exists},
		{'I',ip,sizeof(ip),LOL_OPT_ARG,&I_Exists},
		{'P',&pid,sizeof(pid),LOL_OPT_ARG|LOL_OPT_NOT_EMPTY|LOL_OPT_INT,&P_Exists},
		{'t',time,sizeof(time),LOL_OPT_ARG|LOL_OPT_NOT_EMPTY,&t_Exists},
		{'l',&level,sizeof(level),LOL_OPT_ARG|LOL_OPT_NOT_EMPTY|LOL_OPT_INT,&l_Exists},
		{'d',desc,sizeof(desc),LOL_OPT_ARG,&d_Exists},
		{'p',pattern,sizeof(pattern),LOL_OPT_ARG|LOL_OPT_NOT_EMPTY,&p_Exists},
		{'f',&format,1,LOL_OPT_ARG|LOL_OPT_NOT_EMPTY,NULL}
	};
	LOL_ENVARRAY envarray[2]={
		{"LOG_ADDR",addr,sizeof(addr),0,NULL},
		{"LOG_NAME",name,sizeof(name),0,NULL}
	};
	
	bzero(addr,sizeof(addr));
	bzero(ip,sizeof(ip));
	pid=0;
	bzero(time,sizeof(time));
	level=0;
	bzero(desc,sizeof(desc));
	bzero(pattern,sizeof(pattern));
	format=0;
	bzero(name,sizeof(name));

	if(lol_getenvs(envarray,2)<0){
		lol_showerror();
		exit(-1);
	}
	if(lol_getopts(argc,argv,optarray,10)<0){
		lol_showerror();
		puts(usage);
		exit(-1);
	}
	if(argc-optind>1){
		puts(usage);
		exit(-1);
	}
	if(argc-optind==1)
		strcpy(name,argv[optind]);

	if(L_Exists){	/* local */
		/* get path */
		strcpy(path,addr);
		if(strlen(path)>1 && path[strlen(path)-1]=='/')
			path[strlen(path)-1]='\0';	/* delete last separate char '/' */
		if(strlen(path)==0)
			strcpy(path,".");	/* current dir */

		/* get subpath & filename */
		bzero(subpath,sizeof(subpath));
		bzero(filename,sizeof(filename));
		if(name[0]=='/'){
			printf("name illegal.[%s]\n",name);
			return -1;
		}
		if((r=lol_chrfind(name,'/',-1))<0){
			strcpy(subpath,"");
			strcpy(filename,name);
		}else{
			memcpy(subpath,name,r);
			strcpy(filename,name+r+1);
		}

		/* get fullpath */
		if(strlen(subpath)==0)
			strcpy(fullpath,path);
		else
			sprintf(fullpath,"%s/%s",path,subpath);
		if(automkdir(fullpath,0777)<0){	/* auto mkdir */
			lol_showerror();
			return -1;
		}

		/* get full file */
		sprintf(fullname,"%s/log%s.db",fullpath,filename);
		if((db=lol_db_openx(fullname,LOL_DB_RDONLY,NULL))==NULL){
			lol_showerror();
			return -1;
		}
		lineno=1;
		while(lol_db_fetch(db)==1){
			lol_db_getfield(db,0,logip,sizeof(logip),0);
			lol_db_getfield(db,1,&logpid,sizeof(logpid),LOL_DB_INT);
			lol_db_getfield(db,2,logtime,sizeof(logtime),0);
			lol_db_getfield(db,3,&loglevel,sizeof(loglevel),LOL_DB_INT);
			lol_db_getfield(db,4,logdesc,sizeof(logdesc),0);
			lol_db_getfield(db,5,loginfo,sizeof(loginfo),0);
			lineno++;

			if(I_Exists && strcmp(logip,ip)!=0)
				continue;
			if(P_Exists && logpid!=pid)
				continue;
			if(t_Exists && strcmp(logtime,time)!=0)
				continue;
			if(l_Exists && loglevel<level)
				continue;
			if(d_Exists && strcmp(logdesc,desc)!=0)
				continue;
			if(p_Exists && strstr(loginfo,pattern)==NULL)
				continue;
			logview(logip,logpid,logtime,loglevel,logdesc,loginfo,format);
		}
		lol_db_close(db);
	}else{	/* NET */
		/* init */
		msglen=0;

		/* 1 msgtype */
		if(m_Exists)	/* monitor */
			msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,1,'C',2,"40",2);
		else	/* query */
			msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,1,'C',2,"30",2);

		/* 2 name */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,2,'C',127,name,strlen(name));

		/* 4 ip */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,4,'C',15,ip,strlen(ip));

		/* 5 pid */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,5,'I',0,&pid,sizeof(pid));

		/* 7 time */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,7,'C',19,time,strlen(time));

		/* 8 level */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,8,'I',0,&level,sizeof(level));

		/* 9 desc */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,9,'C',31,desc,strlen(desc));

		/* 10 pattern */
		msglen+=lol_gl_put(msgbuf+msglen,sizeof(msgbuf)-msglen,10,'C',31,pattern,strlen(pattern));

		/* ip:port */
		bzero(servip,sizeof(servip));
		bzero(servport,sizeof(servport));
		if(strlen(addr)==0)
			strcpy(addr,"127.0.0.1:10000");
		lol_strgetfield(addr,':',0,servip,sizeof(servip));
		lol_strgetfield(addr,':',1,servport,sizeof(servport));

		/* net start */
		lol_netstart();

		/* connect */
		if((net=lol_netconnect(servip,atol(servport)))==NULL){
			lol_showerror();
			return -1;
		}

		/* send package */
		if(lol_netsendmsg(net,msgbuf,msglen)<0){
			lol_showerror();
			lol_netclose(net);
			return -1;
		}
		if((msglen=lol_netrecvmsg(net,msgbuf,sizeof(msgbuf)))<0){
			lol_showerror();
			lol_netclose(net);
			return -1;
		}
		bzero(respcode,sizeof(respcode));
		lol_gl_get(msgbuf,msglen,125,'C',2,respcode,sizeof(respcode));
		bzero(resptext,sizeof(resptext));
		lol_gl_get(msgbuf,msglen,126,'C',127,resptext,sizeof(resptext));
		if(strcmp(respcode,"00")!=0){
			lol_error(0,"[%s]:[%s:%s]",__func__,respcode,resptext);
			lol_netclose(net);
			return -1;
		}
		endflag='0';
		while(1){
			if((msglen=lol_netrecvmsg(net,msgbuf,sizeof(msgbuf)))<0){
				lol_showerror();
				break;
			}
			bzero(logip,sizeof(logip));
			logpid=0;
			bzero(logtime,sizeof(logtime));
			loglevel=0;
			bzero(logdesc,sizeof(logdesc));
			bzero(loginfo,sizeof(loginfo));

			lol_gl_get(msgbuf,msglen,4,'C',15,logip,sizeof(logip));
			lol_gl_get(msgbuf,msglen,5,'I',0,&logpid,sizeof(logpid));
			lol_gl_get(msgbuf,msglen,7,'C',19,logtime,sizeof(logtime));
			lol_gl_get(msgbuf,msglen,8,'I',0,&loglevel,sizeof(loglevel));
			lol_gl_get(msgbuf,msglen,9,'C',31,logdesc,sizeof(logdesc));
			lol_gl_get(msgbuf,msglen,10,'C',511,loginfo,sizeof(loginfo));
			lol_gl_get(msgbuf,msglen,11,'C',1,&endflag,1);
			if(endflag=='1')
				break;
			logview(logip,logpid,logtime,loglevel,logdesc,loginfo,format);
		}
		lol_netclose(net);

		/* net stop */
		lol_netstop();
	}

	return 0;
}

void logview(	const char *logip,
		int logpid,
		const char *logtime,
		int loglevel,
		const char *logdesc,
		const char *loginfo,
		char format)
{
	char prompt[128];
	int pos,r;

	/* view mode */
	switch(format){
		case '0':
			sprintf(prompt,"[%s]-[%d]-[%s]-",logtime,loglevel,logdesc);
			break;
		case '1':
			sprintf(prompt,"[%s]-",logtime);
			break;
		case '2':
			sprintf(prompt,"%s","");
			break;
		case '3':
			sprintf(prompt,"[%s]-[%d]-[%s]-[%d]-[%s]-",logip,logpid,logtime,loglevel,logdesc);
			break;
		default:
			sprintf(prompt,"[%s]-[%d]-[%s]-",logtime,loglevel,logdesc);
			break;
	};

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
