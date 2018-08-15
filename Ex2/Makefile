CC=g++
INCS=-I.
CFLAGS=-Wall -g $(INCS)

TAR=tar
TARFLAGS=cvf
TARNAME=ex2.tar
TARSRCS= uthreads.cpp Makefile thread.cpp thread.h README

REMOVE=rm
RMFLAGS=-f

UTHREADSLIB=libuthreads.a
RANLIB=ranlib
    
all: $(UTHREADSLIB)	

$(UTHREADSLIB): thread.o uthreads.o
	$(AR) $(ARFLAGS) $(UTHREADSLIB) uthreads.o thread.o 
	$(REMOVE) *.o
	$(RANLIB) $(UTHREADSLIB)
		
thread.o: uthreads.h thread.h thread.cpp
	$(CC) $(CFLAGS) -c thread.cpp -o thread.o	
	
uthreads.o: uthreads.h thread.h thread.cpp
	$(CC) $(CFLAGS) -c uthreads.cpp -o uthreads.o
		
tar:
	$(REMOVE) $(RMFLAGS) $(TARNAME)
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
	
clean:
	$(REMOVE) $(RMFLAGS) *.o *~ *core $(TARNAME) $(UTHREADSLIB) 


