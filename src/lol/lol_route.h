/*
 * lol_route.h: LOL Route Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_ROUTE_H__
#define __LOL_ROUTE_H__

#include "lol_os.h"
#include "lol_net.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

struct __lol_route {
	LOL_NET *net;
	char addr[32];
	unsigned int id;
};
typedef struct __lol_route LOL_ROUTE;

LOL_ROUTE *lol_routelogin(const char *addr,unsigned int id);
int lol_routesend(LOL_ROUTE *route,unsigned int id,const void *buf,unsigned int len,int flags);
int lol_routerecv(LOL_ROUTE *route,unsigned int *id,void *buf,unsigned int siz,int flags);
int lol_routelogout(LOL_ROUTE *route);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif	/* __LOL_ROUTE_H__ */
