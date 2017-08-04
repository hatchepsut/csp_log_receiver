CC = clang

CFLAGS = -Wall -Wextra -Wpedantic -g
ALL    = csp testfd testlog testsessions

all:: $(ALL)

src = *.c

.c.o:
	cc $(CFLAGS) -c -o $@ $<

csp: log.o poll.o sessions.o csp.c
	$(CC) $(CFLAGS) -o csp log.o poll.o sessions.o csp.c

testfd: poll.o testfd.c
	$(CC) $(CFLAGS) -o testfd poll.o testfd.c
	
testsessions: log.o poll.o sessions.o testsessions.c
	$(CC) $(CFLAGS) -o testsessions log.o poll.o sessions.o testsessions.c
	
testlog: log.o testlog.c
	$(CC) $(CFLAGS) -o testlog log.o testlog.c
	
clean:
	@rm -f *.o $(ALL)

cleanlogs:
	@rm -f csplog-* error.log
