#ifndef CPU_H
#define CPU_H

#include <sys\stat.h>

bool CheckSignature(int *machine_code_buffer)                                            ;
int *ReadToBuffer  (const char *filename)                                                ;
int  CountInts     (const char *filename)                                                ;
void Run           (struct Stack *stk1, int *machine_code_buffer, int machine_cmds_count);


#endif