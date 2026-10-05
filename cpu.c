/* See LICENSE for license details. */
#include <err.h>
#include <stdlib.h>

#include "cpu.h"
#include "bus.h"

/* Fetches an instruction from memory. */
inline uint32_t
fetch(const uint64_t pc, struct bus *bus)
{
	return bus_load(bus, pc, 32);
}

/* Decode and execute an instruction. */
int
execute(struct cpu *cpu, const uint32_t inst)
{
	if (inst == 0)
		return 1;

	uint64_t addr, imm;
	uint32_t shamt;
	uint32_t opcode = (inst >> 0)  & 0x7F;
	uint32_t rd     = (inst >> 7)  & 0x1F;
	uint32_t rs1    = (inst >> 15) & 0x1F;
	uint32_t rs2    = (inst >> 20) & 0x1F;
	uint32_t func3  = (inst >> 12) & 0x07;
	uint32_t func7  = (inst >> 25) & 0x7f;

	cpu->regs[0] = 0x0;

	switch (opcode) {
	case 0x3:
		imm = (uint64_t)((int64_t)(int32_t)inst >> 20);
		addr = cpu->regs[rs1] + imm;

		switch (func3) {
		case 0x0: /* lb. */
			cpu->regs[rd] = (uint64_t)(int64_t)(int8_t)
				bus_load(&cpu->bus, addr, 8);
			break;
		case 0x1: /* lh. */
			cpu->regs[rd] = (uint64_t)(int64_t)(int16_t)
				bus_load(&cpu->bus, addr, 16);
			break;
		case 0x2: /* lw. */
			cpu->regs[rd] = (uint64_t)(int64_t)(int32_t)
				bus_load(&cpu->bus, addr, 32);
			break;
		case 0x3: /* ld. */
			cpu->regs[rd] = bus_load(&cpu->bus, addr, 64);
			break;
		case 0x4: /* lbu. */
			cpu->regs[rd] = bus_load(&cpu->bus, addr, 8);
			break;
		case 0x5: /* lhu. */
			cpu->regs[rd] = bus_load(&cpu->bus, addr, 16);
			break;
		case 0x6: /* lwu. */
			cpu->regs[rd] = bus_load(&cpu->bus, addr, 32);
			break;
		default:
			errx(1, "instruction 0x%x not implemented yet", opcode);
		}
		break;
	case 0x13:
		imm = (uint64_t)((int64_t)(int32_t)inst >> 20);
		shamt = (uint32_t)(imm & 0x3f);

		switch (func3) {
		case 0x0: /* addi. */
			cpu->regs[rd] = cpu->regs[rs1] + imm;
			break;
		case 0x1: /* slli. */
			cpu->regs[rd] = cpu->regs[rs1] << shamt;
			break;
		case 0x2: /* slti. */
			cpu->regs[rd] = (
				(int64_t)cpu->regs[rs1] < (int64_t)imm
			) ? 1 : 0;
			break;
		case 0x3: /* sltiu. */
			cpu->regs[rd] = (cpu->regs[rs1] < imm) ? 1 : 0;
			break;
		case 0x4: /* xori. */
			cpu->regs[rd] = cpu->regs[rs1] ^ imm;
			break;
		case 0x5:
			switch (func7 >> 1) {
			case 0x0: /* srli. */
				cpu->regs[rd] = cpu->regs[rs1] >> shamt;
				break;
			case 0x10: /* srai. */
				cpu->regs[rd] = (uint64_t)(
					(int64_t)cpu->regs[rs1] >> shamt
				);
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x6: /* ori. */
			cpu->regs[rd] = cpu->regs[rs1] | imm;
			break;
		case 0x7: /* andi. */
			cpu->regs[rd] = cpu->regs[rs1] & imm;
			break;
		default:
			errx(1, "instruction 0x%x not implemented yet", opcode);
		}
		break;
	case 0x17: /* auipc. */
		imm = (uint64_t)(int64_t)(int32_t)(inst & 0xfffff000);
		cpu->regs[rd] = cpu->pc + imm + 4;
		break;
	case 0x1b:
		imm = (uint64_t)((int64_t)(int32_t)inst >> 20);
		shamt = (uint32_t)(imm & 0x1f);

		switch (func3) {
		case 0x0: /* addiw. */
			cpu->regs[rd] = (uint64_t)(int64_t)(int32_t)(
				cpu->regs[rs1] + imm
			);
			break;
		case 0x1: /* slliw. */
			cpu->regs[rd] = (uint64_t)(int64_t)(int32_t)(
				cpu->regs[rs1] << shamt
			);
			break;
		case 0x5:
			switch (func7) {
			case 0x0: /* srliw. */
				cpu->regs[rd] = (uint64_t)(int64_t)(int32_t)(
					(uint32_t)cpu->regs[rs1] >> shamt
				);
				break;
			case 0x20: /* sraiw. */
				cpu->regs[rd] = (uint64_t)(int64_t)(
					(int32_t)cpu->regs[rs1] >> shamt
				);
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		default:
			errx(1, "instruction 0x%x not implemented yet", opcode);
		}
		break;
	case 0x23:
		imm = (uint64_t)((int64_t)(int32_t)(inst & 0xfe000000) >> 20)
		    | ((inst >> 7) & 0x1f);
		addr = cpu->regs[rs1] + imm;

		switch (func3) {
		case 0x0: /* sb. */
			bus_store(&cpu->bus, addr,  8, cpu->regs[rs2]);
			break;
		case 0x1: /* sh. */
			bus_store(&cpu->bus, addr, 16, cpu->regs[rs2]);
			break;
		case 0x2: /* sw. */
			bus_store(&cpu->bus, addr, 32, cpu->regs[rs2]);
			break;
		case 0x3: /* sd. */
			bus_store(&cpu->bus, addr, 64, cpu->regs[rs2]);
			break;
		default:
			errx(1, "instruction 0x%x not implemented yet", opcode);
		}
		break;
	case 0x33:
		shamt = (uint32_t)(uint64_t)(cpu->regs[rs2] & 0x3f);

		switch (func3) {
		case 0x0:
			switch (func7) {
			case 0x0:  /* add. */
				cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
				break;
			case 0x1:  /* mul. */
				cpu->regs[rd] = cpu->regs[rs1] * cpu->regs[rs2];
				break;
			case 0x20: /* sub. */
				cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x1:
			switch (func7) {
			case 0x0:  /* sll. */
				cpu->regs[rd] = cpu->regs[rs1] << shamt;
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x2:
			switch (func7) {
			case 0x0:  /* slt. */
				cpu->regs[rd] = (
					  (int64_t)cpu->regs[rs1]
					< (int64_t)cpu->regs[rs2]
				) ? 1 : 0;
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x3:
			switch (func7) {
			case 0x0:  /* sltu. */
				cpu->regs[rd] = (
					cpu->regs[rs1] < cpu->regs[rs2]
				) ? 1 : 0;
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x4:
			switch (func7) {
			case 0x0:  /* xor. */
				cpu->regs[rd] = cpu->regs[rs1] ^ cpu->regs[rs2];
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x5:
			switch (func7) {
			case 0x0:  /* srl. */
				cpu->regs[rd] =
					cpu->regs[rs1] << cpu->regs[rs2];
				break;
			case 0x20:  /* sra. */
				cpu->regs[rd] = (uint64_t)(
					(int64_t)cpu->regs[rs1] >> shamt
				);
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x6:
			switch (func7) {
			case 0x0:  /* or. */
				cpu->regs[rd] = cpu->regs[rs1] | cpu->regs[rs2];
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x7:
			switch (func7) {
			case 0x0:  /* and. */
				cpu->regs[rd] = cpu->regs[rs1] & cpu->regs[rs2];
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		default:
			errx(1, "instruction 0x%x not implemented yet", opcode);
		}
		break;
	case 0x37: /* lui. */
		cpu->regs[rd] = (uint64_t)(int64_t)(int32_t)(inst & 0xfffff000);
		break;
	case 0x3b:
		shamt = (uint32_t)(cpu->regs[rs2] & 0x1f);

		switch (func3) {
		case 0x0:
			switch (func7) {
			case 0x0: /* addw. */
				cpu->regs[rd] = (uint64_t)(int64_t)(int32_t)(
					cpu->regs[rs1] + cpu->regs[rs2]
				);
				break;
			case 0x20: /* subw. */
				cpu->regs[rd] = (uint64_t)(int32_t)(
					cpu->regs[rs1] - cpu->regs[rs2]
				);
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x1:
			switch (func7) {
			case 0x0: /* sllw. */
				cpu->regs[rd] = (uint64_t)(int32_t)(
					(uint32_t)cpu->regs[rs1] << shamt
				);
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		case 0x5:
			switch (func7) {
			case 0x0: /* srlw. */
				cpu->regs[rd] = (uint64_t)(int32_t)(
					(uint32_t)cpu->regs[rs1]
					>> shamt
				);
				break;
			case 0x20: /* sraw. */
				cpu->regs[rd] = (uint64_t)(
					   (int32_t)cpu->regs[rs1]
					>> (int32_t)shamt
				);
				break;
			default:
				errx(
					1,
					"instruction 0x%x not implemented yet",
					opcode
				);
			}
			break;
		}
		break;
	case 0x63:
		imm = (uint64_t)((int64_t)(int32_t)(inst & 0x80000000) >> 19)
		    | ((inst & 0x80) << 4)
		    | ((inst >> 20) & 0x7e0)
		    | ((inst >> 7) & 0x1e);

		switch (func3) {
		case 0x0: /* beq. */
			if (cpu->regs[rs1] == cpu->regs[rs2])
				cpu->pc += imm - 4;
			break;
		case 0x1: /* bne. */
			if (cpu->regs[rs1] != cpu->regs[rs2])
				cpu->pc += imm - 4;
			break;
		case 0x4: /* blt. */
			if ((int64_t)cpu->regs[rs1] < (int64_t)cpu->regs[rs2])
				cpu->pc += imm - 4;
			break;
		case 0x5: /* bge. */
			if ((int64_t)cpu->regs[rs1] >= (int64_t)cpu->regs[rs2])
				cpu->pc += imm - 4;
			break;
		case 0x6: /* bltu. */
			if (cpu->regs[rs1] < cpu->regs[rs2])
				cpu->pc += imm - 4;
			break;
		case 0x7: /* bgeu. */
			if (cpu->regs[rs1] >= cpu->regs[rs2])
				cpu->pc += imm - 4;
			break;
		default:
			errx(1, "instruction 0x%x not implemented yet", opcode);
		}
		break;
	case 0x67: /* jalr. */
		imm = (uint64_t)((int64_t)(int32_t)(inst & 0xfff00000) >> 20);
		cpu->regs[rd] = cpu->pc;
		cpu->pc = (cpu->regs[rs1] + imm) & !1;
		break;
	case 0x6f: /* jal. */
		cpu->regs[rd] = cpu->pc;
		imm = (uint64_t)((int64_t)(int32_t)(inst & 0x80000000) >> 11)
		    | (inst & 0xff000)
		    | ((inst >> 9) & 0x800)
		    | ((inst >> 20) & 0x7fe);
		cpu->pc += imm - 4;
		break;
	default:
		errx(1, "instruction 0x%x not implemented yet", opcode);
	}
	return 0;
}
