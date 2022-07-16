#include <am.h>
#include <klib.h>

static inline uint32_t inl(int port) {
  uint32_t data;
  asm volatile ("inl %1, %0" : "=a"(data) : "d"((uint16_t)port));
  return data;
}

static inline void outl(int port, uint32_t data) {
  asm volatile ("outl %%eax, %%dx" : : "a"(data), "d"((uint16_t)port));
}

uint32_t pciconf_read(uint32_t bus, uint32_t slot, uint32_t func, uint32_t offset) {
   uint32_t reg = (bus  << 16) | (slot << 11)
                | (func << 8)  | (offset) | 0x80000000;
  outl(0xcf8, reg);
  return inl(0xcfc);
}

int main() {
  ioe_init();
  for (int bus = 0; bus < 256; bus ++) {
    for (int slot = 0; slot < 32; slot ++) {
      uint32_t info = pciconf_read(bus, slot, 0, 0);
      uint16_t id   = info >> 16, vendor = info & 0xffff;
      if (vendor != 0xffff) {
        printf("%02d:%02d device %x by vendor %x", bus, slot, id, vendor);
        if (id == 0x100e && vendor == 0x8086) {
          printf(" <-- This is an Intel e1000 NIC card!");
        }
        printf("\n");
      }
    }
  }
}
