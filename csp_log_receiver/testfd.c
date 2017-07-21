#include <stdio.h>
#include "poll.h"

extern int highest_index;
extern struct pollfd fds[];

void print(int n) {

  printf("highest_index: %2d: ", highest_index);
  for(int i=0; i < n; i++) {
    printf(" %2d", fds[i].fd);
  }
  printf("\n");
}

int main() {
  int n=7;
  
  poll_init();

  poll_add_fd(12);
  print(n);

  poll_add_fd(5);
  print(n);

  poll_add_fd(7);
  print(n);

  poll_add_fd(90);
  print(n);

  poll_add_fd(21);
  print(n);

  poll_add_fd(12);
  print(n);

  poll_remove_fd(12);
  print(n);

  poll_remove_fd(90);
  print(n);

  poll_remove_fd(21);
  print(  n);

  poll_remove_fd(7);
  print(n);

  poll_remove_fd(5);
  print(n);

  poll_add_fd(99);
  print(n);

  poll_remove_fd(99);
  print(n);
    
  poll_remove_fd(2);
  print(n);

  return(0);
}
