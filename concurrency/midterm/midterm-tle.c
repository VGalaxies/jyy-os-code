#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Mutex
typedef pthread_mutex_t mutex_t;
#define MUTEX_INIT() PTHREAD_MUTEX_INITIALIZER
void mutex_lock(mutex_t *lk) { pthread_mutex_lock(lk); }
void mutex_unlock(mutex_t *lk) { pthread_mutex_unlock(lk); }

// Conditional Variable
typedef pthread_cond_t cond_t;
#define COND_INIT() PTHREAD_COND_INITIALIZER
#define cond_wait pthread_cond_wait
#define cond_broadcast pthread_cond_broadcast
#define cond_signal pthread_cond_signal

long f(int x);
long sum = 0;
mutex_t sum_lk = MUTEX_INIT();
mutex_t tpool_lk = MUTEX_INIT();
cond_t tpool_cond = COND_INIT();

#define NTHREAD 8
enum {
  T_FREE = 0,
  T_LIVE,
  T_DEAD,
};

struct thread {
  int arg, status;
  pthread_t thread;
};

static struct thread tpool[NTHREAD];

size_t free_index() {
  for (size_t i = 0; i < NTHREAD; ++i) {
    if (tpool[i].status == T_FREE) {
      return i;
    }
  }
  return -1;
}

void *wrapper(void *arg) {
  struct thread *thread = (struct thread *)arg;
  long res = f(thread->arg);

  mutex_lock(&sum_lk);
  sum += res;
  mutex_unlock(&sum_lk);

  mutex_lock(&tpool_lk);
  thread->status = T_FREE;
  cond_broadcast(&tpool_cond);
  mutex_unlock(&tpool_lk);

  return NULL;
}

void create(int arg) {
  mutex_lock(&tpool_lk);

  size_t index;
  while ((index = free_index()) == -1) {
    cond_wait(&tpool_cond, &tpool_lk);
  }
  assert(index != -1);
  assert(tpool[index].status == T_FREE);

  tpool[index] = (struct thread){
      .arg = arg,
      .status = T_LIVE,
  };

  mutex_unlock(&tpool_lk);

  pthread_create(&(tpool[index].thread), NULL, wrapper, &tpool[index]);
}

void join() {
  for (int i = 0; i < NTHREAD; i++) {
    struct thread *t = &tpool[i];
    if (t->status == T_LIVE) {
      pthread_join(t->thread, NULL);
      t->status = T_DEAD;
    }
  }
}

int main() {
  int x;
  while (!feof(stdin) && scanf("%d", &x) == 1) {
    create(x);
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
