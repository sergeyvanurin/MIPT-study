#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#define DIAGNOSE
#define TEST_MODE

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


#define UPDATE(stack)                                    \
        stack->data[-1] = (stack_elem_t)CANARY;          \
        stack->data[stack->size] = (stack_elem_t)CANARY; \
        stack->data_hash = data_hash(stack);             \
        stack->structure_sum = structure_hash(stack);    \



#define STACK_INIT(stack)                     \
        {                                     \
            stack.name = get_var_name(stack); \
            stack_init(&stack);               \
        }                                     \


#define VERIFICATION(stack, return_value)   \
    ERROR_CODE = verification(stack);       \
    if (ERROR_CODE != 0)                    \
    {                                       \
        return return_value;                \
    }                                       \

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
bool stack_init(struct dynamic_stack *stack);

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
bool push(struct dynamic_stack *stack, stack_elem_t element);

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
bool stack_decrease(struct dynamic_stack *stack);

/*******************************************
/Функция, которая увеличивает стэк
/@param[in] указатель на структуру стэка
********************************************/
bool stack_increase(struct dynamic_stack *stack);

/*******************************************
/Функция, которая уничтажает стэк
/@param[in] указатель на структуру стэка
********************************************/
bool stack_deinit(struct dynamic_stack *stack);

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
            if ((&stack->stack_canary_first + i) != (char*)&(stack->structure_sum))
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
    if (i >= 0 && stack != NULL && stack->data != NULL)
    {
        printf("data[%d]: ", i);
        std::cout << stack->data[i] << "\n";
    }
}


void dump(struct dynamic_stack *stack, int code)
{
    $ON_DIAG
    (
        if (VERBOSE)
        {
            printf("STACK: %s of type %s\n", stack->name, typeid(stack).name());
        }
        if ((ERROR_CODE != DESTROYED_STACK) && (ERROR_CODE != MEMMORY_ALLOC_PROBLEMS))
        {
            printf("STACK adress is %p\n", stack);
            for (int i = 0; i < stack->size; i++)
            {
                stack_values(i, stack);
            }
        }
        switch (code)
        {
        case OK:
            if (VERBOSE)
            {
                printf("stack is OK\n");
            }
            break;


        case DEAD_CANARY_IN_DATA:
            if (VERBOSE)
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
                printf("Counter is %d but should be lesser than %d\n", stack->stack_canary_last, stack->size);
            }
            break;


        case STACK_UNDERFLOW:
            if (VERBOSE)
            {
                printf("ERROR %d STACK UNDERFLOW\n", code);
                printf("counter should not be lesser than 0 but is %d\n", stack->counter);
            }
            break;


        case WRONG_SUM:
            if (VERBOSE)
            {
                printf("ERROR %d WRONG HASH SUM\n", code);
                printf("data hash is %u; should be %u\n", data_hash(stack), stack->data_hash);
                printf("structure hash is %lu; should be %lu\n", structure_hash(stack), stack->structure_sum);
            }
            break;


        case DESTROYED_STACK:
            if (VERBOSE)
            {
                printf("STACK: %s of type %s\n", stack->name, typeid(stack).name());
                printf("ERROR %d DESTROYED STACK\n", code);
                printf("STACK IS DESTROYED\n");
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


bool stack_init(struct dynamic_stack *stack)
{
    assert(stack);
    stack_elem_t *mem_block = (stack_elem_t*)calloc(DEFAULT_STACK_SIZE + 2, sizeof(stack_elem_t));
    if (mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        $ON_DIAG(dump(stack, ERROR_CODE);)
        return 0;
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
    return 1;
}


int verification(struct dynamic_stack *stack)
{
    $ON_DIAG
    (
        if (stack->counter == POISON)
        {
            return DESTROYED_STACK;
        }
        if (stack->data - 1 == NULL)
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


bool stack_increase(struct dynamic_stack *stack)
{
    int new_size = sizeof(stack_elem_t) * stack->size * RESIZE_CONSTANT + 2 * sizeof(stack_elem_t);
    void *new_mem_block = realloc((void*)(stack->data - 1), new_size);
    if (new_mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        return 0;
    }
    stack->data = (stack_elem_t*)new_mem_block + 1;
    stack->size = stack->size * RESIZE_CONSTANT;
    $ON_DIAG(UPDATE(stack))
    return 1;
}


bool stack_decrease(struct dynamic_stack *stack)
{
    int new_size = sizeof(stack_elem_t) * stack->size / RESIZE_CONSTANT + 2 * sizeof(stack_elem_t);
    void *new_mem_block = realloc((void*)(stack->data - 1), new_size);
    if (new_mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        return 0;
    }
    stack->data = (stack_elem_t*)new_mem_block + 1;
    stack->size = stack->size / RESIZE_CONSTANT;
    $ON_DIAG(UPDATE(stack));
    return 1;
}



bool push(struct dynamic_stack *stack, stack_elem_t element)
{
    VERIFICATION(stack, 0)
    if (stack->counter != stack->size)
    {
        stack->data[stack->counter] = element;
        stack->counter++;
        $ON_DIAG(UPDATE(stack))
    }
    if (stack->counter == stack->size)
    {
        stack_increase(stack);
    }
    VERIFICATION(stack, 0)
    return 1;
}


stack_elem_t pop(struct dynamic_stack *stack)
{
    VERIFICATION(stack, POISON)
    if (((stack->counter + HYSTERESIS) == (stack->size / RESIZE_CONSTANT)) && ((stack->counter + HYSTERESIS) >= DEFAULT_STACK_SIZE))
    {
        stack_decrease(stack);
    }
    stack->counter--;
    if (stack->counter >= 0)
    {
        stack_elem_t result = stack->data[stack->counter];
        stack->data[stack->counter] = POISON;
        $ON_DIAG(UPDATE(stack));
        VERIFICATION(stack, 0)
        return result;
    }
    else
    {
        ERROR_CODE = STACK_UNDERFLOW;
        return POISON;
    }
    return POISON;
}


bool stack_deinit(struct dynamic_stack *stack)
{
    VERIFICATION(stack, 0)
    free(stack->data - 1);
    //stack->data = NULL;
    //stack->counter = POISON;
    return 1;
}






#ifdef TEST_MODE


bool push_test()
{
    struct dynamic_stack test_stack;
    STACK_INIT(test_stack)
    for (int i = 0; i < 500; i++)
    {
        push(&test_stack, i);
        if (ERROR_CODE != 0)
        {
            dump(&test_stack, ERROR_CODE);
            printf("PUSH 500 test failed\n");
            return 0;
        }
    }
    printf("PUSH 500 test passed\n");
    return 1;
}

bool pop_test()
{
    struct dynamic_stack test_stack;
    STACK_INIT(test_stack)
    for (int i = 0; i < 100; i++)
    {
        push(&test_stack, i);
    }
    for (int i = 0; i < 100; i++)
    {
        int a = 0;
        a = pop(&test_stack);
        if (ERROR_CODE != 0 || a == POISON)
        {
            dump(&test_stack, ERROR_CODE);
            printf("POP 100 test failed\n");
            return 0;
        }
    }
    printf("POP 100 test passed\n");
    return 1;
}
bool hysteresis_test()
{
    struct dynamic_stack test_stack;
    STACK_INIT(test_stack)
    for (int i = 0; i < 70; i++)
    {
        push(&test_stack, i);
    }
    for (int i = 0; i < 25; i++)
    {
        pop(&test_stack);
    }
    if (test_stack.counter == 45 && test_stack.size == 100)
    {
        printf("HYSTERESIS test passed\n");
        return 1;
    }
    else
    {
        printf("HYSTERESIS test failed\n");
        return 0;
    }
    return 0;
}
bool corruption_test()
{
    int result = 0; 
    struct dynamic_stack test_stack;
    STACK_INIT(test_stack)
    if (pop(&test_stack) == POISON && ERROR_CODE != 0)
    {
        result++;
        dump(&test_stack, ERROR_CODE);
    }
    ERROR_CODE = 0;
    test_stack.counter++;
    for (int i = 0; i < 10; i++)
    {
        push(&test_stack, i);
    }
    test_stack.data[5] = 67;
    pop(&test_stack);
    if (ERROR_CODE != 0)
    {
        result++;
        dump(&test_stack, ERROR_CODE);
    }
    ERROR_CODE = 0;
    test_stack.data_hash = data_hash(&test_stack);
    test_stack.counter++;
    push(&test_stack, 67);
    if (ERROR_CODE != 0)
    {
        result++;
        dump(&test_stack, ERROR_CODE);
    }
    if (result == 3)
    {
        printf("CORRUPTION test passed\n");
        return 1;
    }
    else
    {
        printf("CORRUPTION test failed\n");
        return 0;
    }
    return 0;
}
#endif

bool drew_underflow ()
{
    dynamic_stack test_stack = {};
    STACK_INIT (test_stack)
    pop (&test_stack);
    if (ERROR_CODE == STACK_UNDERFLOW) return 1;
    else return 0;
}

bool deinit_crash_test()
{
    dynamic_stack test_stack = {};
    STACK_INIT (test_stack)
    push (&test_stack, 10);
    stack_deinit (&test_stack);
    stack_deinit (&test_stack);
    return 1;
}
int main()
{
    //assert (drew_underflow ());

    //deinit_crash_test ();




    push_test();
    pop_test();
    hysteresis_test();
    corruption_test();
}