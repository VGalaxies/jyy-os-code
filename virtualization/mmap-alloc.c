#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define GiB * (1024LL * 1024 * 1024)

int main() {
  volatile uint8_t *p = mmap(NULL, 3 GiB, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
  printf("mmap: %lx\n", (uintptr_t)p);
  if ((intptr_t)p == -1) {
    perror("cannot map");
    exit(1);
  }
  *(int *)(p + 1 GiB) = 114;
  *(int *)(p + 2 GiB) = 514;
  printf("Read get: %d\n", *(int *)(p + 1 GiB));
  printf("Read get: %d\n", *(int *)(p + 2 GiB));
}
