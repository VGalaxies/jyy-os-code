#include <sys/file.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

int main(int argc, char *argv[]) {
  for (int i = 1; i < argc; i++) {
    int fd = open(argv[i], O_RDONLY);
    assert(fd > 0);
    printf("Acquire %s\n", argv[i]);
    flock(fd, LOCK_EX);
    sleep(3);
  }
  printf("All locks acquired!\n");
}
