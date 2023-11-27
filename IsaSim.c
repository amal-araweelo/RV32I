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

void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg, int32_t *mem_base);
void STypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc, int32_t *mem_base);


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


	int8_t *mem_base = malloc(1048576); // mem base ptr for saving and loading
	reg[2] = 1048576;

	/***************************************************************************************/
	while (1) {
		// Intializing stack pointer to 1 MiB (mebibyte)

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
		case AUIPC: // auipc
		case LUI:   // lui
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
			//                           I-type instructions

		case 0x03: // Loads
			ITypeLoadSwitch(funct3, funct7, rd, rs1, imm, reg, mem_base);
			break;
		
		case JALR: // jalr
			reg[rd] = pc + 4;
			pc = reg[rs1] + imm;
			break;

		case ECALL: // ecall
			EcallSwitch(reg, progr);
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
			reg[rd] = pc + (imm << 12);
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
	}
	printf("Program exit\n");

	// Free program and allocated memory
	free(progr);
	free(mem_base);

	return NO_ERR;
}

void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg, int32_t* mem_base){
	uint32_t* load_at = mem_base+(reg[rs1]+imm);
	switch(funct3){
		//lb
		case 0x0:
			uint8_t to_load_raw = (uint8_t)*load_at;
			uint32_t to_load;
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
				to_load = to_load_raw | 0x11111100;
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


void STypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc, int32_t *mem_base){
	switch(funct3){
		//sb
		case 0x0:
			uint32_t to_store_raw = reg[rs2] & 0x00000011; //isolate lower 8 bits
			uint8_t to_store = (uint8_t)to_store_raw; //typecast to 8bit value
			uint32_t* store_at = mem_base+(reg[rs1]+imm);
			
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
			uint32_t* store_at = mem_base+(reg[rs1]+imm);
			*store_at = reg[rs2];
			break;

		default:
		printf("in STypeSwitch (funct3) error: case %d not defined", funct3);
		break;

	}
}

// 