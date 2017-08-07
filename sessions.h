/*
 * sessions.h
 *
 *  Created on: Jul 21, 2017
 *      Author: peter
 */

#ifndef SESSIONS_H_
#define SESSIONS_H_

#include <time.h>

enum session_type {
  CSP,
  ADMIN
};

typedef struct  {
  int fd; // File descriptor for out file
  int bytes_read;
  int bytes_left_to_read;
  int index;
  char *buf;
  time_t stime;
  struct timespec st;
  int type;
} session_t;

int							sessions_init();
int							sessions_add(int sock, int type);
int							sessions_process(int index);
int							sessions_remove(int index);
void						session_remove_old_sessions();
void 						sessions_set_timeout(int t);
int							sessions_set_start_time(int index);
struct timespec	sessions_get_start_time(int index);

#endif /* SESSIONS_H_ */
