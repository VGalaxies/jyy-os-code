#include <unistd.h>
#include <stdlib.h>

long f(int x) {
  usleep(rand() % 1023);
  return x;
}
