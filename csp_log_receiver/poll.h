#include <poll.h>

#define POLL_OK       0
#define POLL_EFULL    1
#define POLL_NFOUND   2

int poll_init();
int poll_add_fd(int fd);
int poll_remove_fd(int fd);
int poll_size();
int poll_wait_for_event();
int poll_get_fd(int index);
int poll_check_event(int index);
