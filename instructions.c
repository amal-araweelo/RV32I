#include "instructions.h"
#include "globals.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// R-type instructions
void RTypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, int32_t *reg) {
	switch (funct3) {
	case 0x00: // SUB and ADD
		// add
		if (funct7 != 0) {
			reg[rd] = reg[rs1] - reg[rs2];
		} else {
			reg[rd] = reg[rs1] + reg[rs2];
		}
		break;
	case 0x01:
		// SLL
		reg[rd] = reg[rs1] << reg[rs2];
		break;

	case 0x02: // SLT
		if (reg[rs1] < reg[rs2]) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	case 0x03: // SLTU
		if ((uint32_t)reg[rs1] < (uint32_t)reg[rs2]) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	case 0x04: // XOR
		reg[rd] = reg[rs1] ^ reg[rs2];
		break;

	case 0x05: // SRA
		if (funct7 != 0) {
			// sra
			if (reg[rs1] > 0) {
				reg[rd] = (int32_t)(((uint32_t)reg[rs1] >> (uint32_t)reg[rs2]));
			} else {
				reg[rd] = reg[rs1] >> reg[rs2];
			}

		} else {
			// SRL
			reg[rd] = ((uint32_t)reg[rs1] >> (uint32_t)reg[rs2]);
		}
		break;

	case 0x06: // OR
		reg[rd] = reg[rs1] | reg[rs2];
		break;

	case 0x07: // AND
		reg[rd] = reg[rs1] & reg[rs2];
		break;

	// Error handling
	default:
		printf("In ItypeSwitch error (funct3 and funct7): case not defined for funct3 = %d, "
		       "funct7 = %d",
		       funct3, funct7);
		break;
	}
}

// I-type instructions
void ITypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg) {
	switch (funct3) {
	case 0x0: // ADDI
		reg[rd] = reg[rs1] + imm;
		break;

	case 0x1: // SLLI
		reg[rd] = reg[rs1] << imm;
		break;

	case 0x2: // SLTI
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

	case 0x3: // SLTIU
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

	case 0x4: // XORI
		reg[rd] = reg[rs1] ^ imm;
		break;

	case 0x5: // SRLI and SRAI
		switch (funct7) {
		case 0x0: // SRLI
			reg[rd] = (int32_t)((uint32_t)reg[rs1] >> imm);
			break;
		case 0x20: // SRAI
			if (reg[rs1] > 0) {
				reg[rd] = (int32_t)(((uint32_t)reg[rs1] >> imm));
			} else {
				reg[rd] = reg[rs1] >> imm;
			}
			break;
		}
		break;

	case 0x6: // ORI
		reg[rd] = reg[rs1] | imm;
		break;

	case 0x7: // ANDI
		reg[rd] = reg[rs1] & imm;
		break;
	// Error handling
	default:
		printf("In ItypeSwitch error (funct3 and funct7): case not defined for funct3 = %d, "
		       "funct7 = %d",
		       funct3, funct7);
	}
}

// SB-type instructions
void SBTypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc) {

	switch (funct3) {
	case 0x0: // BEQ
		if (reg[rs1] == reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	case 0x1: // BNE
		if (reg[rs1] != reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	case 0x4: // BLT
		if (reg[rs1] < reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	case 0x5: // BGE
		if (reg[rs1] >= reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	case 0x6: // BLTU
		if ((uint32_t)reg[rs1] < (uint32_t)reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	case 0x7: // BGEU
		if ((uint32_t)reg[rs1] >= (uint32_t)reg[rs2]) {
			branch_taken = 1;
			*pc += imm;
		}
		break;

	// Error handling
	default:
		printf("In SBTypeSwitch (funct3) error: case %d not defined", funct3);
	}
}

void STypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc,
		 int8_t *mem_base) {
	// Create variables to represent memory locations
	int8_t *store_at = mem_base + (reg[rs1] + imm);
	int8_t *store_at1 = mem_base + (reg[rs1] + imm + 1);
	int8_t *store_at2 = mem_base + (reg[rs1] + imm + 2);
	int8_t *store_at3 = mem_base + (reg[rs1] + imm + 3);

	switch (funct3) {

	// SB: Store Byte
	case 0x0:
		uint8_t to_store_raw8 = reg[rs2] & 0x000000FF; // isolate lower 8 bits
		*store_at = (int8_t)to_store_raw8;	       // typecast to 8bit value, and place in memory
		break;

	// SH: Store Half
	case 0x01:
		uint16_t to_store_raw16 = reg[rs2] & 0x0000FFFF; // isolate lower 16 bits
		*store_at = (uint8_t)(to_store_raw16 & 0x00FF);	 // typecast lower 8 bits and store
		*store_at1 = (uint8_t)(to_store_raw16 >> 8);	 // isolate upper 8 bits and store
		break;

	// SW: Store Word
	case 0x02:
		*store_at = (uint8_t)(reg[rs2] & 0x000000FF); // isolate each byte, typecast and store
		*store_at1 = (uint8_t)((reg[rs2] >> 8) & 0x000000FF);
		*store_at2 = (uint8_t)((reg[rs2] >> 16) & 0x000000FF);
		*store_at3 = (uint8_t)((reg[rs2] >> 24) & 0x000000FF);
		break;

	// Error handling
	default:
		printf("In STypeSwitch (funct3) error: case %d not defined", funct3);
		break;
	}
}

void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg,
		     int8_t *mem_base) {
	// Create variables to represent memory locations
	int8_t *load_at = mem_base + (reg[rs1] + imm);
	int8_t *load_at1 = mem_base + (reg[rs1] + imm + 1);
	int8_t *load_at2 = mem_base + (reg[rs1] + imm + 2);
	int8_t *load_at3 = mem_base + (reg[rs1] + imm + 3);

	switch (funct3) {
	// LB: Load Byte
	case 0x0:
		int32_t to_load_lb = (uint8_t)*load_at; // load byte into signed var
		if (to_load_lb >> 7) {			// sign extend if needed
			to_load_lb = to_load_lb | 0xFFFFFF00;
		}
		reg[rd] = to_load_lb; // place value in register
		break;

	// LH: Load Half
	case 0x01:
		uint8_t to_load8_lo = *load_at;
		uint8_t to_load8_hi = *load_at1;
		uint16_t to_load16 = ((to_load8_hi << 8) | (to_load8_lo));
		uint32_t to_load32lh = to_load16; // load unsigned
		if ((to_load16 >> 15) == 1) {	  // sign extend if needed
			to_load32lh = to_load32lh | 0xFFFF0000;
		}
		reg[rd] = to_load32lh;
		break;

	// LW: Load Word
	case 0x02:
		uint8_t to_load8_0 = *load_at; // place bytes in vars
		uint8_t to_load8_1 = *load_at1;
		uint8_t to_load8_2 = *load_at2;
		uint8_t to_load8_3 = *load_at3;
		// Place bytes in word by or'ing together
		uint32_t to_load32lw = ((to_load8_3 << 24) | (to_load8_2 << 16) | (to_load8_1 << 8) | (to_load8_0));
		reg[rd] = to_load32lw; // Place word in reg
		break;

	// LBU: Load Byte Unsigned
	case 0x04:
		reg[rd] = (uint8_t)*load_at; // load byte into register
		break;

	// LHU: Load Half Unsigned
	case 0x05:
		uint8_t to_load8_1lhu = *load_at; // Load bytes into variables
		uint8_t to_load8_2lhu = *load_at1;
		uint16_t to_load16lhu =
		    ((to_load8_2lhu << 8) | (to_load8_1lhu)); // Place bytes in halfword by or'ing together
		uint32_t to_load32lhu = to_load16lhu;	      // load into word for placing in reg
		reg[rd] = to_load32lhu;			      // place in reg
		break;

	// Error handling
	default:
		printf("In ITypeLoadSwitch (funct3) error: case %d not defined", funct3);
		break;
	}
}

// Ecall instructions
void EcallSwitch(int32_t *reg, uint32_t *progr, int8_t *mem_base) {
	// exit
	if (reg[17] == 10) {
		RegisterDump(reg);
		printf("Program exit\n");
		free(progr);
		free(mem_base);
		exit(NO_ERR);
		// integer print
	} else {
		// Handle other ecall services
		printf("Error: unsupported ecall service\n");
		free(progr);
		free(mem_base);
		exit(ERR);
	}
}

void RegisterDump(int32_t *reg) {
	// Register dump
	uint8_t k = 0;
	for (uint8_t j = 0; j < 8; j++) {
		for (uint8_t i = 0; i < 4; ++i) {
			printf(" x%d = 0x%X", k, reg[k]);
			if (k != 31) {
				printf(", ");
			}
			k++;
		}
		printf("\n");
	}
	printf("\n");
}