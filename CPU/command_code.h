



#define CMD_EXIT_CODE       \
        exit_flag = 0;      \

#define CMD_PUSH_CODE                                                          \
        push(&cpu_data_stack, *arg[0]);                                        \



#define CMD_POP_CODE                                                            \
        pop(&cpu_data_stack, arg[0]);                                           \




#define CMD_ADD_CODE                                \
        pop(&cpu_data_stack, arg[0]);               \
        pop(&cpu_data_stack, arg[1]);               \
        push(&cpu_data_stack, *arg[0] + *arg[1]);   \



#define CMD_JMP_CODE                                \
        offset = *arg[0];                           \


#define CMD_JE_CODE                                     \
        if (flag_registers[0] == 1)                    \
        {                                               \
            printf("%d\n", *arg[0]);                                               \
            offset = *arg[0];                           \
        }                                               \

#define CMD_JNE_CODE                                    \
        if (flag_registers[ZF] == 0)                    \
        {                                               \
            offset = *arg[0];                           \
        }                                               \



#define CMD_JG_CODE                                                                \
        if (flag_registers[ZF] == 0 && flag_registers[SF] == 0)                    \
        {                                                                          \
            offset = *arg[0];                                                      \
        }                                                                          \

#define CMD_JGE_CODE                                                                \
        if (flag_registers[SF] == 0 || flag_registers[ZF] == 1)                     \
        {                                                                           \
            offset = *arg[0];                                                       \
        }                                                                           \

#define CMD_JL_CODE                                                                 \
        if (flag_registers[SF] == 1 && flag_registers[ZF] == 0)                     \
        {                                                                           \
            offset = *arg[0];                                                       \
        }                                                                           \



#define CMD_JLE_CODE                                                                \
        if (flag_registers[SF] == 1 || flag_registers[ZF] == 1)                     \
        {                                                                           \
            offset = *arg[0];                                                       \
        }                                                                           \
                                
#define CMD_IN_CODE                        \
        scanf("%d", arg[0]);               \
        push(&cpu_data_stack, *arg[0]);    \

#define CMD_OUT_CODE                     \
        pop(&cpu_data_stack, arg[0]);    \
        printf("%d\n", *arg[0]);           \

#define CMD_CMP_CODE \
        printf("%d %d", *arg[0], *arg[1]);                                                \
        if ((*arg[0] - *arg[1]) > 0)                                      \
        {\
            printf("1");                                                        \
            flag_registers[SF] = 0;                                 \
            flag_registers[ZF] = 0;                                 \
        }                                                           \
        if ((*arg[0]-*arg[1]) == 0)                                       \
        {\
            printf("2");                                   \
            flag_registers[SF] = 0;                                 \
            flag_registers[ZF] = 1;                                 \
        }                                                           \
        if ((*arg[0]-*arg[1]) < 0)                                        \
        {\
            printf("3");                                                           \
            flag_registers[SF] = 1;                                 \
            flag_registers[ZF] = 0;                                 \
        }                                                           \


#define CMD_DEC_CODE                                                 \
        pop(&cpu_data_stack, arg[0]);                                \
        (*arg[0])--;                                                 \
        push(&cpu_data_stack, *arg[0]);                              \


#define CMD_INC_CODE                                                 \
        pop(&cpu_data_stack, arg[0]);                                \
        (*arg[0])++;                                                 \
        push(&cpu_data_stack, *arg[0]);                              \

#define CMD_CALL_CODE                                               \
        push(&cpu_call_stack, offset);                              \
        offset = *arg[0];                                           \


#define CMD_RTN_CODE                                                \
        pop(&cpu_call_stack, &offset);                              \





