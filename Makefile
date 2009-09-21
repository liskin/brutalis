# vim:set sw=8 nosta:

CMNFLAGS=-Wall -D_GNU_SOURCE -D_REENTRANT -O2 -g
CFLAGS=$(CMNFLAGS) -std=c99
CXXFLAGS=$(CMNFLAGS)
LDLIBS=-lcrypto -lssl
LDFLAGS=-g

SOURCES=$(wildcard *.c)
OBJECTS=net.o base64.o http.o time.o common.o
BINS=udelejpresne souteze


.PHONY: all clean

all: $(BINS)
clean:
	$(RM) $(wildcard *.o *.so *.a $(BINS))

udelejpresne: udelejpresne.o $(OBJECTS)
souteze: souteze.o $(OBJECTS)

-include $(SOURCES:.c=.d)
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
