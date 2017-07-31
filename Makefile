CC = clang

CFLAGS = -Wall -g
ALL    = csp testfd testlog

all:: $(ALL)

csp: log.c poll.c csp.c sessions.c
	$(CC) $(CFLAGS) -o csp csp.c poll.c sessions.c log.c

testfd: poll.c testfd.c
	$(CC) $(CFLAGS) -o testfd poll.c testfd.c
	
testsessions: sessions.c testsessions.c
	$(CC) $(CFLAGS) -o testsessions sessions.c testsessions.c
	
testlog: log.c testlog.c
	$(CC) $(CFLAGS) -o testlog log.c testlog.c
	
clean:
	@rm -f *.o csp testfd testlog

cleanlogs:
	@rm -f csplog-* error.log
