#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>

void RTypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, uint32_t rs2, int32_t *reg);
void ITypeSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg);
void SBTypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc);
void ITypeLoadSwitch(uint32_t funct3, uint32_t funct7, uint32_t rd, uint32_t rs1, int32_t imm, int32_t *reg,
		     int8_t *mem_base);
void STypeSwitch(uint32_t funct3, uint32_t rs1, uint32_t rs2, int32_t imm, int32_t *reg, int32_t *pc, int8_t *mem_base);
void EcallSwitch(int32_t *reg, uint32_t *progr);

#endif // INSTRUCTIONS_H
