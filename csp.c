#include <stdio.h>
#include <stdio.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include "poll.h"
#include "sessions.h"
#include "log.h"

static void exit_program(int signum) {
  char buf[256];
  fprintf(stderr, "exit_program signal %d!\n", signum);
  poll_close_all_sockets();
  exit(-1);
}

static void problem(int signum) {
  char buf[256];
  fprintf(stderr, "Problem signal %d!\n", signum);
  exit(-1);
}

int main(int argc, char *argv[]) {
  int err, n;
  int nsocks;
  int lsock, esock;
  int lport = 8090;
  int lognr = 0;
  int pid;
  time_t stime = time(0);
  char *timestr;
  int dofork = 1;

  char *logpath = ".";
  char *logfilename = "error.log";
  extern int optind, optopt;
  char c;
  while ((c = getopt(argc, argv, "d:l:np:t:")) != -1) {
    switch (c) {
    case 'd':
      logpath = strdup(optarg);
      break;
    case 'l':
      logfilename = strdup(optarg);
      break;
    case 'n':
      dofork = 0;
      break;
    case 'p':
      lport = atoi(optarg);
      break;
    case 't':
      sessions_set_timeout(atoi(optarg));
      break;
    default:
      printf("Usage: %s [-t]\n", argv[0]);
      exit(-1);
    }

  }

  /* Set up signalhandlning to support logfile rotation */
  struct sigaction sa1, sa2, sa3, sa_ign, *sa;

  //sa1.sa_handler = logrotate;
  //sigemptyset(&sa1.sa_mask);
  //sa1.sa_flags = SA_RESTART;

  sa2.sa_handler = problem;
  sigemptyset(&sa2.sa_mask);
  sa2.sa_flags = SA_RESTART;

  sa3.sa_handler = exit_program;
  sigemptyset(&sa3.sa_mask);
  sa3.sa_flags = SA_RESTART;

  sa_ign.sa_handler = SIG_IGN;
  sigemptyset(&sa_ign.sa_mask);
  sa_ign.sa_flags = SA_RESTART;

  for (int signum = 1; signum <= 30; signum++) {
    switch (signum) {
    //case SIGALRM:
    //  sa = &sa1;
    //  break;

    case SIGSEGV:
      continue;
      break;

    case SIGKILL:
      continue;
      break;

    case SIGHUP:
      sa = &sa_ign;
      break;

    case SIGSTOP:
      sa = NULL;
      break;

    case SIGPIPE:
      sa = &sa_ign;
      break;

    case SIGTERM:
      sa = &sa3;
      break;

    case SIGINT:
      sa = &sa3;
      break;

    default:
      sa = &sa2;
      break;
    }

    if (sigaction(signum, sa, NULL) == -1) {
      fprintf(stderr, "Can't install signalhandler for %d\n", signum);
    }
  }

  // Daemonize here
  if (dofork) {
    close(0);
    close(1);

    pid = fork();
    if (pid > 0)
      exit(0); // Let parent terminate

    if (pid < 0) {
      perror("Can't fork! (first)");
      exit(-1);
    }

    setsid(); // Child becomed session leader

    pid = fork();
    if (pid > 0)
      exit(0); // Let parent terminate
    if (pid < 0) {
      fprintf(stderr, "Can't fork! (second)");
      exit(-1);
    }
  }

  int newfd = open(logfilename, O_CREAT | O_WRONLY | O_APPEND, 0755);
  dup2(newfd, 2);

  fprintf(stderr, "logpath=%s\n", logpath);

  int content_length = 0;
  char clbuf[16];
  char *sb, *cl, *cle;

  int ofd;
  char *ofile;

  struct protoent *proto;

  struct sockaddr_in serv_addr;
  struct sockaddr_in peer_addr;
  socklen_t peer_addr_length;

  pid = getpid();

  ofile = malloc(1024);
  sprintf(ofile, "%s/csplog-%d-%d.log", logpath, pid, lognr++);
  log_new_output_file(ofile);
  free(ofile);

  proto = getprotobyname("TCP");
  lsock = socket(AF_INET, SOCK_STREAM, proto->p_proto);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(lport);

  if (bind(lsock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    perror("ERROR on binding");
    exit(1);
  }

  err = listen(lsock, SOMAXCONN);
  if (err < 0) {
    perror("listen");
    exit(-1);
  }

  poll_init();
  int ret = poll_add_fd(lsock);
  if (ret == POLL_EFULL) {
    fprintf(stderr, "poll queue is full!\n");
    exit(1);
  }

  timestr = ctime(&stime);
  timestr[strlen(timestr) - 1] = '\0';
  fprintf(stderr, "%s starting.\n", timestr);

  sessions_init();

  while (1) {

    struct pollfd *pfd;
    pfd = poll_get_struct();
    for(int i=0; i < 100; i++) {
      if(pfd[i].fd == -1) continue;
    }
 
    nsocks = poll_wait_for_event();

    session_remove_old_sessions();

    /*fprintf(stderr, "nsocks = %d\n", nsocks);*/

    if (nsocks == 0) {
      session_remove_old_sessions();
      continue;
    }

    for (int index = 0; index < poll_size(); index++) {

      int events = poll_check_event(index);






      struct pollfd *pfd;
      pfd = poll_get_struct();

      esock = poll_get_fd(index);

      if (events != 1) {
        continue;
      }


      if (esock == lsock) {

        /* Accept new connection and register socket */
        int csock = accept(lsock, NULL, 0);
        if (csock < 0) {
          perror("write register csock");
          exit(1);
        }

        int eindex = poll_add_fd(csock);
        sessions_add(eindex, csock);

      } else {

        // process request as data comes in
        n = sessions_process(index);
        if (n == 0) {
          sessions_remove(index);
        }

      }

    }

  }
}
