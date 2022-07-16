#include "thread.h"
#include <string.h>

char buf[1 << 30];

void foo(int id) { memset(buf, '0' + id, sizeof(buf) - 1); }

int main() {
  for (int i = 0; i < 4; i++)
    create(foo);
  join();
  puts(buf);
}
