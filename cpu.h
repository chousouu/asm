#ifndef CPU_H
#define CPU_H

#include <sys/stat.h>

const int REG_COUNT = 5;
const int RAM_MAX = 40;
const int OK = 999;
const int NOT_OK = -999;

struct CPU
{
    int *machine_inst; 
    int tokens;
    int reg[REG_COUNT]; // [1] = rax [2] = rbx, rcx, rdx
    int RAM[RAM_MAX];
};


const int LILY_REVERTED = 1498171724;
int   GetArgument   (struct CPU *cpu, int *ip)            ;

int PopIn(struct Stack *stk1, struct CPU *cpu, int *ip);

bool  CheckSignature(int *machine_inst)                                            ;

int  *ReadToBuffer  (const char *filename, int size)                                      ;

int   CountInts     (const char *filename)                                                ;

void Run(struct Stack *stk1, struct CPU *cpu);

void DEBUGStackPrint(struct Stack *stack);

void Listing(struct CPU *cpu);

#endif