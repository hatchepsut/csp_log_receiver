/*
 * session.c
 *
 *  Created on: Jul 21, 2017
 *      Author: peter
 */

#include "sessions.h"
#include "poll.h"
#include "log.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

#define NSS 100
#define BUFSIZE 4096

session_t sessions[NSS];
time_t lastrun;
int timeout = 3;

int sessions_init() {
  for(int i=0; i < NSS; i++)  {
    sessions[i].bytes_left_to_read = 0;
    sessions[i].bytes_read = 0;
    sessions[i].buf = (char *)0;
    sessions[i].stime = 0;
    sessions[i].fd = -1; // Indication of not used session structure
    lastrun = time(0);
  }
  return(0);
}

void sessions_set_timeout(int t) {
  timeout = t;
}

int sessions_add(int sock) {

	struct timespec st;
  //int n;
  //struct sockaddr_in peer_addr;
  //socklen_t peer_addr_length;

  //peer_addr_length = sizeof( struct sockaddr_in);
  //n = getpeername(sock, (struct sockaddr *)&peer_addr, &peer_addr_length);
  //if(n != 0) perror("getpeername");

  int index = poll_add_fd(sock);
  if(index < 0) {
    fprintf(stderr, "poll queue is full!\n");
    exit(1);
  }


  sessions[index].fd = sock;
  sessions[index].bytes_read = 0;
  sessions[index].bytes_left_to_read = BUFSIZE;
  sessions[index].buf = malloc(BUFSIZE);
  sessions[index].stime = time(0);
  //memset(sessions[index].buf, 0, BUFSIZE);

	clock_gettime(CLOCK_REALTIME, &st);
	sessions[index].st = st;

  return(0);
}

int sessions_remove(int index) {

  poll_remove_fd(sessions[index].fd); /* Remove fed from poll structure. */
  close(sessions[index].fd);
  //memset(sessions[index].buf, 0, BUFSIZE);
  if(sessions[index].buf) free(sessions[index].buf);
  //memset(&sessions[index], 0, sizeof(session_t));
  sessions[index].fd = -1; // Indication of not used session structure

  return(0);
}

void session_remove_old_sessions() {
  int now = time(0);

  if(now - lastrun >= 1) {
    for(int i=0; i < NSS; i++) {
      if(sessions[i].fd == -1) continue; // Skip not used sessions
      if(now - sessions[i].stime >= timeout ) {
        sessions_remove(i);
      }
    }
    lastrun = now;
  }
}

int sessions_process(int index) {
  char *sb, *cl, *cle;
  char clbuf[16];
  int content_length;

  char *bufptr = sessions[index].buf+sessions[index].bytes_read;
  int n = read(sessions[index].fd, bufptr, sessions[index].bytes_left_to_read);
  if(n < 0) {
    fprintf(stderr, "error: read socket %d\n\n", sessions[index].fd);
    return(n);
  }

  if(n == 0) {
    return(0);
  }

  sessions[index].bytes_read += n;
  sessions[index].buf[sessions[index].bytes_read] = 0; // null terminate string
  if((sb = strstr(sessions[index].buf, "\r\n\r\n")) != (char *)0 ) {
    content_length = 0;
    sb += 4; // Skip past \r\n\r\n
    cl = strstr(sessions[index].buf, "Content-Length:");
    if(cl != 0 ) {
      cl += 15; // Jump past header name
      while(*cl == ' ') cl++; // Skip optional space
      cle = strstr(cl, "\r\n");
      strncpy(clbuf, cl, cle-cl);
      clbuf[cle-cl+1] = (char)0;
      content_length=atoi(clbuf);
      sessions[index].bytes_left_to_read = content_length - (sessions[index].bytes_read - (sb - sessions[index].buf));
    } else {
      sessions[index].bytes_left_to_read = 0;
    }

    if(sessions[index].bytes_left_to_read-n < 1) {

      // Don't log request with no data. Healthcheck and such.
      if(content_length > 0) {
      	struct timespec st;
      	st = sessions_get_start_time(index);

        log_write(st, sessions[index].buf, sessions[index].bytes_read);
      }

      write(sessions[index].fd, "HTTP/1.1 200 OK\r\n\r\n", 19);
      return(0);
    }
  }
  return(sessions[index].bytes_read);
}

struct timespec sessions_get_start_time(int index) {
	return sessions[index].st;
}


