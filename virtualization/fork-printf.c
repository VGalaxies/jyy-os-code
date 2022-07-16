#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  int n = 2;
  for (int i = 0; i < n; i++) {
    fork();
    printf("Hello\n");
  }
  for (int i = 0; i < n; i++) {
    wait(NULL);
  }  
}
