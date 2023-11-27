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

// SB-type instructions
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

void STypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc, int32_t *mem_base){
	switch(funct3){
		uint32_t* store_at = mem_base+(reg[rs1]+imm);
		//sb
		case 0x0:
			uint32_t to_store_raw = reg[rs2] & 0x00000011; //isolate lower 8 bits
			int8_t to_store = (int8_t)to_store_raw; //typecast to 8bit value
			
			
			/* LEGACY CODE
			uint32_t byte_offset = reg[rs1]+imm % 4;
			uint32_t store_mask;	
			
			// calculate where in word to place byte
			if (byte_offset != 0){
				if (byte_offset == 1){
					store_mask = 0x11110011;
					to_store = to_store << 8;
				}
				else if (byte_offset == 2){
					store_mask = 0x11001111;
					to_store = to_store << 16;
				}
				else{
					store_mask = 0x00111111;
					to_store = to_store << 24;
				}
			}
			else {
				store_mask = 0x11111100;
			}

			// store byte
			*store_at = *store_at & store_mask; //Clear the section in which to store byte
			*store_at = *store_at & to_store;	//Store the byte, preserving the rest of the word */
			*store_at = to_store;
			break;

		//sw rs2, imm(rs1)
		case 0x02:
			*store_at = reg[rs2];
			break;

		default:
		printf("in STypeSwitch (funct3) error: case %d not defined", funct3);
		break;

	}
}

void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg, int32_t* mem_base){
	uint32_t* load_at = mem_base+(reg[rs1]+imm);
	switch(funct3){
		//lb
		case 0x0:
			uint8_t to_load_raw = (uint8_t)*load_at;
			int32_t to_load;
			/* LEGACY CODE
			uint32_t byte_offset = (reg[rs1]+imm) % 4;
			uint32_t load_mask;
			//Calculate which byte to load
			if (byte_offset != 0){
				if (byte_offset == 1){
					load_mask = 0x11110011;
				}
				else if (byte_offset == 2){
					load_mask = 0x11001111;
				}
				else{
					load_mask = 0x00111111;
				}
			}
			else {
				load_mask = 0x11111100;
			}	

			to_load = (*load_at & load_mask) >> byte_offset;

			if ((to_load >> 7) == 1){
			*/
			if ((to_load_raw >> 7) == 1){				//sign extend
				to_load = to_load_raw | 0xFFFFFF00;
			}
			reg[rd] = to_load;
			
			break;
			

		//lw
		case 0x02:
			reg[rd] = *load_at;
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