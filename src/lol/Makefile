LOLHome=../..
CC=cc
CFLAGS=-g -I. -I$(LOLHome)/include
LDFLAGS=-L. -L$(LOLHome)/lib
LDLIBS=-llol# -lsocket -lnsl -lresolv -lrt
AR=ar
ARFLAGS=r
INCLUDES=lol.h
LIBS=liblol.a
APPS=

all:$(LIBS) $(APPS)
liblol.a:lol_os.o lol_error.o lol_utility.o lol_file.o lol_cfg.o lol_conf.o lol_db.o lol_mdb.o lol_net.o lol_gl.o lol_log.o lol_route.o lol_netcenter.o lol_timer.o lol_sem.o lol_shm.o lol_thread.o lol_threadmutex.o lol_mutex.o lol_shmq.o
	$(AR) $(ARFLAGS) $@ $?
handtest:handtest.o
	$(CC) $(LDFLAGS) -o $@ $? $(LDLIBS)
clean:
	rm -f ? *.o *.out *core *~
	rm -f $(LIBS) $(APPS)
install:
	cp -f $(INCLUDES) $(LOLHome)/include
	cp -f $(LIBS) $(LOLHome)/lib
update:
