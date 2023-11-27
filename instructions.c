#include "instructions.h"
#include "globals.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// R-type instructions
void RTypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, int32_t *reg) {
	switch (funct3) {
	case 0x00:
		if (funct7 != 0) {
			// rs1 - rs2
			reg[rd] = reg[rs1] - reg[rs2];
		} else {
			// add HAS IMM
			reg[rd] = reg[rs1] + reg[rs2];
		}
		break;
	case 0x01:
		// sll HAS IMM
		reg[rd] = reg[rs1] << reg[rs2];
		break;

	case 0x02: // set less than
		// Perform comparison
		if (reg[rs1] < reg[rs2]) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	case 0x03: // set less than unsigned
		if ((uint32_t)reg[rs1] < (uint32_t)reg[rs2]) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	case 0x04: // XOR
		reg[rd] = reg[rs1] ^ reg[rs2];
		break;

	case 0x05:
		if (funct7 != 0) {
			// sra
			if (reg[rs1] > 0) {
				reg[rd] = (int32_t)(((uint32_t)reg[rs1] >> (uint32_t)reg[rs2]));
			} else {
				reg[rd] = reg[rs1] >> reg[rs2];
			}

		} else {
			// srl
			reg[rd] = ((uint32_t)reg[rs1] >> (uint32_t)reg[rs2]);
		}
		break;

	case 0x06: // OR
		reg[rd] = reg[rs1] | reg[rs2];
		break;

	case 0x07: // AND
		reg[rd] = reg[rs1] & reg[rs2];
		break;

	default:
		printf("in RtypeSwitch funct3 error: case not defined");
		break;
	}
}

// I-type instructions

void ITypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg) {
	switch (funct3) {
	// addi
	case 0x0:
		reg[rd] = reg[rs1] + imm;
		break;

	// slli
	case 0x1:
		reg[rd] = reg[rs1] << imm;
		break;

	// slti
	case 0x2:
		// Handle sign extension if needed for the 32-bit imm
		imm = (imm << 12) >> 12;
		if (imm & 0x80000000) { // If MSB = 1 (negative integer)
			imm |= 0xFFF00000;
		}
		if (reg[rs1] < imm) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	// sltiu
	case 0x3:
		// Handle sign extension if needed for the 32-bit imm
		imm = (imm << 12) >> 12;
		if (imm & 0x80000000) { // If MSB = 1 (negative integer)
			imm |= 0xFFF00000;
		}
		if ((uint32_t)reg[rs1] < (uint32_t)imm) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	// xori
	case 0x4:
		reg[rd] = reg[rs1] ^ imm;
		break;

	// srli and srai
	case 0x5:
		switch (funct7) {
		// srli
		case 0x0:
			reg[rd] = (int32_t)((uint32_t)reg[rs1] >> imm);
			break;
		// srai
		case 0x20:
			if (reg[rs1] > 0) {
				reg[rd] = (int32_t)(((uint32_t)reg[rs1] >> imm));
			} else {
				reg[rd] = reg[rs1] >> imm;
			}
			break;
		}
		break;

	// ori
	case 0x6:
		reg[rd] = reg[rs1] | imm;
		break;

	// andi
	case 0x7:
		reg[rd] = reg[rs1] & imm;
		break;

	default:
		printf("in ItypeSwitch error: case not defined");
	}
}

// SB-type instructions format:(sx rs2, imm(rs1))
void SBTypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc) {

	switch (funct3) {
	// beq
	case 0x0:
		if (reg[rs1] == reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	// bne
	case 0x1:
		if (reg[rs1] != reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	// blt
	case 0x4:
		if (reg[rs1] < reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	// bge
	case 0x5:
		if (reg[rs1] >= reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	// bltu
	case 0x6:
		if ((uint32_t)reg[rs1] < (uint32_t)reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	// bgeu
	case 0x7:
		if ((uint32_t)reg[rs1] >= (uint32_t)reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	default:
		printf("in SBTypeSwitch error: case not defined");
	}
}

void STypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc,
		 int8_t *mem_base) {
	int8_t *store_at = mem_base + (reg[rs1] + imm);
	int8_t *store_at1 = mem_base + (reg[rs1] + imm + 1);
	int8_t *store_at2 = mem_base + (reg[rs1] + imm + 2);
	int8_t *store_at3 = mem_base + (reg[rs1] + imm + 3);

	switch (funct3) {

	// sb
	case 0x0:
		printf("hej fra sb \n");
		uint8_t to_store_raw8 = reg[rs2] & 0x000000FF; // isolate lower 8 bits
		printf("to_store_raw = %x \n", to_store_raw8);
		int8_t to_store8 = (int8_t)to_store_raw8; // typecast to 8bit value

		*store_at = to_store8;
		printf("to_store8 = %d in store_at = %p \n", to_store8, store_at);
		break;

	// sh
	case 0x01:
		printf("hej fra sh \n");

		uint16_t to_store_raw16 = reg[rs2] & 0x0000FFFF; // isolate lower 16 bits
		printf("to_store_raw16 = %x \n", to_store_raw16);
		*store_at = (uint8_t)(to_store_raw16 & 0x00FF);
		*store_at1 = (uint8_t)(to_store_raw16 >> 8);

		//	CHECKS
		// uint16_t storedval_lo = *store_at;
		// uint16_t storedval_hi = *store_at1;
		// printf("stored %x at %p and -  %x at %p ", storedval_lo, store_at, storedval_hi, store_at2);

		break;

	// sw
	case 0x02:
		// printf("hej fra sw \n");

		// printf("to_store_raw16 = %x \n", to_store_raw16);
		*store_at = (uint8_t)(reg[rs2] & 0x000000FF);
		*store_at1 = (uint8_t)((reg[rs2] >> 8) & 0x000000FF);
		*store_at2 = (uint8_t)((reg[rs2] >> 16) & 0x000000FF);
		*store_at3 = (uint8_t)((reg[rs2] >> 24) & 0x000000FF);

		// CHECKS
		// uint16_t storedval_0 = *store_at;
		// uint16_t storedval_1 = *store_at1;
		// uint16_t storedval_2 = *store_at2;
		// uint16_t storedval_3 = *store_at3;
		// printf("stored: \n %x at %p \n %x at %p \n %x at %p \n %x at %p ", storedval_0, store_at,
		// storedval_1, store_at1, storedval_2, store_at2, storedval_3, store_at3);
		break;

	default:
		printf("in STypeSwitch (funct3) error: case %d not defined", funct3);
		break;
	}
}

void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg,
		     int8_t *mem_base) {
	printf("Immediate: %x", imm);
	int8_t *load_at = mem_base + (reg[rs1] + imm);
	int8_t *load_at1 = mem_base + (reg[rs1] + imm + 1);
	int8_t *load_at2 = mem_base + (reg[rs1] + imm + 2);
	int8_t *load_at3 = mem_base + (reg[rs1] + imm + 3);
	printf("Register data: %x\n", reg[rs1]);
	switch (funct3) {
	// lb
	case 0x0:
		printf("load_at data: %x\n", *load_at);
		uint8_t to_load_raw8 = (uint8_t)*load_at;
		int32_t to_load_lb = to_load_raw8; // load unsigned

		if (to_load_raw8 >> 7) { // sign extend if needed
			to_load_lb = to_load_raw8 | 0xFFFFFF00;
		}
		reg[rd] = to_load_lb;

		break;

	// lh
	case 0x01:
		// printf("hello from lh \n");
		uint8_t to_load8_lo = *load_at;
		uint8_t to_load8_hi = *load_at1;

		// printf("to load LO: %x HI: %x \n", to_load8_lo, to_load8_hi);

		uint16_t to_load16 = ((to_load8_hi << 8) | (to_load8_lo));

		// printf("loaded tl16 with %x \n", to_load16);

		uint32_t to_load32lh = to_load16; // load unsigned
		// printf("loaded tl32 with %x \n", to_load32);

		if ((to_load16 >> 15) == 1) { // sign extend if needed
			to_load32lh = to_load32lh | 0xFFFF0000;
		}
		reg[rd] = to_load32lh;
		break;

	// lw
	case 0x02:
		// printf("hello from lw \n");
		uint8_t to_load8_0 = *load_at;
		uint8_t to_load8_1 = *load_at1;
		uint8_t to_load8_2 = *load_at2;
		uint8_t to_load8_3 = *load_at3;

		uint32_t to_load32lw = ((to_load8_3 << 24) | (to_load8_2 << 16) | (to_load8_1 << 8) | (to_load8_0));
		reg[rd] = to_load32lw;
		break;

	// lbu
	case 0x04:
		uint32_t to_load_lbu = (uint8_t)*load_at;
		reg[rd] = to_load_lbu;
		break;

	// lhu
	case 0x05:
		// printf("hello from lhu \n");
		uint8_t to_load8_lolhu = *load_at;
		uint8_t to_load8_hilhu = *load_at1;

		// printf("to load LO: %x HI: %x \n", to_load8_lo, to_load8_hi);

		uint16_t to_load16lhu = ((to_load8_hilhu << 8) | (to_load8_lolhu));

		// printf("loaded tl16 with %x \n", to_load16);
		uint32_t to_load32lhu = to_load16lhu; // load unsigned
		// printf("loaded tl32 with %x \n", to_load32);

		reg[rd] = to_load32lhu;

		break;

	default:
		printf("in ITypeLoadSwitch (funct3) error: case %d not defined", funct3);
		break;
	}
}

// ecall instructions
void EcallSwitch(int32_t *reg, uint32_t *progr, int8_t *mem_base) {
	// exit
	if (reg[17] == 10) {
		printf("Program exit\n");
		free(progr);
		free(mem_base);
		exit(NO_ERR);
		// integer print
	} else {
		// Handle other ecall services
		printf("Unsupported ecall service\n");
		free(progr);
		free(mem_base);
		exit(ERR);
	}
}