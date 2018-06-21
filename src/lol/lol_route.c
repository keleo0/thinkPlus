#include "lol_error.h"
#include "lol_utility.h"
#include "lol_gl.h"
#include "lol_route.h"

LOL_ROUTE *lol_routelogin(const char *addr,unsigned int id)
{
	LOL_ROUTE *route;
	LOL_NET *net;
	char msgbuf[1024],respcode[3],resptext[128];
	int msglen;
	char ip[16],port[6];

	/* connect to router */
	lol_strgetfield(addr,':',0,ip,sizeof(ip));
	lol_strgetfield(addr,':',1,port,sizeof(port));
	if((net=lol_netconnect(ip,atol(port)))==NULL){
		lol_errorerror();
		return NULL;
	}

	/* login */
	msglen=0;
	msglen+=lol_gl_put(msgbuf,sizeof(msgbuf)-msglen,1,'C',2,"10",2);
	msglen+=lol_gl_put(msgbuf,sizeof(msgbuf)-msglen,2,'I',0,&id,sizeof(id));
	if(lol_netsendmsg(net,msgbuf,msglen)<0){
		lol_errorerror();
		lol_netclose(net);
		return NULL;
	}
	if((msglen=lol_netrecvmsg(net,msgbuf,sizeof(msgbuf)))<0){
		lol_errorerror();
		lol_netclose(net);
		return NULL;
	}
	bzero(respcode,sizeof(respcode));
	if(lol_gl_get(msgbuf,msglen,125,'C',2,respcode,sizeof(respcode))<0){
		lol_errorerror();
		lol_netclose(net);
		return NULL;
	}
	if(lol_gl_get(msgbuf,msglen,126,'C',127,resptext,sizeof(resptext))<0){
		lol_errorerror();
		lol_netclose(net);
		return NULL;
	}
	if(strcmp(respcode,"00")!=0){
		lol_error(0,"[%s]:login failed.[%s:%s]",__func__,respcode,resptext);
		lol_netclose(net);
		return NULL;
	}
	if((route=malloc(sizeof(LOL_ROUTE)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		lol_netclose(net);
		return NULL;
	}
	bzero(route,sizeof(LOL_ROUTE));
	route->net=net;
	snprintf(route->addr,sizeof(route->addr),"%s",addr);
	route->id=id;

	return route;
}

int lol_routesend(LOL_ROUTE *route,unsigned int id,const void *buf,unsigned int len,int flags)
{
	char msgbuf[1024];
	int msglen;

	msglen=0;
	msglen+=lol_gl_put(msgbuf,sizeof(msgbuf)-msglen,1,'C',2,"20",2);
	msglen+=lol_gl_put(msgbuf,sizeof(msgbuf)-msglen,4,'I',0,&id,sizeof(id));
	msglen+=lol_gl_put(msgbuf,sizeof(msgbuf)-msglen,5,'I',0,&len,sizeof(len));
	if(lol_netsendmsg(route->net,msgbuf,msglen)<0){
		lol_errorerror();
		return -1;
	}
	if(lol_netsend(route->net,buf,len,LOL_NET_WAIT)<0){
		lol_errorerror();
		return -1;
	}

	return 0;
}
int lol_routerecv(LOL_ROUTE *route,unsigned int *id,void *buf,unsigned int siz,int flags)
{
	char msgbuf[1024],msgtype[3];
	int msglen,len;

	msglen=0;
	if((msglen=lol_netrecvmsg(route->net,msgbuf,sizeof(msgbuf)))<0){
		lol_errorerror();
		return -1;
	}
	bzero(msgtype,sizeof(msgtype));
	lol_gl_get(msgbuf,msglen,1,'C',2,msgtype,sizeof(msgtype));
	if(strcmp(msgtype,"20")!=0){
		lol_error(0,"[%s]:msgtype illegal.[%s]",__func__,msgtype);
		return -1;
	}
	lol_gl_get(msgbuf,msglen,3,'I',0,id,sizeof(int));
	lol_gl_get(msgbuf,msglen,5,'I',0,&len,sizeof(len));
	if(len>siz){
		lol_error(0,"[%s]:message too long.[%d>%d]",__func__,len,siz);
		return -1;
	}
	if(lol_netrecv(route->net,buf,len,LOL_NET_WAIT)<0){
		lol_errorerror();
		return -1;
	}

	return len;
}
int lol_routelogout(LOL_ROUTE *route)
{
	lol_netclose(route->net);
	free(route);

	return 0;
}
