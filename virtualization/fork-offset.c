#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
  int fd = open("a.txt", O_WRONLY | O_CREAT);
  assert(fd > 0);
  pid_t pid = fork();
  assert(pid >= 0);
  if (pid == 0) {
    write(fd, "Hello", 5);
  } else {
    write(fd, "World", 5);
  }
  close(fd);
}
