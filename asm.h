#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <ctype.h>

enum CMD
{
    CMD_PUSH  = 1,
    CMD_POP   = 2,
    CMD_ADD   = 3,
    CMD_MUL   = 4,
    CMD_DIV   = 5,
    CMD_DUP   = 6,
    CMD_JMP   = 7,
    CMD_OUT   = 10,
    CMD_HALT  = 11,
};

const int LABEL_AMOUNT = 50;

struct Label 
{
    int   label_to;
    char *label_name; 
};

//#define DEBUG_MODE

#ifdef DEBUG_MODE
    #define DEB(...) printf(__VA_ARGS__)
#else 
    #define DEB(...) 
#endif// DEBUG_MODE

#define ELSE_IF_CMP(CMND) else if(strincmp(cmd, #CMND, cmd_size) == 0) {machine_code_buffer[ip++] = CMD_##CMND; prev_push = 0;}

FILE *WriteBinaryFile(int *machine_code_buffer, int machine_symbols_count);
void OddSpaceRemoveArray(char *buffer);
char **GetString(char* buffer, int string_count);
char *ReadToBuffer(const char *filename);
int CountString(char *buffer);
int strincmp(const char *str1, const char *str2, int n);
int CountSymbols(const char *filename);
int *Assembly(char **stringed_buffer, int strings_count, int *machine_symbols_count);


#endif// ASM_H