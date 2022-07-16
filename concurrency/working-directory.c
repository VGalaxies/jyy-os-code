#include "thread.h"
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

static const char *dir_1 = "../";
static const char *dir_2 = "../..";
static const char *file_name = "demo";

void worker() {
  int ret;
  if (rand() % 2) {
    printf("create %s at %s\n", file_name, dir_1);
    ret = chdir(dir_1);
    assert(ret == 0);
    ret = creat(file_name, O_RDONLY);
  } else {
    printf("create %s at %s\n", file_name, dir_2);
    ret = chdir(dir_2);
    assert(ret == 0);
    ret = creat(file_name, O_RDONLY);
  }
}

int main(int argc, char *argv[]) {
  for (int i = 0; i < 2; i++) {
    create(worker);
  }
}
