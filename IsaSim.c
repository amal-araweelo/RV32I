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

void ItypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2);
void RtypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2);

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
        uint32_t funct3 = (instr >> 12) & 0x007;
        uint32_t funct7 = (instr >> 25) & 0x07f; 
        
        switch (opcode) {
            // I-type instructions
            case 0x13:
                ItypeSwitch(funct3, funct7, rd, rs1, rs2);
                break;
            case 0x33:
                RtypeSwitch(funct3, funct7,rd, rs1, rs2);
                break;
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

void RtypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2){
switch (funct3)
{
case 0x00:
    if(funct7 != 0){
        //sub 
    }
    else{
        //add HAS IMM
    }
    break;

case 0x01:
    //sll HAS IMM
    break;

case 0x02:
    //slt HAS IMM
    break;

case 0x03:
    //sltu HAS IMM
    break;

case 0x04:
    //xor HAS IMM
    break;

case 0x05:
    if(funct7 != 0){
        //sra HAS IMM
    }
    else{
        //srl HAS IMM
    }
    break;

case 0x06:
    //or HAS IMM
    break;

case 0x07:
    //and HAS IMM
    break;


default:
    printf("in RtypeSwitch funct3 error: case not defined");
    printf("funct3 value (unsigned int): %u", funct3);
    break;
}
}

void ItypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2){
    switch (funct3){
        // addi
        case 0x0:
            reg[rd] = reg[rs1] + imm;
        break;
        
        // slli
        case 0x1:
            reg[rd] = reg[rs1] << imm;
        break;

        // slti
        case 0x3:
            if (reg[rs1] < imm) reg[rd] = 1 break;
            reg[rd] = 0;
        break;

        // xori
        case 0x4:
            reg[rd] = reg[rs1] ^ imm;
        break;

        // srli and srai
        case 0x5:
            switch (funt7):
                // srli
                case 0x0:
                    reg[rd] = reg[rs1] >> imm;
                break;
                // srai
                case 0x20:
                    if (reg[rs1] > 0) reg[rd] = reg[rs1] >> imm break;
                    reg[rd] = (reg[rs1] >> imm) & 0xFFFFFFFF;
                break;       
        break;
        
        // ori
        case 0x6:
            reg[rd] = reg[rsl] | imm;
        break;

        // andi
        case 0x7:
            reg[rd] = reg[rs1] & imm;
        break;
    }
}