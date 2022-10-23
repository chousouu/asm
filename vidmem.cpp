#include "vidmem.h"
#include "cpu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int FillRam(struct CPU *cpu)
{
    int size = CountSymbols("huylo.txt"); 

    char *gen = RTB("huylo.txt", size);

    for(int i = 0; i < size; i++) //+ 1 for \n 
    {
        cpu->RAM[i] = (int)gen[i];
    }

    free(gen);

    return 0;
}

char *RTB(const char *filename, int size)
{    
    FILE *fp = fopen(filename, "r");

    char *buffer = (char *)calloc(size + 1, sizeof(char));

    if(buffer == NULL)
    {
        printf("buffer = NULL (READTOBUFFER)\n");
    }

    fread(buffer, sizeof(char), size, fp);
    buffer[size] = '\0';
    
    fclose(fp);

    return buffer;
}

int CountSymbols(const char *filename)
{
    struct stat buff = {};
    stat(filename, &buff);

    printf("size = %ld\n", buff.st_size);

    return buff.st_size;
}