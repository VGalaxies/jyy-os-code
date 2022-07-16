#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/syscall.h>

void func() {
  printf("Goodbye, Cruel OS World!\n");
}

int main(int argc, char *argv[]) {
  atexit(func);

  if (argc < 2) return EXIT_FAILURE;

  if (strcmp(argv[1], "exit") == 0)
    exit(0);
  if (strcmp(argv[1], "_exit") == 0)
    _exit(0);
  if (strcmp(argv[1], "__exit") == 0)
    syscall(SYS_exit, 0);
}
