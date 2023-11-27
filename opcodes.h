#ifndef OPCODE_H
#define OPCODE_H

#include <stdint.h>

// Opcodes - instructions
#define LUI 0x37
#define AUIPC 0x17
#define JALR 0x67
#define ECALL 0x73
#define JAL 0x6F

// Opcodes - instruction type
#define I_TYPE 0x13
#define R_TYPE 0x33
#define SB_TYPE 0x63

#endif // OPCODE_H