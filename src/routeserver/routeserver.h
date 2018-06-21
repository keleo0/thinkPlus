#ifndef __ROUTESERVER_H__
#define __ROUTESERVER_H__

#include "lol.h"

#ifdef __cplusplus
extern "C" {
#endif

struct cliplist_t{
	struct cliunit_t *cliunit;
	struct viewlist_t *viewunit;
	struct cliplist_t *prior;
	struct cliplist_t *next;
};
struct viewlist_t{
	char name[128];
	char ip[16];
	unsigned int pid;
	int level;
	char desc[32];
	char pattern[32];
	int viewflag;	/* viewing or not */
	struct logunit_t *logunit;	/* for quick search */
	struct cliunit_t *cliunit;	/* send info to client */
	struct viewlist_t *prior;
	struct viewlist_t *next;
};
struct cliunit_t{
	LOL_NETCENTER_NET *net;
	char svflag;	/* send or view */
	unsigned int pid;	/* sender's pid */
	struct logunit_t *logunit;	/* can only send to one log */
	struct viewlist_t *viewlist;	/* can view many logs */
	struct cliunit_t *prior;
	struct cliunit_t *next;
};
struct logunit_t{
	char logname[128];
	LOL_DB *db;
	int flags;
	char file[128];
	struct cliplist_t *viewlist;	/* many viewers (view) */
	struct cliplist_t *clilist;	/* many writes if not excl (send) */
	struct logunit_t *prior;
	struct logunit_t *next;
};
struct routerlist_t{
	char ip[16];
	int port;
	LOL_NET *net;
	LOL_TIMER *timer;
	struct routerlist_t *prior;
	struct routerlist_t *netxt;
};

char cfgpath[256]="../etc",cfgfile[128]="logserver.cfg",cfgdomain[128]="logserver";
char log_name[256]="logserver",log_flags[128]="";
char log_addr[256]="127.0.0.1:10000",lsn_addr[32]="*:10008";
char lsn_addr_ctrl[32]="*:10009";
char next_routes[256]="";
int alive_time=120;
int route_id=1;
struct cliunit_t *clilist=NULL;
struct logunit_t *loglist=NULL;
LOL_NETCENTER *netcenter=NULL;
LOL_TIMERLIST *timerlist=NULL;
LOL_TIMER *connect_timer=NULL;
LOL_LOG *lp=NULL;
LOL_NET *lsnnet=NULL; /* normal listen net */
LOL_NET *lsnnet_ctrl=NULL; /* control listen net */

LOL_DB_FIELD fieldarray[6]={
	{"IP",'C',15,0},
	{"PID",'N',5,0},
	{"Time",'C',19,0},
	{"Level",'N',5,0},
	{"Desc",'C',31,0},
	{"Info",'C',127,0}
};
int _logrecv(LOL_NETCENTER_NET *net,char *msgbuf,int msgsiz);
int _logopen(LOL_NETCENTER_NET *net,char *msgbuf,int msglen);
int _logsend(LOL_NETCENTER_NET *net,char *msgbuf,int msglen);
int _logview(LOL_NETCENTER_NET *net,char *msgbuf,int msglen);
int _logclose(LOL_NETCENTER_NET *net,char *msgbuf,int msglen);

int cliunitadd(LOL_NET *net);
struct cliunit_t *cliunitsearch(LOL_NETCENTER_NET *net);
int cliunitdel(LOL_NETCENTER_NET *net);

int logunitadd(const char *logname,LOL_DB *db,int flags,const char *file);
struct logunit_t *logunitsearch(const char *logname);
int logunitdel(const char *logname);

int logunit_cliunitadd(struct logunit_t *logunit,struct cliunit_t *cliunit);

int cliplistadd(struct cliplist_t **cliplist,struct cliunit_t *cliunit,struct viewlist_t *viewunit);

void acceptnewclient(LOL_NETCENTER_NET *net,int event);
void clientnethandle(LOL_NETCENTER_NET *net,int event);
int clientmsgprocess(LOL_NETCENTER_NET *net,char *msgbuf,int msglen);

#ifdef __cplusplus
}
#endif

#endif
