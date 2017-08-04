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


int main(int argc, char *argv[]) {

  struct timespec st;
  char data[4096];
  int len=0;

  int fd = open(argv[1], O_RDONLY, 0);

  printf("Opened %s as fd %d\n", argv[argc], fd);

  do {

  	len = log_read(fd, &st, data);

    if(len == 0 ) break;

    if(len < 0 ) {
      printf("error? WTF!!!\n");
      return(-1);
    }

  	printf("Len=%d\n++++++++++++++++++++++++++++++++\n", len);
    fwrite(data, len, 1, stdout);
    printf("\n----------------------------------\n");
  } while(len > 0);

  return(0);
}
