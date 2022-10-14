#include "stack.h"
#include "enum.h"
#include "cpu.h"

int CountInts(const char *filename)
{
    struct stat buff = {};
    stat(filename, &buff);

    return (buff.st_size / sizeof(int));
}

int *ReadToBuffer(const char *filename)
{
    int size = CountInts(filename);
    FILE *fp = fopen(filename, "rb");

    int *buffer = (int *)calloc(size, sizeof(int));

    int PutZero = fread(buffer, sizeof(int), size, fp);
    buffer[PutZero] = '\0';

    for (int i = 0; i < size; i++)
        DEB("%d ", buffer[i]);
    DEB("\n");

    return buffer;
}

bool CheckSignature(int *machine_code_buffer)
{ // sign : LILY
    if (
        ((char *)machine_code_buffer)[0] == 'L' &&
        ((char *)machine_code_buffer)[1] == 'I' &&
        ((char *)machine_code_buffer)[2] == 'L' &&
        ((char *)machine_code_buffer)[3] == 'Y') // TODO: check as int or memcmp
    {
        return 1;
    }
    return 0;
}

void Run(struct Stack *stk1, int *machine_code_buffer, int machine_cmds_count)
{
    DEB("machine code: [max = %d]", machine_cmds_count);
    for (int i = 0; i < machine_cmds_count; i++)
    {
        DEB("%d ", machine_code_buffer[i]);
    }
    DEB("\n");

    StackPrint(stk1);
    
    int err_code = 0;
    int ip = 1;
    while (machine_code_buffer[ip] != CMD_HALT && ip < machine_cmds_count)
    {
        DEB("before switch : %d (?%d) [ip = %d < %d]\n", machine_code_buffer[ip], CMD_HALT, ip, machine_cmds_count);

        switch (machine_code_buffer[ip])
        {
#define DEF_CMD(name, num, arg, code)  \
    case CMD_##name:                   \
        code;                          \
        ip++;                          \
        DEB("ip = %d, done\n", ip); \
        break;

#include "commands.h"
        default:
            printf("Something went wrong!\n");
            break;
        }
#undef DEF_CMD
        DEB("after switch : %d (?%d) [ip = %d]\n", machine_code_buffer[ip], (CMD_HALT), ip);
        StackPrint(stk1);
    }

    DEB("End of run\n");
}

int main()
{
    Stack stk1 = {};
    StackCtor_(stk1, 10);

    // int test[] = {1234, 17, 1, 17, 2, 1, 2}; ////////////////////////////////

    int *machine_code_buffer = ReadToBuffer("asm.bin");
    if (CheckSignature(machine_code_buffer))
    {
        int cmds_counts = CountInts("asm.bin");
        DEB("CMD_COUNTS = %d\n", cmds_counts);
        Run(&stk1, machine_code_buffer, cmds_counts);
        // Run(&stk1, test, 7);

        StackPrint(&stk1);
    }
    else
    {
        printf("Signature did not match\n");
    }

    printf("done\n");
}

/*
#include "stack.h"
#include "enum.h"
#include "cpu.h"

int CountInts(const char *filename)
{
    struct stat buff = {};
    stat(filename, &buff);

    return (buff.st_size);
}


char *ReadToBuffer(const char *filename)
{
    int size = CountInts(filename);
    FILE *fp = fopen(filename, "rb");

    char *buffer = (char *)calloc(size, sizeof(char));

    int PutZero = fread(buffer, sizeof(char), size, fp);
    buffer[PutZero] = '\0';

DEB("PUTZERO = %d\n", PutZero);
    for(int i = 0; i < PutZero; i++)
        DEB("%d ", buffer[i]);


    return buffer;
}

bool CheckSignature(char *machine_code_buffer)
{ // sign : LILY
        if
        (
            ((char *)machine_code_buffer)[0] == 'L' &&
            ((char *)machine_code_buffer)[1] == 'I' &&
            ((char *)machine_code_buffer)[2] == 'L' &&
            ((char *)machine_code_buffer)[3] == 'Y'
        )
        {
            return 1;
        }
        return 0;
}

void Run(struct Stack *stk1, char *machine_code_buffer, int machine_cmds_count)
{
    DEB("machine code: [max = %d]\n", machine_cmds_count);
    for(int i = 0; i < machine_cmds_count; i += 4)
    {
        DEB("%d ", *((int *)(machine_code_buffer + i)));
    }
    DEB("\n");

    int err_code = 0;
    int ip = 4;
    while(machine_code_buffer[ip] != CMD_HALT && ip < machine_cmds_count)
    {
        DEB("b4 switch : %d (?%d) [ip = %d < %d]\n", machine_code_buffer[ip], CMD_HALT, ip, machine_cmds_count);

        switch(machine_code_buffer[ip])
        {
            #define DEF_CMD(name, num, arg, code) \
                    case CMD_##name:              \
                    code;                         \
                    ip += sizeof(int);                         \
                    printf("ip = %d, done\n", ip); \
                    break;

            #include "commands.h"
            default:
                    printf("Something went wrong!\n");
                    break;
        }
            #undef DEF_CMD
        DEB("after switch : %d (?%d) [ip = %d]\n", machine_code_buffer[ip], CMD_HALT, ip);
        StackPrint(stk1);
    }

    DEB("reached the end\n");
}

int main()
{
    Stack stk1;
    StackCtor_(stk1, 12);

    char *machine_code_buffer = ReadToBuffer("asm.bin");
    if (CheckSignature(machine_code_buffer))
    {
        int cmds_counts = CountInts("asm.bin");
        Run(&stk1, machine_code_buffer, cmds_counts);
        // StackPrint(&stk1);
    }
    else
    {
        printf("Signature is not matched\n");
    }

}
*/