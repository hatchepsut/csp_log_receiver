/*
 * sessions.h
 *
 *  Created on: Jul 21, 2017
 *      Author: peter
 */

#ifndef SESSIONS_H_
#define SESSIONS_H_

typedef struct  {
  int fd; // File descriptor for out file
  int bytes_read;
  int bytes_left_to_read;
  int index;
  char *buf;
} session_t;

int sessions_init();
int sessions_add(int index, int sock);
int sessions_process(int index);
int sessions_remove(int index);

#endif /* SESSIONS_H_ */
