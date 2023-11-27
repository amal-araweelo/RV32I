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

// OBS: VI MANGLER JAL, JALR, FUNCTIONS
// TEST om store og load virker
/*****************************************************************************************/

// Header files
#include "globals.h"
#include "instructions.h"
#include "opcodes.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Defintions

#define NO_ERR 0
#define ERR 1

// Function prototypes

/*****************************************************************************************/

int main(int argc, char *argv[]) {
	uint32_t *progr = NULL;
	uint32_t num_instructions;
	int32_t pc = 0;
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

	// Check if there are any instructions in the program
	if (num_instructions == 0) {
		printf("Error: Empty program\n");
		free(progr);
		exit(ERR);
	}

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

	int8_t *mem_base = malloc(0x800000); // mem base ptr for saving and loading
	reg[2] = 0x800000;
	printf("MEM: %p \n", mem_base);

	for (int i = 0; i < num_instructions; ++i) {
		mem_base[i * 4] = (progr[i] >> 24) & 0xFF;
		mem_base[i * 4 + 1] = (progr[i] >> 16) & 0xFF;
		mem_base[i * 4 + 2] = (progr[i] >> 8) & 0xFF;
		mem_base[i * 4 + 3] = (progr[i]) & 0xFF;
	}

	/***************************************************************************************/
	while (1) {

		// Intializing stack pointer to 1 MiB (mebibyte)
		printf("PC = %x \n", pc);
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
		printf("instr: %x \n", instr);
		printf("opcode: %x \n", opcode);
		switch (opcode) {

			// U-type instruction
		case AUIPC: // auipc
			imm = (instr >> 12) << 12;
			printf("in AUIPC \n");
			break;

		case LUI: // lui
			// No need to handle sign-extension since only upper 20 bits are used
			imm = instr >> 12;
			break;

		// SB-type instructions
		case SB_TYPE:
			imm = (((instr >> 31) & 0x1) << 12) | // imm[12]
			      (((instr >> 7) & 0x1) << 11) |  // imm[11]
			      (((instr >> 25) & 0x3F) << 5) | // imm[10:5]
			      (((instr >> 8) & 0xF) << 1);    // imm[4:1]

			// Handle sign extension if needed for 13-bit immediate
			if (imm & 0x1000) { // If MSB = 1 (negative integer)
				imm |= 0xFFFFE000;
			}
			break;

			// UJ-type instructions
		case JAL:

			imm = (((instr >> 31) & 0x1) << 20) |  // imm[20]
			      (((instr >> 12) & 0xFF) << 12) | // imm[19:12]
			      (((instr >> 20) & 0x1) << 11) |  // imm[11]
			      (((instr >> 21) & 0x3FF) << 1);  // imm[10:1]

			// Handle sign extension if needed for 20-bit immediate
			if (imm & 0x100000) { // If MSB = 1 (negative integer)
				imm |= 0xFFE00000;
			}
			printf("JAL imm made: %x \n", imm);
			break;
		// S-type instructions
		case 0x23:
			imm = (((instr >> 7) & 0x1F) |	 // imm[4:0]
			       ((instr >> 20) & 0xFE0)); // imm[11:5]

			// Handle sign extension if needed for 21-bit immediate
			if (imm & 0x800) { // If MSB = 1 (negative integer)
				imm |= 0xFFFFF000;
			}
			break;

		// I-type instructions
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

			/*******************************************************************************/
			//                           UJ-type instruction
		case JAL:
			printf("hello from JAL\n");
			printf("JAL received imm: %x \n", imm);
			reg[rd] = pc + 4;
			pc += imm - 4; // Decrement pc by 4, since we increment later
			break;

			/*******************************************************************************/
			//                           I-type instructions

		case 0x03: // Loads
			ITypeLoadSwitch(funct3, funct7, rd, rs1, imm, reg, mem_base);
			break;

		case JALR: // jalr
			reg[rd] = pc + 4;
			pc = reg[rs1] + imm - 4;
			break;

		case ECALL: // ecall
			EcallSwitch(reg, progr, mem_base);
			break;
		case I_TYPE:
			ITypeSwitch(funct3, funct7, rd, rs1, imm, reg);
			break;
			/*******************************************************************************/
			//                             R-type instructions

		case R_TYPE:
			RTypeSwitch(funct3, funct7, rd, rs1, rs2, reg);
			break;

			/*******************************************************************************/
			//                             U-type instructions

		case LUI: // lui instruction
			reg[rd] = imm << 12;
			break;

		case AUIPC: // auipc instruction
			printf("in AUIPC \n");
			reg[rd] = pc + imm;
			printf("reg[rd]: %d \n", reg[rd]);
			break;

			/*******************************************************************************/
			//                            SB-type instructions

		case SB_TYPE:
			SBTypeSwitch(funct3, rs1, rs2, imm, reg, &pc);
			break;

			/*******************************************************************************/
			//                            S-type instruction
		case 0x23:
			STypeSwitch(funct3, rs1, rs2, imm, reg, &pc, mem_base);
			break;
		default:
			printf("Opcode %u not yet implemented\n", opcode);
			break;
			/*******************************************************************************/
		}
		// If branch was not taken PC is incremented
		if (!branch_taken) {
			pc += 4;
		}
		branch_taken = 0;

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

		reg[0] = 0;
	}
	printf("Program exit\n");

	// Free program and allocated memory
	free(progr);
	free(mem_base);

	return NO_ERR;
}

//