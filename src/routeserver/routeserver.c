#include "routeserver.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: routeserver [-p cfgpath] [-c cfgfile] [-d cfgdomain]";
	int lsnport;
	char lsnip[16],str[6];
	LOL_NET *net;
	LOL_CFG *cfg;
	int r_net,r_timer;
	LOL_OPTARRAY optarray[3]={
		{'p',cfgpath,sizeof(cfgpath),LOL_OPT_ARG,NULL},
		{'c',cfgfile,sizeof(cfgfile),LOL_OPT_ARG|LOL_OPT_NOT_EMPTY,NULL},
		{'d',cfgdomain,sizeof(cfgdomain),LOL_OPT_ARG,NULL}
	};
	LOL_CFGARRAY cfgarray[8]={
		{cfgdomain,"logname",log_name,sizeof(log_name),0,NULL},
		{cfgdomain,"logflags",log_flags,sizeof(log_flags),0,NULL},
		{cfgdomain,"logaddr",log_addr,sizeof(log_addr),0,NULL},
		{cfgdomain,"routeid",&route_id,sizeof(route_id),LOL_CFG_INT|LOL_CFG_NOT_EMPTY,NULL},
		{cfgdomain,"nextroutes",next_routes,sizeof(next_routes),0,NULL},
		{cfgdomain,"alivetime",&alive_time,sizeof(alive_time),LOL_CFG_INT|LOL_CFG_NOT_EMPTY,NULL},
		{cfgdomain,"lsnaddr",lsn_addr,sizeof(lsn_addr),LOL_CFG_NOT_EMPTY,NULL},
		{cfgdomain,"lsnaddr_ctrl",lsn_addr_ctrl,sizeof(lsn_addr_ctrl),LOL_CFG_NOT_EMPTY,NULL}
	};

	/* signal set */
	signal(SIGPIPE,SIG_IGN);

	/* get options from command line */
	if(lol_getopts(argc,argv,optarray,3)<0){
		lol_showerror();
		return -1;
	}

	/* get cfgs from cfg file */
	if((cfg=lol_loadcfg(cfgfile,cfgpath))==NULL){
		lol_showerror();
		return -1;
	}
	if(lol_getcfgs(cfg,cfgarray,8)<0){
		lol_showerror();
		return -1;
	}
	lol_freecfg(cfg);

	/* init next router list */
	initrouterlist(next_routes);

	/* net start */
	lol_netstart();

	/* netcenter */
	netcenter=lol_netcenter_new();

	/* listening ctrl port ... */
	lol_strgetfield(lsn_addr_ctrl,':',0,lsnip,sizeof(lsnip));
	lol_strgetfield(lsn_addr_ctrl,':',1,str,sizeof(str));
	lsnport=atol(str);
	if((net=lol_netlisten(lsnip,lsnport))==NULL){
		lol_showerror();
		return -1;
	}
	lol_netcenter_netadd(netcenter,net,acceptnewclient_ctrl);
	lsnnet_ctrl=net; /* lsnnet_ctrl */

	/* listening normal port ... */
	lol_strgetfield(lsn_addr,':',0,lsnip,sizeof(lsnip));
	lol_strgetfield(lsn_addr,':',1,str,sizeof(str));
	lsnport=atol(str);
	if((net=lol_netlisten(lsnip,lsnport))==NULL){
		lol_showerror();
		return -1;
	}
	lol_netcenter_netadd(netcenter,net,acceptnewclient);
	lsnnet=net; /* lsnnet */

	/* open log */
	if((lp=lol_logopen(log_addr,log_name,lol_loggetflags(log_flags)))==NULL){
		lol_showerror();
		return -1;
	}

	/* idle */
	lol_logsend(lp,LOL_LOG_PRINT,"system","working ...");
	while(1){
		lol_logsend(lp,0,"system","netcenter idle ...");
		if((r_net=lol_netcenter_idle(netcenter,-1))<0){
			lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
			lol_showerror();
			break;
		}
		lol_logsend(lp,0,"system","timerlist idle ...");
		r_timer=lol_timerlist_idle(&timerlist);
		if(r_net==0 && r_timer==0)
			lol_sleep(10);
	}

	/* net stop */
	lol_netstop();

	return 0;
}
void acceptnewclient(LOL_NETCENTER_NET *net,int event)
{
	LOL_NET *client;

	switch(event){
		case LOL_NETCENTER_ACCEPT:
			if((client=lol_netaccept(net->net))==NULL){
				lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
				return;
			}
			lol_logsend(lp,LOL_LOG_INFO|LOL_LOG_PRINT,"network","connected by %s:%d",client->ip,client->port);
			cliunitadd(client);
			break;
		case LOL_NETCENTER_ERROR:
			lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","net error on connect [%s:%d]",net->net->ip,net->net->port);
			break;
		default:
			break;
	}
}
void clientnethandle(LOL_NETCENTER_NET *net,int event)
{
	char msgbuf[1024];
	int msglen;
	int r;

	switch(event){
		case LOL_NETCENTER_DATARECVED:
			/* check msg */
			while(1){
				if((r=lol_netcenter_ismsgok(net))<0){
					lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
					cliunitdel(net);
					return;
				}
				if(r==0)
					break;
				/* recv msg */
				if((msglen=lol_netcenter_recvmsg(net,msgbuf,sizeof(msgbuf)))<0){
					lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
					cliunitdel(net);
					return;
				}
				/* process msg */
				if(clientmsgprocess(net,msgbuf,msglen)<0){
					lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
					cliunitdel(net);
					lol_netcenter_netdel(netcenter,net->net);
					return;
				}
			}
			break;
		case LOL_NETCENTER_DISCONNECTED:
			lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","net disconnected [%s:%d]",net->net->ip,net->net->port);
			cliunitdel(net);
			break;
		case LOL_NETCENTER_ERROR:
			lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","net error on connect [%s:%d]",net->net->ip,net->net->port);
			cliunitdel(net);
			break;
		default:
			break;
	}
}
int clientmsgprocess(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	char msgtype[3];

	bzero(msgtype,sizeof(msgtype));
	if(lol_gl_get(msgbuf,msglen,1,'C',2,msgtype,sizeof(msgtype))<0)
		return -1;
	if(strcmp(msgtype,"00")==0){
		if(client_test(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"10")==0){
		if(client_login(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"12")==0){
		if(client_logout(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"20")==0){
		if(client_msgroute(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"30")==0){
		if(client_routenotice(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"40")==0){
		if(client_ping(net,msgbuf,msglen)<0)
			return -1;
	}else{
		lol_error(0,"[%s]:unsupported msgtype[%s]",__func__,msgtype);
		return -1;
	}

	return 0;
}
void acceptnewclient_ctrl(LOL_NETCENTER_NET *net,int event)
{
	LOL_NET *client;

	switch(event){
		case LOL_NETCENTER_ACCEPT:
			if((client=lol_netaccept(net->net))==NULL){
				lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
				return;
			}
			lol_logsend(lp,LOL_LOG_INFO|LOL_LOG_PRINT,"network","connected by %s:%d",client->ip,client->port);
			cliunitadd_ctrl(client);
			break;
		case LOL_NETCENTER_ERROR:
			lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","net error on connect [%s:%d]",net->net->ip,net->net->port);
			break;
		default:
			break;
	}
}
void clientnethandle_ctrl(LOL_NETCENTER_NET *net,int event)
{
	char msgbuf[1024];
	int msglen;
	int r;

	switch(event){
		case LOL_NETCENTER_DATARECVED:
			/* check msg */
			while(1){
				if((r=lol_netcenter_ismsgok(net))<0){
					lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
					cliunitdel_ctrl(net);
					return;
				}
				if(r==0)
					break;
				/* recv msg */
				if((msglen=lol_netcenter_recvmsg(net,msgbuf,sizeof(msgbuf)))<0){
					lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
					cliunitdel_ctrl(net);
					return;
				}
				/* process msg */
				if(clientmsgprocess_ctrl(net,msgbuf,msglen)<0){
					lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
					cliunitdel_ctrl(net);
					lol_netcenter_netdel(netcenter,net->net);
					return;
				}
			}
			break;
		case LOL_NETCENTER_DISCONNECTED:
			lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","net disconnected [%s:%d]",net->net->ip,net->net->port);
			cliunitdel_ctrl(net);
			break;
		case LOL_NETCENTER_ERROR:
			lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"network","net error on connect [%s:%d]",net->net->ip,net->net->port);
			cliunitdel_ctrl(net);
			break;
		default:
			break;
	}
}
int clientmsgprocess_ctrl(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	char msgtype[3];

	bzero(msgtype,sizeof(msgtype));
	if(lol_gl_get(msgbuf,msglen,1,'C',2,msgtype,sizeof(msgtype))<0)
		return -1;
	if(strcmp(msgtype,"10")==0){
		if(_logopen(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"20")==0){
		if(_logsend(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"30")==0 || strcmp(msgtype,"40")==0){
		if(_logview(net,msgbuf,msglen)<0)
			return -1;
	}else if(strcmp(msgtype,"50")==0){
		if(_logclose(net,msgbuf,msglen)<0)
			return -1;
	}else{
		lol_error(0,"[%s]:unsupported msgtype[%s]",__func__,msgtype);
		return -1;
	}

	return 0;
}
int client_login(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	return 0;
}
int client_logout(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	return 0;
}
int _logopen(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	char name[128];
	LOL_DB *db;
	struct logunit_t *logp;
	struct cliunit_t *clip;
	struct viewlist_t *viewlistp;
	int r,oflags,flags,pid;
	char subpath[256],fullpath[256],filename[256],file[256];
	char msgbufout[1024],respcode[3],resptext[128];
	int msglenout;

	lol_logsend(lp,LOL_LOG_INFO,"system","logopen ...");
	/* 2 name */
	bzero(name,sizeof(name));
	if(lol_gl_get(msgbuf,msglen,2,'C',127,name,sizeof(name)-1)<0){
		lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"protocol","%s",__lol_errmsg__);
		return -1;
	}
	lol_logsend(lp,LOL_LOG_DEBUG,"business","logname=%s",name);
	if(strcmp(name,log_name)==0){
		lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"business","open on this log is forbiddened!");
		return -1;
	}

	/* 3 flag */
	flags=0;
	if(lol_gl_get(msgbuf,msglen,3,'I',0,&flags,sizeof(flags))<0){
		lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"protocol","%s",__lol_errmsg__);
		return -1;
	}
	/* 4 pid */
	pid=0;
	if(lol_gl_get(msgbuf,msglen,5,'I',0,&pid,sizeof(pid))<0){
		lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"protocol","%s",__lol_errmsg__);
		return -1;
	}
	if((logp=logunitsearch(name))==NULL){
		lol_logsend(lp,LOL_LOG_DEBUG,"system","this is a new log!");
		/* get subpath & filename */
		bzero(subpath,sizeof(subpath));
		bzero(filename,sizeof(filename));
		if(name[0]=='/'){
			lol_logsend(lp,LOL_LOG_ERROR,"business","name illegal.");
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
			strcpy(fullpath,log_path);
		else
			sprintf(fullpath,"%s/%s",log_path,subpath);
		lol_logsend(lp,LOL_LOG_DEBUG,"system","creating dir [%s] ...",fullpath);
		if(automkdir(fullpath,0777)<0){	/* auto mkdir */
			lol_logsend(lp,LOL_LOG_DEBUG,"system","	[Failed]\n%s",__lol_errmsg__);
			return -1;
		}
		lol_logsend(lp,LOL_LOG_DEBUG,"system","	[OK]");

		/* get full file */
		sprintf(file,"%s/log%s.db",fullpath,filename);

		/* db */
		oflags=LOL_DB_CREAT|LOL_DB_WRONLY|LOL_DB_APPEND|LOL_DB_SYNC;
		if(flags & LOL_LOG_TRUNC)
			oflags|=LOL_DB_TRUNC;
		if(flags & LOL_LOG_EXCL)
			oflags|=LOL_DB_EXCL;
		if((db=lol_db_open(file,oflags,fieldarray,6))==NULL){
			lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"system","%s",__lol_errmsg__);
			return -1;
		}
		logunitadd(name,db,oflags,file);
		logp=logunitsearch(name);
		/* go though clilist for add to logunit's viewers */
		lol_logsend(lp,LOL_LOG_NORMAL,"business","add viewers waiting to this log ...");
		clip=clilist;
		do{
			if(clip->svflag!='1'){	/* not viewer */
				clip=clip->next;
				continue;
			}
			viewlistp=clip->viewlist;
			do{
				if(strcmp(viewlistp->name,name)==0){
					cliplistadd(&logp->viewlist,clip,viewlistp);
					viewlistp->logunit=logp;
					lol_logsend(lp,LOL_LOG_NORMAL,"business","%s:%d added",clip->net->net->ip,clip->net->net->port);
				}
				viewlistp=viewlistp->next;
			}while(viewlistp!=clip->viewlist);
			clip=clip->next;
		}while(clip!=clilist);
		lol_logsend(lp,LOL_LOG_NORMAL,"business","viewers added ok!");
	}
	clip=cliunitsearch(net);
	clip->logunit=logp;
	clip->svflag='0';	/* sender */
	clip->pid=pid;
	cliplistadd(&logp->clilist,clip,NULL);	/* add to logunit's clilist */
	lol_logsend(lp,LOL_LOG_DEBUG,"business","added to sender list.");

	/* resp */
	strcpy(respcode,"00");
	strcpy(resptext,"succeed!");
	msglenout=0;
	msglenout+=lol_gl_put(msgbufout,sizeof(msgbufout),1,'C',2,"11",2);
	msglenout+=lol_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,125,'C',2,respcode,strlen(respcode));
	msglenout+=lol_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,126,'C',127,resptext,strlen(resptext));
	if(lol_netcenter_sendmsg(net,msgbufout,msglenout)<0){
		lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"net","pkgsend error![%s]",__lol_errmsg__);
		return -1;
	}


	return 0;
}
int _logsend(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	struct cliunit_t *p;
	LOL_DB *db;
	char logbuf[1024];
	int loglen;
	struct cliplist_t *cliplistp;
	char logtime[20],logdesc[32],loginfo[512];
	int loglevel;

	lol_logsend(lp,LOL_LOG_INFO,"system","lol_logsend ...");
	memcpy(logbuf,msgbuf,msglen);
	loglen=msglen;
	p=cliunitsearch(net);
	if(p->svflag!='0'){
		lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"business","this is not a sender!");
		return -1;
	}
	db=p->logunit->db;

	/* 7 level */
	bzero(logtime,sizeof(logtime));
	lol_gl_get(logbuf,loglen,7,'C',19,logtime,sizeof(logtime)-1);

	/* 8 level */
	loglevel=0;	/* normal */
	lol_gl_get(logbuf,loglen,8,'I',0,&loglevel,sizeof(loglevel));

	/* 9 desc */
	bzero(logdesc,sizeof(logdesc));
	lol_gl_get(logbuf,loglen,9,'C',31,logdesc,sizeof(logdesc));

	/* 10 info */
	bzero(loginfo,sizeof(loginfo));
	lol_gl_get(logbuf,loglen,10,'C',511,loginfo,sizeof(loginfo));

	lol_db_new(db);
	lol_db_putfield(db,0,p->net->net->ip,strlen(p->net->net->ip),0);
	lol_db_putfield(db,1,&(p->pid),sizeof(p->pid),LOL_DB_INT);
	lol_db_putfield(db,2,logtime,strlen(logtime),0);
	lol_db_putfield(db,3,&loglevel,sizeof(loglevel),LOL_DB_INT);
	lol_db_putfield(db,4,logdesc,strlen(logdesc),0);
	lol_db_putfield(db,5,loginfo,strlen(loginfo),0);
	lol_db_insert(db);

	/* send to logunit's viewers */
	cliplistp=p->logunit->viewlist;
	if(cliplistp==NULL)	/* no viewers */
		return 0;

	do{
		if(strlen(cliplistp->viewunit->ip)>0){
			if(strcmp(p->net->net->ip,cliplistp->viewunit->ip)!=0){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		if(cliplistp->viewunit->pid>0){
			if(p->pid!=cliplistp->viewunit->pid){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		if(loglevel<cliplistp->viewunit->level){
			cliplistp=cliplistp->next;
			continue;
		}
		if(strlen(cliplistp->viewunit->desc)>0){
			if(strcmp(logdesc,cliplistp->viewunit->desc)!=0){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		if(strlen(cliplistp->viewunit->pattern)>0){
			if(strstr(loginfo,cliplistp->viewunit->pattern)==NULL){
				cliplistp=cliplistp->next;
				continue;
			}
		}
		loglen=0;
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,1,'C',2,"41",2);
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,4,'C',15,p->net->net->ip,strlen(p->net->net->ip));
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,5,'I',0,&p->pid,sizeof(p->pid));
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,7,'C',19,logtime,strlen(logtime));
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,8,'I',0,&loglevel,sizeof(loglevel));
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,9,'C',31,logdesc,strlen(logdesc));
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,10,'C',511,loginfo,strlen(loginfo));
		if(lol_netcenter_sendmsg(cliplistp->cliunit->net,logbuf,loglen)<0)
			lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
		cliplistp=cliplistp->next;
	}while(cliplistp!=p->logunit->viewlist);

	return 0;
}
int _logview(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	char v_ip[16],v_time[20],v_desc[32],v_pattern[32];
	char logip[16],logtime[20],logdesc[32],loginfo[512];
	int v_pid,v_level;
	int logpid,loglevel;
	LOL_DB *db;
	char logbuf[1024];
	int loglen,r;
	struct cliunit_t *clip;
	struct viewlist_t **viewlist,*viewunit,*first,*last;
	struct logunit_t *logunit;
	char msgtype[3],v_name[128];
	int oflags;
	char subpath[256],fullpath[256],filename[256],file[256];
	char msgbufout[1024],respcode[3],resptext[128];
	int msglenout;

	lol_logsend(lp,LOL_LOG_INFO,"system","logview ...");

	bzero(msgtype,sizeof(msgtype));
	lol_gl_get(msgbuf,msglen,1,'C',2,msgtype,sizeof(msgtype)-1);

	bzero(v_name,sizeof(v_name));
	if(lol_gl_get(msgbuf,msglen,2,'C',31,v_name,sizeof(v_name)-1)<0){
		lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"protocol","%s",__lol_errmsg__);
		return -1;
	}
	lol_logsend(lp,LOL_LOG_NORMAL,"business","logname=%s",v_name);
	if(strcmp(v_name,log_name)==0)
		lol_logsend(lp,LOL_LOG_INFO|LOL_LOG_PRINT,"business","viewing logserver's log!");

	bzero(v_ip,sizeof(v_ip));
	lol_gl_get(msgbuf,msglen,4,'C',15,v_ip,sizeof(v_ip)-1);

	v_pid=0;
	lol_gl_get(msgbuf,msglen,5,'I',0,&v_pid,sizeof(v_pid));

	bzero(v_time,sizeof(v_time));
	lol_gl_get(msgbuf,msglen,7,'C',19,v_time,sizeof(v_time)-1);

	v_level=LOL_LOG_NORMAL;	/* default level */
	lol_gl_get(msgbuf,msglen,8,'I',0,&v_level,sizeof(v_level));

	bzero(v_desc,sizeof(v_desc));
	lol_gl_get(msgbuf,msglen,9,'C',31,v_desc,sizeof(v_desc)-1);

	bzero(v_pattern,sizeof(v_pattern));
	lol_gl_get(msgbuf,msglen,10,'C',31,v_pattern,sizeof(v_pattern)-1);

	clip=cliunitsearch(net);
	if(strcmp(msgtype,"30")==0){	/* query */
		lol_logsend(lp,LOL_LOG_DEBUG,"business","this is a query.");
		if(strcmp(v_name,log_name)==0){
			db=lp->db;
		}else if((logunit=logunitsearch(v_name))==NULL){
			/* get subpath & filename */
			bzero(subpath,sizeof(subpath));
			bzero(filename,sizeof(filename));
			if(v_name[0]=='/'){
				lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"business","name illegal");
				return -1;
			}
			if((r=lol_chrfind(v_name,'/',-1))<0){
				strcpy(subpath,"");
				strcpy(filename,v_name);
			}else{
				memcpy(subpath,v_name,r);
				strcpy(filename,v_name+r+1);
			}

			/* get fullpath */
			if(strlen(subpath)==0)
				strcpy(fullpath,log_path);
			else
				sprintf(fullpath,"%s/%s",log_path,subpath);

			/* get full file */
			sprintf(file,"%s/log%s.db",fullpath,filename);

			lol_logsend(lp,LOL_LOG_NORMAL,"system","db opening ...");
			/* db */
			oflags=LOL_DB_RDONLY;
			if((db=lol_db_open(file,oflags,NULL,0))==NULL){
				lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"system","%s",__lol_errmsg__);
				return -1;
			}
		}else{
			db=logunit->db;
		}

		/* resp */
		strcpy(respcode,"00");
		strcpy(resptext,"succeed!");
		msglenout=0;
		msglenout+=lol_gl_put(msgbufout,sizeof(msgbufout),1,'C',2,"31",2);
		msglenout+=lol_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,125,'C',2,respcode,strlen(respcode));
		msglenout+=lol_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,126,'C',127,resptext,strlen(resptext));
		lol_netcenter_sendmsg(net,msgbufout,msglenout);

		/* query */
		lol_logsend(lp,LOL_LOG_NORMAL,"system","db querying ...");
		lol_db_seek(db,0,LOL_DB_SEEK_SET);
		while(lol_db_fetch(db)==1){
			lol_db_getfield(db,0,logip,sizeof(logip),0);
			lol_db_getfield(db,1,&logpid,sizeof(logpid),LOL_DB_INT);
			lol_db_getfield(db,2,logtime,sizeof(logtime),0);
			lol_db_getfield(db,3,&loglevel,sizeof(loglevel),LOL_DB_INT);
			lol_db_getfield(db,4,logdesc,sizeof(logdesc),0);
			lol_db_getfield(db,5,loginfo,sizeof(loginfo),0);
			if(strlen(v_time)>0){
				if(strcmp(logtime,v_time)<0)
					continue;
			}
			if(strlen(v_ip)>0){
				if(strcmp(logip,v_ip)!=0)
					continue;
			}
			if(strlen(v_time)>0){
				if(strcmp(logtime,v_time)<0)
					continue;
			}
			if(v_pid>0){
				if(logpid!=v_pid)
					continue;
			}
			if(loglevel<v_level)
				continue;
			if(strlen(v_desc)>0){
				if(strcmp(logdesc,v_desc)!=0)
					continue;
			}
			if(strlen(v_pattern)>0){
				if(strstr(loginfo,v_pattern)==NULL)
					continue;
			}
			loglen=0;
			loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,1,'C',2,"31",2);
			loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,4,'C',15,logip,strlen(logip));
			loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,5,'I',0,&logpid,sizeof(logpid));
			loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,7,'C',19,logtime,strlen(logtime));
			loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,8,'I',0,&loglevel,sizeof(loglevel));
			loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,9,'C',31,logdesc,strlen(logdesc));
			loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,10,'C',511,loginfo,strlen(loginfo));
			if(lol_netcenter_sendmsg(net,logbuf,loglen)<0){
				lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
				if(logunit==NULL)
					lol_db_close(db);
				return -1;
			}
		}
		if(strcmp(v_name,log_name)!=0 && logunit==NULL)
			lol_db_close(db);
		lol_logsend(lp,LOL_LOG_NORMAL,"business","query completed!");
		loglen=0;
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,1,'C',2,"31",2); /* msgtype */
		loglen+=lol_gl_put(logbuf+loglen,sizeof(logbuf)-loglen,11,'C',1,"1",1); /* endflag */
		if(lol_netcenter_sendmsg(net,logbuf,loglen)<0){
			lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"network","%s",__lol_errmsg__);
			return -1;
		}
	}else{	/* viewer */
		lol_logsend(lp,LOL_LOG_NORMAL,"business","this is a monitor.");
		if(strcmp(v_name,log_name)==0){
			lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"business","unsupported operation on logserver's log.");
			return -1;
		}
		clip->svflag='1';

		/* add to cliunit's viewlist */
		viewunit=malloc(sizeof(struct viewlist_t));
		bzero(viewunit,sizeof(struct viewlist_t));
		strcpy(viewunit->name,v_name);
		strcpy(viewunit->ip,v_ip);
		viewunit->pid=v_pid;
		viewunit->level=v_level;
		strcpy(viewunit->desc,v_desc);
		strcpy(viewunit->pattern,v_pattern);
		viewunit->prior=NULL;
		viewunit->next=NULL;
		viewlist=&clip->viewlist;
		if(*viewlist==NULL){
			*viewlist=viewunit;
			viewunit->prior=viewunit;
			viewunit->next=viewunit;
		}else{
			first=*viewlist;
			last=(*viewlist)->prior;
			viewunit->next=first;
			viewunit->prior=last;
			first->prior=viewunit;
			last->next=viewunit;
		}

		/* add to logunit's viewlist */
		lol_logsend(lp,LOL_LOG_NORMAL,"business","adding to logname's viewlist.");
		logunit=loglist;
		if(loglist==NULL){
			lol_logsend(lp,LOL_LOG_WARN,"business","waiting for log opened!");
		}else{
			do{
				if(strcmp(logunit->logname,v_name)==0){
					cliplistadd(&logunit->viewlist,clip,viewunit);
					viewunit->logunit=logunit;
					break;
				}
				logunit=logunit->next;
			}while(logunit!=loglist);
			if(viewunit->logunit)
				lol_logsend(lp,LOL_LOG_NORMAL,"business","add ok.");
			else
				lol_logsend(lp,LOL_LOG_WARN,"business","waiting for log opened!");
		}
		/* resp */
		strcpy(respcode,"00");
		strcpy(resptext,"succeed!");
		msglenout=0;
		msglenout+=lol_gl_put(msgbufout,sizeof(msgbufout),1,'C',2,"41",2);
		msglenout+=lol_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,125,'C',2,respcode,strlen(respcode));
		msglenout+=lol_gl_put(msgbufout+msglenout,sizeof(msgbufout)-msglenout,126,'C',127,resptext,strlen(resptext));
		if(lol_netcenter_sendmsg(net,msgbufout,msglenout)<0){
			lol_logsend(lp,LOL_LOG_ERROR|LOL_LOG_PRINT,"net","pkgsend error![%s]",__lol_errmsg__);
			return -1;
		}
	}

	return 0;
}
int _logclose(LOL_NETCENTER_NET *net,char *msgbuf,int msglen)
{
	//close viewlists

	return 0;
}
int logunitadd(const char *logname,LOL_DB *db,int flags,const char *file)
{
	struct logunit_t *p,*first,*last;

	p=malloc(sizeof(struct logunit_t));
	bzero(p,sizeof(struct logunit_t));
	strcpy(p->logname,logname);
	p->db=db;
	strcpy(p->file,file);
	p->flags=flags;
	p->clilist=NULL;
	p->viewlist=NULL;
	p->prior=NULL;
	p->next=NULL;
	if(loglist==NULL){
		loglist=p;
		loglist->prior=loglist;
		loglist->next=loglist;
	}else{
		first=loglist;
		last=loglist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}

	return 0;
}
int logunitdel(const char *logname)
{
	struct logunit_t *p,*prior,*next;

	p=logunitsearch(logname);
	if(p==p->next){
		free(p);
		loglist=NULL;
	}else{
		if(p==loglist)
			loglist=loglist->next;
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		free(p);
	}

	return 0;
}
struct logunit_t *logunitsearch(const char *logname)
{
	struct logunit_t *p;

	p=loglist;
	if(p==NULL)
		return NULL;
	do{
		if(strcmp(p->logname,logname)==0)
			return p;
		p=p->next;
	}while(p!=loglist);

	return NULL;
}
struct cliunit_t *cliunitsearch(LOL_NETCENTER_NET *net)
{
	struct cliunit_t *p;
	p=clilist;

	do{
		if(p->net==net)
			break;
		p=p->next;
	}while(p!=clilist);

	return p;
}
int cliunitdel(LOL_NETCENTER_NET *net)
{
	struct cliunit_t *p,*prior,*next;
	struct logunit_t *logunit;
	struct cliplist_t *_clilist,*viewplist,*nextclip,*priorclip;
	struct viewlist_t *viewlist,*nextviewlist,*priorviewlist;
	struct cliplist_t *priorviewplist,*nextviewplist;

	lol_netclose(net->net);
	p=cliunitsearch(net);
	if(p->svflag=='0'){ /* sender */
		lol_logsend(lp,LOL_LOG_NORMAL,"system","removing a sender.");
		//remove logunit's cliuser
		lol_logsend(lp,LOL_LOG_NORMAL,"system","remove from logunit's sender list.");
		logunit=p->logunit;
		_clilist=logunit->clilist;
		do{
			if(_clilist->cliunit!=p){
				_clilist=_clilist->next;
				continue;
			}
			if(_clilist==_clilist->next){ /* only me send to this logunit */
				free(_clilist);
				logunit->clilist=NULL;
			}else{
				if(_clilist==logunit->clilist)
					logunit->clilist=_clilist->next;
				priorclip=_clilist->prior;
				nextclip=_clilist->next;
				priorclip->next=nextclip;
				nextclip->prior=priorclip;
				free(_clilist);
			}
			break;
		}while(_clilist!=logunit->clilist);
		if(logunit->clilist==NULL){ /* close this logunit if no user send */
			lol_logsend(lp,LOL_LOG_INFO|LOL_LOG_PRINT,"business","close log because no sender.");
			/* set related viewflag to 0 */
			lol_logsend(lp,LOL_LOG_NORMAL,"business","make viewer waiting.");
			viewplist=logunit->viewlist;
			while(viewplist){
				viewplist->viewunit->logunit=NULL;
				lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"business","status changed to wait.%s:%d",viewplist->cliunit->net->net->ip,viewplist->cliunit->net->net->port);
				if(viewplist==viewplist->next){
					free(viewplist);
					break;
				}
				logunit->viewlist=viewplist->next;
				priorviewplist=viewplist->prior;
				nextviewplist=viewplist->next;
				priorviewplist->next=nextviewplist;
				nextviewplist->prior=priorviewplist;
				free(viewplist);
				viewplist=logunit->viewlist;
			}
			lol_db_close(logunit->db);
			logunitdel(logunit->logname);
		}
	}else if(p->svflag=='1'){	/* viewer */
		lol_logsend(lp,LOL_LOG_NORMAL,"business","removing a viewer.");
		lol_logsend(lp,LOL_LOG_NORMAL,"business","removing from logunit's viewer list.");
		//remove all viewlists
		viewlist=p->viewlist;
		while(viewlist){
			if(viewlist->logunit){ /* viewing */
				viewplist=viewlist->logunit->viewlist;
				do{
					if(viewplist->viewunit!=viewlist){
						viewplist=viewplist->next;
						continue;
					}
					if(viewplist==viewplist->next){
						viewlist->logunit->viewlist=NULL;
						free(viewplist);
						break;
					}
					if(viewlist->logunit->viewlist==viewplist)
						viewlist->logunit->viewlist=viewplist->next;
					priorviewplist=viewplist->prior;
					nextviewplist=viewplist->next;
					priorviewplist->next=nextviewplist;
					nextviewplist->prior=priorviewplist;
					free(viewplist);
					break;
				}while(viewplist!=viewlist->logunit->viewlist);
			}
			if(viewlist==viewlist->next){ /* last unit */
				free(viewlist);
				break;
			}
			if(viewlist==p->viewlist)
				p->viewlist=viewlist->next;
			priorviewlist=viewlist->prior;
			nextviewlist=viewlist->next;
			priorviewlist->next=nextviewlist;
			nextviewlist->prior=priorviewlist;
			free(viewlist);
			viewlist=p->viewlist;
		}
	}
	if(p==p->next){
		free(p);
		clilist=NULL;
	}else{
		if(p==clilist)
			clilist=clilist->next;
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		free(p);
	}

	return 0;
}
int cliunitadd(LOL_NET *net)
{
	struct cliunit_t *p,*first,*last;

	p=malloc(sizeof(struct cliunit_t));
	bzero(p,sizeof(struct cliunit_t));
	p->prior=NULL;
	p->next=NULL;
	if(clilist==NULL){
		clilist=p;
		clilist->prior=clilist;
		clilist->next=clilist;
	}else{
		first=clilist;
		last=clilist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}
	p->net=lol_netcenter_netadd(netcenter,net,clientnethandle)->net;

	return 0;
}
int cliplistadd(struct cliplist_t **cliplist,struct cliunit_t *cliunit,struct viewlist_t *viewunit)
{
	struct cliplist_t *p,*first,*last;

	p=malloc(sizeof(struct cliplist_t));
	bzero(p,sizeof(struct cliplist_t));
	p->cliunit=cliunit;
	p->viewunit=viewunit;
	p->prior=NULL;
	p->next=NULL;
	if(*cliplist==NULL){
		*cliplist=p;
		p->prior=p;
		p->next=p;
	}else{
		first=*cliplist;
		last=(*cliplist)->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}
	return 0;
}
struct routerlist_t *routeradd(const char *ip,int port)
{
	struct routerlist_t *p,*first,*last;
	int r;

	if((p=malloc(sizeof(struct routerlist_t)))==NULL){
		lol_error(0,"[%s]:malloc error![errno=%d]",__func__,errno);
		return NULL;
	}
	strcpy(p->ip,ip);
	p->port=port;
	p->net=NULL;
	p->timer=NULL;
	p->prior=NULL;
	p->next=NULL;
	if(routerlist==NULL){
		routerlist=p;
		routerlist->prior=routerlist;
		routerlist->next=routerlist;
	}else{
		first=routerlist;
		last=routerlist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}

	return p;
}
int routerdel(const char *ip,int port)
{
	struct routerlist_t *p,*prior,*next;
	p=routersearch(ip,port);
	if(p==p->next){
		free(p);
		routerlist=NULL;
	}else{
		if(p==routerlist)
			routerlist=routerlist->next;
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		free(p);
	}
	return 0;
}
struct routerlist_t *routersearch(const char *ip,int port)
{
	struct routerlist_t *p;
	p=routerlist;
	if(p==NULL)
		return NULL;
	do{
		if(strcmp(p->ip,ip)==0 && p->port==port)
			return p;
		p=p->next;
	}while(p!=routerlist);
	return NULL;
}
int initrouterlist(const char *nextrouters)
{
	char addr[32],ip[16],port[6];
	int i,count;
	struct routerlist_t *router;
	LOL_TIMER *timer;
	
	if(strlen(nextrouters)==0)
		return 0;
	count=lol_chrstat(nextrouters,'|');
	count++;
	for(i=0;i<count;i++){
		lol_strgetfield(nextrouters,'|',i,addr,sizeof(addr));
		lol_strgetfield(addr,':',0,ip,sizeof(ip));
		lol_strgetfield(addr,':',1,port,sizeof(port));
		router=routeradd(ip,atol(port));
		/* connect timer */
		timer=lol_timernew(routerconnect,3000);
		router->timer=timer;
	}

	return 0;
}
void routerconnect(LOL_TIMER *timer)
{
	struct routelist_t *router;

	router=routersearchbytimer(timer);
	if((router->net=lol_netconnect(router->ip,router->port))==NULL){
		lol_logsend(lp,LOL_LOG_WARN|LOL_LOG_PRINT,"routerconnect","connect to router failed![%s:%d][%s]",router->ip,router->port,__lol_errmsg__);
		return;
	}
	lol_logsend(lp,LOL_LOG_INFO|LOL_LOG_PRINT,"routerconnect","connect to router succeeded![%s:%d]",router->ip,router->port);
	lol_timerdel(&timerlist,timer);
	router->timer=NULL;
}
struct routerlist_t *routersearchbytimer(LOL_TIMER *timer)
{
	struct routerlist_t *p;
	p=routerlist;
	if(p==NULL)
		return NULL;
	do{
		if(p->timer==timer)
			return p;
		p=p->next;
	}while(p!=routerlist);
	return NULL;
}
struct route_t *routeadd(int destrouteid,LOL_NET *net,int distance)
{
	struct routelist_t *p,*first,*last;
	int r;

	if((p=malloc(sizeof(struct routelist_t)))==NULL){
		lol_error(0,"[%s]:malloc error![errno=%d]",__func__,errno);
		return NULL;
	}
	strcpy(p->ip,ip);
	p->port=port;
	p->net=NULL;
	p->timer=NULL;
	p->prior=NULL;
	p->next=NULL;
	if(routelist==NULL){
		routelist=p;
		routelist->prior=routelist;
		routelist->next=routelist;
	}else{
		first=routelist;
		last=routelist->prior;
		p->next=first;
		p->prior=last;
		first->prior=p;
		last->next=p;
	}

	return p;
}
int routedel(const char *ip,int port)
{
	struct routelist_t *p,*prior,*next;
	p=routesearch(ip,port);
	if(p==p->next){
		free(p);
		routelist=NULL;
	}else{
		if(p==routelist)
			routelist=routelist->next;
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		free(p);
	}
	return 0;
}
struct routelist_t *routesearch(const char *ip,int port)
{
	struct routelist_t *p;
	p=routelist;
	if(p==NULL)
		return NULL;
	do{
		if(strcmp(p->ip,ip)==0 && p->port==port)
			return p;
		p=p->next;
	}while(p!=routelist);
	return NULL;
}
