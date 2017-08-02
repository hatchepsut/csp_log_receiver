/*
 * testlog.c
 *
 *  Created on: Jul 27, 2017
 *      Author: peter
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "log.h"


int main() {

  struct timespec st;
  char data[4096];
  int len=0;

  int fd = open("csplog-6324-1.log", O_RDONLY, 0);

  printf("Opened csplog-6324-1.log as fd %d\n", fd);

  do {

  	len = log_read(fd, &st, data);

  	printf("Len=%d\n++++++++++++++++++++++++++++++++\n", len);
    fwrite(data, len, 1, stdout);
    printf("\n----------------------------------\n");
  } while(len > 0);

  return(0);
}
