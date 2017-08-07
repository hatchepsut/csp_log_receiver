CC = clang

CFLAGS = -Wall -Wextra -Wpedantic -g
ALL    = csp testfd testlog testsessions testspace
SUBDIRS = lua
all:: $(ALL) $(SUBDIRS)

src = *.c

.c.o:
	cc $(CFLAGS) -c -o $@ $<

csp: log.o poll.o sessions.o csp.o
	$(CC) $(CFLAGS) -o csp log.o poll.o sessions.o csp.o

testfd: poll.o testfd.o
	$(CC) $(CFLAGS) -o testfd poll.o testfd.o
	
testsessions: log.o poll.o sessions.o testsessions.o
	$(CC) $(CFLAGS) -o testsessions log.o poll.o sessions.o testsessions.o
	
testlog: log.o testlog.o
	$(CC) $(CFLAGS) -o testlog log.o testlog.o

testspace: testspace.o
	$(CC) $(CFLAGS) -o testspace testspace.o

lua: force_look
	@cd lua; make
	
clean:
	@rm -f *.o $(ALL)
	@cd lua; make clean

cleanlogs:
	@rm -f csplog-* error.log

force_look:
	@true
