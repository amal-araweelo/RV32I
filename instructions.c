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

void STypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc, int8_t *mem_base) {
	//Create variables to represent memory locations
	int8_t *store_at = mem_base + (reg[rs1] + imm);
	int8_t *store_at1 = mem_base + (reg[rs1] + imm + 1);
	int8_t *store_at2 = mem_base + (reg[rs1] + imm + 2);
	int8_t *store_at3 = mem_base + (reg[rs1] + imm + 3);

	switch (funct3) {

	// SB: Store Byte
	case 0x0:
		uint8_t to_store_raw8 = reg[rs2] & 0x000000FF; 	// isolate lower 8 bits
		*store_at = (int8_t)to_store_raw8; 				// typecast to 8bit value, and place in memory
		break;

	// SH: Store Half
	case 0x01:
		uint16_t to_store_raw16 = reg[rs2] & 0x0000FFFF; 	// isolate lower 16 bits
		*store_at = (uint8_t)(to_store_raw16 & 0x00FF);		// typecast lower 8 bits and store
		*store_at1 = (uint8_t)(to_store_raw16 >> 8);		// isolate upper 8 bits and store
		break;

	// SW: Store Word
	case 0x02:
		*store_at = (uint8_t)(reg[rs2] & 0x000000FF);			// isolate each byte, typecast and store
		*store_at1 = (uint8_t)((reg[rs2] >> 8) & 0x000000FF);
		*store_at2 = (uint8_t)((reg[rs2] >> 16) & 0x000000FF);
		*store_at3 = (uint8_t)((reg[rs2] >> 24) & 0x000000FF);
		break;

	//Error handling
	default:
		printf("in STypeSwitch (funct3) error: case %d not defined", funct3);
		break;
	}
}

void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg, int8_t *mem_base) {
	//Create variables to represent memory locations
	int8_t *load_at = mem_base + (reg[rs1] + imm);
	int8_t *load_at1 = mem_base + (reg[rs1] + imm + 1);
	int8_t *load_at2 = mem_base + (reg[rs1] + imm + 2);
	int8_t *load_at3 = mem_base + (reg[rs1] + imm + 3);

	switch (funct3) {
	// LB: Load Byte
	case 0x0:
		int32_t to_load_lb = (uint8_t)*load_at;		// load byte into signed var
		if (to_load_lb >> 7) { 						// sign extend if needed
			to_load_lb = to_load_lb | 0xFFFFFF00;
		}
		reg[rd] = to_load_lb;						// place value in register
		break;

	// LH: Load Half
	case 0x01:
		int32_t to_load_lh = ((*load_at1 << 8) 		//load bytes into signed var
							 |(*load_at)); 			//	by shifting upper 8 bits and or'ing together
		if ((to_load_lh >> 15) == 1) { 				// 	sign extend if needed
			to_load_lh = to_load_lh | 0xFFFF0000;
		}
		reg[rd] = to_load_lh;						// place value in register
		break;

	// LW: Load Word
	case 0x02:
		uint32_t to_load32lw = ((*load_at3 << 24) 	// load bytes into var
							   | (*load_at2<< 16) 	// 	by shifting where needed
							   | (*load_at1 << 8) 	//	and or'ing together
							   | (*load_at));
		reg[rd] = to_load32lw;						// place value in register
		break;

	// LBU: Load Byte Unsigned
	case 0x04:
		reg[rd] = (uint8_t)*load_at;	// load byte into register
		break;

	// LHU: Load Half Unsigned
	case 0x05:

		uint32_t to_load_lhu = ((*load_at1 << 8) // load bytes into unsigned var
								|(*load_at));	 
		reg[rd] = to_load_lhu;					 // load value into register
		break;

	// Error handling
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