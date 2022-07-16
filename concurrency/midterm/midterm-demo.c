#include <thread-sync.h>
#include <thread.h>

int volatile sum = 0;

void do_sum() {
  int t;
  /* 1 */ t = sum;
  /* 2 */ t += 1;
  /* 3 */ sum = t;

  /* 4 */ t = sum;
  /* 5 */ t += 1;
  /* 6 */ sum = t;

  /* 7 */ t = sum;
  /* 8 */ t += 1;
  /* 9 */ sum = t;
}

int main() {
  create(do_sum);
  create(do_sum);
  join();
  printf("%d\n", sum);
}
