CC=g++
INCS=-I.
CFLAGS=-Wall -g -std=c++11  $(INCS)

TAR=tar
TARFLAGS=cvf
TARNAME=ex4.tar
TARSRCS= Makefile README Answers.pdf Block.cpp Block.h Cache_Manage_Alg.h FBR.cpp LFU.cpp LRU.cpp CacheFS.cpp FBR.h LFU.h LRU.h

REMOVE=rm
RMFLAGS=-f

CACHEFS=CacheFS.a
RANLIB=ranlib
    
all: $(CACHEFS)	

$(CACHEFS): CacheFS.o LRU.o LFU.o FBR.o Block.o
	$(AR) $(ARFLAGS) $(CACHEFS) CacheFS.o LRU.o LFU.o FBR.o Block.o
	$(REMOVE) *.o
	$(RANLIB) $(CACHEFS)
	
CacheFS.o: CacheFS.h CacheFS.cpp Block.h
	$(CC) $(CFLAGS) -c CacheFS.cpp -o CacheFS.o

LRU.o: Cache_Manage_Alg.h LRU.cpp LRU.h Block.h
	$(CC) $(CFLAGS) -c LRU.cpp -o LRU.o

LFU.o: Cache_Manage_Alg.h LFU.h LFU.cpp Block.h
	$(CC) $(CFLAGS) -c LFU.cpp -o LFU.o

FBR.o: Cache_Manage_Alg.h FBR.cpp FBR.h Block.h
	$(CC) $(CFLAGS) -c FBR.cpp -o FBR.o

Block.o: Block.cpp Block.h
	$(CC) $(CFLAGS) -c Block.cpp -o Block.o
tar:
	$(REMOVE) $(RMFLAGS) $(TARNAME)
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
	
clean:
	$(REMOVE) $(RMFLAGS) *.o *~ *core $(TARNAME) $(CACHEFS) 



