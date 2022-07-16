#include "thread.h"

void Ta() {
  while (1) {
    printf("a"); 
  }
}
void Tb() {
  while (1) {
    printf("b"); 
  }
}

int main() {
  create(Ta);
  create(Tb);
}
