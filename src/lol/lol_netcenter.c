#include "lol_error.h"
#include "lol_netcenter.h"

int lol_netcenter_recvto(LOL_NETCENTER_NET *net)
{
	LOL_NETCENTER_BUFLIST *p,*head,*tail;
	char *buf;
	int n;

	if((buf=malloc(LOL_NETCENTER_BUFSIZE))==NULL){
		lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
	if((n=lol_netrecv(net->net,buf,LOL_NETCENTER_BUFSIZE,0))<0){
		lol_errorerror();
		free(buf);
		return -1;
	}
	if(n==0)
		return 0;
	if((p=malloc(sizeof(LOL_NETCENTER_BUFLIST)))==NULL){
		lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(buf);
		return -1;
	}
	memset(p,0x00,sizeof(LOL_NETCENTER_BUFLIST));
	p->buf=buf;
	p->pos=0;
	p->len=n;
	p->prior=NULL;
	p->next=NULL;
	if(net->rlist==NULL){
		p->prior=p;
		p->next=p;
		net->rlist=p;
	}else{
		head=net->rlist;
		tail=net->rlist->prior;
		p->prior=tail;
		p->next=head;
		tail->next=p;
		head->prior=p;
	}
	net->nrbytes+=n;

	return n;
}

int lol_netcenter_send(LOL_NETCENTER_NET *net,const void *buf,unsigned int len)
{
	LOL_NETCENTER_BUFLIST *p,*head,*tail;
	int pos,n;
	char *buffer;

	for(pos=0;pos<len;pos+=n,net->nwbytes+=n){
		n=len-pos>LOL_NETCENTER_BUFSIZE?LOL_NETCENTER_BUFSIZE:len-pos;
		if((buffer=malloc(n))==NULL){
			lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			return -1;
		}
		memcpy(buffer,buf,n);
		if((p=malloc(sizeof(LOL_NETCENTER_BUFLIST)))==NULL){
			lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
			free(buffer);
			return -1;
		}
		memset(p,0x00,sizeof(LOL_NETCENTER_BUFLIST));
		p->buf=(char *)buffer;
		p->pos=0;
		p->len=n;
		p->prior=NULL;
		p->next=NULL;
		if(net->wlist==NULL){
			p->next=p;
			p->prior=p;
			net->wlist=p;
		}else{
			head=net->wlist;
			tail=net->wlist->prior;
			p->prior=tail;
			p->next=head;
			tail->next=p;
			head->prior=p;
		}
	}

	return 0;
}

int lol_netcenter_recv(LOL_NETCENTER_NET *net,void *buf,unsigned int siz)
{
	LOL_NETCENTER_BUFLIST *p,*prior,*next;
	int n,len;

	for(len=0;len<siz;len+=n,net->nrbytes-=n){
		p=net->rlist;
		if(!p)
			break;
		n=siz-len>p->len?p->len:siz-len;
		memcpy((char *)buf+len,p->buf+p->pos,n);
		p->len-=n;
		p->pos+=n;
		if(p->len==0){
			if(p==p->next){
				free(p->buf);
				free(p);
				net->rlist=NULL;
			}else{
				prior=p->prior;
				next=p->next;
				prior->next=next;
				next->prior=prior;
				net->rlist=next;
				free(p->buf);
				free(p);
			}
		}
	}

	return len;
}

int lol_netcenter_sendfrom(LOL_NETCENTER_NET *net)
{
	LOL_NETCENTER_BUFLIST *p,*prior,*next;
	char *buf;
	int n,pos,len;

	buf=net->wlist->buf;
	len=net->wlist->len;
	pos=net->wlist->pos;
	if((n=lol_netsend(net->net,buf+pos,len,0))<0)
		return -1;
	if(n==0)
		return 0;
	net->nwbytes-=n;
	len-=n;
	pos+=n;
	net->wlist->len=len;
	net->wlist->pos=pos;
	if(len>0)
		return 0;
	p=net->wlist;
	if(p==p->next){
		free(p->buf);
		free(p);
		net->wlist=NULL;
	}else{
		prior=p->prior;
		next=p->next;
		prior->next=next;
		next->prior=prior;
		net->wlist=next;
		free(p->buf);
		free(p);
	}

	return n;
}

int lol_netcenter_peek(LOL_NETCENTER_NET *net,void *buf,unsigned int siz)
{
	LOL_NETCENTER_BUFLIST *p;
	int n,len;

	if(net->rlist==NULL)
		return 0;
	len=0;
	p=net->rlist;
	do{
		n=siz-len>p->len?p->len:siz-len;
		memcpy((char *)buf+len,p->buf+p->pos,n);
		len+=n;
		if(len==siz)
			break;
		p=p->next;
	}while(p!=net->rlist);
	
	return len;
}

int lol_netcenter_select(LOL_NETCENTER *netcenter,int timeout)
{
	LOL_NETCENTER_NETLIST *netlist,*p;
	LOL_NETLIST *np;

	lol_netcenter_netclean(netcenter);
	netlist=netcenter->netlist;
	if(netlist==NULL)
		return 0;
	p=netlist;
	do{
		np=lol_netlist_find(netcenter->nlist,p->net->net);
		np->flags=LOL_NET_READ; /* read */
		if(p->net->wlist)
			np->flags|=LOL_NET_WRITE; /* write */
		np->flags|=LOL_NET_ERROR; /* error */
		p=p->next;
	}while(p!=netlist);

	return lol_netselect(&netcenter->nlist,timeout);
}
LOL_NETCENTER *lol_netcenter_new()
{
	LOL_NETCENTER *netcenter;

	if((netcenter=malloc(sizeof(LOL_NETCENTER)))==NULL){
		lol_error(0,"[%s]:malloc error![%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	memset(netcenter,0x00,sizeof(LOL_NETCENTER));

	return netcenter;
}
int lol_netcenter_idle(LOL_NETCENTER *netcenter,int timeout)
{
	LOL_NETCENTER_NETLIST *p;
	LOL_NETLIST *np;
	int r;

	if((r=lol_netcenter_select(netcenter,timeout))<0){
		lol_errorerror();
		return -1;
	}
	if(r==0)
		return 0;
	p=netcenter->netlist;
	do{
		if(p->net==NULL){
			p=p->next;
			continue;
		}
		np=lol_netlist_find(netcenter->nlist,p->net->net);
		if(np->flags & LOL_NET_READ){ /* read */
			if(p->net->net->flags & LOL_NET_LISTENER){
				p->net->nethandle(p->net,LOL_NETCENTER_ACCEPT);
			}else{
				if((r=lol_netcenter_recvto(p->net))<0){
					p->net->nethandle(p->net,LOL_NETCENTER_ERROR);
					lol_netcenter_netdel(netcenter,p->net->net);
				}else if(r==0){
					p->net->nethandle(p->net,LOL_NETCENTER_DISCONNECTED);
					lol_netcenter_netdel(netcenter,p->net->net);
				}else{
					p->net->nethandle(p->net,LOL_NETCENTER_DATARECVED);
				}
			}
		}

		if(p->net==NULL){
			p=p->next;
			continue;
		}
		if(np->flags & LOL_NET_WRITE){
			if((r=lol_netcenter_sendfrom(p->net))<0){
				p->net->nethandle(p->net,LOL_NETCENTER_ERROR);
				lol_netcenter_netdel(netcenter,p->net->net);
			}else if(r==0){
				p->net->nethandle(p->net,LOL_NETCENTER_DISCONNECTED);
				lol_netcenter_netdel(netcenter,p->net->net);
			}
		}

		if(p->net==NULL){
			p=p->next;
			continue;
		}
		if(np->flags & LOL_NET_ERROR){
			p->net->nethandle(p->net,LOL_NETCENTER_ERROR);
			lol_netcenter_netdel(netcenter,p->net->net);
		}
		p=p->next;
	}while(p!=netcenter->netlist);

	return 1;
}
int lol_netcenter_free(LOL_NETCENTER *netcenter)
{
	LOL_NETCENTER_NETLIST *p,**netlist;

	netlist=&netcenter->netlist;
	if(*netlist==NULL)
		return 0;
	lol_netlist_free(&netcenter->nlist);
	p=*netlist;
	do{
		p->net=NULL;
		p=p->next;
	}while(p!=*netlist);

	return 0;
}
LOL_NETCENTER_NETLIST *lol_netcenter_netadd(LOL_NETCENTER *netcenter,LOL_NET *net,LOL_NETCENTER_NETHANDLE nethandle)
{
	LOL_NETCENTER_NETLIST *head,*tail;
	LOL_NETCENTER_NETLIST *p,**netlist;

	if((p=malloc(sizeof(LOL_NETCENTER_NETLIST)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	memset(p,0x00,sizeof(LOL_NETCENTER_NETLIST));
	if((p->net=malloc(sizeof(LOL_NETCENTER_NET)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		free(p);
		return NULL;
	}
	memset(p->net,0x00,sizeof(LOL_NETCENTER_NET));
	p->net->net=net;
	p->net->nethandle=nethandle;
	p->prior=NULL;
	p->next=NULL;
	netlist=&netcenter->netlist;
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
	lol_netlist_add(&netcenter->nlist,net);

	return p;
}
int lol_netcenter_netdel(LOL_NETCENTER *netcenter,LOL_NET *net)
{
	LOL_NETCENTER_NETLIST *p;
	LOL_NETCENTER_BUFLIST *np;

	if((p=lol_netcenter_netfind(netcenter,net))==NULL){
		lol_error(0,"[%s]:net not found.",__func__);
		return -1;
	}
	lol_netlist_del(&netcenter->nlist,net);
	/* free rlist */
	while(p->net->rlist){
		np=p->net->rlist;
		if(np==np->next){
			free(np->buf);
			free(np);
			break;
		}
		np->next->prior=np->prior;
		np->prior->next=np->next;
		p->net->rlist=np->next;
		free(np->buf);
		free(np);
	}

	/* free wlist */
	while(p->net->wlist){
		np=p->net->wlist;
		if(np==np->next){
			free(np->buf);
			free(np);
			break;
		}
		np->next->prior=np->prior;
		np->prior->next=np->next;
		p->net->wlist=np->next;
		free(np->buf);
		free(np);
	}
	p->net=NULL;

	return 0;
}
LOL_NETCENTER_NETLIST *lol_netcenter_netfind(LOL_NETCENTER *netcenter,LOL_NET *net)
{
	LOL_NETCENTER_NETLIST *p;
	
	if(netcenter->netlist==NULL)
		return NULL;
	p=netcenter->netlist;
	do{
		if(p->net)
			if(p->net->net==net)
				return p;
		p=p->next;
	}while(p!=netcenter->netlist);

	return NULL;
}
int lol_netcenter_netclean(LOL_NETCENTER *netcenter)
{
	LOL_NETCENTER_NETLIST *next,*prior;
	LOL_NETCENTER_NETLIST *p,**netlist;

	netlist=&netcenter->netlist;
	if(*netlist==NULL)
		return 0;
	lol_netlist_clean(&netcenter->nlist);
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

int lol_netcenter_sendmsg(LOL_NETCENTER_NET *net,const void *buf,unsigned int len)
{
	int t;

	t=htonl(len);
	if(lol_netcenter_send(net,&t,4)<0)
		return -1;
	if(lol_netcenter_send(net,buf,len)<0)
		return -1;

	return 0;
}
int lol_netcenter_recvmsg(LOL_NETCENTER_NET *net,void *buf,unsigned int siz)
{
	int len;

	if(lol_netcenter_recv(net,&len,4)<0)
		return -1;
	len=ntohl(len);
	if(len>siz){
		lol_error(0,"[%s]:msg too long![siz=%d][len=%d]",__func__,siz,len);
		return -1;
	}
	if(lol_netcenter_recv(net,buf,len)<0)
		return -1;

	return len;
}
int lol_netcenter_ismsgok(LOL_NETCENTER_NET *net)
{
	int len;

	if(lol_netcenter_peek(net,&len,4)!=4)
		return 0;
	len=ntohl(len);
	if(net->nrbytes>=len+4)
		return 1;

	return 0;
}
