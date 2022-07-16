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

// Conditional Variable
typedef pthread_cond_t cond_t;
#define COND_INIT() PTHREAD_COND_INITIALIZER
#define cond_wait pthread_cond_wait
#define cond_broadcast pthread_cond_broadcast
#define cond_signal pthread_cond_signal

long f(int x);
long sum = 0;
mutex_t sum_lk = MUTEX_INIT();

#define NTHREAD 6400
#define P 320 // producer

static pthread_t tpool[NTHREAD];

void join() {
  for (int i = 0; i < NTHREAD; i++) {
    pthread_join(tpool[i], NULL);
  }
}

mutex_t buffer_lk = MUTEX_INIT();
cond_t buffer_cv = COND_INIT();
static bool is_eof = false;

typedef struct stack stack_t;
struct stack {
  int x;
  stack_t *next;
};

static stack_t *buffer = NULL;

void push(int x) {
  if (buffer == NULL) {
    buffer = (stack_t *)malloc(sizeof(stack_t));
    buffer->x = x;
    buffer->next = NULL;
  } else {
    stack_t *head = (stack_t *)malloc(sizeof(stack_t));
    head->x = x;
    head->next = buffer;
    buffer = head;
  }
}

int pop() {
  stack_t *tmp = buffer;
  int res = tmp->x;
  buffer = tmp->next;
  free(tmp);
  return res;
}

void *consumer(void *arg) {
  while (true) {
    mutex_lock(&buffer_lk);

    while (buffer == NULL && !is_eof) {
      cond_wait(&buffer_cv, &buffer_lk);
    }

    if (is_eof && buffer == NULL) {
      mutex_unlock(&buffer_lk);
      break;
    }

    int x = pop();
    mutex_unlock(&buffer_lk);

    long res = f(x);

    mutex_lock(&sum_lk);
    sum += res;
    mutex_unlock(&sum_lk);
  }

  return NULL;
}

void *producer(void *arg) {
  int x;
  while (true) {
    bool flag = (!feof(stdin) && scanf("%d", &x) == 1);
    mutex_lock(&buffer_lk);

    if (!flag) {
      if (buffer == NULL) {
        is_eof = true;
        mutex_unlock(&buffer_lk);
        cond_broadcast(&buffer_cv);
        break;
      } else {
        mutex_unlock(&buffer_lk);
        cond_broadcast(&buffer_cv);
        continue;
      }
    }

    push(x);
    mutex_unlock(&buffer_lk);
    cond_broadcast(&buffer_cv);
  }

  return NULL;
}

int main() {
  for (int i = 0; i < P; ++i) {
    pthread_create(&(tpool[i]), NULL, producer, NULL);
  }
  for (int i = P; i < NTHREAD; ++i) {
    pthread_create(&(tpool[i]), NULL, consumer, NULL);
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
