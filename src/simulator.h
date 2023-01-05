#ifndef __SIMULATOR_H
#define __SIMULATOR_H
#include "main.h"

#define INITIAL_MEMMORY_BLOCK_SIZE      200
#define LINE_BUFFER_LENGTH  50
#define USER_REGISTER       8


/**
 * @brief 错误信息枚举变量。遇见一个就添加一个。
 */
typedef enum _o_err_info_type{
    SUCCESS,
    REGISTER_FILE_OPEN_FAIL,
    INVALID_REGISTER_VALUE,
    MEMMORY_FILE_OPEN_FAIL,
    INVALID_MEMMORY_VALUE,
    MEMMORY_LIST_OVERFLOW,
    INVALID_TRAP_VECTOR_VALUE
}__err_info_type;


/**
 * @brief 错误枚举型变量
 * 
 */
typedef struct _o_err_type{
    int line;                   // 出错的行数
    __err_info_type err_info;   // 错误信息
}_err_type;

/**
 * @brief 模拟器参数信息
 *          其实是当时还不清楚需要哪些settings就封装了一下
 */
typedef struct _o_setting_type{
    int single_step_mode;
    int detailed_mode;
}__setting_type;

/**
 * @brief 模拟器的部分寄存器的值（我不知道的就没写进来）
 */
typedef struct _o_register_type{
    uint16_t user_register[USER_REGISTER];
    uint16_t PC;
    uint16_t MAR;
    uint16_t MDR;
    uint16_t PSR;
}__register_type;


/**
 * @brief 一个内存单元
 */
typedef struct _o_memmory_type{
    int address;                    // 地址，本来也该是uint16_t但是代码跑起来了我就不敢动它了(
    uint16_t content;               // 内容
}___memmory_type;


/**
 * @brief 内存单元块
 */
typedef struct _o_memmory_block_type{
    int size;                       // 大小
    int length;                     // 占用大小
    ___memmory_type memmory[INITIAL_MEMMORY_BLOCK_SIZE];
}__memmory_block_type;


/**
 * @brief 断点信息。但是相关内容我懒得写了。有缘再补充
 */
typedef struct _o_break_point_type{
    int line;
    int physical_line;
    int address;
}__break_point_type;


/**
 * @brief 模拟器结构体
 * 
 */
typedef struct _o_simulator_type{
    int step_counter;
    uint16_t base_address;
    uint8_t NPZ;                        // 最后三位有效
    uint8_t stop_signal;
    __setting_type  setting;
    __register_type register_value;
    __memmory_block_type memmory_block;
}_simulator_type;

int simulate(_global_param_type *_global_param);
#endif
