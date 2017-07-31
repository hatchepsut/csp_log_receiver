/*
 * testlog.c
 *
 *  Created on: Jul 27, 2017
 *      Author: peter
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "log.h"


int main() {

	char *data1 = "Detta är en logg!";
	char *data2 = "Kort log!";
	char *data3 = "Detta är en logg som är längre än dom andra!";

	struct timespec st;

	int n = clock_gettime(CLOCK_REALTIME, &st);
  printf("clock_gettime returnerar %d\n", n );

	log_new_output_file("kalle");
	log_write(st, data1, strlen(data1));
	log_write(st, data2, strlen(data2));
	log_write(st, data3, strlen(data3));
}
