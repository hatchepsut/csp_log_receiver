CC = clang

CFLAGS = -Wall -Wextra -Wpedantic -g
ALL    = csp testfd testlog testsessions

all:: $(ALL)

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

clean:
	@rm -f *.o $(ALL)

cleanlogs:
	@rm -f csplog-* error.log
