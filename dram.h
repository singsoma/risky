/* See LICENSE for license details. */
#define DRAMSIZE (128 * 1024 * 1024)
#define DRAMBASE (0x80000000)

uint64_t dram_load(uint8_t *dram, uint64_t addr, uint64_t size);

void dram_store(uint8_t *dram, uint64_t addr, uint64_t size, uint64_t val);
