CC=g++
INCS=-I.
CFLAGS=-Wall -g -std=c++11  $(INCS)

TAR=tar
TARFLAGS=cvf
TARNAME=ex3.tar
TARSRCS= MapReduceFramework.cpp Makefile README Search.cpp

REMOVE=rm
RMFLAGS=-f

MAPREDUCELIB=MapReduceFramework.a
RANLIB=ranlib
    
all: $(MAPREDUCELIB) Search	

$(MAPREDUCELIB): MapReduceFramework.o 
	$(AR) $(ARFLAGS) $(MAPREDUCELIB) MapReduceFramework.o 
	$(REMOVE) *.o
	$(RANLIB) $(MAPREDUCELIB)

Search : Search.o MapReduceClient.h MapReduceFramework.h
	$(CC) $(CFLAGS) Search.o -o Search -L. -l:MapReduceFramework.a -lpthread
		
Search.o: Search.cpp MapReduceClient.h MapReduceFramework.h
	$(CC) $(CFLAGS) -c Search.cpp MapReduceFramework.h MapReduceClient.h
	
MapReduceFramework.o: MapReduceFramework.h MapReduceClient.h MapReduceFramework.cpp
	$(CC) $(CFLAGS) -c MapReduceFramework.cpp -o MapReduceFramework.o
		
tar:
	$(REMOVE) $(RMFLAGS) $(TARNAME)
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
	
clean:
	$(REMOVE) $(RMFLAGS) *.o *~ *core $(TARNAME) $(MAPREDUCELIB) 



