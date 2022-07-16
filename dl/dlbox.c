#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "dl.h"

#define SIZE 4096
#define LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

struct dlib {
  struct dl_hdr hdr;
  struct symbol *symtab; // borrowed spaces from header
  const char *path;
};

static struct dlib *dlopen(const char *path);

struct dlib *dlopen_chk(const char *path) {
  struct dlib *lib = dlopen(path);
  if (!lib) {
    fprintf(stderr, "Not a valid dlib file: %s.\n", path);
    exit(1);
  }
  return lib;
}

// Implementation of binutils

void dl_gcc(const char *path) {
  char buf[256], *dot = strrchr(path, '.');
  if (dot) {
    *dot = '\0';
    sprintf(buf, "gcc -m64 -fPIC -c %s.S && "
      "objcopy -S -j .text -O binary %s.o %s.dl", path, path, path);
    system(buf);
  }
}


void dl_readdl(const char *path) {
  struct dlib *h = dlopen_chk(path);
  printf("DLIB file %s:\n\n", h->path);
  for (struct symbol *sym = h->symtab; sym->type; sym++) {
    switch (sym->type) {
      case '+': printf("    LOAD  %s\n", sym->name); break;
      case '?': printf("  EXTERN  %s\n", sym->name); break;
      case '#': printf(   "%08lx  %s\n", sym->offset, sym->name); break;
    }
  }
}

void dl_objdump(const char *path) {
  struct dlib *h = dlopen_chk(path);
  char *hc = (char *)h, cmd[64];
  FILE *fp = NULL;

  printf("Disassembly of binary %s:\n", h->path);

  for (char *code = hc + h->hdr.code_off; code < hc + h->hdr.file_sz; code++) {
    for (struct symbol *sym = h->symtab; sym->type; sym++) {
      if (hc + sym->offset == code) {
        int off = code - hc - h->hdr.code_off;
        if (fp) pclose(fp);
        sprintf(cmd, "ndisasm - -b 64 -o 0x%08x\n", off);
        fp = popen(cmd, "w");
        printf("\n%016x <%s>:\n", off, sym->name);
        fflush(stdout);
      }
    }
    if (fp) fputc(*code, fp);
  }
  if (fp) pclose(fp);
}

// binutils: interpreter
void dl_interp(const char *path) {
  struct dlib *h = dlopen_chk(path);
  int (*entry)() = NULL;
  for (struct symbol *sym = h->symtab; sym->type; sym++)
    if (strcmp(sym->name, "main") == 0)
      entry = (void *)((char *)h + sym->offset);
  if (entry) {
    exit(entry());
  }
}

struct cmd {
  const char *cmd;
  void (*handler)(const char *path);
} commands[] = {
  { "gcc",     dl_gcc },
  { "readdl",  dl_readdl },
  { "objdump", dl_objdump },
  { "interp",  dl_interp },
  { "",        NULL },
};

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s {gcc|readdl|objdump|interp} FILE...\n", argv[0]);
    return 1;
  }

  for (struct cmd *cmd = &commands[0]; cmd->handler; cmd++) {
    for (char **path = &argv[2]; *path && strcmp(argv[1], cmd->cmd) == 0; path++) {
      if (path != argv + 2) printf("\n");
      cmd->handler(*path);
    }
  }
}

// Implementation of dlopen()

static struct symbol *libs[16], syms[128];

static void *dlsym(const char *name);
static void dlexport(const char *name, void *addr);
static void dlload(struct symbol *sym);

static struct dlib *dlopen(const char *path) {
  struct dl_hdr hdr;
  struct dlib *h;

  int fd = open(path, O_RDONLY);
  if (fd < 0) goto bad;
  if (read(fd, &hdr, sizeof(hdr)) < sizeof(hdr)) goto bad;
  if (strncmp(hdr.magic, DL_MAGIC, strlen(DL_MAGIC)) != 0) goto bad;

  h = mmap(NULL, hdr.file_sz, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE, fd, 0);
  if (h == (void *)-1) goto bad;

  h->symtab = (struct symbol *)((char *)h + REC_SZ);
  h->path = path;

  for (struct symbol *sym = h->symtab; sym->type; sym++) {
    switch (sym->type) {
      case '+': dlload(sym); break; // (recursively) load
      case '?': sym->offset = (uintptr_t)dlsym(sym->name); break; // resolve
      case '#': dlexport(sym->name, (char *)h + sym->offset); break; // export
    }
  }

  return h;

bad:
  if (fd > 0) close(fd);
  return NULL;
}

static void *dlsym(const char *name) {
  for (int i = 0; i < LENGTH(syms); i++)
    if (strcmp(syms[i].name, name) == 0)
      return (void *)syms[i].offset;
  assert(0);
}

static void dlexport(const char *name, void *addr) {
  for (int i = 0; i < LENGTH(syms); i++)
    if (!syms[i].name[0]) {
      syms[i].offset = (uintptr_t)addr; // load-time offset
      strcpy(syms[i].name, name);
      return;
    }
  assert(0);
}

static void dlload(struct symbol *sym) {
  for (int i = 0; i < LENGTH(libs); i++) {
    if (libs[i] && strcmp(libs[i]->name, sym->name) == 0) return; // already loaded
    if (!libs[i]) {
      libs[i] = sym;
      dlopen(sym->name); // load recursively
      return;
    }
  }
  assert(0);
}
