#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

//#define DUMP
#define DIAGNOSE
#define TEST_MODE

#define get_var_name( var ) #var

#ifdef DIAGNOSE
    #define $ON_DIAG( code ) code
#else
    #define $ON_DIAG( code ) 
#endif



#ifndef DUMP
    #define VERBOSE 0
#else
    #define VERBOSE 1
#endif




#define STACK_INIT(stack)                     \
        {                                     \
            stack.name = get_var_name(stack); \
            stack_init(&stack);               \
        }                                     \


#define VERIFICATION(stack, return_value)     \
    ERROR_CODE = verification(stack);         \
    if (ERROR_CODE != 0)                      \
    {                                         \
        if (VERBOSE)                          \
        {                                     \
            dump(stack, ERROR_CODE);          \
        }                                     \
        return return_value;                  \
    }                                         \

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
bool pop(struct dynamic_stack *stack, stack_elem_t *popped_element);

/*******************************************
/Функция, которая заносит эдемент в стек
/@param[in] указатель на структуру стэка
/@param[in] элемент
********************************************/
bool push(struct dynamic_stack *stack, stack_elem_t element);

/*******************************************
/Функция, которая хэширует структуру
/@param[in] указатель на структуру стэка
/param[out] хэш
********************************************/
static unsigned long hash_function(const void *first_ptr,const  void* last_pointer);

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
{   
    $ON_DIAG(char stack_canary_first;)
    
    stack_elem_t *data;
    int counter;
    int size;

    $ON_DIAG
    (
        const char *name;
        unsigned long data_hash;
        unsigned long structure_sum;
        char stack_canary_last;
    )
};



static unsigned long hash_function(const void *first_ptr, const void *last_ptr)
{
    unsigned long hash = 0;
    int i = 0;
    while((char*)first_ptr + i != (char*)last_ptr)
    {
        hash = hash + (unsigned long)*((char*)first_ptr + i);
        i++;
    }
    return hash;
}


static void update(struct dynamic_stack *stack)
{                                                                                                                                                      
    stack->data[-1] = (stack_elem_t)CANARY;                                                                                                                            
    stack->data[stack->size] = (stack_elem_t)CANARY;                                                                                                                  
    stack->data_hash = hash_function(stack->data, stack->data + stack->size + 1);                                                                                      
    stack->structure_sum = hash_function(&stack->stack_canary_first, &stack->structure_sum) + hash_function(&stack->structure_sum + 1, &stack->stack_canary_last + 1); 
}

void dump(struct dynamic_stack *stack, int code)
{
    printf("STACK: %s of type %s\n", stack->name, typeid(stack).name());

    if ((ERROR_CODE != DESTROYED_STACK) && (ERROR_CODE != MEMMORY_ALLOC_PROBLEMS))
    {
        printf("STACK adress is %p\n", stack);
        for (int i = 0; i < stack->size; i++)
        {
            printf("data[%d]: ", i + 1);
            std::cout << stack->data[i] << "\n";
        }
    }

    switch (code)
    {
        case OK:
            printf("stack is OK\n");
        break;


        $ON_DIAG
        (
            case DEAD_CANARY_IN_DATA:
                printf("ERROR %d DEAD CANARY IN DATA\n", code);
                printf("canaries are:\nfirst:%c   last:%c\n\n", (int)stack->data[-1], (int)stack->data[stack->size]);
                printf("should be   :\nfirst:%c   last:%c\n", CANARY, CANARY);
            break;


            case DEAD_CANARY_IN_STRUCTURE:
                printf("ERROR %d DEAD CANARY IN STRUCTURES\n", code);
                printf("canaries are:\nfirst:%d   last:%d\n\n", (int)stack->stack_canary_first, (int)stack->stack_canary_last);
                printf("should be:\nfirst:%d   last:%d\n", (int)CANARY, (int)CANARY);
            break;

            
            case WRONG_SUM:
                printf("ERROR %d WRONG HASH SUM\n", code);
                printf("data hash is %lu; should be %lu\n", hash_function(stack->data, stack->data + stack->size + 1), stack->data_hash);
                printf("structure hash is %lu; should be %lu\n", hash_function(&stack->stack_canary_first, &stack->structure_sum) + hash_function(&stack->structure_sum + 1, &stack->stack_canary_last + 1), stack->structure_sum);
            break;
        )


        case STACK_OVERFLOW:
            printf("ERROR %d STACK OVERFLOW\n", code);
            printf("Counter is %d but should be lesser than %d\n", stack->counter, stack->size);
        break;


        case STACK_UNDERFLOW:
            printf("ERROR %d STACK UNDERFLOW\n", code);
            printf("counter should not be lesser than 0 but is %d\n", stack->counter);
        break;


        case DESTROYED_STACK:
            printf("ERROR %d DESTROYED STACK\n", code);
            printf("STACK IS DESTROYED\n");
        break;


        case MEMMORY_ALLOC_PROBLEMS:
            printf("ERROR %d MEMORY ALLOCATION ERROR\n", code);
        break;
        };
}


bool stack_init(struct dynamic_stack *stack)
{
    assert(stack);
    stack_elem_t *mem_block = (stack_elem_t*)calloc(DEFAULT_STACK_SIZE + 2, sizeof(stack_elem_t));
    if (mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        if (VERBOSE) dump(stack, ERROR_CODE);
        return 0;
    }

    $ON_DIAG(stack->stack_canary_first = CANARY;)

    stack->data = mem_block + 1;
    stack->counter = 0;
    stack->size = DEFAULT_STACK_SIZE;

    $ON_DIAG
    (
        stack->stack_canary_last = CANARY;
        update(stack);
    )

    return 1;
}


int verification(struct dynamic_stack *stack)
{
    if (stack == NULL || stack->counter == POISON)
    {
        return DESTROYED_STACK;
    }
    $ON_DIAG
    (
        if (hash_function(stack->data, stack->data + stack->size + 1) != stack->data_hash || hash_function(&stack->stack_canary_first, &stack->structure_sum) + hash_function(&stack->structure_sum + 1, &stack->stack_canary_last + 1) != stack->structure_sum)
        {
            return WRONG_SUM;
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
    )

    if (stack->counter > stack->size)
    {
        return STACK_OVERFLOW;
    }

    if (stack->counter < 0)
    {
        return STACK_UNDERFLOW;
    }

    return 0;
}


bool stack_increase(struct dynamic_stack *stack)
{
    int new_size = sizeof(stack_elem_t) * stack->size * RESIZE_CONSTANT + 2 * sizeof(stack_elem_t);
    void *new_mem_block = realloc((void*)(stack->data - 1), new_size);
    if (new_mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        if (VERBOSE) dump(stack, ERROR_CODE);
        return 0;
    }
    stack->data = (stack_elem_t*)new_mem_block + 1;
    stack->size = stack->size * RESIZE_CONSTANT;
    $ON_DIAG(update(stack);)
    return 1;
}


bool stack_decrease(struct dynamic_stack *stack)
{
    int new_size = sizeof(stack_elem_t) * stack->size / RESIZE_CONSTANT + 2 * sizeof(stack_elem_t);
    void *new_mem_block = realloc((void*)(stack->data - 1), new_size);
    if (new_mem_block == NULL)
    {
        ERROR_CODE = MEMMORY_ALLOC_PROBLEMS;
        if (VERBOSE) dump(stack, ERROR_CODE);
        return 0;
    }
    stack->data = (stack_elem_t*)new_mem_block + 1;
    stack->size = stack->size / RESIZE_CONSTANT;
    $ON_DIAG(update(stack);)
    return 1;
}



bool push(struct dynamic_stack *stack, stack_elem_t element)
{
    VERIFICATION(stack, 0)
    if (stack->counter != stack->size)
    {
        stack->data[stack->counter] = element;
        stack->counter++;
        $ON_DIAG(update(stack);)
    }
    if (stack->counter == stack->size)
    {
        stack_increase(stack);
    }
    VERIFICATION(stack, 0)
    return 1;
}


bool pop(struct dynamic_stack *stack, stack_elem_t *popped_element)
{
    VERIFICATION(stack, 0)
    if (((stack->counter + HYSTERESIS) == (stack->size / RESIZE_CONSTANT)) && ((stack->counter + HYSTERESIS) >= DEFAULT_STACK_SIZE))
    {
        stack_decrease(stack);
    }
    stack->counter--;
    if (stack->counter >= 0)
    {
        stack_elem_t result = stack->data[stack->counter];
        stack->data[stack->counter] = POISON;
        $ON_DIAG(update(stack);)
        VERIFICATION(stack, 0)
        *popped_element = result;
        return 1; 
    }
    else
    {
        ERROR_CODE = STACK_UNDERFLOW;
        if (VERBOSE) dump(stack, ERROR_CODE);
        return 0;
    }
    return 0;
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
    for (int i = 0; i < 500; i++)
    {
        push(&test_stack, i);
        if (ERROR_CODE != 0)
        {
            dump(&test_stack, ERROR_CODE);
        }
    }
    for (int i = 0; i < 100; i++)
    {
        int a = 0;
        pop(&test_stack, &a);
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
    int a = 0;
    struct dynamic_stack test_stack;
    STACK_INIT(test_stack)
    for (int i = 0; i < 70; i++)
    {
        push(&test_stack, i);
    }
    for (int i = 0; i < 25; i++)
    {
        pop(&test_stack, &a);
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
    int a = 0;
    int result = 0; 
    struct dynamic_stack test_stack;
    STACK_INIT(test_stack)
    if (pop(&test_stack, &a) == 0 && ERROR_CODE != 0)
    {
        result++;
    }
    ERROR_CODE = 0;
    test_stack.counter++;
    for (int i = 0; i < 10; i++)
    {
        push(&test_stack, i);
    }
    test_stack.data[5] = 67;
    pop(&test_stack, &a);
    if (ERROR_CODE != 0)
    {
        result++;
    }
    ERROR_CODE = 0;
    test_stack.data_hash = hash_function(test_stack.data, test_stack.data + test_stack.size + 1);
    test_stack.counter++;
    push(&test_stack, 67);
    if (ERROR_CODE != 0)
    {
        result++;
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
    int a = 0;
    dynamic_stack test_stack = {};
    STACK_INIT (test_stack)
    pop(&test_stack, &a);
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

#define $do( code ) printf ("\n<<< " #code " START:\n"); code; printf (">>> " #code " END:\n\n"); 

int main()
{
    $do ( assert (drew_underflow ()); )

    //deinit_crash_test ();




    $do ( push_test();      )
    $do ( pop_test();       )
    $do (hysteresis_test(); )
    $do (corruption_test(); )
}