#include <stdio.h>
#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "poll.h"
#include "sessions.h"
#include "log.h"

int close_connection( session_t sessions[], int index) {
  int sock;


  sock = poll_get_fd(index);

  printf("Closing connection %d and removing session %d\n", sock, index);
  close(sock);
  close(sessions[index].fd);
  free(sessions[index].buf);
  poll_remove_fd(sock);
  return(0);
}


int main(int argc, char *argv[]) {
  int err, n;
  int nsocks;
  int lsock, esock;
  int lognr=0;
  int pid;

  char *logpath=".";
  
  extern int optind, optopt;
  char c;
  while ((c = getopt(argc, argv, "td:")) != -1) {
    switch (c) {
      case 'd':
        logpath = strdup(optarg);
        break;
      case 't':
        printf("OK\n");
        exit(0);
      default:
        printf("Usage: %s [-t]\n", argv[0]);
        exit(-1);
    }
    
  }

#if DAEMON
  // Daemonize here
  for (int fd = 0; fd < 256; fd++)
    close(fd); /* close all file descriptors */

  // signal(SIGHUP, NULL);
  // signal(SIGCHLD, NULL);

  pid = fork();
  if(pid > 0) exit(0); // Let parent terminate

  if(pid < 0) {
    perror("Can't fork! (first)");
    exit(-1);
  }

  setsid(); // Child becomed session leader 

  pid = fork();
  if(pid > 0) exit(0); // Let parent terminate
  if(pid < 0) {
    perror("Can't fork! (second)");
    exit(-1);
  }
#endif
 
  printf("logpath=%s\n", logpath );
  
  int content_length=0;
  char clbuf[16];
  char *sb, *cl, *cle;
  
  int  ofd;
  char ofile[40];
  
  
  struct protoent *proto;
  
  struct sockaddr_in serv_addr;
  struct sockaddr_in peer_addr;
  socklen_t peer_addr_length;
  
  pid = getpid();
  
  log_new_output_file("kalle");

  proto = getprotobyname("TCP");
  lsock = socket(AF_INET, SOCK_STREAM, proto->p_proto);
  
  
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(8090);
  
  if(bind(lsock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0 ) {
    perror("ERROR on binding");
    exit(1);
  }
  
  err = listen(lsock, SOMAXCONN);
  if(err <0) {
    perror("listen");
    exit(-1);
  }

  poll_init();
  int ret = poll_add_fd(lsock);
  if(ret == POLL_EFULL) {
    printf("poll queue is full!\n");
    exit(1);
  }
  
  while(1) {

    nsocks = poll_wait_for_event();

    printf("nsocks = %d\n", nsocks);

    if(nsocks == 0) continue;

    for(int index=0; index < poll_size(); index++) {

	    int events = poll_check_event(index);

	    if(events != 1) continue;

	    esock = poll_get_fd(index);

	    if( esock == lsock ) {

		    /* Accept new connection and register socket to /dev/poll */
		    int csock = accept(lsock, NULL, 0);
		    if(csock < 0) {
			    perror("write register csock");
			    exit(1);
		    }

		    int eindex = poll_add_fd(csock);
		    sessions_add(eindex, csock);

	    } else {

		    n = sessions_process(index);

		    if(n == 0) {

			    /* printf("Nu säger read 0. Jag tar bort sessionen!\n"); */
			    sessions_remove(index);

		    }
	    }

	    printf("Loopar runt!\n");
    }
  }
}
