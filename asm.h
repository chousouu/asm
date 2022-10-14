#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <stdlib.h>
#include <sys\stat.h>
#include <ctype.h>

const int LABEL_MAX = 50;

struct Label 
{
    int   label_to;
    char *label_name; 
};


#define ELSE_IF_CMP(CMND) else if(strincmp(cmd, #CMND, cmd_size) == 0) {machine_code_buffer[ip++] = CMD_##CMND; prev_push = 0; DEB("doing %s\n", #CMND);}

bool WriteBinaryFile(int *opcode_buffer, int tokens);
void RedundantSpaces(char *buffer);
int strincmp(const char *str1, const char *str2, int n);
char **GetString(char* buffer, int string_count);
char *ReadToBuffer(const char *filename);
int CountStrings(char *buffer);
int CountSymbols(const char *filename);
int *Assemble(char **stringed_buffer, struct Label *labels, int strings_count, int *machine_symbols_count);


#endif// ASM_H