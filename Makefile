all:: csp testfd testlog

csp: log.c poll.c csp.c
	gcc -g -o csp csp.c poll.c sessions.c log.c

testfd: poll.c testfd.c
	gcc -g -o testfd poll.c testfd.c
	
testsessions: sessions.c testsessions.c
	gcc -g -o testsessions sessions.c testsessions.c
	
testlog: log.c testlog.c
	gcc -g -o testlog log.c testlog.c
	
clean:
	rm -f *.o csp testfd