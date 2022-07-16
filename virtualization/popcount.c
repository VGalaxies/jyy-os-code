#include <stdio.h>

__attribute__((noinline))
int popcount(int x) {
  int s = 0;
  int b0 = (x >> 0) & 1;
  s += b0;
  int b1 = (x >> 1) & 1;
  s += b1;
  int b2 = (x >> 2) & 1;
  s += b2;
  int b3 = (x >> 3) & 1;
  s += b3;
  return s;
}

int main() {
  printf("%d\n", popcount(0b1101));
}
