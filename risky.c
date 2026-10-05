/* See LICENSE for license details. */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#include "cpu.h"
#include "bus.h"
#include "dram.h"
#include "io.h"

/* Takes a RISC-V assembly file as an argument, and executes it. */
int
main(int argc, char **argv)
{
	if (argc != 2)
		errx(1, "usage: risky [file]");

	uint8_t *dram = calloc(DRAMSIZE, 1);
	if (dram == NULL)
		err(1, "failed calloc");
	load_program(argv[1], dram);

	struct cpu cpu;
	for (size_t i = 0; i < CPUREGS; i++)
		cpu.regs[i] = 0x0;
		
	cpu.regs[0]  = 0x0;
	cpu.regs[2]  = DRAMBASE + DRAMSIZE;
	cpu.pc       = DRAMBASE;
	cpu.bus.dram = dram;

	/* Instruction cycle. */
	for (;;) {
		uint32_t inst = fetch(cpu.pc, &cpu.bus); /* Fetch. */
		cpu.pc += 0x4;
		if (execute(&cpu, inst)) /* Decode and execute. */
			break;
		if (cpu.pc == 0x0)
			break;
	}

	free(dram);
	print_cpu(&cpu);
	return 0;
}
