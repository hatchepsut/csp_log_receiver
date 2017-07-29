all:: csp testfd testlog

csp: log.c poll.c csp.c sessions.c
	cc -g -o csp csp.c poll.c sessions.c log.c

testfd: poll.c testfd.c
	cc -g -o testfd poll.c testfd.c
	
testsessions: sessions.c testsessions.c
	cc -g -o testsessions sessions.c testsessions.c
	
testlog: log.c testlog.c
	cc -g -o testlog log.c testlog.c
	
clean:
	rm -f *.o csp testfd testlog

cleanlogs:
	@rm -f csplog-* error.log
