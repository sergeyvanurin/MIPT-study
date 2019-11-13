



#define CMD_EXIT_CODE       \
        exit_flag = 0;      \

#define CMD_PUSH_CODE                                                            \
        if (arg_type[0] != '2') push(&cpu_data_stack,*arg[0]);                  \



#define CMD_POP_CODE                                                            \
        pop(&cpu_data_stack, arg[0]);                                           \




#define CMD_ADD_CODE                                            \
        pop(&cpu_data_stack, &a);                               \
        pop(&cpu_data_stack, &b);                               \
        if (arg_type[0] == '2') push(&cpu_data_stack, a + b);   \
        if (arg_type[0] == '0') *arg[0] = a + b;                 \



#define CMD_JMP_CODE                                \
        offset = *arg[0];                           \


#define CMD_JE_CODE                                     \
        if (flag_registers[ZF] == 1)                    \
        {                                               \
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
                                
#define CMD_IN_CODE                                                         \
        a = 0;                                                  \
        scanf("%d", &a);                                            \
        if (arg_type[0] == '2') push(&cpu_data_stack, a);           \
        if (arg_type[0] == '0') *arg[0] = a;                        \

#define CMD_OUT_CODE                                        \
        a = 0;                                 \
        if (arg_type[0] == '2')                             \
        {                                                   \
            pop(&cpu_data_stack, &a);              \
            printf("%d\n", a);                       \
        }                                                   \
        if (arg_type[0] == '0') printf("%d\n", *arg[0]);      \

#define CMD_CMP_CODE                                                \
        if (*arg[0] > *arg[1])                                      \
        {                                                           \
            flag_registers[SF] = 0;                                 \
            flag_registers[ZF] = 0;                                 \
        }                                                           \
        if (*arg[0] == *arg[1])                                     \
        {                                                           \
            flag_registers[SF] = 0;                                 \
            flag_registers[ZF] = 1;                                 \
        }                                                           \
        if (*arg[0] < *arg[1])                                      \
        {                                                           \
            flag_registers[SF] = 1;                                 \
            flag_registers[ZF] = 0;                                 \
        }                                                           \


#define CMD_DEC_CODE                                                 \
        if (arg_type[0] == '2')                                      \
        {                                                            \
            pop(&cpu_data_stack, arg[0]);                            \
            (*arg[0])--;                                             \
            push(&cpu_data_stack, *arg[0]);                          \
        }                                                            \
        if (arg_type[0] == '0')                                      \
        {                                                            \
            (*arg[0])--;                                             \
        }                                                            \

#define CMD_INC_CODE                                                 \
        if (arg_type[0] == '2')                                      \
        {                                                            \
            pop(&cpu_data_stack, arg[0]);                            \
            (*arg[0])++;                                             \
            push(&cpu_data_stack, *arg[0]);                          \
        }                                                            \
        if (arg_type[0] == '0')                                      \
        {                                                            \
            (*arg[0])++;                                             \
        }                                                            \

#define CMD_CALL_CODE                                               \
        push(&cpu_call_stack, offset);                              \
        offset = *arg[0];                                           \


#define CMD_RTN_CODE                                                \
        pop(&cpu_call_stack, &offset);                              \

#define CMD_CPY_CODE                                                \
        pop(&cpu_data_stack, arg[0]);                               \
        push(&cpu_data_stack, *arg[0]);                             \
        push(&cpu_data_stack, *arg[0]);                             \


#define CMD_MUL_CODE                            \
        pop(&cpu_data_stack, &a);               \
        pop(&cpu_data_stack, &b);               \
        push(&cpu_data_stack, a * b);           \

#define CMD_DIV_CODE                            \
        pop(&cpu_data_stack, &a);               \
        pop(&cpu_data_stack, &b);               \
        push(&cpu_data_stack, a / b);           \

#define CMD_SQRT_CODE                       \
        pop(&cpu_data_stack, &a);           \
        push(&cpu_data_stack, sqrt(a));     \

#define CMD_SUB_CODE                            \
        pop(&cpu_data_stack, &a);               \
        pop(&cpu_data_stack, &b);               \
        push(&cpu_data_stack, a - b);           \


#define CMD_PRINT_CODE                  \
        printf("%c", *arg[0]);          \

#define CMD_DRAW_CODE                               \
        for (int i = 0; i < *arg[0]; i++)            \
        {                                           \
            for (int j = 0; j < *arg[1]; j++)        \
            {\
                switch (vram[i*(*arg[0]) + j])         \
                {                                           \
                    COLOUR(WHITE, "\033[40m \033[0m")   \
                    COLOUR(BLACK, "\033[107m \033[0m")            \
                }                                   \
            }                                       \
            printf("\n");                               \
        }                                           \









