all:: csp testfd

csp: poll.c csp.c
	gcc -g -o csp csp.c poll.c

testfd: poll.c testfd.c
	gcc -g -o testfd poll.c testfd.c
	
testsessions: sessions.c testsessions.c
	gcc -g -o testsessions sessions.c testsessions.c
	
clean:
	rm -f *.o csp testfd