#include "lol_error.h"
#include "lol_gl.h"

const void *__lol_gl_mmap(const char *msgbuf,unsigned int msglen,unsigned int no);

/* lol_gl_get */
int lol_gl_get(const char *msgbuf,unsigned int msglen,unsigned int no,char type,unsigned int maxlen,void *data,unsigned int size)
{
	const char *p;
	int len;
	int h,l;
	/* for byte align problem on hpux */
	unsigned short t_short;
	unsigned int t_int;

	if((p=__lol_gl_mmap(msgbuf,msglen,no))==NULL){
		lol_errorerror();
		return -1;
	}

	/* data length */
	memcpy(&t_int,p+4,4);
	len=ntohl(t_int);

	p+=8;	/* points to data */
	switch(type){
		case LOL_GL_CHAR:		/* char */
			if(maxlen>size){
				lol_error(0,"[%s]:illegal rule![no=%d],[type='%c'],[maxlen=%d],[size=%d]",__func__,no,type,maxlen,size);
				return -1;
			}
			if(len>maxlen){
				lol_error(0,"[%s]:data too long![no=%d],[type='%c'],[maxlen=%d],[len=%d]",__func__,no,type,maxlen,len);
				return -1;
			}
			memcpy(data,p,len);
			break;
		case LOL_GL_SHORT:		/* short */
			if(size!=2){
				lol_error(0,"[%s]:data size not match![no=%d],[type='%c'],[size=%d]",__func__,no,type,size);
				return -1;
			}
			if(len!=2){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			memcpy(&t_short,p,2);
			t_short=ntohs(t_short);
			memcpy(data,&t_short,2);
			break;
		case LOL_GL_INT:	/* int */
			if(size!=4){
				lol_error(0,"[%s]:data size not match![no=%d],[type='%c'],[size=%d]",__func__,no,type,size);
				return -1;
			}
			if(len!=4){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			memcpy(&t_int,p,4);
			t_int=ntohl(t_int);
			memcpy(data,&t_int,4);
			break;
		case LOL_GL_LONG:	/* long */
			if(size!=8){
				lol_error(0,"[%s]:data size not match![no=%d],[type='%c'],[size=%d]",__func__,no,type,size);
				return -1;
			}
			if(len!=8){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
#ifdef __LOL_LITTLE_ENDIAN__
			memcpy(&h,p,4);
			memcpy(&l,p+4,4);
			h=ntohl(h);
			l=ntohl(l);
			memcpy(data,&l,4);
			memcpy((char *)data+4,&h,4);
#else
			memcpy(data,p,8);
#endif
			break;
		case LOL_GL_FLOAT:		/* float */
			if(size!=4){
				lol_error(0,"[%s]:data size not match![no=%d],[type='%c'],[size=%d]",__func__,no,type,size);
				return -1;
			}
			if(len!=4){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			/* do as int */
			memcpy(&t_int,p,4);
			t_int=ntohl(t_int);
			memcpy(data,&t_int,4);
			break;
		case LOL_GL_DOUBLE:		/* double */
			if(size!=8){
				lol_error(0,"[%s]:data size not match![no=%d],[type='%c'],[size=%d]",__func__,no,type,size);
				return -1;
			}
			if(len!=8){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
#ifdef __LOL_LITTLE_ENDIAN__
			memcpy(&h,p,4);
			memcpy(&l,p+4,4);
			h=ntohl(h);
			l=ntohl(l);
			memcpy(data,&l,4);
			memcpy((char *)data+4,&h,4);
#else
			memcpy(data,p,8);
#endif
			break;
		default:
			lol_error(0,"[%s]:illegal rule![no=%d],[type='%c']",__func__,no,type);
			return -1;
	}

	return len;
}

/* lol_gl_put */
int lol_gl_put(char *msgbuf,unsigned int msgsiz,unsigned int no,char type,unsigned int maxlen,const void *data,unsigned int len)
{
	int h,l;
	/* for byte align problem on hpux */
	unsigned short t_short;
	unsigned int t_int;

	switch(type){
		case LOL_GL_CHAR:		/* char */
			if(len>maxlen){
				lol_error(0,"[%s]:data too long![no=%d],[type='%c'],[maxlen=%d],[len=%d]",__func__,no,type,maxlen,len);
				return -1;
			}
			if(8+len>msgsiz){
				lol_error(0,"[%s]:msgbuf too short![no=%d],[type='%c'],[len=%d],[msgsiz=%d]",__func__,no,type,len,msgsiz);
				return -1;
			}
			memcpy(msgbuf+8,data,len);
			t_int=htonl(no);
			memcpy(msgbuf,&t_int,4);
			t_int=htonl(len);
			memcpy(msgbuf+4,&t_int,4);
			break;
		case LOL_GL_SHORT:		/* short */
			if(len!=2){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			if(8+len>msgsiz){
				lol_error(0,"[%s]:msgbuf too short![no=%d],[type='%c'],[msgsiz=%d],len=[%d]",__func__,no,type,msgsiz,len);
				return -1;
			}
			memcpy(&t_short,data,2);
			t_short=htons(t_short);
			memcpy(msgbuf+8,&t_short,2);
			t_int=htonl(no);
			memcpy(msgbuf,&t_int,4);
			t_int=htonl(len);
			memcpy(msgbuf+4,&t_int,4);
			break;
		case LOL_GL_INT:		/* int */
			if(len!=4){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			if(8+len>msgsiz){
				lol_error(0,"[%s]:msgbuf too short![no=%d],[type='%c'],[msgsiz=%d],len=[%d]",__func__,no,type,msgsiz,len);
				return -1;
			}
			memcpy(&t_int,data,4);
			t_int=htonl(t_int);
			memcpy(msgbuf+8,&t_int,4);
			t_int=htonl(no);
			memcpy(msgbuf,&t_int,4);
			t_int=htonl(len);
			memcpy(msgbuf+4,&t_int,4);
			break;
		case LOL_GL_LONG:		/* long */
			if(len!=8){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			if(8+len>msgsiz){
				lol_error(0,"[%s]:msgbuf too short![no=%d],[type='%c'],[msgsiz=%d],len=[%d]",__func__,no,type,msgsiz,len);
				return -1;
			}
#ifdef __LOL_LITTLE_ENDIAN__
			memcpy(&h,(char *)data+4,4);
			memcpy(&l,data,4);
			h=ntohl(h);
			l=ntohl(l);
			memcpy(msgbuf+8,&h,4);
			memcpy(msgbuf+8+4,&l,4);
#else
			memcpy(msgbuf+8,data,8);
#endif
			t_int=htonl(no);
			memcpy(msgbuf,&t_int,4);
			t_int=htonl(len);
			memcpy(msgbuf+4,&t_int,4);
			break;
		case LOL_GL_FLOAT:		/* float */
			if(len!=4){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			if(8+len>msgsiz){
				lol_error(0,"[%s]:msgbuf too short![no=%d],[type='%c'],[msgsiz=%d],len=[%d]",__func__,no,type,msgsiz,len);
				return -1;
			}
			memcpy(&t_int,data,4);
			t_int=htonl(t_int);
			memcpy(msgbuf+8,&t_int,4);
			t_int=htonl(no);
			memcpy(msgbuf,&t_int,4);
			t_int=htonl(len);
			memcpy(msgbuf+4,&t_int,4);
			break;
		case LOL_GL_DOUBLE:		/* double */
			if(len!=8){
				lol_error(0,"[%s]:data len not match![no=%d],[type='%c'],[len=%d]",__func__,no,type,len);
				return -1;
			}
			if(8+len>msgsiz){
				lol_error(0,"[%s]:msgbuf too short![no=%d],[type='%c'],[msgsiz=%d],len=[%d]",__func__,no,type,msgsiz,len);
				return -1;
			}
#ifdef __LOL_LITTLE_ENDIAN__
			memcpy(&h,(char *)data+4,4);
			memcpy(&l,data,4);
			h=ntohl(h);
			l=ntohl(l);
			memcpy(msgbuf+8,&h,4);
			memcpy(msgbuf+8+4,&l,4);
#else
			memcpy(msgbuf+8,data,8);
#endif
			t_int=htonl(no);
			memcpy(msgbuf,&t_int,4);
			t_int=htonl(len);
			memcpy(msgbuf+4,&t_int,4);
			break;
		default:
			lol_error(0,"[%s]:illegal rule![no=%d],[type='%c']",__func__,no,type);
			return -1;
	}

	return 8+len;
}

const void *__lol_gl_mmap(const char *msgbuf,unsigned int msglen,unsigned int no)
{
	const char *p;
	unsigned int pos;
	unsigned int t_no,t_len;

	pos=0;
	while(pos!=msglen){
		if(msglen-pos<8){
			lol_error(0,"[%s]:message not complete!",__func__);
			return NULL;
		}
		p=(char *)msgbuf+pos;
		memcpy(&t_no,p,4);
		memcpy(&t_len,p+4,4);
		t_no=ntohl(t_no);
		t_len=ntohl(t_len);
		if(t_no==no){
			if((8+t_len)>msglen){
				lol_error(0,"[%s]:message not complete!",__func__);
				return NULL;
			}
			return p;
		}
		pos+=8;
		pos+=t_len;
	}
	lol_error(0,"[%s]:field not exists![no=%d]",__func__,no);

	return NULL;
}
