#include "thread.h"

#define N 100000000
/* long volatile sum = 0; */
long sum = 0;

void Tsum() {
  for (int i = 0; i < N; i++) {
    asm volatile("addq $1, %0;" : "+m"(sum));
    /* asm volatile("" ::: "memory"); */
    /* sum++; */
  }
}

int main() {
  create(Tsum);
  create(Tsum);
  join();
  printf("sum = %ld\n", sum);
}
