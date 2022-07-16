#include <fcntl.h>

#define SECRET "\x01\x14\x05\x14"

int main() {
  int fd = open("/dev/nuke", O_WRONLY);
  if (fd > 0) {
    write(fd, SECRET, sizeof(SECRET) - 1);
    close(fd);
  } else {
    perror("launcher");
  }
}
