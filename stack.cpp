#include "stack.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
FILE *logfile = NULL;

#ifdef HASH_PROT
static long unsigned HashCounter(void *data, int len)
{
    unsigned long a = 0 , b = 0, c = 0;
    unsigned int fool      = 0xAF00L;
    unsigned int lightning = 1988  ;

    char *datawalker = (char*)data;
    int hash_counter = 0;
    while (len > 4)
    {
        a += fool++;
        b += fool++;
        c += fool++;

        c = a << 2;
        b = a << 3;
        a = a << 1;
        len -= 4;
    }
    c += (unsigned long)datawalker[2] << 8;
    b += (unsigned long)datawalker[1] << 4 ;
    a += (unsigned long)datawalker[0] << 1 ; 

    hash_counter = a * (fool) + b * (fool + lightning) + c * (fool + 311);

    return hash_counter;
}
#endif// HASH_PROT

static int *PoisonAllocStack(int start, int max)
{
    #ifdef CANARY_PROT
    int size_calloc = max + 2;
    #else 
    int size_calloc = max;
    #endif

    int* buff =(int*)calloc(size_calloc, sizeof(int));
    
    if(buff == NULL)
    {
        printf("Cannot allocate memory\n");
        return buff;
    }   
    
    #ifdef CANARY_PROT
        buff++;
    #endif

    FILL_POISON(buff, start, max);

    return buff;
}

static int StackResize(struct Stack *stack, bool mode)
{    
    if(mode == STACK_DECREASE)
    {   
        stack->capacity /= CAP_MULTIPLIER;    
    }
    else 
    {
        stack->capacity *= CAP_MULTIPLIER;
    }  
    
    #ifdef CANARY_PROT
        int *buff = (int*)realloc(stack->data - 1, (stack->capacity + 2)* sizeof(int));

        if(buff == NULL)
        {
            printf("Cannot allocate memory\n");
            return MEM_ALLOC_FAIL; 
        }   
            *buff = DATA_LEFT_CANARY;
            buff[stack->capacity] = DATA_RIGHT_CANARY;
            stack->data = buff;
            stack->data++;
    #else 
        int *buff = (int*)realloc(stack->data, stack->capacity * sizeof(int));
        if(buff == NULL)
        {
            printf("Cannot allocate memory\n");
            return MEM_ALLOC_FAIL; 
        }
        stack->data = buff;
    #endif

    FILL_POISON(stack->data, stack->size, stack->capacity);

    return 0; // on success
}

int StackCtor(struct Stack *stack, int capacity VAR_INFO)
{
    *stack = 
    {
        #ifdef CANARY_PROT
        .L_canary = STACK_LEFT_CANARY,
        #endif //CANARY_PROT

        .data = PoisonAllocStack(0, capacity),
        
        .capacity = capacity,
        .size = 0,

        #ifdef CANARY_PROT
        .R_canary = STACK_RIGHT_CANARY,
        #endif //CANARY_PROT        
        
        #ifdef HASH_PROT
        .HashValueData = 0,
        .HashValueStruct = 0,
        #endif //HASH_PROT 

        #ifdef DEBUG_INFO
        .line_ctor = line,
        .file_ctor = file,
        .line_action = 0,
        .file_action = NULL, 
        .action_funcname = NULL,
        .stackname = name,
        #endif //DEBUG_INFO
    };

    if(stack->data == NULL)
    {
        StackDump(stack, STACK_NULL);
        return STACK_NULL;
    }

    #ifdef CANARY_PROT
    *(stack->data - 1) = DATA_LEFT_CANARY;
    stack->data[stack->capacity] = DATA_RIGHT_CANARY;
    #endif// CANARY_PROT
    GET_STRUCT_HASH;
    GET_DATA_HASH;

    return 0;
}

int StackPush(struct Stack *stack, int number) 
{
    Stack_OK(stack);

    if(stack->size == stack->capacity - 1)
    {
        int err_resize = StackResize(stack, STACK_INCREASE);

        if(err_resize == MEM_ALLOC_FAIL)
        { 
            StackDump(stack, err_resize);
            return err_resize;
        }

        #ifdef CANARY_PROT
            stack->data[stack->capacity] = DATA_RIGHT_CANARY;
        #endif// CANARY_PROT
    }
    
    stack->data[stack->size] = number;
    stack->size++;

    GET_STRUCT_HASH;
    GET_DATA_HASH;

    Stack_OK(stack);

    return 0; 
}

int StackTop(struct Stack *stack, int *error_code)
{
    Stack_OK(stack);

    if(stack->size < 1 && (stack->data[stack->size] == POISON))
    {
        *error_code = TOP_EMPTY_STACK;
        StackDump(stack, *error_code);
        return *error_code;
    }

    Stack_OK(stack);
        
    return stack->data[stack->size - 1];
}

int StackPop(struct Stack *stack, int *error_code)
{
    Stack_OK(stack);

    if(stack->size < 1 && (stack->data[stack->size] == POISON))
    {
        *error_code = POP_EMPTY_STACK;
        StackDump(stack, *error_code);
        return *error_code;
    }

    if(stack->size == (stack->capacity / 4 - 1))
    {
        int err_resize = StackResize(stack, STACK_DECREASE);
        if(err_resize  == MEM_ALLOC_FAIL)
        {

            *error_code = err_resize; 
            StackDump(stack, *error_code);
            return *error_code;
        }
        #ifdef CANARY_PROT
            stack->data[stack->capacity] = DATA_RIGHT_CANARY;
        #endif//CANARY_PROT
    }

    stack->size--; 
    int temp = stack->data[stack->size];
    stack->data[stack->size] = POISON;

    GET_STRUCT_HASH;
    GET_DATA_HASH;

    Stack_OK(stack);
    return temp;
}

#ifdef DEBUG_INFO 
void GetActionInfo(struct Stack *stack VAR_INFO)
{
    stack->line_action = line; 
    stack->file_action = file;
    stack->action_funcname = name;
}
#endif// DEBUG_INFO

int StackVerify(struct Stack *stack) 
{
    int problem_code = 0;

    if(stack == NULL)
    {
        problem_code |= STACK_NULL;   
    }
    else
    {
        IF_ERR(stack->size < 0, NEGATIVE_SIZE);
        
        IF_ERR(stack->capacity < 0, NEGATIVE_CAPACITY);

        IF_ERR(stack->capacity < stack->size, CAP_SMALLER_SIZE);
        
        IF_ERR(stack->data == NULL, MEM_ALLOC_FAIL);

        #ifdef CANARY_PROT
        IF_ERR(stack->L_canary != STACK_LEFT_CANARY, S_LEFT_CANARY_DEAD);

        IF_ERR(stack->R_canary != STACK_RIGHT_CANARY, S_RIGHT_CANARY_DEAD);

        IF_ERR(*(stack->data - 1) != DATA_LEFT_CANARY, D_LEFT_CANARY_DEAD);
        
        IF_ERR(stack->data[stack->capacity] != DATA_RIGHT_CANARY, D_RIGHT_CANARY_DEAD);
        #endif// CANARY_PROT
        

        #ifdef HASH_PROT
        IF_ERR(stack->HashValueData != HashCounter(stack->data, stack->capacity * sizeof(int)), HASH_DATA_DEAD);

        IF_ERR(stack->HashValueStruct != HashCounter(stack, sizeof(stack->size) + sizeof(stack->capacity) + sizeof(stack->data)), HASH_STRUCT_DEAD);
        #endif// HASH_PROT
    }

    return problem_code;
}

void DecodeProblem(struct Stack *stack, int problem_code)
{
    FPRINT_ERR(logfile, STACK_NULL          , "STACK IS NULL\n");

    FPRINT_ERR(logfile, NEGATIVE_SIZE       , "Stack size cannot be negative. [stack.size = %d]\n", stack->size);

    FPRINT_ERR(logfile, NEGATIVE_CAPACITY   , "Stack capacity cannot be negative. [stack.capacity = %d]\n", stack->capacity);

    FPRINT_ERR(logfile, CAP_SMALLER_SIZE    , "Stack capacity cannot be smaller than size.\n"
                                           "\t[stack.capacity = %d]\n   [stack.size = %d]\n", 
                                                  stack->capacity      ,    stack->size);

    FPRINT_ERR(logfile, TOP_EMPTY_STACK     , "ERROR : ZERO STACK->SIZE TOP.\n");

    FPRINT_ERR(logfile, POP_EMPTY_STACK     , "ERROR : ZERO STACK->SIZE POP.\n");

    FPRINT_ERR(logfile, MEM_ALLOC_FAIL      , "Failed to allocate memory for stack.\n");

    #ifdef CANARY_PROT
    FPRINT_ERR(logfile, S_LEFT_CANARY_DEAD  , "Left canary attacked! (stack struct)\n");

    FPRINT_ERR(logfile, S_RIGHT_CANARY_DEAD , "Right canary attacked! (stack struct)\n");

    FPRINT_ERR(logfile, D_LEFT_CANARY_DEAD  , "Left canary attacked! (data)\n");

    FPRINT_ERR(logfile, D_RIGHT_CANARY_DEAD , "Right canary attacked! (data)\n");
    #endif// CANARY_PROT

    #ifdef HASH_PROT
        FPRINT_ERR(logfile, HASH_DATA_DEAD  , "HASH DATA CHANGED\n");
        FPRINT_ERR(logfile, HASH_STRUCT_DEAD, "HASH STRUCT CHANGED\n");
    #endif// HASH_PROT
}


void StackDump(struct Stack *stack, int problem_code) // ***********// 
{
#ifdef DEBUG_INFO
    logfile = fopen("log.txt", "a");

    if(logfile == NULL)
    {
        printf("Cannot open file\n");
    }
    else 
    {
        fprintf(logfile, "=====================\n");
        fprintf(logfile, "%s at %s (%d):\n"                         "Stack[%p](%s) \"%s\" at main()\n", 
  stack->action_funcname ,stack->file_action, stack->line_action,     stack, (problem_code ? "ERROR" : "ok"), stack->stackname);
        if(problem_code)
        {
            DecodeProblem(stack, problem_code); 

            fprintf(logfile, "%s(%d)\n", stack->file_ctor, stack->line_ctor);

            fprintf(logfile, "{\n");

            fprintf(logfile, "\tsize = %d\n\tcapacity = %d\n\tdata[%p]\n", stack->size, stack->capacity, stack->data);
            
            fprintf(logfile, "\t{\n");
                StackPrint(stack);
            fprintf(logfile, "\t}\n");
            
            fprintf(logfile, "}\n");
            
        }
        fclose(logfile);
    }
#endif//DEBUG_INFO
}

void StackPrint(struct Stack *stack)
{
    int i = 0;
    int print_to = stack->capacity;
    #ifdef CANARY_PROT
    i--;
    print_to++;
    #endif// CANARY_PROT
    for(; i < print_to; i++)
    {
        char        in_stack    =   ' ';
        const char *isPOISONED  = "(POISON)";
        const char *notPOISONED = " "; 
        const char *isCANARY    = "(CANARY)";
        const char *situation   = isPOISONED;

        if(*(stack->data + i) == DATA_LEFT_CANARY || *(stack->data + i) == DATA_RIGHT_CANARY)
        {
            in_stack = '#';
            situation = isCANARY;
        }
        else if(stack->data[i] != POISON)
        {
            in_stack = '*';
            situation = notPOISONED;
        }
        fprintf(logfile, "\t\t%c[%d] = %d%s\n", in_stack, i, *(stack->data + i), situation);
    }    

}


void StackDtor(struct Stack *stack)
{
    FILL_POISON(stack->data, 0, stack->capacity);

    stack->size = 0;
    stack->capacity = 0;
    free (stack->data - 1);
} 