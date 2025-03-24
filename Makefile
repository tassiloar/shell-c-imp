CC=gcc
LD=$(CC)
CPPFLAGS=-g -std=gnu11 -Wpedantic -Wall
CFLAGS=-I.
LDFLAGS=
LDLIBS=
PROGRAM=shell

all: clean $(PROGRAM)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

$(PROGRAM): $(PROGRAM).o
	$(LD) $(CPPFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $<

.PHONY: all clean
clean:
	rm -f *.o $(PROGRAM)
