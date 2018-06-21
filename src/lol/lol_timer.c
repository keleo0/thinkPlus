#include "lol_error.h"
#include "lol_utility.h"
#include "lol_timer.h"

LOL_TIMER *lol_timernew(LOL_TIMERHANDLE timerhandle,unsigned int timeout)
{
	LOL_TIMER *timer;
	lol_time t;
	unsigned int timenow;

	if((timer=malloc(sizeof(LOL_TIMER)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return NULL;
	}
	memset(timer,0x00,sizeof(LOL_TIMER));
	timer->timeout=timeout;
	timer->timerhandle=timerhandle;
	lol_gettime(&t);
	timenow=(t.hour*3600+t.minute*60+t.second)*1000+t.msecond;
	timer->lasttime=timenow;

	return timer;
}

int lol_timerfree(LOL_TIMER *timer)
{
	free(timer);

	return 0;
}

int lol_timerrefresh(LOL_TIMER *timer)
{
	lol_time t;
	unsigned int timenow;

	lol_gettime(&t);
	timenow=(t.hour*3600+t.minute*60+t.second)*1000+t.msecond;
	if(timenow-timer->lasttime>timer->timeout)
		timer->timerhandle(timer);
	timer->lasttime=timenow;

	return 0;
}
int lol_timerreset(LOL_TIMER *timer)
{
	lol_time t;
	unsigned int timenow;
	
	lol_gettime(&t);
	timenow=(t.hour*3600+t.minute*60+t.second)*1000+t.msecond;
	timer->lasttime=timenow;

	return 0;
}
int lol_timerlist_add(LOL_TIMERLIST **timerlist,LOL_TIMER *timer)
{
	LOL_TIMERLIST *head,*tail;
	LOL_TIMERLIST *p;
	
	if((p=malloc(sizeof(LOL_TIMERLIST)))==NULL){
		lol_error(0,"[%s]:malloc error.[%d:%s]",__func__,lol_errno,lol_strerror(lol_errno));
		return -1;
	}
	p->timer=timer;
	p->prior=NULL;
	p->next=NULL;
	if(*timerlist==NULL){
		p->prior=p;
		p->next=p;
		*timerlist=p;
	}else{
		head=*timerlist;
		tail=(*timerlist)->prior;
		p->prior=tail;
		tail->next=p;
		p->next=head;
		head->prior=p;
	}
	
	return 0;
}

int lol_timerlist_del(LOL_TIMERLIST **timerlist,LOL_TIMER *timer)
{
	LOL_TIMERLIST *p;
	
	if((p=lol_timerlist_find(*timerlist,timer))==NULL){
		lol_error(0,"[%s]:net not found.",__func__);
		return -1;
	}
	p->timer=NULL;
	
	return 0;
}

LOL_TIMERLIST *lol_timerlist_find(LOL_TIMERLIST *timerlist,LOL_TIMER *timer)
{
	LOL_TIMERLIST *p;
	
	if(timerlist==NULL)
		return NULL;
	p=timerlist;
	do{
		if(p->timer==timer)
			return p;
		p=p->next;
	}while(p!=timerlist);
	
	return NULL;
}
int lol_timerlist_free(LOL_TIMERLIST **timerlist)
{
	LOL_TIMERLIST *p;

	if(*timerlist==NULL)
		return 0;
	p=*timerlist;
	do{
		p->timer=NULL;
		p=p->next;
	}while(p!=*timerlist);
	lol_timerlist_clean(timerlist);

	return 0;
}
int lol_timerlist_clean(LOL_TIMERLIST **timerlist)
{
	LOL_TIMERLIST *next,*prior;
	LOL_TIMERLIST *p;
	
	if(*timerlist==NULL)
		return 0;
	/* timerclean */
	p=*timerlist;
	do{
		if(p==*timerlist && p->timer==NULL){
			if(p==p->next){
				*timerlist=NULL;
				free(p);
				return 0;
			}else{
				*timerlist=p->next;
				next=p->next;
				prior=p->prior;
				prior->next=next;
				next->prior=prior;				
				free(p);
				p=*timerlist;
				continue;
			}
		}
		if(p->timer!=NULL){
			p=p->next;
			if(p==*timerlist)
				break;
			continue;
		}
		next=p->next;
		prior=p->prior;
		prior->next=next;
		next->prior=prior;
		free(p);
		p=next;
		if(p==*timerlist)
			break;
	}while(1);
	
	return 0;
}

int lol_timerlist_idle(LOL_TIMERLIST **timerlist)
{
	LOL_TIMERLIST *p;
	LOL_TIMER *timer;
	lol_time t;
	unsigned int timenow,n;
	
	lol_timerlist_clean(timerlist);
	if(*timerlist==NULL)
		return 0;

	lol_gettime(&t);
	timenow=(t.hour*3600+t.minute*60+t.second)*1000+t.msecond;

	p=*timerlist;
	n=0;
	do{
		timer=p->timer;
		if(timenow-timer->lasttime>timer->timeout){
			timer->timerhandle(timer);
			timer->lasttime=timenow;
			n++;
		}
		p=p->next;
	}while(p!=*timerlist);

	return n;
}
