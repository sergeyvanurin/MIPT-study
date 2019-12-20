#define STACKCONSTRUCT( _stack ) stack_construct(_stack, #_stack)
#define STACKDUMP( _stack, err ) stack_dump(_stack, err, __LINE__, __FILE__)
#define _DEBUG
typedef float stack_type;
#define OUT_TYPE "%0.2f"
#define IN_TYPE "%f"
#define TRANSLATE_FUNC atof

const int          CANARYCONST1     = 0xFAAAAAAC; //First canary in struct stacktyp value
const int          CANARYCONST2     = 0XFAAAAACC; //Second canary in struct stacktyp value
const int          DATACANARYCONST1 = 0xFAAAACCC; //First canary in data in struct stacktyp value
const int          DATACANARYCONST2 = 0xFAAACCCC; //Second canary in data in struct stacktyp value
const stack_type   POISON           = -123456789; //Poison value for empty stack elements
const float        STACK_INCREASER  = 2;          //Stack size increaser

enum stack_error_names                            //errors list
{
    STACK_POINTER = 1,
    DATA_POINTER,
    CANARY,
    IS_UNDERFLOW,
    IS_OVERFLOW,
    BAD_SIZE,
    BAD_HASH
};

enum stack_funcs                                  //funcs list
    {
        NEEDPOP,
        NEEDPUSH,
        CONSTRUCT,
        DESTRUCT
    };

struct stacktyp
{
    #ifdef _DEBUG

        long int canary_1;
        char* name;

    #endif

    stack_type* data;
    int size;
    int max_size;

    #ifdef _DEBUG

        long int canary_2;
        long long int hash_sum;

    #endif
};

int           stack_destruct(stacktyp* _stack);
bool          stack_construct(stacktyp* _stack, char stack_name[]);
bool          stack_push(stacktyp* _stack, stack_type value);
int           stack_pop(stacktyp* _stack, stack_type* result);
bool          change_stack_size(stacktyp* _stack, int parameter);

long long int get_hash(stacktyp* _stack);
int           stack_not_ok(stacktyp* _stack, int func);
bool          stack_dump(stacktyp* _stack, int err_int, int line, char* file);



