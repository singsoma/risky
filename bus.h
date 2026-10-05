/* See LICENSE for license details. */

uint64_t bus_load(struct bus *bus, uint64_t addr, uint64_t size);

void bus_store(struct bus *bus, uint64_t addr, uint64_t size, uint64_t val);
