#ifndef VIDMEM_H
#define VIDMEM_H

const int MAX_COL = 101;
const int MAX_LN = 75; 

int CountSymbols(const char *filename);

char *RTB(const char *filename, int size);

int FillRam(struct CPU *cpu);

#endif// VIDMEM_H