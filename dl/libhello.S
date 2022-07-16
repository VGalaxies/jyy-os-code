#include "dl.h"

DL_HEAD

LOAD("libc.dl")
IMPORT(putchar)
EXPORT(hello)

DL_CODE

hello:
  lea str(%rip), %rdi
  mov count(%rip), %eax
  push %rbx
  mov %rdi, %rbx
  inc %eax
  mov %eax, count(%rip)
  add $0x30, %eax
  movb %al, 0x6(%rdi)
loop:
  movsbl (%rbx),%edi
  test %dil,%dil
  je out
  call DSYM(putchar)
  inc  %rbx
  jmp loop
out:
  pop %rbx
  ret

str:
  .asciz "Hello X\n"

count:
  .int 0

DL_END
