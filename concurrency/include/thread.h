#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NTHREAD 6400
enum {
  T_FREE = 0,
  T_LIVE,
  T_DEAD,
};
struct thread {
  int id, status;
  pthread_t thread;
  void (*entry)(int);
};

struct thread tpool[NTHREAD], *tptr = tpool;

void *wrapper(void *arg) {
  struct thread *thread = (struct thread *)arg;
  thread->entry(thread->id);
  return NULL;
}

void create(void *fn) {
  // pthread_attr_t attr;
  // pthread_attr_init(&attr);
  // pthread_attr_setstacksize(&attr, 1024 * 1024);
  assert(tptr - tpool < NTHREAD);
  *tptr = (struct thread){
      .id = tptr - tpool + 1,
      .status = T_LIVE,
      .entry = fn,
  };
  pthread_create(&(tptr->thread), NULL, wrapper, tptr);
  // pthread_create(&(tptr->thread), &attr, wrapper, tptr);
  // pthread_attr_destroy(&attr);
  ++tptr;
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

__attribute__((destructor)) void cleanup() { join(); }
