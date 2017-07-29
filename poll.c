#include "poll.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

struct pollfd fds[NFDS];
int highest_index = -1;


int poll_init() {
  for(int i=0; i < NFDS; i++) {
    fds[i].fd = -1;
  }
  highest_index = -1;
  return(0);
}

int poll_add_fd(int fd) {
  for(int i=0; i < NFDS; i++) {
    if(fds[i].fd == -1) {
      fds[i].fd = fd;
      fds[i].events = POLLIN;
      if(i > highest_index) highest_index = i;
      return(i);
    }
  } 
  return(POLL_EFULL);
}

int poll_remove_fd(int fd) {
  int last_removed=-1;
  for(int i=0; i < NFDS; i++) {
    if(fds[i].fd == fd) {
      fds[i].fd = -1;
      fds[i].events = 0;
      fds[i].revents = 0;
      last_removed = i;
    }
  }

  // Set highest_index to highest index with fd > -1
  if(last_removed == highest_index) {
    int hi = highest_index;
    highest_index = -1; // Set in case the array becomes empty
    for(int j=hi; j >= 0; j--) {
      if(fds[j].fd != -1) {
        highest_index = j; // A new highest_index is found! We are finished.
        break;
      }
    }
  }
  return(last_removed != -1 ? POLL_OK : POLL_NFOUND);
}

int poll_size() {
  int size;
  size = highest_index + 1;
  return(size); // Size is always one more than the highest index
}

int poll_wait_for_event() {
  int n;
  n = poll(fds, poll_size(), 1000);
  return(n);
}

int poll_get_fd(int index) {
  return(fds[index].fd);
}

short int poll_check_event(int index) {
  short int e = fds[index].revents;
  return(e);
}

struct pollfd *poll_get_struct() {
  return(fds);
}

void poll_close_all_sockets() {
  for(int i=0; i < NFDS; i++) {
    if(fds[i].fd == -1) continue;
    printf("Closing %d(%d)\n", fds[i].fd, i);
    shutdown(fds[i].fd, SHUT_RDWR);
  }
}
