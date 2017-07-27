/*
 * testlog.c
 *
 *  Created on: Jul 27, 2017
 *      Author: peter
 */

#include <stdlib.h>
#include <string.h>
#include "log.h"


int main() {

	char *data1 = "Detta är en logg!";
	char *data2 = "Kort log!";
	char *data3 = "Detta är en logg som är längre än dom andra!";


	log_new_output_file("kalle");
	log_write(data1, strlen(data1));
	log_write(data2, strlen(data2));
	log_write(data3, strlen(data3));
}
