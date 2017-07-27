/*
 * log.h
 *
 *  Created on: Jul 27, 2017
 *      Author: peter
 */

#ifndef LOG_H_
#define LOG_H_

int   log_set_log_location(char *dir);
char *log_new_output_file(char *name);
int   log_write(char *data, int len);

#endif /* LOG_H_ */
