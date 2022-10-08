#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DEBUG_INFO
#define CANARY_PROT
#define HASH_PROT

enum Options 
{
    CAP_MULTIPLIER = 2,
    STACK_DECREASE = 1, 
    STACK_INCREASE = 0,
};

enum ERRORS
{
    POP_EMPTY_STACK     = 1 << 0,
    TOP_EMPTY_STACK     = 1 << 1 ,
    NEGATIVE_SIZE       = 1 << 2 ,
    NEGATIVE_CAPACITY   = 1 << 3 ,
    CAP_SMALLER_SIZE    = 1 << 4 ,
    STACK_NULL          = 1 << 5 ,
    MEM_ALLOC_FAIL      = 1 << 6 ,
    S_LEFT_CANARY_DEAD  = 1 << 7 ,
    S_RIGHT_CANARY_DEAD = 1 << 8 ,
    D_LEFT_CANARY_DEAD  = 1 << 9 ,
    D_RIGHT_CANARY_DEAD = 1 << 10,
    HASH_DATA_DEAD      = 1 << 11,
    HASH_STRUCT_DEAD    = 1 << 12,
};

enum HexConst 
{
    POISON              = 0x91DF00 ,
    STACK_LEFT_CANARY   = 0x5BE4   ,
    STACK_RIGHT_CANARY  = 0x5AF1E91,
    DATA_LEFT_CANARY    = 0xDBE4   , 
    DATA_RIGHT_CANARY   = 0xDAF1E91, 
};

#ifdef CANARY_PROT
    #define  ON_CANARY_PROT(SIDE) int SIDE##_canary;
#else 
    #define ON_CANARY_PROT(SIDE) 
#endif //CANARY_PROT

#ifdef HASH_PROT
    #define  ON_HASH_PROT         \
    unsigned long HashValueData;  \
    unsigned long HashValueStruct;
#else 
    #define ON_HASH_PROT 
#endif //HASH_PROT

 
struct Stack
{
    ON_CANARY_PROT(L)

    int *data;
    int  capacity;
    int  size;
    
    ON_CANARY_PROT(R)

    ON_HASH_PROT

    #ifdef DEBUG_INFO
    int          line_ctor;
    const char  *file_ctor;
    int          line_action;
    const char  *file_action;
    const char  *action_funcname;
    const char  *stackname;
    #endif //DEBUG_INFO
};


#define IF_ERR(ERROR, ERRCODE) do { if((ERROR)) problem_code |= (ERRCODE); } while(0)

#define FPRINT_ERR(FILENAME, ERRCODE, ...) do { if(problem_code & ERRCODE)fprintf(FILENAME, __VA_ARGS__); } while(0) 

#define FILL_POISON(arr, start, end)     \
        do                               \
        {                                \
        for(int i = start; i < end; i++) \
        arr[i] = POISON;                 \
        } while(0) 

#ifdef HASH_PROT
    #define GET_DATA_HASH stack->HashValueData = HashCounter(stack->data, stack->capacity * sizeof(int))

    #define GET_STRUCT_HASH                                                                                         \
    stack->HashValueStruct = HashCounter(stack, sizeof(stack->size) + sizeof(stack->capacity) + sizeof(stack->data))
#else
    #define GET_DATA_HASH  
    #define GET_STRUCT_HASH   
#endif

#ifdef DEBUG_INFO
    #define VAR_INFO , int line, const char* file, const char *name
    
    #define Stack_OK(stack)        \
    do                             \
    {                              \
    int err_code = 0;              \
    err_code = StackVerify(stack); \
    StackDump(stack, err_code);    \
    if(err_code) return err_code;  \
    } while(0)

    #define StackCtor_(stack, X)   StackCtor(&stack, X, __LINE__, __FILE__, #stack)
    
    #define StackPush_(stack, X)                              \
    do                                                        \
    {                                                         \
    GetActionInfo(&stack, __LINE__, __FILE__, "StackPush()"); \
    StackPush    (&stack, X);                                 \
    } while (0)

    #define StackPop_(stack, err)                             \
    do                                                        \
    {                                                         \
    GetActionInfo(&stack, __LINE__, __FILE__, "StackPop()");  \
    StackPop     (&stack, &err);                              \
    } while (0)

    #define StackTop_(stack, err)                             \
    do                                                        \
    {                                                         \
    GetActionInfo(&stack, __LINE__, __FILE__, "StackTop()");  \
    StackTop     (&stack, &err);                              \
    } while(0)
    
#else 
    #define VAR_INFO 
    #define Stack_OK(stack) 
    #define StackCtor_(stack, X)  StackCtor(&stack, X)
    #define StackPush_(stack, X)  StackPush(&stack, X) 
    #define StackPop_(stack, err) StackPop(&stack, &err)    
#endif



int  StackCtor    (struct Stack *stack, int number VAR_INFO);
int  StackPush    (struct Stack *stack, int number)         ; 
int  StackPop     (Stack *stack, int *err_code)             ;
int  StackTop     (struct Stack *stack, int *error_code)    ;

void GetActionInfo(struct Stack *stack VAR_INFO)            ;
void StackDump    (struct Stack *stack, int problem_code)   ;
int  StackVerify  (struct Stack *stack)                     ;
void DecodeProblem(struct Stack *stack, int problem_code)   ;

void StackPrint   (struct Stack *stack)                     ;
void StackDtor    (struct Stack *stack)                     ;

#endif //STACK_H