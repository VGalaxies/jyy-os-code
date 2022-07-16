#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

#define LENGTH(arr)  (sizeof(arr) / sizeof(arr[0]))

int n, fd, pid;
uint64_t found[4096];
bool reset;

void scan(uint16_t val) {
  uintptr_t start, kb;
  char perm[16];
  FILE *fp = popen("pmap -x $(pidof dosbox) | tail -n +3", "r"); assert(fp);

  if (reset) n = 0;
  while (fscanf(fp, "%lx", &start) == 1 && (intptr_t)start > 0) {
    assert(fscanf(fp, "%ld%*ld%*ld%s%*[^\n]s", &kb, perm) >= 1);
    if (perm[1] != 'w') continue;

    uintptr_t size = kb * 1024;
    char *mem = malloc(size); assert(mem);
    assert(lseek(fd, start, SEEK_SET) != (off_t)-1);
    assert(read(fd, mem, size) == size);
    for (int i = 0; i < size; i += 2) {
      uint16_t v = *(uint16_t *)(&mem[i]);
      if (reset) {
        if (val == v && n < LENGTH(found)) found[n++] = start + i;
      } else {
        for (int j = 0; j < n; j++) {
	  if (found[j] == start + i && v != val) found[j] = 0;
	}
      }
    }
    free(mem);
  }
  pclose(fp);

  int s = 0;
  for (int i = 0; i < n; i++) {
    if (found[i] != 0) s++;
  }
  reset = false;
  printf("There are %d match(es).\n", s);
}

void overwrite(uint16_t val) {
  int s = 0;
  for (int i = 0; i < n; i++)
    if (found[i] != 0) {
      assert(lseek(fd, found[i], SEEK_SET) != (off_t)-1);
      write(fd, &val, 2);
      s++;
    }
  printf("%d value(s) written.\n", s);
}

int main() {
  char buf[32];
  setbuf(stdout, NULL);

  FILE *fp = popen("pidof dosbox", "r");
  assert(fscanf(fp, "%d", &pid) == 1);
  pclose(fp);

  sprintf(buf, "/proc/%d/mem", pid);
  fd = open(buf, O_RDWR); assert(fd > 0);

  for (reset = true; !feof(stdin); ) {
    int val;
    printf("(DOSBox %d) ", pid);
    if (scanf("%s", buf) <= 0) { close(fd); exit(0); }
    switch (buf[0]) {
      case 'q': close(fd); exit(0); break;
      case 's': scanf("%d", &val); scan(val); break;
      case 'w': scanf("%d", &val); overwrite(val); break;
      case 'r': reset = true; printf("Search results reset.\n"); break;
    }
  }
}
