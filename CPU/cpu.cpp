#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "command_enum.h"
#include "vanur_lib.h"


const int  MAX_ARGS_COUNT   =               3;
const int  REGISTER_OFFSET  =               2;
const int  INTEGER_OFFSET   = sizeof(int) + 1;
const int  NO_ARG_OFFSET    =               1;
const int  MEMMORY_SIZE     =          100000;

#define COMMAND_DEFINE(name, num, code, args)                                               \
        case CMD_##name:                                                                    \
        printf("COMMAND: %d\n OFFSET: %d\n", num, offset);                                  \
            for (int i = 0; i < args; i++)                                                  \
            {                                                                               \
                if (*(current_command + 1) == '0')                                          \
                {                                                                           \
                    arg[i] = &registers[*(current_command + 2) - 100];                      \
                    offset += REGISTER_OFFSET;                                              \
                    current_command += REGISTER_OFFSET;                                     \
                }                                                                           \
                if (*(current_command + 1) == '1')                                          \
                {                                                                           \
                    arg[i] = ((int*)(current_command + 2));                                 \
                    offset += INTEGER_OFFSET;                                               \
                    current_command += INTEGER_OFFSET;                                      \
                    printf("%d:", *arg[i]);                                                 \
                }                                                                           \
                if (*(current_command + 1) == '2')                                          \
                {                                                                           \
                    arg[i] = &buffer[i];                                                    \
                    offset +=  NO_ARG_OFFSET;                                               \
                    current_command += NO_ARG_OFFSET;                                       \
                }                                                                           \
                if (*(current_command + 1) == '3')                                          \
                {                                                                           \
                    current_command++;                                                      \
                    offset++;                                                               \
                    if (*(current_command + 1) == '0')                                      \
                    {                                                                       \
                        arg[i] = &memmory[registers[*(current_command + 2) - 100]];         \
                        offset += REGISTER_OFFSET;                                          \
                        current_command += REGISTER_OFFSET;                                 \
                    }                                                                       \
                    if (*(current_command + 1) == '1')                                      \
                    {                                                                       \
                        arg[i] = &memmory[*((int*)(current_command + 2))];                  \
                        offset += INTEGER_OFFSET;                                           \
                        current_command += INTEGER_OFFSET;                                  \
                    }                                                                       \
                }                                                                           \
            }                                                                               \
            if (args == 0)                                                                  \
            {                                                                               \
                arg[0] = &buffer[0];                                                        \
            }                                                                               \
            printf("ARGUMENT: %d\n", *arg[0]);                                              \
            offset++;                                                                       \
            code                                                                            \
            printf("CODE COMPLETED\n");                                                     \
        break;                                                                              \

enum REGISTERS
{
    ZF = 0,
    SF = 1,
    AX = 100,
    BX = 101,
    CX = 102,
    DX = 103
};

void cpu()
{
    bool exit_flag = 1;
    int offset = 0;
    char path[] = "binary.binary";
    void *commands = file_read(path);
    struct dynamic_stack cpu_data_stack;
    STACK_INIT(cpu_data_stack)
    struct dynamic_stack cpu_call_stack;
    STACK_INIT(cpu_call_stack)
    int registers[4] = {0, 0, 0, 0};
    bool flag_registers[2] = {0, 0};
    int memmory[MEMMORY_SIZE];
    while (exit_flag)
    {
        char *current_command = (char*)commands + offset;
        int *arg[MAX_ARGS_COUNT] = {};
        int buffer[MAX_ARGS_COUNT] = {};        
        switch (*current_command)
        {
            #include "commands.h"
        }
    }
    free(commands);
}

int main()
{
    printf("HELLO\n");
    cpu();
    return 0;
}