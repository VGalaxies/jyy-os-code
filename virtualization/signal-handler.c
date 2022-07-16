#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handler(int signum) {
  switch (signum) {
    case SIGINT:
      printf("Received SIGINT!\n");
      break;
    case SIGQUIT:
      printf("Received SIGQUIT!\n");
      exit(0);
      break;
  }
}

void cleanup() {
  printf("atexit() cleanup\n");
}

int main() {
  signal(SIGINT,  handler);
  signal(SIGQUIT, handler);
  atexit(cleanup);

  fork();

  while (1) {
    char buf[4096];
    int nread = read(STDIN_FILENO, buf, sizeof(buf));
    buf[nread - 1] = '\0';
    printf("[%d] Got: %s\n", getpid(), buf);
    if (nread < 0) {
      perror("read");
      exit(1);
    }
    sleep(1);
  }
}
