/*
 * lol_netcenter.h: LOL NetCenter Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * author: luojian(enigma1983@qq.com)
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_NETCENTER_H__
#define __LOL_NETCENTER_H__

#include "lol_os.h"
#include "lol_net.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define LOL_NETCENTER_BUFSIZE	8192

#define LOL_NETCENTER_ACCEPT 			0x00000001	/* accept */
#define LOL_NETCENTER_DISCONNECTED	0x00000002	/* disconnected */
#define LOL_NETCENTER_ERROR			0x00000004	/* error */
#define LOL_NETCENTER_DATARECVED		0x00000008	/* datarecved */

struct __lol_netcenter_buflist {
	char *buf;
	unsigned int pos;
	unsigned int len;
	struct __lol_netcenter_buflist *prior;
	struct __lol_netcenter_buflist *next;
};
typedef struct __lol_netcenter_buflist LOL_NETCENTER_BUFLIST;

struct __lol_netcenter_net;
typedef struct __lol_netcenter_net LOL_NETCENTER_NET;
typedef void (*LOL_NETCENTER_NETHANDLE)(LOL_NETCENTER_NET *,int);
struct __lol_netcenter_net {
	LOL_NET *net;
	LOL_NETCENTER_BUFLIST *rlist;
	LOL_NETCENTER_BUFLIST *wlist;
	unsigned int nrbytes;
	unsigned int nwbytes;
	LOL_NETCENTER_NETHANDLE nethandle;
};

struct __lol_netcenter_netlist{
	LOL_NETCENTER_NET *net;
	struct __lol_netcenter_netlist *prior;
	struct __lol_netcenter_netlist *next;
};
typedef struct __lol_netcenter_netlist LOL_NETCENTER_NETLIST;

struct __lol_netcenter{
	LOL_NETCENTER_NETLIST *netlist;
	LOL_NETLIST *nlist;
};
typedef struct __lol_netcenter LOL_NETCENTER;

int lol_netcenter_recvto(LOL_NETCENTER_NET *net);
int lol_netcenter_send(LOL_NETCENTER_NET *net,const void *buf,unsigned int len);
int lol_netcenter_recv(LOL_NETCENTER_NET *net,void *buf,unsigned int siz);
int lol_netcenter_sendfrom(LOL_NETCENTER_NET *net);
int lol_netcenter_peek(LOL_NETCENTER_NET *net,void *buf,unsigned int siz);
int lol_netcenter_select(LOL_NETCENTER *netcenter,int timeout);

LOL_NETCENTER *lol_netcenter_new();
int lol_netcenter_idle(LOL_NETCENTER *netcenter,int timeout);
int lol_netcenter_free(LOL_NETCENTER *netcenter);

LOL_NETCENTER_NETLIST *lol_netcenter_netadd(LOL_NETCENTER *netcenter,LOL_NET *net,LOL_NETCENTER_NETHANDLE nethandle);
int lol_netcenter_netdel(LOL_NETCENTER *netcenter,LOL_NET *net);
LOL_NETCENTER_NETLIST *lol_netcenter_netfind(LOL_NETCENTER *netcenter,LOL_NET *net);
int lol_netcenter_netclean(LOL_NETCENTER *netcenter);

int lol_netcenter_ismsgok(LOL_NETCENTER_NET *net);
int lol_netcenter_recvmsg(LOL_NETCENTER_NET *net,void *buf,unsigned int siz);
int lol_netcenter_sendmsg(LOL_NETCENTER_NET *net,const void *buf,unsigned int len);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_NETCENTER_H__ */
