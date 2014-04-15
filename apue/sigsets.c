#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void sig_quit(int);

int
main(void)
{
  sigset_t newmask, oldmask, pendmask;

  if (signal(SIGQUIT, sig_quit) == SIG_ERR) {
    fprintf(stderr, "can't catch SIGQUIT");
    exit(1);
  }

  /*
   * Block SiGQUIT and save current signal mask.
   */
  sigemptyset(&newmask);
  sigaddset(&newmask, SIGQUIT);
  if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
    fprintf(stderr, "SIG_BLOCK error");
    exit(1);
  }

  sleep(5);
  if (sigpending(&pendmask) < 0) {
    fprintf(stderr, "sigpending error");
    exit(1);
  }
  if (sigismember(&pendmask, SIGQUIT))
    printf("\nSIGQUIT pending\n");

  /*
   * Reset signal mask which unblocks SIGQUIT.
   */
  if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
    fprintf(stderr, "SIG_SETMASK error");
    exit(1);
  }

  sleep(5);
  exit(0);
}

static void
sig_quit(int signo)
{
  printf("caught SIGQUIT\n");
  if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) {
    fprintf(stderr, "can't reset SIGQUIT");
    exit(1);
  }
}
