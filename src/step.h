#ifndef __STEP_H
#define __STEP_H
#include "simulator.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef __err_info_type (*__instruction_handler_type)(_simulator_type *, uint16_t);

__err_info_type BR_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type ADD_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type LD_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type ST_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type JSR_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type AND_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type JSR_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type LDR_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type STR_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type RTI_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type NOT_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type LDI_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type STI_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type JMP_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type reserved(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type LEA_handler(_simulator_type *p_simulator, uint16_t instruction_code);
__err_info_type TRAP_handler(_simulator_type *p_simulator, uint16_t instruction_code);

void print_state(FILE *p_file, _simulator_type *p_simulator, uint16_t instruction_code);

__err_info_type _fetch(_simulator_type *p_simulator, uint16_t *instruction);
uint16_t _decode(uint16_t instruction_code);
int __get_memmory_content(_simulator_type *p_simulator, uint16_t address, uint16_t *content);
int __assign_memmory_content(_simulator_type *p_simulator, uint16_t address, uint16_t content);
int __extend_signed_2_com_int(uint16_t src_value, int src_bit_num, int dst_bit_num, uint16_t *dst_value);

_err_type step(_simulator_type *p_simulator, FILE *p_output_file);
#endif
