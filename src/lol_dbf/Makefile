LOLHome=../..
CC=cc
CFLAGS=-g -I. -I$(LOLHome)/include
LDFLAGS=-L. -L$(LOLHome)/lib
LDLIBS=-llol_dbf -llol# -lsocket -lnsl -lresolv -lrt
AR=ar
ARFLAGS=r
INCLUDES=lol_dbf.h
LIBS=liblol_dbf.a
APPS=

all:$(LIBS) $(APPS)
liblol_dbf.a:lol_dbf.o
	$(AR) $(ARFLAGS) $@ $?
clean:
	rm -f ? *.o *core
	rm -f $(LIBS) $(APPS)
install:
	cp -f $(INCLUDES) $(LOLHome)/include
	cp -f $(LIBS) $(LOLHome)/lib
update:
