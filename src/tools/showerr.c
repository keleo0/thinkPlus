#include "lol.h"

int main(int argc,char *argv[])
{
	char usage[]="usage: showerr errno";

	if(lol_getopts(argc,argv,NULL,0)<0){
		puts(usage);
		exit(-1);
	}
	if(argc-optind!=1){
		puts(usage);
		exit(-1);
	}
	printf("%s\n",lol_strerror(atol(argv[optind])));

	return 0;
}
