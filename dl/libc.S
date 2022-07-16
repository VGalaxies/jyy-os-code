#include "dl.h"
#include <sys/syscall.h>

DL_HEAD

EXPORT(putchar)
EXPORT(exit)

DL_CODE

putchar:
  mov %dil, buf(%rip)
  mov $SYS_write, %rax
  mov $1, %rdi
  lea buf(%rip), %rsi
  mov $1, %rdx
  syscall
  ret
buf:
  .byte 0

exit:
  movq $SYS_exit, %rax
  syscall

DL_END
