#ifndef __MAIN_H
#define __MAIN_H

#include <stdint.h>

#define FILE_NAME_SIZE                  100

/**
 * @brief 全局参数结构体
 */
typedef struct _o_global_param_type{
    char input_file_name[FILE_NAME_SIZE];
    char output_file_name[FILE_NAME_SIZE];
    char register_file_name[FILE_NAME_SIZE];
    int single_step_mod;                        // 是否为当步运行模式
    int detailed_mod;                           // 是否为详细模式
    int time_out;                               // 为了避免死循环设置的timeout
    uint16_t begin_address;                     // 程序的起始地址
}_global_param_type;

int _str_to_int(char *str, int *value);

#endif
