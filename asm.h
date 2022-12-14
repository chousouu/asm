#ifndef ASM_H
#define ASM_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>

enum LABELS
{
    CMD_USES_LABEL = 12,
    LABEL_MAX = 50,
    LABEL_TO_UNTOUCHED = -1,
};
const unsigned long LABEL_USED = 0x1ABE11ED;
const unsigned long LABEL_FREE = 0xCEA5ED;

const int INCORRECT_INPUT = 1111638594;

#define IF_NOT_BRACKET                                                                            \
    do                                                                                            \
    {                                                                                             \
        if(bracket != ']')                                                                        \
        {                                                                                         \
            printf("INCCORECT INPUT (%s). Did you mean to put ] instead of %c?\n", str, bracket); \
            return INCORRECT_INPUT;                                                               \
        }                                                                                         \
    } while (0)

struct ASM
{
    int *opcode_buffer;
    int strings_count;
    int tokens;
    int *jmp_after;
    int jmp_after_count;
};

struct Label
{
    int label_to;
    unsigned long label_hash;
};

int strincmp(const char *str1, const char *str2, int n);

int CountSymbols(const char *filename);

char *ReadToBuffer(const char *filename, int size);

int CountStrings(char *buffer);

void RemoveSpaces(char *buffer, int size);

char **GetString(char *buffer, int string_count);

int AddLabel(struct Label *labels, char *str, int ip);

void RemoveComments(char *stringed_buffer);

int jmp_to(struct Label *labels, char *str);

int SearchLabel(struct Label *labels, unsigned long hash);

int ChooseArgs(char *opcode_tmp, char *str);

bool WriteBinaryFile(int *opcode_buffer, int tokens);

int *Assemble(char **stringed_buffer, struct Label *labels, struct ASM *assembler);

#endif// ASM_H