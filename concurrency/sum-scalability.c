#include "thread.h"
#include "thread-sync.h"

#define N 10000000
#define SPIN

#ifdef SPIN
spinlock_t lock = SPIN_INIT();
#else
mutex_t lock = MUTEX_INIT();
#endif

long n, sum = 0;

void Tsum() {
  for (int i = 0; i < n; i++) {
#ifdef SPIN
    spin_lock(&lock);
#else
    mutex_lock(&lock);
#endif
    sum++;
#ifdef SPIN
    spin_unlock(&lock);
#else
    mutex_unlock(&lock);
#endif
  }
}

int main(int argc, char *argv[]) {
  assert(argc == 2);
  int nthread = atoi(argv[1]);
  n = N / nthread;
  for (int i = 0; i < nthread; i++) {
    create(Tsum);
  }
  join();
  assert(sum == n * nthread);
}
