#include "simulator.h"
#include "step.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int __binary_string_to_uint16_t(char *str, uint16_t *value);
_err_type _preprocess(
    _simulator_type *p_simulator,
    _global_param_type *_global_param,
    __register_type *__origional_reigster_value,
    __memmory_block_type *__origional_memmory_block
);
__err_info_type _simulator_reset(_simulator_type *p_simulator, __register_type *__origional_reigster_value, __memmory_block_type *__origional_memmory_block);
__err_info_type _simulator_init(_simulator_type *p_simulator, _global_param_type *_global_param, __register_type *__origional_reigster_value, __memmory_block_type *__origional_memmory_block);
_err_type _get_origional_reigster_value(_global_param_type *_global_param, __register_type *__origional_reigster_value);
_err_type _get_origional_memmory_block(_global_param_type *_global_param, __memmory_block_type *__origional_memmory_block);



/**
 * @brief 用于输出错误信息。如果没有错那就返回1，否则就打印
 *          错误信息并且返回0.
 * 
 * @param err 
 * @return int 
 */
int raise_error(_err_type err){
    if(err.err_info == SUCCESS) return 1;
    else{
        printf("line : %d, error code : %d\n", err.line, err.err_info);
        return 0;
    }
    return 1;
}


/**
 * @brief   唯一能被主函数调用的函数。
 *          如果程序正常运行就返回1，否则就返回0.
 * 
 * @return int 
 */
int simulate(_global_param_type *_global_param){
    _err_type err = {-1, SUCCESS};
    _simulator_type _simulator;
    __register_type __origional_reigster_value;
    __memmory_block_type __origional_memmory_block;
    int time_out = _global_param->time_out;
    FILE *p_output_file = NULL;

    err = _preprocess(&_simulator, _global_param, &__origional_reigster_value, &__origional_memmory_block);
    if(raise_error(err) == 0) return 0;

    p_output_file = fopen(_global_param->output_file_name, "w");
    if(p_output_file == NULL){
        printf("Can not creat output file %s.\n", _global_param->output_file_name);
        return 0;
    }

    while (time_out > 0){
        time_out --;
        err = step(&_simulator, p_output_file);
        ++ _simulator.step_counter;
        if(_global_param->single_step_mod == 1)  system("pause");
        if(raise_error(err) == 0)   break;
        if(_simulator.stop_signal == 1) break;
    }
    
    if(time_out <= 0){
        printf("warning time out!\n");
    }

    if(err.err_info != SUCCESS) return 0;

    fprintf(p_output_file, "simulate done, steps : %d\n", _simulator.step_counter);
    printf("simulate done, steps : %d\n", _simulator.step_counter);
    fclose(p_output_file);
    return 1;
}


/**
 * @brief 预处理，获取初始的寄存器值，初始的内存块值，然后初始化 simulator
 * 
 * @param p_simulator 
 * @return _err_type 
 */
_err_type _preprocess(
    _simulator_type *p_simulator,
    _global_param_type *_global_param,
    __register_type *__origional_reigster_value,
    __memmory_block_type *__origional_memmory_block
    ){
    _err_type err = {-1, SUCCESS};

    err = _get_origional_reigster_value(_global_param, __origional_reigster_value);
    if(err.err_info != SUCCESS) return err;

    err = _get_origional_memmory_block(_global_param, __origional_memmory_block);
    if(err.err_info != SUCCESS) return err;

    err.err_info = _simulator_init(p_simulator, _global_param, __origional_reigster_value, __origional_memmory_block);
    if(err.err_info != SUCCESS) return err;

    return err;
}


/**
 * @brief 将2进制字符串的值转化为 uint16_t 16位无符号整数的值
 * 
 * @param str 
 * @param value 
 * @return int 
 */
int __binary_string_to_uint16_t(char *str, uint16_t *value){
    uint16_t bit = 0x8000;
    if(str == NULL || value == NULL)    return 0;
    *value &= 0;
    while(bit != 0 && *str != '\0'){
        if(*str == '0'){
            *value |= 0;
        }else if(*str == '1'){
            *value |= bit;
        }else return 0;
        bit >>= 1;
        str ++;
    }
    if(bit != 0 || *str != '\0')  return 0;
    return 1;
}


/**
 * @brief 重新初始化 simulator，但是目前还没用上。
 * 
 * @param p_simulator 
 * @return __err_info_type 
 */
__err_info_type _simulator_reset(_simulator_type *p_simulator, __register_type *__origional_reigster_value, __memmory_block_type *__origional_memmory_block){
    p_simulator->step_counter = 0;
    p_simulator->stop_signal = 0;
    p_simulator->register_value = *__origional_reigster_value;
    p_simulator->memmory_block = *__origional_memmory_block;
    p_simulator->register_value.PC = p_simulator->base_address;
    p_simulator->NPZ = 0;
    return SUCCESS;
}


/**
 * @brief 初始化
 * 
 * @param p_simulator 
 * @return __err_info_type 
 */
__err_info_type _simulator_init(_simulator_type *p_simulator, _global_param_type *_global_param, __register_type *__origional_reigster_value, __memmory_block_type *__origional_memmory_block){
    p_simulator->base_address = _global_param->begin_address;
    p_simulator->stop_signal = 0;
    p_simulator->memmory_block = *__origional_memmory_block;
    p_simulator->register_value = *__origional_reigster_value;
    p_simulator->setting.detailed_mode = _global_param->detailed_mod;
    p_simulator->setting.single_step_mode = _global_param->single_step_mod;
    p_simulator->step_counter = 0;
    p_simulator->register_value.PC = _global_param->begin_address;
    p_simulator->NPZ = 0;
    return SUCCESS;
}


/**
 * @brief 读写文件以获取初始化寄存器的值。
 * 
 * @return _err_type 
 */
_err_type _get_origional_reigster_value(_global_param_type *_global_param, __register_type *__origional_reigster_value){
    _err_type err = {-1, SUCCESS};
    if(strcmp(_global_param->register_file_name, "__default") == 0){
        int i = 0;
        __origional_reigster_value->MAR = 0;
        __origional_reigster_value->MDR = 0;
        __origional_reigster_value->PC = 0;
        for(i = 0; i < 8; ++ i){
            __origional_reigster_value->user_register[i] = 0;
        }
    }else{
        FILE *p_file = NULL;
        p_file = fopen(_global_param->register_file_name, "r");
        if(p_file != NULL){
            char line_buffer[LINE_BUFFER_LENGTH];
            int i = 0;
            int register_value = 0;

            for(i = 0; i < 8; ++ i){
                fscanf(p_file, "%s\n", line_buffer);
                printf("---%s---\n", line_buffer);
                if(feof(p_file) == 1)   break;
                else{
                    if(_str_to_int(line_buffer, &register_value) == 0){
                        err.err_info = INVALID_REGISTER_VALUE;
                        err.line = i + 1;
                        return err;
                    }
                    __origional_reigster_value->user_register[i] = (uint16_t)register_value;
                }
            }
            for(; i < 8; ++ i){
                __origional_reigster_value->user_register[i] = 0;
            }
            fclose(p_file);
        }else{
            err.err_info = REGISTER_FILE_OPEN_FAIL;
            return err;
        }
    }
    return err;
}


/**
 * @brief 读写文件初始化内存块的值
 * 
 * @return _err_type 
 */
_err_type _get_origional_memmory_block(_global_param_type *_global_param, __memmory_block_type *__origional_memmory_block){
    _err_type err = {-1, SUCCESS};
    FILE *p_file = NULL;

    __origional_memmory_block->size = INITIAL_MEMMORY_BLOCK_SIZE;

    p_file = fopen(_global_param->input_file_name, "r");
    if(p_file != NULL){
        char line_buffer[LINE_BUFFER_LENGTH] = {'\0'};
        uint16_t line_value = 0x0000;
        int line_counter = 0;
        while(1){
            fscanf(p_file, "%s\n", line_buffer);
            if(strlen(line_buffer) == 0 && !feof(p_file))    continue;
printf("--%s--\n", line_buffer);
            if(__binary_string_to_uint16_t(line_buffer, &line_value) == 0){
                err.err_info = INVALID_MEMMORY_VALUE;
                err.line = line_counter;
                return err;
            }
            __origional_memmory_block->memmory[__origional_memmory_block->length].address = line_counter + _global_param->begin_address;
            __origional_memmory_block->memmory[__origional_memmory_block->length].content = line_value;
            __origional_memmory_block->length ++;
            if(__origional_memmory_block->length == __origional_memmory_block->size){
                err.err_info = MEMMORY_LIST_OVERFLOW;
                return err;
            }
            line_counter ++;
            if(feof(p_file))   break;
        }
        fclose(p_file);
    }else{
        err.err_info = MEMMORY_FILE_OPEN_FAIL;
        return err;
    }
    return err;
}
