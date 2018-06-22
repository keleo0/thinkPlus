/*
 * lol_timer.h: LOL Timer Interface
 * version: 1.0
 * OS: AIX,HP-UX,Solaris,FreeBSD,Linux,Mac OS X,Windows
 * history:
 * 2008-11-07	1.0 released
 *
 */

#ifndef __LOL_TIMER_H__
#define __LOL_TIMER_H__

#include "lol_os.h"

/* C++ */
#ifdef __cplusplus
extern "C" {
#endif

#define LOL_TIMER_TIMEOUT 0x00000001

struct __lol_timer;
typedef struct __lol_timer LOL_TIMER;
typedef void (*LOL_TIMERHANDLE)(LOL_TIMER *);
struct __lol_timer {
	unsigned int timeout;
	unsigned int lasttime;
	LOL_TIMERHANDLE timerhandle;
};

struct __lol_timerlist {
	LOL_TIMER *timer;
	struct __lol_timerlist *prior;
	struct __lol_timerlist *next;
};
typedef struct __lol_timerlist LOL_TIMERLIST;

LOL_TIMER *lol_timernew(LOL_TIMERHANDLE timerhandle,unsigned int timeout);
int lol_timerfree(LOL_TIMER *timer);
int lol_timerrefresh(LOL_TIMER *timer);
int lol_timerreset(LOL_TIMER *timer);

int lol_timerlist_add(LOL_TIMERLIST **timerlist,LOL_TIMER *timer);
int lol_timerlist_del(LOL_TIMERLIST **timerlist,LOL_TIMER *timer);
LOL_TIMERLIST *lol_timerlist_find(LOL_TIMERLIST *timerlist,LOL_TIMER *timer);
int lol_timerlist_clean(LOL_TIMERLIST **timerlist);
int lol_timerlist_free(LOL_TIMERLIST **timerlist);
int lol_timerlist_idle(LOL_TIMERLIST **timerlist);

/* C++ */
#ifdef __cplusplus
}
#endif

#endif /* __LOL_TIMER_H__ */
