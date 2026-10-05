/* See LICENSE for license details. */
#include <err.h>
#include <stdlib.h>

#include "dram.h"

/* Load an object from memory. */
uint64_t
dram_load(uint8_t *dram, uint64_t addr, uint64_t size)
{
	uint64_t i = addr - DRAMBASE;
	switch (size) {
	case 8:
		return ((uint64_t)dram[i + 0] << 0);
	case 16:
		return ((uint64_t)dram[i + 0] << 0)
		     | ((uint64_t)dram[i + 1] << 8);
	case 32:
		return ((uint64_t)dram[i + 0] << 0)
		     | ((uint64_t)dram[i + 1] << 8)
		     | ((uint64_t)dram[i + 2] << 16)
		     | ((uint64_t)dram[i + 3] << 24);
	case 64:
		return ((uint64_t)dram[i + 0] << 0)
		     | ((uint64_t)dram[i + 1] << 8)
		     | ((uint64_t)dram[i + 2] << 16)
		     | ((uint64_t)dram[i + 3] << 24)
		     | ((uint64_t)dram[i + 4] << 32)
		     | ((uint64_t)dram[i + 5] << 40)
		     | ((uint64_t)dram[i + 6] << 48)
		     | ((uint64_t)dram[i + 7] << 56);
	default:
		errx(1, "invalid object size");
	}
}

/* Store an object into dram memory. */
void
dram_store(uint8_t *dram, uint64_t addr, uint64_t size, uint64_t val)
{
	uint64_t i = addr - DRAMBASE;
	switch (size) {
	case 8:
		dram[i + 0] = (uint8_t)((val >> 0)  & 0xff);
		break;
	case 16:
		dram[i + 0] = (uint8_t)((val >> 0)  & 0xff);
		dram[i + 1] = (uint8_t)((val >> 8)  & 0xff);
		break;
	case 32:
		dram[i + 0] = (uint8_t)((val >> 0)  & 0xff);
		dram[i + 1] = (uint8_t)((val >> 8)  & 0xff);
		dram[i + 2] = (uint8_t)((val >> 16) & 0xff);
		dram[i + 3] = (uint8_t)((val >> 24) & 0xff);
		break;
	case 64:
		dram[i + 0] = (uint8_t)((val >>  0) & 0xff);
		dram[i + 1] = (uint8_t)((val >>  8) & 0xff);
		dram[i + 2] = (uint8_t)((val >> 16) & 0xff);
		dram[i + 3] = (uint8_t)((val >> 24) & 0xff);
		dram[i + 4] = (uint8_t)((val >> 32) & 0xff);
		dram[i + 5] = (uint8_t)((val >> 40) & 0xff);
		dram[i + 6] = (uint8_t)((val >> 48) & 0xff);
		dram[i + 7] = (uint8_t)((val >> 56) & 0xff);
		break;
	default:
		errx(1, "invalid object size");
	}
}
