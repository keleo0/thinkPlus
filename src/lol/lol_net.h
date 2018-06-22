/*
 * lol_net.h: LOL Net Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_NET_H__
#define __LOL_NET_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __LOL_WINDOWS__
#define socketclose closesocket
#define socklen_t int
#else
#define socketclose close
#endif

#ifndef INADDR_NONE
#define INADDR_NONE -1
#endif

#define LOL_NET_LISTENER	0x00000001		/* listener */

#define LOL_NET_WAIT		0x00000001		/* wait */
#define LOL_NET_READ		0x00000002		/* read */
#define LOL_NET_WRITE		0x00000004		/* write */
#define LOL_NET_ERROR		0x00000008		/* error */

struct __lol_net {
	int sockfd;
	char ip[16];
	unsigned short port;
	int flags;
};
typedef struct __lol_net LOL_NET;

struct __lol_netlist {
	LOL_NET *net;
	int flags;
	struct __lol_netlist *prior;
	struct __lol_netlist *next;
};
typedef struct __lol_netlist LOL_NETLIST;

int lol_netstart(void);
int lol_netstop(void);

LOL_NET *lol_netconnect(const char *ip,unsigned short port);
LOL_NET *lol_netlisten(const char *ip,unsigned short port);
LOL_NET *lol_netaccept(LOL_NET *net);
int lol_netrecv(LOL_NET *net,void *buf,unsigned int siz,int flags);
int lol_netsend(LOL_NET *net,const void *buf,unsigned int len,int flags);
int lol_netclose(LOL_NET *net);

/* select & poll */
int lol_netlist_add(LOL_NETLIST **netlist,LOL_NET *net);
int lol_netlist_del(LOL_NETLIST **netlist,LOL_NET *net);
LOL_NETLIST *lol_netlist_find(LOL_NETLIST *netlist,LOL_NET *net);
int lol_netlist_clean(LOL_NETLIST **netlist);
int lol_netselect(LOL_NETLIST **netlist,int timeout);
#ifdef __LOL_UNIX__
int lol_netpoll(LOL_NETLIST **netlist,int timeout);
#endif
int lol_netlist_free(LOL_NETLIST **netlist);

int lol_netsendmsg(LOL_NET *net,const void *buf,unsigned int len);
int lol_netrecvmsg(LOL_NET *net,void *buf,unsigned int siz);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_NET_H__ */
