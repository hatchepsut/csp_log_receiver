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
#define NSS 100
#define BUFSIZE 4096

session_t sessions[NSS];


int sessions_init() {
	for(int i=0; i < NSS; i++)  {
		sessions[i].bytes_left_to_read = 0;
		sessions[i].bytes_read = 0;
		sessions[i].buf = malloc(BUFSIZE);
		memset(sessions[i].buf, 0, BUFSIZE);
	}
	return(0);
}

int sessions_add(int index, int sock) {

  int n;
  struct sockaddr_in peer_addr;
	socklen_t peer_addr_length;

	// Create out file */
  peer_addr_length = sizeof( struct sockaddr_in);
  n = getpeername(sock, (struct sockaddr *)&peer_addr, &peer_addr_length);
  if(n != 0) perror("getpeername");

  //sprintf(ofile, "%s/csplog-%u-%d-%d.log", logpath, peer_addr.sin_addr.s_addr, pid, lognr++);
  //printf("ofile: %s\n", ofile);
  //ofd = open(ofile, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

  sessions[index].fd = sock;
  sessions[index].bytes_read = 0;
  sessions[index].bytes_left_to_read = 4096;
  sessions[index].buf = malloc(4096);
  memset(sessions[index].buf, 0, 4096);

  return(0);
}

int sessions_remove(int index) {

	poll_remove_fd(sessions[index].fd); /* Remove fed from poll structure. */
	close(sessions[index].fd);
	memset(sessions[index].buf, 0, 4096);
	if(sessions[index].buf) free(sessions[index].buf);
	memset(&sessions[index], 0, sizeof(session_t));

	return(0);
}

int sessions_process(int index) {
	char *sb, *cl, *cle;
	char clbuf[16];
	int content_length;

	//fprintf(stderr, "Ska läsa socket %d!\n", sessions[index].fd);

	char *bufptr = sessions[index].buf+sessions[index].bytes_read;
  int n = read(sessions[index].fd, bufptr, sessions[index].bytes_left_to_read);
  if(n < 0) {
    //fprintf(stderr, "error: read socket %d\n\n", sessions[index].fd);
    return(n);
	}

  if(n == 0) {
  	// Kan inte stänga här efter som klineten kan göra en halvstängning. Måste alltid skicka response.
  	//  	sessions_remove(index);
  	//  	return(0);
  }

  sessions[index].bytes_read += n;
  sessions[index].buf[sessions[index].bytes_read] = 0; // null terminate string
  //fprintf(stderr, "Read %d bytes!\n", n);
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
      //fprintf(stderr, "My-Content-Length: %d\n", content_length);
      //write(esock, sessions[index].buf, sessions[index].bytes_read);

      log_write(sessions[index].buf, sessions[index].bytes_read);

      //write(sessions[index].ofd, sessions[index].buf, sessions[index].bytes_read);
      write(sessions[index].fd, "HTTP/1.1 200 OK\r\n\r\n", 19);
      //sessions_remove(index);
      return(0);
    }
  }
  return(sessions[index].bytes_read);
}

