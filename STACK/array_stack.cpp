#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#define get_var_name(var) #var
#define DIAGNOSE
#ifndef DIAGNOSE
#define verbose 0
#else
#define verbose 1
#endif

#define STACK_INIT(stack) \
        {                 \
            stack.name = get_var_name(stack); \
            stack_init(&stack); \
        }                       \

        
 
int ERROR_CODE = 0;
const char CANARY = 0xAB;
const int POISON = -666;
const int MAX_NAME_LENGTH = 30;
const float RESIZE_CONSTANT = 2;
const int DEFAULT_STACK_SIZE = 50;
const int HYSTERESIS = 10;
typedef int stack_elem_t;


enum ERROR_CODES
{
    OK,
    DEAD_CANARY_IN_DATA,
    DEAD_CANARY_IN_STRUCTURE,
    STACK_OVERFLOW,
    STACK_UNDERFLOW,
    WRONG_SUM,
    DESTROYED_STACK,
    MEMMORY_ALLOC_PROBLEMS
};


struct dynamic_stack
{   
    char stack_canary_first;
    stack_elem_t *data;
    int counter;
    int size;
    unsigned int hash;
    char stack_canary_last;
    const char *name;

};

unsigned int hash(struct dynamic_stack *stack)
{
    unsigned int hash = 0;

    for(int i = 0; i < stack->size; i++)
        hash = (hash * 1664525) + (unsigned char)stack->data[i] + 1013904223;
    return hash;
}


void stack_values(int i, struct dynamic_stack *stack)
{
    printf("data[%d]: ", i);
    std::cout << stack->data[i] << "\n";
}
void crash(struct dynamic_stack *stack, int code)
{
    switch (code)
    {
    case DEAD_CANARY_IN_DATA:
        if(verbose)
        {
        printf(" STACK: %s of type %s", stack->name, typeid(stack).name());
        printf("ERROR %d DEAD CANARY IN DATA\n", code);
        printf("canaries are:\nfirst:%c   last:%c\n\n", (int)stack->data[-1], (int)stack->data[stack->size]);
        printf("should be   :\nfirst:%c   last:%c\n", CANARY, CANARY);
        }
        exit(DEAD_CANARY_IN_DATA);
        break;
    case DEAD_CANARY_IN_STRUCTURE:
        if (verbose)
        {
            printf("ERROR %d DEAD CANARY IN STRUCTURES\n", code);
            printf("canaries are:\nfirst:%c   last:%c\n\n", (int)stack->stack_canary_first, (int)stack->stack_canary_last);
            printf("should be   :\nfirst:%c   last:%c\n", (int)CANARY, (int)CANARY);
        }
        exit(DEAD_CANARY_IN_STRUCTURE);
        break;
    case STACK_OVERFLOW:
        if (verbose)
        {
            printf("ERROR %d STACK OVERFLOW\n", code);
            for (int i = stack->size - 6; i < stack->size; i++)
            {
                stack_values(i, stack);
            }
            printf("counter should not be greater than %d but is %d", stack->size, stack->counter);
        }
        exit(STACK_OVERFLOW);
        break;
    case STACK_UNDERFLOW:
        if (verbose)
        {
            printf("ERROR %d STACK UNDERFLOW\n", code);
            for (int i = 5; i >= 0; i--)
            {
                stack_values(i, stack);
            }
            printf("counter should not be lesser than 0 but is %d", stack->counter);
        }
        exit(STACK_UNDERFLOW);
        break;
    case WRONG_SUM:
        if (verbose)
        {
            printf("ERROR %d WRONG HASH SUM\n", code);
            printf ("hash is %u but should be %u", stack->hash, hash(stack));
        }
        exit(WRONG_SUM);
        break;
    case DESTROYED_STACK:
        if (verbose)
        {
            printf("ERROR %d DESTROYED STACK\n", code);
            printf("STACK IS DESTROYED");
        }
        exit(DEFAULT_STACK_SIZE);
        break;
    case MEMMORY_ALLOC_PROBLEMS:
        if (verbose)
        {
            printf("ERROR %d MEMORY ALLOCATION ERROR\n", code);
        }
    };
}

void stack_init(struct dynamic_stack *stack)
{
    assert(stack);
    stack_elem_t *mem_block = (stack_elem_t*)calloc(DEFAULT_STACK_SIZE + 2, sizeof(stack_elem_t));
    mem_block[0] = (stack_elem_t)CANARY;
    mem_block[DEFAULT_STACK_SIZE + 1] = (stack_elem_t)CANARY;
    stack->stack_canary_first = CANARY;
    stack->data = mem_block + 1;
    stack->counter = 0;
    stack->size = DEFAULT_STACK_SIZE;
    stack->hash = hash(stack);
    stack->stack_canary_last = CANARY;
}


int verification(struct dynamic_stack *stack)
{
    if (stack->counter == POISON || stack == NULL)
    {
        return DESTROYED_STACK;
    }
    if (stack->data[-1] != CANARY || stack->data[stack->size] != CANARY)
    {
        return DEAD_CANARY_IN_DATA;
    }
    if (stack->stack_canary_first != CANARY || stack->stack_canary_last != CANARY)
    {
        return DEAD_CANARY_IN_STRUCTURE;
    }
    if (stack->counter > stack->size)
    {
        return STACK_OVERFLOW;
    }
    if (stack->counter < 0)
    {
        return STACK_UNDERFLOW;
    }
    if (hash(stack) != stack->hash)
    {
        return WRONG_SUM;
    }
    if (stack->data == NULL)
    {
        return MEMMORY_ALLOC_PROBLEMS;
    }
    return 0;
}


void stack_increase(struct dynamic_stack *stack)
{
    if (stack->counter == stack->size)
    {
        int new_size = sizeof(stack_elem_t) * stack->size * RESIZE_CONSTANT + 2 * sizeof(stack_elem_t);
        void *new_mem_block = realloc((void*)(stack->data - 1), new_size);
        stack->data = (stack_elem_t*)new_mem_block + 1;
        stack->size = stack->size * RESIZE_CONSTANT;
        stack->data[-1] = (stack_elem_t)CANARY;
        stack->data[stack->size] = (stack_elem_t)CANARY;
        stack->hash = hash(stack);
    }
}
void stack_decrease(struct dynamic_stack *stack)
{
    if (((stack->counter + HYSTERESIS) == (stack->size / RESIZE_CONSTANT)) && ((stack->counter + HYSTERESIS) >= DEFAULT_STACK_SIZE))
    {
        stack->data = ((stack_elem_t*)realloc(stack->data - 1, sizeof(stack_elem_t) * stack->size / RESIZE_CONSTANT + 2 * sizeof(stack_elem_t))) + 1;
        stack->size = stack->size / RESIZE_CONSTANT;
        stack->data[-1] = (stack_elem_t)CANARY;
        stack->data[stack->size] = (stack_elem_t)CANARY;
        stack->hash = hash(stack);
    }
}


void push(struct dynamic_stack *stack, stack_elem_t element)
{
    ERROR_CODE = verification(stack);
    if (ERROR_CODE == 0)
    {
        if (stack->counter != stack->size)
        {
            stack->data[stack->counter] = element;
            stack->counter++;
            stack->hash = hash(stack);
        }
        if (stack->counter == stack->size)
        {
            stack_increase(stack);
        }
    }
    else
    {
        crash(stack, ERROR_CODE);
    }
    ERROR_CODE = verification(stack);
    if (ERROR_CODE != 0)
    {
        crash(stack, ERROR_CODE);
    }
}


stack_elem_t pop(struct dynamic_stack *stack)
{
    ERROR_CODE = verification(stack);
    if (ERROR_CODE == 0)
    {
        stack_decrease(stack);
        stack->counter--;
        stack_elem_t result = stack->data[stack->counter];
        stack->data[stack->counter] = POISON;
        stack->hash = hash(stack);
        ERROR_CODE = verification(stack);
        if (ERROR_CODE == 0)
        {
            return result;
        }
        else
        {
            crash(stack, ERROR_CODE);
        }
    }
    else
    {
        crash(stack, ERROR_CODE);
    }
    return -1;
}


void stack_deinit(struct dynamic_stack *stack)
{
    ERROR_CODE = verification(stack);
    if (ERROR_CODE == 0)
    {
        free(stack->data - 1);
        stack->counter = POISON;
        stack->size = POISON;
        stack->hash = POISON;
        stack = NULL;
    }
    else
    {
        crash(stack, ERROR_CODE);
    }
}


int main()
{
    int a;
    struct dynamic_stack stack1;
    STACK_INIT(stack1);
    for (int i = 0; i < 70; i++)
    {
        push(&stack1, i);
    }
    stack1.data[-1] = 67;
    push(&stack1, 69);
}