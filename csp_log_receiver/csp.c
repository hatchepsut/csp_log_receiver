#include <stdio.h>
#include <sys/event.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "poll/poll.h"


typedef struct {
  int ofd; // File descriptor for out file
  int bytes_read;
  int bytes_left_to_read;
  char *buf;
} session_t;


int close_connection( session_t sessions[], int esock) {
  printf("Closing connection %d\n", esock);
  close(esock); // Socket is automatically deregistered from /dev/poll on close */
  close(sessions[esock].ofd);
  free(sessions[esock].buf);
  return(0);
}


int main(int argc, char *argv[]) {
  int err, n;
  int nsocks;
  //int evpfd;
  int lsock, csock, esock;
  int lognr=0;
  int pid;
  
  char *logpath=".";
  
  //ssize_t retval;
  
  // kqueue
  //int kq;
  //struct kevent evSet;
  
  char c;
  extern int optind, optopt;
  
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
  
  session_t sessions[65536];
  
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
  
  //fdset = (struct pollfd *)malloc(sizeof(struct pollfd) * 100);
  //if(fdset == NULL) {
  //    perror("mallox");
  //    exit(1);;
  //}
  
  
  //evpfd = open("/dev/poll", O_RDWR);
  
  
  
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
    
    for(int i=0; i < poll_size(); i++) {
      
      
      int events = poll_check_event(i);
      
      if(events != 1) continue;
      
      esock = poll_get_fd(i);
      
      
      
      if( esock == lsock ) {
        
        /* Accept new connection and register socket to /dev/poll */
        csock = accept(lsock, NULL, 0);
        if(csock < 0) {
          perror("write register csock");
          exit(1);
        }
        
        poll_add_fd(csock);
        
        // Create out filed */
        peer_addr_length = sizeof( struct sockaddr_in);
        n = getpeername(csock, (struct sockaddr *)&peer_addr, &peer_addr_length);
        if(n != 0) perror("getpeername");
        
        sprintf(ofile, "%s/csplog-%u-%d-%d.log", logpath, peer_addr.sin_addr.s_addr, pid, lognr++);
        printf("ofile: %s\n", ofile);
        ofd = open(ofile, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
        
        sessions[csock].ofd = ofd;
        sessions[csock].bytes_read = 0;
        sessions[csock].bytes_left_to_read = 4096;
        sessions[csock].buf = malloc(4096);
        memset(sessions[csock].buf, 0, 4096);
        
      } else {
        
        printf("Ska läsa socket %d!\n", esock);
        n = read(esock, sessions[esock].buf+sessions[esock].bytes_read, sessions[esock].bytes_left_to_read);
        if(n < 0) {
          perror("read esock");
        }
        sessions[esock].bytes_read += n;
        sessions[esock].buf[sessions[esock].bytes_read] = NULL;
        printf("Read %d bytes!\n", n);
        if(sb = strstr(sessions[esock].buf, "\r\n\r\n")) {
          if(strncmp("GET ", sessions[esock].buf, 3) == 0 ) sessions[esock].bytes_left_to_read = 0;
          sb += 4; // Skip past \r\n\r\n
          cl = strstr(sessions[esock].buf, "Content-Length:");
          if(cl > NULL) {
            cl += 15; // Jump past header name
            while(*cl == ' ') cl++; // Skip optional space
            cle = strstr(cl, "\r\n");
            strncpy(clbuf, cl, cle-cl);
            clbuf[cle-cl+1] = NULL;
            content_length=atoi(clbuf);
            sessions[esock].bytes_left_to_read = content_length - (sessions[esock].bytes_read - (sb - sessions[esock].buf));
          }
          
          if(sessions[csock].bytes_left_to_read-n < 1) {
            printf("My-Content-Length: %d\n", content_length);
            write(1, sessions[esock].buf, sessions[esock].bytes_read);
            write(sessions[esock].ofd, sessions[esock].buf, sessions[esock].bytes_read);
            write(esock, "HTTP/1.1 200 OK\r\n\r\n", 19);
            close_connection(sessions, esock);
          }
        }
      }
      printf("Loopar runt!\n");
    }
    
    
  }
  
  
  // int write(int filedes, const struct pollfd *buf, size_t nbyte);
  
}
