CC=g++
RANLIB=ranlib

LIBSRC=osm.c 
LIBOBJ=$(LIBSRC:.c=.o)

INCS=-I.
CFLAGS = -Wall -g $(INCS)
LOADLIBES = -L./ 

OSMLIB = libosm.a
TARGETS = $(OSMLIB)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex1.tar
TARSRCS=$(LIBSRC) Makefile README

all: $(TARGETS) 

$(TARGETS): $(LIBOBJ)
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@
	$(RM) $(LIBOBJ)

clean:
	$(RM) $(TARGETS) $(OSMLIB) $(OBJ) $(LIBOBJ) $(TARNAME) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
