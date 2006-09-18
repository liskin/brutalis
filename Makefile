# vim:set sw=8 nosta:

CMNFLAGS=-Wall -D_GNU_SOURCE -D_REENTRANT -O2 -g
CFLAGS=$(CMNFLAGS) -std=c99
CXXFLAGS=$(CMNFLAGS)
LDLIBS=-lcrypto -lssl
LDFLAGS=-g

.PHONY: all clean dep

all: brutalis

MAKEDEP=-gcc -MM $(wildcard *.c *.cc) > .depend
dep:
	$(MAKEDEP)
.depend:
	$(MAKEDEP)
	
-include .depend

clean:
	$(RM) $(wildcard *.o *.so *.a brutalis)

brutalis: brutalis.o net.o base64.o http.o time.o
