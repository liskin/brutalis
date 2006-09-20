# vim:set sw=8 nosta:

CMNFLAGS=-Wall -D_GNU_SOURCE -D_REENTRANT -O2 -g
CFLAGS=$(CMNFLAGS) -std=c99
CXXFLAGS=$(CMNFLAGS)
LDLIBS=-lcrypto -lssl
LDFLAGS=-g

OBJECTS=net.o base64.o http.o time.o common.o
BINS=brutalis


.PHONY: all clean dep

all: $(BINS)

brutalis: brutalis.o $(OBJECTS)


MAKEDEP=-gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)
	
-include .depend

clean:
	$(RM) $(wildcard *.o *.so *.a $(BINS))
