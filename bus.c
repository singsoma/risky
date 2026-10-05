/* See LICENSE for license details. */
#include <err.h>
#include <stdlib.h>

#include "cpu.h"
#include "dram.h"

#include "bus.h"

/*
 * Given the bus, an address, and the size of the value, returns the value
 * stored at the relative address (addr - DEVBASE).
 */
uint64_t
bus_load(struct bus *bus, uint64_t addr, uint64_t size)
{
	if (addr >= DRAMBASE)
		return dram_load(bus->dram, addr, size);
	else
		errx(1, "invalid bus address load 0x%llx", addr);
}

/*
 * Given the bus, an address, the size of the value, and a value, stores the
 * value at the relative address (addr - DEVBASE).
 */
void
bus_store(struct bus *bus, uint64_t addr, uint64_t size, uint64_t val)
{
	if (addr >= DRAMBASE)
		dram_store(bus->dram, addr, size, val);
	else
		errx(1, "invalid bus address store 0x%llx", addr);
}
