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
		uint16_t to_store_raw16 = reg[rs2] & 0x0000FFFF;	       // isolate lower 16 bits
		uint8_t to_store16low = (uint8_t)((to_store_raw16 << 8) >> 8); // isolate lower 8 bits
		uint8_t to_store16high = (uint8_t)(to_store_raw16 >> 8);       // isolate upper 8 bits

		*store_at = to_store16low;
		uint32_t storedval = *store_at;
		uint8_t *store_high = store_at++;
		*store_high = to_store16high;
		printf("LOW : to_store16 = %x in store_at = %p \n", to_store16low, store_at);
		printf("HIGH: to_store16 = %x in store_at = %p \n", to_store16high, store_high);
		break;

	// sw
	case 0x02:
		*store_at = reg[rs2];
		break;

	default:
		printf("in STypeSwitch (funct3) error: case %d not defined", funct3);
		break;
	}
}

void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg,
		     int8_t *mem_base) {
	int8_t *load_at = mem_base + (reg[rs1] + imm);

	switch (funct3) {
	// lb
	case 0x0:
		uint8_t to_load_raw8 = (uint8_t)*load_at;
		int32_t to_load_lb = to_load_raw8; // load unsigned

		if ((to_load_raw8 >> 7) == 1) { // sign extend if needed
			to_load_lb = to_load_raw8 | 0xFFFFFF00;
		}
		reg[rd] = to_load_lb;

		break;

	// lh
	case 0x01:
		// uint32_t valtoload = *load_at;
		uint16_t to_load_raw16 = (uint16_t)*load_at;
		// uint32_t to_load_lh = to_load_raw16; // load unsigned
		// min tilføjelse
		uint8_t to_load_8high = *(load_at);
		printf("HIGH: address: %p \n", load_at);
		printf("LOW: address: %p \n", load_at);
		uint8_t to_load_8low = *(++load_at);
		printf("LOAD LOW: %x\n from %p \n", to_load_8low, load_at);
		printf("LOAD HIGH: %x from %p \n", to_load_8high, load_at--);
		uint16_t to_load_lh = (((uint16_t)to_load_8low)) & (((uint16_t)to_load_8high));
		printf("HHHH: to_load_lh %x from %p \n", to_load_lh, load_at);

		if ((to_load_raw16 >> 15) == 1) { // sign extend if needed
			to_load_lh = to_load_raw16 | 0xFFFF0000;
		}
		reg[rd] = to_load_lh;
		break;

	// lw
	case 0x02:
		reg[rd] = *load_at;
		break;

	// lbu
	case 0x04:
		uint32_t to_load_lbu = (uint8_t)*load_at;
		reg[rd] = to_load_lbu;
		break;

	// lhu
	case 0x05:
		uint32_t to_load_lhu = (uint16_t)*load_at;
		reg[rd] = to_load_lhu;
		break;

	default:
		printf("in ITypeLoadSwitch (funct3) error: case %d not defined", funct3);
		break;
	}
}

// ecall instructions
void EcallSwitch(int32_t *reg, uint32_t *progr) {
	// exit
	if (reg[17] == 10) {
		printf("Program exit\n");
		free(progr);
		exit(NO_ERR);
		// integer print
	} else {
		// Handle other ecall services
		printf("Unsupported ecall service\n");
		free(progr);
		exit(ERR);
	}
}