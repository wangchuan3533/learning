#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

static void sig_user(int); /* one handler for both signals */

int
main(void)
{
  if (signal(SIGUSR1, sig_user) == SIG_ERR)
    exit(1);
  if (signal(SIGUSR2, sig_user) == SIG_ERR)
    exit(1);

  for (;;)
    pause();
}

static void
sig_user(int signo)
{
  if (signo == SIGUSR1)
    printf("received SIGUSR1\n");
  else if (signo == SIGUSR2)
    printf("received SIGUSR2\n");
  else
    printf("received signal %d\n", signo);
}
