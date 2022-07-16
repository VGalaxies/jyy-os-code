#include "dl.h"

DL_HEAD

LOAD("libc.dl")
LOAD("libhello.dl")
IMPORT(hello)
EXPORT(main)

DL_CODE

main:
  call DSYM(hello)
  call DSYM(hello)
  call DSYM(hello)
  call DSYM(hello)
  movq $0, %rax
  ret

DL_END
