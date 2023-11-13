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
 * @author Martin Schoeberl (martin@jopdesign.com), Andrew Dobis (andrew.dobis@alumni.epfl.ch)
 */
#include <stdio.h>
#include <stdint.h>

#define NO_ERR 0

//Function Prototypes
void RtypeSwitch(uint32_t func3, uint32_t func7, uint32_t rd, uint32_t rs1, uint32_t rs2);

int main(void) {
    uint32_t pc = 0;
    uint32_t reg[32] = {0};
    
    uint32_t progr[3] = {
        0x00200093, // addi x1 x0 2
        0x00300113, // addi x2 x0 3
        0x002081b3, // add x3 x1 x2
    };
    
    printf("Hello RISC-V World!\n");
    
    while(1) {
        uint32_t instr = progr[pc >> 2];
        uint32_t opcode = instr & 0x7f;
        uint32_t rd = (instr >> 7) & 0x01f;
        uint32_t rs1 = (instr >> 15) & 0x01f;
        uint32_t rs2 = (instr >> 20) & 0x01f;
        uint32_t imm = (instr >> 20);
        
        switch (opcode) {
            // addi x1, x2, imm
            case 0x13:
                reg[rd] = reg[rs1] + imm;
                break;
            case 0

            case 0x33:
                void 

            default:
                printf("Opcode %u not yet implemented\n", opcode);
                break;
        }
        
        pc += 4; 
        
        if ((pc >> 2) >= sizeof(progr) / sizeof(progr[0])) {
            break;
        }
        
        for (size_t i = 0; i < sizeof(reg) / sizeof(reg[0]); ++i) {
            printf("%u ", reg[i]);
        }
        
        printf("\n");
    }
    
    printf("Program exit\n");
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