/*
 * log.h
 *
 *  Created on: Jul 27, 2017
 *      Author: peter
 */

#ifndef LOG_H_
#define LOG_H_

int   log_set_log_location(char *dir);
int   log_new_output_file(char *name);
int   log_write(struct timespec st, char *data, int len);
int 	set_log_time();
struct timespec get_log_time();

#endif /* LOG_H_ */
