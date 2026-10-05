/* See LICENSE for license details. */
#include <err.h>
#include <stdio.h>

#include "cpu.h"
#include "dram.h"

#include "io.h"

/* Reads a RISC-V program and loads it into memory. */
void
load_program(const char *path, uint8_t *dram)
{
	FILE *f = fopen(path, "r");
	if (f == NULL)
		err(1, "failed to open file %s", path);

	if (fseek(f, 0L, SEEK_END) != 0) {
		fclose(f);
		err(1, "failed to seek file %s", path);
	}
	long filesize = ftell(f);
	if (filesize == -1) {
		fclose(f);
		err(1, "failed to tell file %s", path);
	}
	size_t size = (size_t)filesize;
	if (size > DRAMSIZE) {
		fclose(f);
		errx(1, "the file must be at most %d KiB", DRAMSIZE / 1024);
	}
	rewind(f);
	if (fread(dram, 1, size, f) < size) {
		fclose(f);
		errx(1, "failed to read file %s", path);
	}
	fclose(f);
}

/* Print cpu's registers state. */
void
print_cpu(struct cpu *cpu)
{
	for (size_t i = 0; i < CPUREGS; i++)
		if (cpu->regs[i])
			printf("x%02zu = 0x%llx\n", i, cpu->regs[i]);
}
