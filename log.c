/*
 * log.c
 *
 *  Created on: Jul 27, 2017
 *      Author: peter
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/uio.h>

char *directory = "./";
char  output_file[256];
int ofd=0;

int log_set_log_location(char *dir) {
	directory = strdup(dir);
	//TODO: Add checks for writability and such!
	return(0);
}

int log_new_output_file(char *name) {

	strcpy(output_file, directory);
  strcat(output_file, "/");
	strcat(output_file, name);

  if(ofd) close(ofd); // Close file if open
	ofd = open(output_file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

	return(ofd);
}

int log_write(struct timespec st, char *data, int len) {
  int ret;
	struct iovec iov[3];

	iov[0].iov_base = &st;
	iov[0].iov_len = (int)sizeof(st);

	iov[1].iov_base = &len;
	iov[1].iov_len = sizeof(int);

	iov[2].iov_base = data;
	iov[2].iov_len = len;

	ret = writev(ofd, iov, 3);

	return(ret);
}
