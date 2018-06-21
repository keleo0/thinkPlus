#include "lol_error.h"
#include "lol_net.h"

int lol_netstart(void)
{
#ifdef __LOL_WINDOWS__
	WSADATA data;
	
	if(WSAStartup(MAKEWORD(2,2),&data)<0){
		lol_error(0,"[%s]:WSAStartup error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return -1;
	}
#endif
	
	return 0;
}

int lol_netstop(void)
{
#ifdef __LOL_WINDOWS__
	if(WSACleanup()<0){
		lol_error(0,"[%s]:WSACleanup error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return -1;
	}
#endif

	return 0;
}

LOL_NET *lol_netconnect(const char *ip,unsigned short port)
{
	struct sockaddr_in servaddr;
	int sockfd;
	LOL_NET *net;

	bzero(&servaddr,sizeof(struct sockaddr_in));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	if((servaddr.sin_addr.s_addr=inet_addr(ip))==INADDR_NONE){
		lol_error(0,"[%s]:inet_addr error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return NULL;
	}
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		lol_error(0,"[%s]:socket create error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return NULL;
	}
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(struct sockaddr_in))<0){
		lol_error(0,"[%s]:connect error.[%d:%s][%s:%d]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno),ip,port);
		socketclose(sockfd);
		return NULL;
	}
	if((net=malloc(sizeof(LOL_NET)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		socketclose(sockfd);
		return NULL;
	}
	memset(net,0x00,sizeof(LOL_NET));
	net->sockfd=sockfd;
	snprintf(net->ip,sizeof(net->ip),"%s",ip);
	net->port=port;

	return net;
}

LOL_NET *lol_netlisten(const char *ip,unsigned short port)
{
	struct sockaddr_in servaddr;
	int sockfd;
	struct linger so_linger;
	int option;
	LOL_NET *net;

	bzero(&servaddr,sizeof(struct sockaddr_in));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	if(strcmp(ip,"*")==0){
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}else{
		if((servaddr.sin_addr.s_addr=inet_addr(ip))==INADDR_NONE){
			lol_error(0,"[%s]:inet_addr error.[%d:%s][%s:%d]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno),ip,port);
			return NULL;
		}
	}
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){
		lol_error(0,"[%s]:socket create error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return NULL;
	}
	so_linger.l_onoff=1;
	so_linger.l_linger=0;
	if(setsockopt(sockfd,SOL_SOCKET,SO_LINGER,(void *)&so_linger,sizeof(so_linger))<0){
		lol_error(0,"[%s]:setsockopt error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		socketclose(sockfd);
		return NULL;
	}
	option=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(void *)&option,sizeof(option))<0){
		lol_error(0,"[%s]:setsockopt error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		socketclose(sockfd);
		return NULL;
	}
	option=1;
	if(setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,(void *)&option,sizeof(option))<0){
		lol_error(0,"[%s]:setsockopt error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		socketclose(sockfd);
		return NULL;
	}
	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
		lol_error(0,"[%s]:bind error.[%d:%s][%s:%d]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno),ip,port);
		socketclose(sockfd);
		return NULL;
	}
	if(listen(sockfd,5)<0){
		lol_error(0,"[%s]:listen error.[%d:%s][%s:%d]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno),ip,port);
		socketclose(sockfd);
		return NULL;
	}
	if((net=malloc(sizeof(LOL_NET)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		socketclose(sockfd);
		return NULL;
	}
	memset(net,0x00,sizeof(LOL_NET));
	net->sockfd=sockfd;
	snprintf(net->ip,sizeof(net->ip),"%s",ip);
	net->port=port;
	net->flags|=LOL_NET_LISTENER;

	return net;
}

LOL_NET *lol_netaccept(LOL_NET *net)
{
	struct sockaddr_in addr;
	socklen_t addrlen;
	int sockfd;
	char *p;
	LOL_NET *client;

	if(!(net->flags & LOL_NET_LISTENER)){
		lol_error(0,"[%s]:net is not a listener.",__func__);
		return NULL;
	}
	addrlen=sizeof(addr);
	if((sockfd=accept(net->sockfd,(struct sockaddr *)&addr,&addrlen))<0){
		lol_error(0,"[%s]:accept error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return NULL;
	}
	if((p=inet_ntoa(addr.sin_addr))==NULL){
		lol_error(0,"[%s]:inet_ntoa error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return NULL;
	}
	if((client=malloc(sizeof(LOL_NET)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		socketclose(sockfd);
		return NULL;
	}
	memset(client,0x00,sizeof(LOL_NET));
	client->sockfd=sockfd;
	snprintf(client->ip,sizeof(client->ip),"%s",p);
	client->port=ntohs(addr.sin_port);

	return client;
}

int lol_netrecv(LOL_NET *net,void *buf,unsigned int siz,int flags)
{
	int n,nbytes;

	for(nbytes=0;siz>0;nbytes+=n,siz-=n){
		if((n=recv(net->sockfd,(char *)buf+nbytes,siz,0))<=0){
			lol_error(0,"[%s]:read error![%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
			return -1;
		}
		if(!(flags & LOL_NET_WAIT))
			return n;
	}

	return nbytes;
}

int lol_netsend(LOL_NET *net,const void *buf,unsigned int len,int flags)
{
	int n,nbytes;

	for(nbytes=0;len>0;nbytes+=n,len-=n){
		if((n=send(net->sockfd,(char *)buf+nbytes,len,0))<=0){
			lol_error(0,"[%s]:write error![%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
			return -1;
		}
		if(!(flags & LOL_NET_WAIT))
			return n;
	}

	return nbytes;
}

int lol_netlist_add(LOL_NETLIST **netlist,LOL_NET *net)
{
	LOL_NETLIST *head,*tail;
	LOL_NETLIST *p;

	if((p=malloc(sizeof(LOL_NETLIST)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
	memset(p,0x00,sizeof(LOL_NETLIST));
	p->net=net;
	p->prior=NULL;
	p->next=NULL;
	if(*netlist==NULL){
		p->prior=p;
		p->next=p;
		*netlist=p;
	}else{
		head=*netlist;
		tail=(*netlist)->prior;
		p->prior=tail;
		tail->next=p;
		p->next=head;
		head->prior=p;
	}

	return 0;
}

int lol_netlist_del(LOL_NETLIST **netlist,LOL_NET *net)
{
	LOL_NETLIST *p;

	if((p=lol_netlist_find(*netlist,net))==NULL){
		lol_error(0,"[%s]:net not found.",__func__);
		return -1;
	}
	p->net=NULL;

	return 0;
}

LOL_NETLIST *lol_netlist_find(LOL_NETLIST *netlist,LOL_NET *net)
{
	LOL_NETLIST *p;
	
	if(netlist==NULL)
		return NULL;
	p=netlist;
	do{
		if(p->net==net)
			return p;
		p=p->next;
	}while(p!=netlist);

	return NULL;
}

int lol_netlist_clean(LOL_NETLIST **netlist)
{
	LOL_NETLIST *next,*prior;
	LOL_NETLIST *p;

	if(*netlist==NULL)
		return 0;
	/* netclean */
	p=*netlist;
	do{
		if(p==*netlist && p->net==NULL){
			if(p==p->next){
				*netlist=NULL;
				free(p);
				return 0;
			}else{
				*netlist=p->next;
				next=p->next;
				prior=p->prior;
				prior->next=next;
				next->prior=prior;				
				free(p);
				p=*netlist;
				continue;
			}
		}
		if(p->net!=NULL){
			p=p->next;
			if(p==*netlist)
				break;
			continue;
		}
		next=p->next;
		prior=p->prior;
		prior->next=next;
		next->prior=prior;
		free(p);
		p=next;
		if(p==*netlist)
			break;
	}while(1);

	return 0;
}
int lol_netlist_free(LOL_NETLIST **netlist)
{
	LOL_NETLIST *p;

	if(*netlist==NULL)
		return 0;
	p=*netlist;
	do{
		p->net=NULL;
		p=p->next;
	}while(p!=*netlist);
	lol_netlist_clean(netlist);

	return 0;
}
int lol_netselect(LOL_NETLIST **netlist,int timeout)
{
	fd_set fdrset,fdwset,fdeset;
	int maxfd;
	LOL_NET *net;
	LOL_NETLIST *p;
	struct timeval t,*pt;
	int r;

	lol_netlist_clean(netlist);
	if(*netlist==NULL)
		return 0;
	FD_ZERO(&fdrset);
	FD_ZERO(&fdwset);
	FD_ZERO(&fdeset);
	maxfd=0;
	p=*netlist;
	do{
		net=p->net;
		if(p->flags & LOL_NET_READ)
			FD_SET(net->sockfd,&fdrset);
		if(p->flags & LOL_NET_WRITE)
			FD_SET(net->sockfd,&fdwset);
		if(p->flags & LOL_NET_ERROR)
			FD_SET(net->sockfd,&fdeset);
		maxfd=net->sockfd>maxfd?net->sockfd:maxfd;
		p=p->next;
	}while(p!=*netlist);
	if(timeout<0)
		pt=NULL;
	else{
		pt=&t;
		t.tv_sec=0;
		t.tv_usec=timeout;
	}
	if((r=select(maxfd+1,&fdrset,&fdwset,&fdeset,pt))<0){
		if(lol_socketerrno==EINTR)
			return 0;
		lol_error(0,"[%s]:select error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return -1;
	}
	if(r==0)
		return 0;
	p=*netlist;
	do{
		p->flags&=~LOL_NET_READ;
		p->flags&=~LOL_NET_WRITE;
		p->flags&=~LOL_NET_ERROR;
		net=p->net;
		if(FD_ISSET(net->sockfd,&fdrset))
			p->flags|=LOL_NET_READ;
		if(FD_ISSET(net->sockfd,&fdwset))
			p->flags|=LOL_NET_WRITE;
		if(FD_ISSET(net->sockfd,&fdeset))
			p->flags|=LOL_NET_ERROR;
		p=p->next;
	}while(p!=*netlist);

	return r;
}

#ifdef __LOL_UNIX__
int lol_netpoll(LOL_NETLIST **netlist,int timeout)
{
	struct pollfd fdarray[1024];
	int nfds;
	LOL_NET *net;
	LOL_NETLIST *p;
	int i,j,r;
	
	lol_netlist_clean(netlist);
	if(*netlist==NULL)
		return 0;
	nfds=0;
	p=*netlist;
	do{
		net=p->net;
		fdarray[nfds].fd=net->sockfd;
		if(p->flags & LOL_NET_READ)
			fdarray[nfds].events=POLLIN;
		if(p->flags & LOL_NET_WRITE)
			fdarray[nfds].events&=POLLOUT;
		if(p->flags & LOL_NET_ERROR)
			fdarray[nfds].events&=POLLERR;
		fdarray[nfds].revents=0;
		nfds++;
		p=p->next;
	}while(p!=*netlist);
	if((r=poll(fdarray,nfds,timeout))<0){
		if(lol_socketerrno==EINTR)
			return 0;
		lol_error(0,"[%s]:poll error.[%d:%s]",__func__,lol_socketerrno,lol_strerror(lol_socketerrno));
		return -1;
	}
	if(r==0)
		return 0;
	p=*netlist;
	do{
		net=p->net;
		net->flags&=~LOL_NET_READ;
		net->flags&=~LOL_NET_WRITE;
		net->flags&=~LOL_NET_ERROR;
		for(i=0;i<nfds;i++){
			if(fdarray[i].fd==net->sockfd){
				if(fdarray[i].revents&POLLIN)
					net->flags|=LOL_NET_READ;
				if(fdarray[i].revents&POLLOUT)
					net->flags|=LOL_NET_WRITE;
				if(fdarray[i].revents&POLLERR)
					net->flags|=LOL_NET_ERROR;
				break;
			}
		}
		p=p->next;
	}while(p!=*netlist);

	return r;
}
#endif

int lol_netclose(LOL_NET *net)
{
	socketclose(net->sockfd);
	free(net);

	return 0;
}

int lol_netsendmsg(LOL_NET *net,const void *buf,unsigned int len)
{
	int t;

	t=htonl(len);
	if(lol_netsend(net,&t,4,LOL_NET_WAIT)<0){
		lol_errorerror();
		return -1;
	}
	if(lol_netsend(net,buf,len,LOL_NET_WAIT)<0){
		lol_errorerror();
		return -1;
	}

	return 0;
}

int lol_netrecvmsg(LOL_NET *net,void *buf,unsigned int siz)
{
	int len;

	if(lol_netrecv(net,&len,4,LOL_NET_WAIT)<0){
		lol_errorerror();
		return -1;
	}
	len=ntohl(len);
	if(len>siz){
		lol_error(0,"[%s]:msg too long![siz=%d][len=%d]",__func__,siz,len);
		return -1;
	}
	if(lol_netrecv(net,buf,len,LOL_NET_WAIT)<0){
		lol_errorerror();
		return -1;
	}

	return len;
}
