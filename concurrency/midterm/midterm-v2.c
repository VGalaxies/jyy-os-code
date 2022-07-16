#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Mutex
typedef pthread_mutex_t mutex_t;
#define MUTEX_INIT() PTHREAD_MUTEX_INITIALIZER
void mutex_lock(mutex_t *lk) { pthread_mutex_lock(lk); }
void mutex_unlock(mutex_t *lk) { pthread_mutex_unlock(lk); }

long f(int x);
long sum = 0;
mutex_t sum_lk = MUTEX_INIT();
// mutex_t stream_lk = MUTEX_INIT();

#define NTHREAD 4096
static pthread_t tpool[NTHREAD];

void join() {
  for (int i = 0; i < NTHREAD; i++) {
    pthread_join(tpool[i], NULL);
  }
}

void *handler(void *arg) {
  int x;

  while (true) {
    // mutex_lock(&stream_lk);
    bool flag = (!feof(stdin) && scanf("%d", &x) == 1);
    // mutex_unlock(&stream_lk);

    if (!flag) {
      break;
    }

    long res = f(x);
    mutex_lock(&sum_lk);
    sum += res;
    mutex_unlock(&sum_lk);
  }

  return NULL;
}

int main() {
  for (size_t i = 0; i < NTHREAD; ++i) {
    pthread_create(&(tpool[i]), NULL, handler, NULL);
  }
  join();
  printf("%ld\n", sum);
  exit(EXIT_SUCCESS);
}

/*
T1.1
T1.2
T2.1
T2.2
T1.3
T1.4
T1.5
T1.6
T2.3
T1.7
T1.8
T2.4
T2.5
T2.6
T2.7
T2.8
T2.9
T1.9
*/
