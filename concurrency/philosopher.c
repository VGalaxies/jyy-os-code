#include "thread.h"
#include "thread-sync.h"

#define N 3
sem_t locks[N];

void Tphilosopher(int id) {
  int lhs = (N + id - 1) % N;
  int rhs = id % N;
  if (id == N) {
    while (1) {
      P(&locks[rhs]);
      printf("T%d Got %d\n", id, rhs + 1);
      P(&locks[lhs]);
      printf("T%d Got %d\n", id, lhs + 1);
      V(&locks[rhs]);
      V(&locks[lhs]);
    }
  } else {
    while (1) {
      P(&locks[lhs]);
      printf("T%d Got %d\n", id, lhs + 1);
      P(&locks[rhs]);
      printf("T%d Got %d\n", id, rhs + 1);
      V(&locks[lhs]);
      V(&locks[rhs]);
    }
  }
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < N; i++) {
    SEM_INIT(&locks[i], 1);
  }
  for (int i = 0; i < N; i++) {
    create(Tphilosopher);
  }
}
