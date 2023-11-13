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

#define NO_ERR 0

int main(void) {
    unsigned int pc = 0;
    unsigned int reg[32] = {0};
    
    unsigned int progr[3] = {
        0x00200093, 
        0x00300113, 
        0x002081b3, 
    };
    
    printf("Hello RISC-V World!\n");
    
    while(1) {
        unsigned int instr = progr[pc >> 2];
        unsigned int opcode = instr & 0x7f;
        unsigned int rd = (instr >> 7) & 0x01f;
        unsigned int rs1 = (instr >> 15) & 0x01f;
        unsigned int imm = (instr >> 20);
        
        switch (opcode) {
            // add x1, x2, x3
            case 0x13:
                reg[rd] = reg[rs1] + imm;
                break;
            // addi x1, x2, imm
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


