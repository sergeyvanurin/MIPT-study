#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#define DIAGNOSE

#define get_var_name( var ) #var

#ifdef DIAGNOSE
#define $ON_DIAG( code ) code
#else
#define $ON_DIAG( code ) 
#endif


#ifndef DIAGNOSE
#define VERBOSE 0
#else
#define VERBOSE 1
#endif


#define UPDATE( stack )                                  \
        stack->data[-1] = (stack_elem_t)CANARY;          \
        stack->data[stack->size] = (stack_elem_t)CANARY; \
        stack->data_hash = data_hash(stack);             \
        stack->structure_sum = structure_hash(stack);    \

#define STACK_INIT( stack )                   \
        {                                     \
            stack.name = get_var_name(stack); \
            stack_init(&stack);               \
        }                                     \


#define VERIFICATION( stack )          \
    ERROR_CODE = verification(stack);  \
    if (ERROR_CODE != 0)               \
    {                                  \
        return POISON;                 \
    }                                  \

int ERROR_CODE = 0;

const   char  CANARY = 0xAB;
const   int   POISON = -666;
const   float RESIZE_CONSTANT = 2;
const   int DEFAULT_STACK_SIZE = 50;
const   int HYSTERESIS = 10;
typedef int stack_elem_t;



/*******************************************
/Функция, которая инциализирует стэк
/@param[in] указатель на структуру стэка
********************************************/
void stack_init(struct dynamic_stack *stack);

/*******************************************
/Функция, забирающая значение с верхушки
/стэка
/@param[in] указатель на структуру стэка
/@param[out] элемент стэка
********************************************/
stack_elem_t pop(struct dynamic_stack *stack);

/*******************************************
/Функция, которая заносит эдемент в стек
/@param[in] указатель на структуру стэка
/@param[in] элемент
********************************************/
int push(struct dynamic_stack *stack, stack_elem_t element);

/*******************************************
/Функция, которая хэширует данные
/@param[in] указатель на структуру стэка
/param[out] хэш
********************************************/
unsigned int data_hash(struct dynamic_stack *stack);

/*******************************************
/Функция, которая хэширует структуру
/@param[in] указатель на структуру стэка
/param[out] хэш
********************************************/
unsigned long structure_hash(struct dynamic_stack *stack);

/*******************************************
/Функция, которая уменьшает стэк с гистрезисом
/@param[in] указатель на структуру стэка
********************************************/
int stack_decrease(struct dynamic_stack *stack);

/*******************************************
/Функция, которая увеличивает стэк
/@param[in] указатель на структуру стэка
********************************************/
int stack_increase(struct dynamic_stack *stack);

/*******************************************
/Функция, которая уничтажает стэк
/@param[in] указатель на структуру стэка
********************************************/
void stack_deinit(struct dynamic_stack *stack);

/*******************************************
/Функция, которая печатает i-ый элемент стэка
/@param[in] указатель на структуру стэка
********************************************/
void stack_values(int i, struct dynamic_stack *stack);

/*******************************************
/Функция, которая проеверяет стэк
/@param[in] указатель на структуру стэка
/@param[out] код ошибки
********************************************/
int verification(struct dynamic_stack *stack);


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
{   $ON_DIAG(char stack_canary_first;)
    
    stack_elem_t *data;
    int counter;
    int size;

    $ON_DIAG
    (
    const char *name;
    unsigned int data_hash;
    unsigned long structure_sum;
    char stack_canary_last;
    )
};


unsigned int data_hash(struct dynamic_stack *stack)
{
    $ON_DIAG
    (
        unsigned int hash = 0;

        for (int i = 0; i < stack->size; i++)
        {
            hash = (hash * 1664525) + (unsigned char)stack->data[i] + 1013904223;
        }
        return hash;
    )
}


unsigned long structure_hash(struct dynamic_stack *stack)
{
    $ON_DIAG
    (
        unsigned long hash = 0;
        char i = 0;
        while (((char*)&stack->stack_canary_first + i) != (char*)&(stack->stack_canary_last))
        {
            if((&stack->stack_canary_first + i) != (char*)&(stack->structure_sum))
            {
                hash = hash + (long)*(&stack->stack_canary_first + i);
                i++;
            }
            else
            {
                i = i + sizeof(long);
            }
        }   
        return hash;
    )
}


void stack_values(int i, struct dynamic_stack *stack)
{
    if (i >= 0 && i < stack->counter)
    {
        printf("data[%d]: ", i);
        std::cout << stack->data[i] << "\n";
    }
}


void dump(struct dynamic_stack *stack, int code)
{
    $ON_DIAG
    (
    printf("STACK: %s of type %s\n", stack->name, typeid(stack).name());
    switch (code)
    {
    case OK:
        if(VERBOSE)
        {
            printf("stack is OK\n");
        }
        break;


    case DEAD_CANARY_IN_DATA:
        if(VERBOSE)
        {
            printf("ERROR %d DEAD CANARY IN DATA\n", code);
            printf("canaries are:\nfirst:%c   last:%c\n\n", (int)stack->data[-1], (int)stack->data[stack->size]);
            printf("should be   :\nfirst:%c   last:%c\n", CANARY, CANARY);
        }
        break;


    case DEAD_CANARY_IN_STRUCTURE:
        if (VERBOSE)
        {
            printf("ERROR %d DEAD CANARY IN STRUCTURES\n", code);
            printf("canaries are:\nfirst:%d   last:%d\n\n", (int)stack->stack_canary_first, (int)stack->stack_canary_last);
            printf("should be:\nfirst:%d   last:%d\n", (int)CANARY, (int)CANARY);
        }
        break;


    case STACK_OVERFLOW:
        if (VERBOSE)
        {
            printf("ERROR %d STACK OVERFLOW\n", code);
            printf("Counter is %d but should be lesser than %d", stack->stack_canary_last, stack->size);
        }
        break;


    case STACK_UNDERFLOW:
        if (VERBOSE)
        {
            printf("ERROR %d STACK UNDERFLOW\n", code);
            printf("counter should not be lesser than 0 but is %d", stack->counter);
        }
        break;


    case WRONG_SUM:
        if (VERBOSE)
        {
            printf("ERROR %d WRONG HASH SUM\n", code);
            printf ("data hash is %u; should be %u\n", stack->data_hash, data_hash(stack));
            printf ("structure hash is %lu; should be %lu\n", stack->structure_sum, structure_hash(stack));
        }
        break;


    case DESTROYED_STACK:
        if (VERBOSE)
        {
            printf("STACK: %s of type %s\n", stack->name, typeid(stack).name());
            printf("ERROR %d DESTROYED STACK\n", code);
            printf("STACK IS DESTROYED");
        }
        break;


    case MEMMORY_ALLOC_PROBLEMS:
        if (VERBOSE)
        {
            printf("ERROR %d MEMORY ALLOCATION ERROR\n", code);
        }
        break;

    };
    )
}


void stack_init(struct dynamic_stack *stack)
{
    assert(stack);
    stack_elem_t *mem_block = (stack_elem_t*)calloc(DEFAULT_STACK_SIZE + 2, sizeof(stack_elem_t));
    if (mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        dump(stack, ERROR_CODE);
        assert(0);
    }
    $ON_DIAG(stack->stack_canary_first = CANARY;)

    stack->data = mem_block + 1;
    stack->counter = 0;
    stack->size = DEFAULT_STACK_SIZE;
    $ON_DIAG
    (
        UPDATE(stack)
        stack->stack_canary_last = CANARY;
    )
}


int verification(struct dynamic_stack *stack)
{
    $ON_DIAG(
    if (stack == NULL || stack->counter == POISON)
    {
        return DESTROYED_STACK;
    }
    if (stack->data == NULL)
    {
        return MEMMORY_ALLOC_PROBLEMS;
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
    if (data_hash(stack) != stack->data_hash || structure_hash(stack) != stack->structure_sum)
    {
        return WRONG_SUM;
    }
    return 0;
    )
}


int stack_increase(struct dynamic_stack *stack)
{
    int new_size = sizeof(stack_elem_t) * stack->size * RESIZE_CONSTANT + 2 * sizeof(stack_elem_t);
    void *new_mem_block = realloc((void*)(stack->data - 1), new_size);
    if (new_mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        return 1;
    }
    stack->data = (stack_elem_t*)new_mem_block + 1;
    stack->size = stack->size * RESIZE_CONSTANT;
    $ON_DIAG(UPDATE(stack))
    return 0;
}


int stack_decrease(struct dynamic_stack *stack)
{
    if (((stack->counter + HYSTERESIS) == (stack->size / RESIZE_CONSTANT)) && ((stack->counter + HYSTERESIS) >= DEFAULT_STACK_SIZE))
    {
        int new_size = sizeof(stack_elem_t) * stack->size / RESIZE_CONSTANT + 2 * sizeof(stack_elem_t);
        void *new_mem_block = realloc((void*)(stack->data - 1), new_size);
        if (new_mem_block == NULL)
        {
            ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
            return 1;
        }
        stack->data = (stack_elem_t*)new_mem_block + 1;
        stack->size = stack->size / RESIZE_CONSTANT;
        $ON_DIAG(UPDATE(stack));
        return 0;
    }
    return 0;
}


int push(struct dynamic_stack *stack, stack_elem_t element)
{
    $ON_DIAG(VERIFICATION(stack))


    if (stack->counter != stack->size)
    {
        stack->data[stack->counter] = element;
        stack->counter++;
        $ON_DIAG(UPDATE(stack))
    }
    if (stack->counter == stack->size)
    {
        assert(!(stack_increase(stack)));
    }


    $ON_DIAG(VERIFICATION(stack))
    return 0;
}


stack_elem_t pop(struct dynamic_stack *stack)
{
    $ON_DIAG(VERIFICATION(stack))


    if (stack->counter > 0)
    {
        assert(!(stack_decrease(stack)));
        stack->counter--;
        stack_elem_t result = stack->data[stack->counter];
        stack->data[stack->counter] = POISON;


        $ON_DIAG(UPDATE(stack));


        $ON_DIAG(VERIFICATION(stack))


        return result;
    }
    if (stack->counter == 0)
    {
        printf("stack is empty!");
        return 0;
    }
    return 0;
}


void stack_deinit(struct dynamic_stack *stack)
{
    ERROR_CODE = verification(stack);
    if (ERROR_CODE == 0)
    assert(stack->data - 1);
    free(stack->data - 1);
    stack->counter = POISON;
    stack->size = POISON; 
    $ON_DIAG
    (
    stack->data_hash = POISON; 
    stack->structure_sum = POISON;
    stack = NULL;
    )
}


int main()
{
    struct dynamic_stack stack1;
    STACK_INIT(stack1)
    
    /*
    for (int i = 0; i < 500; i++)
    {
        push(&stack1, i);
        stack_values(i, &stack1);
    }
     if(ERROR_CODE == 0 )printf("test 1: PASSED\n");


    for (int i = 499; i >= 0; i--)
    {
        printf("%d ", pop(&stack1));
        printf("%lu\n", stack1.structure_sum);
    }
    if(ERROR_CODE == 0 )printf("test 2: PASSED\n");

    
    for (int i = 0; i <= 55; i++)
    {
        push(&stack1, i);
        printf("%d\n\n", stack1.size);
    }
    for (int i = 55; i >= 45; i--)
    {
        pop(&stack1);
    }
    if (stack1.counter == 45 && stack1.size == 100)
    {
         if(ERROR_CODE == 0 ) printf("test 3:PASSED\n");
    }

    
    for (int i = 0; i < 10; i++)
    {
        push(&stack1, i);
    }
    stack1.data[5] = 567;
    pop(&stack1);
    printf("%d\n", ERROR_CODE);
    ERROR_CODE = 0;
    
    for (int i = 0; i < 10; i++)
    {
        push(&stack1, i);
    }
    stack1.data_hash = 5647;
    pop(&stack1);
    printf("%d\n", ERROR_CODE);

    ERROR_CODE = 0;

    for (int i = 0; i < 10; i++)
    {
        push(&stack1, i);
    }
    stack1.structure_sum = 5648;
    pop(&stack1);

    printf("%d %u %lu\n", ERROR_CODE, stack1.data_hash, stack1.structure_sum);
    ERROR_CODE =  0;
    */
    /*for (int i = 0; i < 10; i++)
    {
        push(&stack1, i);
    }*/
}