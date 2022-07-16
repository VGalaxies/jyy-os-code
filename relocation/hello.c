#include <stdio.h>
#include <stdint.h>
#include <assert.h>

int main();

void hello() {
  char *p = (char *)main + 0x6 + 1;
  int32_t offset = *(int32_t *)p;
  assert((char *)main + 0xb + offset == (char *)hello);
}
