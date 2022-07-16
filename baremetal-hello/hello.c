#include "baremetal.h"

int entry() {
  putch('H');
  putch('i');
  putch('\n');
  halt();
}
