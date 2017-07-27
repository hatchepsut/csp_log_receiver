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

int close_connection( session_t sessions[], int index) {
  int sock;


  sock = poll_get_fd(index);

  printf("Closing connection %d and removing session %d\n", sock, index);
  close(sock);
  close(sessions[index].ofd);
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
  
  printf("logpath=%s\n", logpath );
  
  session_t sessions[NFDS];
  
  //char buf[2048];
  //int  bufsize=2048;
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
        
				printf("Ska läsa socket %d!\n", esock);
        n = read(esock, sessions[index].buf+sessions[index].bytes_read, sessions[index].bytes_left_to_read);
        if(n < 0) {
          perror("read esock")	;
        }

        if(n == 0) {
        	// Kan inte stänga här efter som klineten kan göra en halvstängning. Måste alltid skicka response.
        	close_connection(sessions, index);
        	continue;
        }

        sessions[index].bytes_read += n;
        sessions[index].buf[sessions[index].bytes_read] = 0; // null terminate string
        printf("Read %d bytes!\n", n);
        if((sb = strstr(sessions[index].buf, "\r\n\r\n")) != NULL) {

					// GET requests are allowed to have a body. So we must read it if it has.
        	//if(strncmp("GET ", sessions[index].buf, 3) == 0 ) sessions[index].bytes_left_to_read = 0;
          sb += 4; // Skip past \r\n\r\n
          cl = strstr(sessions[index].buf, "Content-Length:");
          if(cl != NULL ) {
        	cl += 15; // Jump past header name
            while(*cl == ' ') cl++; // Skip optional space
            cle = strstr(cl, "\r\n");
            strncpy(clbuf, cl, cle-cl);
            clbuf[cle-cl+1] = (char)0;
            content_length=atoi(clbuf);
            sessions[index].bytes_left_to_read = content_length - (sessions[index].bytes_read - (sb - sessions[index].buf));
          }
          
          if(sessions[index].bytes_left_to_read-n < 1) {
            printf("My-Content-Length: %d\n", content_length);
            //write(esock, sessions[index].buf, sessions[index].bytes_read);
            write(sessions[index].ofd, sessions[index].buf, sessions[index].bytes_read);
            write(esock, "HTTP/1.1 200 OK\r\n\r\n", 19);
            close_connection(sessions, index);
          }
        }
      }
      printf("Loopar runt!\n");
    }
    
    
  }
  
  
  // int write(int filedes, const struct pollfd *buf, size_t nbyte);
  
}
