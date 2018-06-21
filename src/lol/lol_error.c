#include "lol_error.h"

int __lol_errno__=0;
char __lol_errmsg__[LOL_ERROR_ERRMSG_SIZE]={'\0'};

void lol_error(unsigned int e,const char *fmt,...)
{
	va_list args;
	char errmsg[LOL_ERROR_ERRMSG_SIZE];

	va_start(args,fmt);
	vsnprintf(errmsg,sizeof(errmsg),fmt,args);
	va_end(args);

	__lol_errno__=e;
	strcpy(__lol_errmsg__,errmsg);
}
void lol_showerror(void)
{
	printf("%d:%s\n",__lol_errno__,__lol_errmsg__);
}

#ifdef __LOL_WINDOWS__
const char *lol_strerror(int e)
{
	static char errmsg[1024]={'\0'};
	
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,e,MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US),(LPSTR)errmsg,sizeof(errmsg),NULL);
	if(strlen(errmsg)>1 && errmsg[strlen(errmsg)-2]=='\r')
		errmsg[strlen(errmsg)-2]='\0';

	return errmsg;
}
#endif	/* __LOL_WINDOWS__ */
