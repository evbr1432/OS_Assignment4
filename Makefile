CC = gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -g -Wall -Wextra

INPUTFILESIZEMEGABYTES = 1

KILO = 1024
MEGA = $(shell echo $(KILO)\*$(KILO) | bc)
INPUTFILESIZEBYTES = $(shell echo $(MEGA)\*$(INPUTFILESIZEMEGABYTES) | bc)
INPUTBLOCKSIZEBYTES = $(KILO)
INPUTBLOCKS = $(shell echo $(INPUTFILESIZEBYTES)\/$(INPUTBLOCKSIZEBYTES) | bc)

.PHONY: all clean

all: pi pi_fork pi-sched mix_fork rw rw_fork

pi.o: pi.c
	$(CC) $(CFLAGS) $<

mix_fork.o: mix_fork.c
	$(CC) $(CFLAGS) $<

pi_fork.o: pi_fork.c
	$(CC) $(CFLAGS) $<

rw_fork.o: rw_fork.c
	$(CC) $(CFLAGS) $<

pi-sched.o: pi-sched.c
	$(CC) $(CFLAGS) $<

rw.o: rw.c
	$(CC) $(CFLAGS) $<

mix_fork: mix_fork.o
	$(CC) $(LFLAGS) $^ -o $@ -lm

pi_fork: pi_fork.o
	$(CC) $(LFLAGS) $^ -o $@ -lm

rw_fork: rw_fork.o rwinput
	$(CC) $(LFLAGS) rw.o -o $@ -lm

pi: pi.o
	$(CC) $(LFLAGS) $^ -o $@ -lm

pi-sched: pi-sched.o
	$(CC) $(LFLAGS) $^ -o $@ -lm

rw: rw.o rwinput
	$(CC) $(LFLAGS) rw.o -o $@ -lm

rwinput: Makefile
	dd if=/dev/urandom of=./rwinput bs=$(INPUTBLOCKSIZEBYTES) count=$(INPUTBLOCKS)

clean: testclean
	rm -f pi pi-sched rw pi_fork mix_fork
	rm -f rwinput
	rm -f *.o
	rm -f *~
	rm -f handout/*~
	rm -f handout/*.log
	rm -f handout/*.aux

testclean:
	rm -f rwoutput*
