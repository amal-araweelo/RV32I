#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

void RTypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, int32_t *reg);
void ITypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg);
void SBTypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc);
void EcallSwitch(int32_t *reg, uint32_t *progr);

#endif // INSTRUCTIONS_H
