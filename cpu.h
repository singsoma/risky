/* See LICENSE for license details. */
#define CPUREGS 32

struct bus {
	uint8_t *dram;
};

struct cpu {
	uint64_t regs[CPUREGS];
	uint64_t pc;
	struct bus bus;
};

uint32_t fetch(const uint64_t pc, struct bus *bus);

int execute(struct cpu *cpu, const uint32_t inst);
