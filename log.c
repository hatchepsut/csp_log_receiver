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

char *directory = "./";
char  output_file[256];
int ofd;

int log_set_log_location(char *dir) {
	directory = strdup(dir);
	//TODO: Add checks for writability and such!
	return(0);
}

int log_new_output_file(char *name) {

	strcpy(output_file, directory);
	strcat(output_file, name);

	ofd = open(output_file, O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);

	return(ofd);
}

int log_write(char *data, int len) {
	write(ofd, &len,  sizeof(int));
	write(ofd, data, len);
	return(0);
}
