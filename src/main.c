#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "simulator.h"

/**
 * @brief 帮助信息字符串常量
 */
const char _help_info[] = "\n\
LC3 SIMULATOR\n\
Options :\n\
    -h [ --help ]                           Help screen\n\
    -f [ --file ] arg (=input.txt)          Input file\n\
    -r [ --register ] arg (=register.txt)   Register Status\n\
    -s [ --single ]                         Single Step Mode\n\
    -b [ --begin ] arg (=12288)             Begin address (0x3000)\n\
    -o [ --output ] arg                     Output file\n\
    -d [ --detail ]                         Detailed Mode\n\n";

/**
 * @brief 函数声明
 */
int _option_handle(int argc, char **argv, _global_param_type *_global_param);
int __find_substr(
    int argc, char **argv, const char *_str
);


/**
 * @brief 主函数
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main(int argc, char **argv){
    int state = 0;
    _global_param_type _global_param = {
        {'\0'}, {'\0'}, {'\0'},
        0, 0, 0
    };

    state = _option_handle(argc, argv, &_global_param);
    if(state == 0)  return 0;
    
    state = simulate(&_global_param);
    if(state == 0)  return 0;

    return 0;
}


/**
 * @brief 处理那一堆option的函数，同时设置全局参数
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int _option_handle(int argc, char **argv, _global_param_type *_global_param){
    int state = 0;

    state = __find_substr(argc, argv, "-h");
    if(state > 0){
        printf("%s", _help_info);
        return 1;
    }

    state = __find_substr(argc, argv, "-f");
    if(state > 0 && state < argc - 1){
        strcpy(_global_param->input_file_name,
                argv[state + 1]);
    }else{
        printf("No input file detected!\n");
        return 0;
    }

    state = __find_substr(argc, argv, "-r");
    if(state > 0 && state < argc - 1){
        strcpy(_global_param->register_file_name,
                argv[state + 1]);
    }else if(state == argc - 1){
        printf("No register status\
                 file detected!\n");
        return 0;
    }else{
        strcpy(_global_param->register_file_name,
                "__default");
    }

    state = __find_substr(argc, argv, "-s");
    if(state > 0){
        _global_param->single_step_mod = 1;
    }else{
        _global_param->single_step_mod = 0;
    }

    state = __find_substr(argc, argv, "-b");
    if(state > 0 && state < argc - 1){
        int address_value = 0;
        state = _str_to_int(argv[state + 1],
                            &address_value);
        if(state == 0)  return 0;
        _global_param->begin_address = 0xFFFF & address_value;
    }else if(state == argc - 1){
        printf("No valid begin address detected!\n");
        return 0;
    }else{
        _global_param->begin_address = 0x3000;
    }

    state = __find_substr(argc, argv, "-o");
    if(state > 0 && state < argc - 1){
        strcpy(_global_param->output_file_name,
                argv[state + 1]);
    }else if(state == argc - 1){
        printf("No output file name detected!\n");
        return 0;
    }else{
        strcpy(_global_param->output_file_name, "a.txt");
    }

    state = __find_substr(argc, argv, "-d");
    if(state > 0){
        _global_param->detailed_mod = 1;
    }else{
        _global_param->detailed_mod = 0;
    }

    /* 此处可添加 time out 设置 */
    _global_param->time_out = 100000;

    return 1;
}


/**
 * @brief 在 argv 中寻找指定字符串，找到了就返回下标，反之则返回 -1
 * 
 * @param argc 
 * @param argv 
 * @param _str 
 * @return int 
 */
int __find_substr(
    int argc, char **argv, const char *_str
){
    int i = 0;
    for(i = 0; i < argc; ++ i){
        if(strcmp(argv[i], _str) == 0)    return i;
    }
    return -1;
}


/**
 * @brief 将10进制字符串表示的值转化为 int，成功返回1，否则返回0
 * 
 * @param str 
 * @param value 
 * @return int 
 */
int _str_to_int(char *str, int *value){
    *value = 0;
    while(*str != '\0'){
        if(*str >= '0' && *str <= '9'){
            *value *= 10;
            *value += *str - '0';
        }else   return 0;
        str ++;
    }
    
    return 1;
}
