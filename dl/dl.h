#define REC_SZ 32
#define DL_MAGIC "\x01\x14\x05\x14"

#ifdef __ASSEMBLER__
  #define DL_HEAD     __hdr: \
                      /* magic */    .ascii DL_MAGIC; \
                      /* file_sz */  .4byte (__end - __hdr); \
                      /* code_off */ .4byte (__code - __hdr)
  #define DL_CODE     .fill REC_SZ - 1, 1, 0; \
                      .align REC_SZ, 0; \
                      __code:
  #define DL_END      __end:

  #define RECORD(sym, off, name) \
    .align REC_SZ, 0; \
    sym .8byte (off); .ascii name

  #define IMPORT(sym) RECORD(sym:,           0, "?" #sym "\0")
  #define EXPORT(sym) RECORD(    , sym - __hdr, "#" #sym "\0")
  #define LOAD(lib)   RECORD(    ,           0, "+" lib  "\0")
  #define DSYM(sym)   *sym(%rip)
#else
  #include <stdint.h>

  struct dl_hdr {
    char magic[4];
    uint32_t file_sz, code_off;
  };

  struct symbol {
    int64_t offset;
    char type, name[REC_SZ - sizeof(int64_t) - 1];
  };
#endif
