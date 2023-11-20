/*
 * RISC-V Instruction Set Simulator
 * <p>
 * A tiny first step to get the simulator started. Can execute just a single
 * RISC-V instruction.
 *
 * To run this program:
 *      mkdir build
 *      cd build
 *      cmake ..
 *      make
 *      ./isasim
 *
 * @author Martin Schoeberl (martin@jopdesign.com), Andrew Dobis
 * (andrew.dobis@alumni.epfl.ch)
 * For at køre program med fil
 * ./isasim "/mht/c/Users/.../test.bin"
 *
 */

/*****************************************************************************************/

// Header files

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Defintions
#define NO_ERR 0
#define ERR 1

// Function prototypes

void ItypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg);
void RtypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, int32_t *reg);

/*****************************************************************************************/

int main(int argc, char *argv[]) {
	uint32_t *progr;
	uint32_t num_instructions;
	uint32_t pc = 0;
	int32_t reg[32] = {0};

	//                     Reading binary file into program array

	/***************************************************************************************/
	// Declare file pointer
	FILE *file;

	// Checks if input file is given
	if (argc != 2) {
		printf("in file input error: no input file\n");
		exit(ERR);
	}

	// Open file for reading
	file = fopen(argv[1], "r");

	// Error handling if fopen() returns NULL pointer
	if (file == NULL) {
		fprintf(stderr, "in file opening: couldn't open %s for reading\n", argv[1]);
		exit(ERR);
	}

	// Determines the length of the file
	fseek(file, 0, SEEK_END); // moves pointer to end of file
	uint32_t file_size = ftell(file);
	fseek(file, 0, SEEK_SET); // moves pointer to beginning of file

	// Assuming each instruction is 4 bytes (32 bits)
	num_instructions = file_size / sizeof(uint32_t);

	// Allocating memory for the program array
	progr = (uint32_t *)malloc(num_instructions * sizeof(uint32_t));

	// Error handling if memory allocation fails
	if (progr == NULL) {
		fprintf(stderr, "error in memory allocation\n");
		fclose(file);
		exit(ERR);
	}

	// Reading the binary file into the program array
	fread(progr, sizeof(uint32_t), num_instructions, file);

	// Closing file
	fclose(file);

	/***************************************************************************************/
	while (1) {
		// Implement ecall exit

		// Breaks out of while(1)-loop if end of instructions is met
		if ((pc >> 2) >= num_instructions) {
			break;
		}

		uint32_t instr = progr[pc >> 2];
		uint32_t opcode = instr & 0x7f;
		uint32_t rd = (instr >> 7) & 0x01f;
		uint32_t rs1 = (instr >> 15) & 0x01f;
		uint32_t rs2 = (instr >> 20) & 0x01f;
		uint32_t funct3 = (instr >> 12) & 0x007;
		uint32_t funct7 = (instr >> 25) & 0x07f;

		/************************************************************************************/
		//                     Immediate extraction based on opcode

		int32_t imm;

		switch (opcode) {

		// U-type instruction
		case 0x17: // auipc
		case 0x37: // lui
			// No need to handle sign-extension since only upper 20 bits are used
			imm = instr >> 12;
			break;

		// I-type instruction
		default:
			// Handle sign extension if needed for 12-bit immediate
			imm = instr >> 20;
			if (imm & 0x800) { // If MSB = 1 (negative integer)
				imm |= 0xFFFFF000;
			}
			break;
		}
		/***********************************************************************************/

		switch (opcode) {
		// I-type instructions
		case 0x13:
			ItypeSwitch(funct3, funct7, rd, rs1, imm, reg);
			break;

		// R-type instructions
		case 0x33:
			RtypeSwitch(funct3, funct7, rd, rs1, rs2, reg);
			break;

		// lui instruction
		case 0x37:
			reg[rd] = imm << 12;
			break;

		// auipc instruction
		case 0x17:

			break;
		default:
			printf("Opcode %u not yet implemented\n", opcode);
			break;
		}

		pc += 4;

		for (size_t i = 0; i < sizeof(reg) / sizeof(reg[0]); ++i) {
			printf("%d ", reg[i]);
		}
		printf("\n");
	}

	// TODO: Register dump

	printf("Program exit\n");

	// Free program
	free(progr);

	return NO_ERR;
}

/* example function to handle both rs2 and imm
int add(int isImm){
		uint32_t a = rs1;
		uint32_t b = rs2;
		if (isImm == true){
				b = imm;
		}

		rd = a+b;
}
*/

void RtypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, int32_t *reg) {
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
		//Make signed variables
		int32_t reg1 = reg[rs1]
		int32_t reg2 = reg[rs2]
		//Perform comparison
		if (reg1 < reg2) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	case 0x03: // set less than unsigned
		if (reg[rs1] < reg[rs2]) {
			reg[rd] = 1;
		} else {
			reg[rd] = 0;
		}
		break;

	case 0x04:
		reg[rd] = reg[rs1] ^ reg[rs2];
		break;

	case 0x05:
		if (funct7 != 0) {
			if (reg[rs1] > 0) {
				reg[rd] = (int32_t)((uint32_t)(reg[rs1] >> reg[rs2]));
			} else {
				reg[rd] = reg[rs1] >> reg[rs2];
			}

		} else {
			reg[rd] = (int32_t)((uint32_t)(reg[rs1] >> reg[rs2]));
		}
		break;

	case 0x06:
		// or HAS IMM
		reg[rd] = reg[rs1] | reg[rs2];
		break;

	case 0x07:
		// and HAS IMM
		reg[rd] = reg[rs1] & reg[rs2];
		break;

	default:
		printf("in RtypeSwitch funct3 error: case not defined");
		break;
	}
}

void ItypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg) {
	switch (funct3) {
	// addi and ecall
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
			reg[rd] = (int32_t)((uint32_t)(reg[rs1] >> imm));
			break;
		// srai
		case 0x20:
			if (reg[rs1] > 0) {
				reg[rd] = (int32_t)((uint32_t)(reg[rs1] >> imm));
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
