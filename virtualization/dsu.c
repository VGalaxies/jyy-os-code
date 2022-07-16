#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#define PG_SIZE (getpagesize())

struct jmp {
  uint32_t opcode : 8;
  int32_t offset : 32;
} __attribute__((packed));

#define JMP(off) ((struct jmp){0xe9, off - sizeof(struct jmp)})

static inline bool within_page(void *addr) {
  return (uintptr_t)addr % PG_SIZE + sizeof(struct jmp) <= PG_SIZE;
}

void DSU(void *old, void *new) {
  void *base = (void *)((uintptr_t)old & ~(PG_SIZE - 1));
  size_t len = PG_SIZE * (within_page(old) ? 1 : 2);
  int flags = PROT_WRITE | PROT_READ | PROT_EXEC;
  if (mprotect(base, len, flags) == 0) {
    *(struct jmp *)old = JMP((char *)new - (char *)old); // **PATCH**
    mprotect(base, len, flags & ~PROT_WRITE);
  } else {
    perror("DSU fail");
  }
}

void foo() { printf("In %s();\n", __func__); }
void foo_new() { printf("In updated %s();\n", __func__); }

int main() {
  foo();
  DSU(foo, foo_new);
  foo();
}
