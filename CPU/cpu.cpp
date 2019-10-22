#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "command_enum.h"
#include "vanur_lib.h"

#define COMMAND_DEFINE(name, num, code, args)    \
        case CMD_##name:                         \
             code                                \
        break;                                   \

enum REGISTERS
{
    AX = 100,
    BX = 101,
    CX = 102,
    DX = 103
};

void cpu()
{
    bool exit_flag = 1;
    int offset = 4;
    void *commands = file_read("binary.binary");
    int command_count = *(int*)commands;
    struct dynamic_stack cpu_data_stack;
    STACK_INIT(cpu_data_stack)
    int registers[4] = {0, 0, 0, 0};
    while (exit_flag)
    {
        char *current_command = (char*)commands + offset;
        int arg1 = 0, arg2 = 0;
        switch (*current_command)
        {
            #include "commands.h"
        }
    }
    /*
    printf("1.%d ",cpu_data_stack.data[0]);
    printf("2.%d ",cpu_data_stack.counter);
    printf("3.%d ",registers[0]);
    printf("4.%d ",registers[1]);
    printf("5.%d ",registers[2]);
    printf("6.%d ",registers[3]);
    */
}

int main()
{
    cpu();
    return 0;
}