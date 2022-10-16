#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
enum LABELS
{
    LABEL_MAX  = 50,
    LABEL_TO_UNTOUCHED = -1,
};
    const unsigned long LABEL_USED = 0x1ABE11ED; 
    const unsigned long LABEL_FREE = 0xCEA5ED; 

struct Label 
{
    int label_to;
    unsigned long label_hash;
};


#define ELSE_IF_CMP(CMND) else if(strincmp(cmd, #CMND, cmd_size) == 0) {machine_code_buffer[ip++] = CMD_##CMND; prev_push = 0; DEB("doing %s\n", #CMND);}

int AddLabel(struct Label *labels, char *str, int ip);
void RemoveComments(char *stringed_buffer);
int SearchLabel(struct Label *labels, unsigned long hash);
int jmp_to(struct Label *labels, char *str);

bool WriteBinaryFile(int *opcode_buffer, int tokens);
int strincmp(const char *str1, const char *str2, int n);
char **GetString(char* buffer, int string_count);
void RedundantSpaces(char *buffer, int size);
char *ReadToBuffer(const char *filename, int size);
int CountSymbols(const char *filename);
int CountStrings(char *buffer);
int *Assemble(char **stringed_buffer, struct Label *labels, int strings_count, int *machine_symbols_count);


#endif// ASM_H