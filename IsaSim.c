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

int main(void) {
    uint32_t pc = 0;
    uint32_t reg[32] = {0};
    
    uint32_t progr[3] = {
        0x00200093, 
        0x00300113, 
        0x002081b3, 
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


