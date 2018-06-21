LOLHome=../..
CC=cc
CFLAGS=-g -I. -I$(LOLHome)/include
LDFLAGS=-L. -L$(LOLHome)/lib
LDLIBS=-llol# -lsocket -lnsl -lresolv -lrt
AR=
ARFLAGS=
INCLUDES=
LIBS=
APPS=
TOOLS=threadtest
SHELLS=

all:$(LIBS) $(APPS) $(TOOLS)
threadtest:threadtest.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS) -lpthread
clean:
	rm -f ? *.o *core
	rm -f $(LIBS) $(APPS) $(TOOLS)
install:
	cp -f $(SHELLS) $(TOOLS) $(LOLHome)/bin
update:
