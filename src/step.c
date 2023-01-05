#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "step.h"

/**
 * @brief 运行一步
 * 
 * @param p_simulator 
 * @param p_output_file 
 * @return _err_type 
 */
_err_type step(_simulator_type *p_simulator, FILE *p_output_file){
    const __instruction_handler_type __instruction_handler[] = {
        BR_handler,
        ADD_handler,
        LD_handler,
        ST_handler,
        JSR_handler,
        AND_handler,
        LDR_handler,
        STR_handler,
        RTI_handler,
        NOT_handler,
        LDI_handler,
        STI_handler,
        JMP_handler,
        reserved,
        LEA_handler,
        TRAP_handler
    };
    _err_type err = {p_simulator->step_counter, SUCCESS};
    uint16_t instruction_code = 0, instruction = 0;

    err.err_info = _fetch(p_simulator, &instruction_code);
    if(err.err_info != SUCCESS) return err;

    instruction = _decode(instruction_code);

    err.err_info = __instruction_handler[instruction](p_simulator, instruction_code);

    if(p_simulator->setting.detailed_mode == 1) print_state(stdout, p_simulator, instruction);
    print_state(p_output_file, p_simulator, instruction);
    return err;
}


/**
 * @brief 字面意思
 * 
 * @param p_simulator 
 * @param instruction 
 * @return __err_info_type 
 */
__err_info_type _fetch(_simulator_type *p_simulator, uint16_t *instruction){
    if(__get_memmory_content(p_simulator, p_simulator->register_value.PC, instruction) == 0){
        if(__assign_memmory_content(p_simulator, p_simulator->register_value.PC, 0x0000) == 0){
            return MEMMORY_LIST_OVERFLOW;
        }
        *instruction = 0x0000;
    }
    p_simulator->register_value.MAR = p_simulator->register_value.PC;
    p_simulator->register_value.MDR = *instruction;
    p_simulator->register_value.PC ++;
    return SUCCESS;
}


/**
 * @brief 也是字面意思
 * 
 * @param instruction_code 
 * @return uint16_t 
 */
uint16_t _decode(uint16_t instruction_code){
    return (instruction_code & 0xF000) >> 12;
}


/**
 * @brief 获取给定地址的内存块的值。成功就返回1，否则返回0
 * 
 * @param p_simulator 
 * @param address 
 * @param content 
 * @return int 
 */
int __get_memmory_content(_simulator_type *p_simulator, uint16_t address, uint16_t *content){
    int i = 0;
    for(i = 0; i < p_simulator->memmory_block.length; ++ i){
        if(address == p_simulator->memmory_block.memmory[i].address){
            *content = p_simulator->memmory_block.memmory[i].content;
            return 1;
        }
    }
    return 0;
}


/**
 * @brief 给对应地址的内存赋值。若已经在数组里面就直接修改，否则就插入一个新的元素
 * 
 * @param p_simulator 
 * @param address 
 * @param content 
 * @return int 
 */
int __assign_memmory_content(_simulator_type *p_simulator, uint16_t address, uint16_t content){
    int i = 0;
    for(i = 0; i < p_simulator->memmory_block.length; ++ i){
        if(address == p_simulator->memmory_block.memmory[i].address){   // 已经存在
            p_simulator->memmory_block.memmory[i].content = content;
            return 1;
        }
    }

    if(p_simulator->memmory_block.size == p_simulator->memmory_block.length){
        printf("size = %d, length = %d\n", p_simulator->memmory_block.size, p_simulator->memmory_block.length);
        // 空间已满
        return 0;
    }else{
        p_simulator->memmory_block.memmory[p_simulator->memmory_block.length].address = address;
        p_simulator->memmory_block.memmory[p_simulator->memmory_block.length].content = content;
        p_simulator->memmory_block.length ++;
        return 2;
    }
}


/**
 * @brief 将低位二补数转化为高位表示
 * 
 * @param src_value     原来的数
 * @param src_bit_num   原来的位数
 * @param dst_bit_num   目标位数
 * @param dst_value     目标位数的对应的值的表示
 * @return int 
 */
int __extend_signed_2_com_int(uint16_t src_value, int src_bit_num, int dst_bit_num, uint16_t *dst_value){
    uint16_t src_highest_bit = (uint16_t)(1 << (src_bit_num - 1));
    uint16_t src_all_one = (uint16_t)(1 << src_bit_num) - 1;

    if(dst_bit_num < src_bit_num)   return 0;
    else{
        int i = src_bit_num;
        int signal = (src_value & src_highest_bit) == 0 ? 0 : 1;
        *dst_value = src_all_one & src_value;
        if(signal == 1){
            for(i = src_bit_num; i < dst_bit_num; ++ i){
                src_highest_bit <<= 1;
                *dst_value |= src_highest_bit;
            }
        }
        return 1;
    }
}


/**
 * @brief 更新NPZ的值
 * 
 * @param p_simulator 
 * @param result 
 */
void __refresh_npz(_simulator_type *p_simulator, uint16_t result){
    if(result == 0){
        p_simulator->NPZ = 0x02;
    }else if((result & 0x8000) != 0){
        p_simulator->NPZ = 0x04;
    }else{
        p_simulator->NPZ = 0x01;
    }
}


/**
 * @brief 打印出当前状态，数值均为16进制
 * 
 * @param p_file 
 * @param p_simulator 
 * @param instruction 
 */
void print_state(FILE *p_file, _simulator_type *p_simulator, uint16_t instruction){
    const char instruction_name[][5] = {
        "BR",
        "ADD",
        "LD",
        "ST",
        "JSR",
        "AND",
        "LDR",
        "STR",
        "RTI",
        "NOT",
        "LDI",
        "STI",
        "JMP",
        "",
        "LEA",
        "TRAP"
    };
    int i = 0;
    fprintf(p_file, "step %d : %s\n", p_simulator->step_counter + 1, instruction_name[instruction]);
    for(i = 0; i < 4; ++ i){
        fprintf(p_file, "R%d = 0x%x, ", i, p_simulator->register_value.user_register[i]);
    }
    fprintf(p_file, "\n");
    for(i = 4; i < 8; ++ i){
        fprintf(p_file, "R%d = 0x%x, ", i, p_simulator->register_value.user_register[i]);
    }
    fprintf(p_file, "\n");
    fprintf(p_file, "NPZ = %d%d%d\n",
        ((p_simulator->NPZ) & 0b100) == 0 ? 0 : 1,
        ((p_simulator->NPZ) & 0b10) == 0 ? 0 : 1,
        ((p_simulator->NPZ) & 0b1) == 0 ? 0 : 1
    );
    fprintf(p_file, "MAR = 0x%x\n", p_simulator->register_value.MAR);
    fprintf(p_file, "MDR = 0x%x\n", p_simulator->register_value.MDR);
    fprintf(p_file, "PC = 0x%x\n\n", p_simulator->register_value.PC);
}


/**
 * @brief 以下就是15条指令的处理函数。只写了test123.bin里面出现的，其他的有缘再写
 */
__err_info_type BR_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    uint8_t instruction_nzp_value = (uint8_t)(((instruction_code & 0x0E00) >> 9) & 0x07);
    if((instruction_nzp_value & p_simulator->NPZ) != 0){    // PC + offset
        uint16_t offset_value = 0;
        __extend_signed_2_com_int((instruction_code & 0x01FF), 9, 16, &offset_value);
        p_simulator->register_value.PC += offset_value;
    }else{
        // 什么也不做
    }
    return SUCCESS;
}

__err_info_type ADD_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int dst_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    int src_register_index_1 = ((instruction_code & 0x01C0) >> 6) & 0x0007;
    if((instruction_code & 0x0020) != 0){       // imm
        uint16_t immidiate_value = 0;
        __extend_signed_2_com_int(instruction_code & 0x001F, 5, 16, &immidiate_value);
        p_simulator->register_value.user_register[dst_register_index] = p_simulator->register_value.user_register[src_register_index_1] + immidiate_value;
    }else{
        int src_register_index_2 = (uint8_t)(instruction_code & 0x07);
        p_simulator->register_value.user_register[dst_register_index] = p_simulator->register_value.user_register[src_register_index_1] + p_simulator->register_value.user_register[src_register_index_2];
    }
    __refresh_npz(p_simulator, p_simulator->register_value.user_register[dst_register_index]);
    return SUCCESS;
}

__err_info_type LD_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int dst_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    uint16_t address = 0, offset_value = 0, content = 0;

    __extend_signed_2_com_int((instruction_code & 0x01FF), 9, 16, &offset_value);
    address = offset_value + p_simulator->register_value.PC;

    if(__get_memmory_content(p_simulator, address, &content) == 0){
        content = 0;
        if(__assign_memmory_content(p_simulator, address, content) == 0){
            return MEMMORY_LIST_OVERFLOW;
        }
    }

    p_simulator->register_value.user_register[dst_register_index] = content;
    p_simulator->register_value.MAR = address;
    p_simulator->register_value.MDR = content;

    __refresh_npz(p_simulator, p_simulator->register_value.user_register[dst_register_index]);
    return SUCCESS;
}

__err_info_type ST_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int src_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    uint16_t address = 0, offset_value = 0;

    __extend_signed_2_com_int((instruction_code & 0x01FF), 9, 16, &offset_value);
    address = offset_value + p_simulator->register_value.PC;

    if(__assign_memmory_content(p_simulator, address, p_simulator->register_value.user_register[src_register_index]) == 0){
        return MEMMORY_LIST_OVERFLOW;
    }

    p_simulator->register_value.MAR = address;
    p_simulator->register_value.MDR = p_simulator->register_value.user_register[src_register_index];

    return SUCCESS;
}

__err_info_type JSR_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    p_simulator->register_value.user_register[7] = p_simulator->register_value.PC;
    if((instruction_code & 0x0800) != 0){   // JSR
        uint16_t offset_value = 0;
        __extend_signed_2_com_int((instruction_code & 0x07FF), 11, 16, &offset_value);
        p_simulator->register_value.PC += offset_value;
    }else{  // JSRR
        int base_register_index = ((instruction_code & 0x01C0) >> 6) & 0x0007;
        p_simulator->register_value.PC = p_simulator->register_value.user_register[base_register_index];
    }
    return SUCCESS;
}

__err_info_type AND_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int dst_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    int src_register_index_1 = ((instruction_code & 0x01C0) >> 6) & 0x0007;
    if((instruction_code & 0x0020) != 0){       // imm
        uint16_t immidiate_value = 0;
        __extend_signed_2_com_int(instruction_code & 0x001F, 5, 16, &immidiate_value);
        p_simulator->register_value.user_register[dst_register_index] = (p_simulator->register_value.user_register[src_register_index_1]) & immidiate_value;
    }else{
        int src_register_index_2 = (uint8_t)(instruction_code & 0x07);
        p_simulator->register_value.user_register[dst_register_index] = (p_simulator->register_value.user_register[src_register_index_1]) & (p_simulator->register_value.user_register[src_register_index_2]);
    }
    __refresh_npz(p_simulator, p_simulator->register_value.user_register[dst_register_index]);
    return SUCCESS;
}

__err_info_type LDR_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int dst_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    int base_register_index = ((instruction_code & 0x01C0) >> 6) & 0x0007;
    uint16_t address = 0, offset_value = 0, content = 0;

    __extend_signed_2_com_int((instruction_code & 0x003F), 6, 16, &offset_value);
    address = offset_value + p_simulator->register_value.user_register[base_register_index];

    if(__get_memmory_content(p_simulator, address, &content) == 0){
        content = 0;
        if(__assign_memmory_content(p_simulator, address, content) == 0){
            return MEMMORY_LIST_OVERFLOW;
        }
    }

    p_simulator->register_value.user_register[dst_register_index] = content;

    __refresh_npz(p_simulator, content);
    return SUCCESS;
}

__err_info_type STR_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int src_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    int base_register_index = ((instruction_code & 0x01C0) >> 6) & 0x0007;
    uint16_t address = 0, offset_value = 0;

    __extend_signed_2_com_int((instruction_code & 0x003F), 6, 16, &offset_value);
    address = offset_value + p_simulator->register_value.user_register[base_register_index];


    if(__assign_memmory_content(p_simulator, address, p_simulator->register_value.user_register[src_register_index]) == 0){
        return MEMMORY_LIST_OVERFLOW;
    }

    p_simulator->register_value.MAR = address;
    p_simulator->register_value.MDR = p_simulator->register_value.user_register[src_register_index];

    return SUCCESS;
}

__err_info_type RTI_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    if((p_simulator->register_value.PSR & 0x8000) != 0){
        // 教材说 Initiate a privilege mode exception 但是我不理解
    }else{
        uint16_t address = p_simulator->register_value.user_register[6];
        uint16_t content = 0;

        if(__get_memmory_content(p_simulator, address, &content) == 0){
            content = 0;
            if(__assign_memmory_content(p_simulator, address, content) == 0){
                return MEMMORY_LIST_OVERFLOW;
            }
        }

        p_simulator->register_value.PC = content;
        p_simulator->register_value.user_register[6] += 1;

        address = p_simulator->register_value.user_register[6];
        if(__get_memmory_content(p_simulator, address, &content) == 0){
            content = 0;
            if(__assign_memmory_content(p_simulator, address, content) == 0){
                return MEMMORY_LIST_OVERFLOW;
            }
        }

        p_simulator->register_value.PSR = content;
        p_simulator->register_value.user_register[6] += 1;
    
        if((p_simulator->register_value.PSR & 0x8000) != 0){
            // 教材上说 Saved SSP=R6 and R6=Saved USP 但是我也不理解
        }
    }
    return SUCCESS;
}

__err_info_type NOT_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int dst_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    int src_register_index = ((instruction_code & 0x01C0) >> 6) & 0x0007;

    p_simulator->register_value.user_register[dst_register_index] = ~ (p_simulator->register_value.user_register[src_register_index]);
    __refresh_npz(p_simulator, p_simulator->register_value.user_register[dst_register_index]);

    return SUCCESS;
}

__err_info_type LDI_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int dst_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    uint16_t offset_value = 0, address = 0, content = 0;

    __extend_signed_2_com_int(instruction_code & 0x01FF, 9, 16, &offset_value);
    address = p_simulator->register_value.PC + offset_value;
    if(__get_memmory_content(p_simulator, address, &content) == 0){
        content = 0;
        if(__assign_memmory_content(p_simulator, address, content) == 0){
            return MEMMORY_LIST_OVERFLOW;
        }
    }

    address = content;
    if(__get_memmory_content(p_simulator, address, &content) == 0){
        content = 0;
        if(__assign_memmory_content(p_simulator, address, content) == 0){
            return MEMMORY_LIST_OVERFLOW;
        }
    }

    p_simulator->register_value.user_register[dst_register_index] = content;
    p_simulator->register_value.MAR = address;
    p_simulator->register_value.MDR = content;
    __refresh_npz(p_simulator, p_simulator->register_value.user_register[dst_register_index]);
    return SUCCESS;
}

__err_info_type STI_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int src_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    uint16_t offset_value = 0, address = 0, content = 0;

    __extend_signed_2_com_int(instruction_code & 0x01FF, 9, 16, &offset_value);
    address = p_simulator->register_value.PC + offset_value;
    if(__get_memmory_content(p_simulator, address, &content) == 0){
        content = 0;
        if(__assign_memmory_content(p_simulator, address, content) == 0){
            return MEMMORY_LIST_OVERFLOW;
        }
    }

    address = content;
    if(__assign_memmory_content(p_simulator, address, p_simulator->register_value.user_register[src_register_index]) == 0){
        return MEMMORY_LIST_OVERFLOW;
    }

    p_simulator->register_value.MAR = address;
    p_simulator->register_value.MDR = p_simulator->register_value.user_register[src_register_index];
    return SUCCESS;
}

__err_info_type JMP_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int base_register_index = ((instruction_code & 0x01C0) >> 6) & 0x0007;
    p_simulator->register_value.PC = p_simulator->register_value.user_register[base_register_index];
    return SUCCESS;
}

__err_info_type reserved(_simulator_type *p_simulator, uint16_t instruction_code){
    return SUCCESS;
}

__err_info_type LEA_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    int dst_register_index = ((instruction_code & 0x0E00) >> 9) & 0x0007;
    uint16_t address = 0, offset_value = 0;

    __extend_signed_2_com_int((instruction_code & 0x01FF), 9, 16, &offset_value);
    address = offset_value + p_simulator->register_value.PC;

    p_simulator->register_value.user_register[dst_register_index] = address;
    __refresh_npz(p_simulator, address);
    return SUCCESS;
}

__err_info_type TRAP_handler(_simulator_type *p_simulator, uint16_t instruction_code){
    uint8_t trap_vector = (uint8_t)(instruction_code & 0x00FF);
    switch(trap_vector){
        case 0x25:
            p_simulator->stop_signal = 1;
            break;
        default :
            return INVALID_TRAP_VECTOR_VALUE;
    }
    return SUCCESS;
}
