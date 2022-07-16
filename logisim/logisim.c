#include <stdio.h>
#include <unistd.h>

#define FORALL_REGS(_)  _(X) _(Y)
#define OUTPUTS(_)      _(A) _(B) _(C) _(D) _(E) _(F) _(G)
#define LOGIC           X1 = !X && Y; \
                        Y1 = !X && !Y; \
                        A  = (!X && !Y) || (X && !Y); \
                        B  = 1; \
                        C  = (!X && !Y) || (!X && Y); \
                        D  = (!X && !Y) || (X && !Y); \
                        E  = (!X && !Y) || (X && !Y); \
                        F  = (!X && !Y); \
                        G  = (X && !Y); 

#define DEFINE(X)   static int X, X##1;
#define UPDATE(X)   X = X##1;
#define PRINT(X)    printf(#X " = %d; ", X);

int main() {
  FORALL_REGS(DEFINE);
  OUTPUTS(DEFINE);
  while (1) { // clock
    LOGIC;
    OUTPUTS(PRINT);
    putchar('\n');
    fflush(stdout);
    sleep(1);
    FORALL_REGS(UPDATE);
  }
}
