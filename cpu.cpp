#include "stack.h"
#include "enum.h"
#include "cpu.h"

#define DEF_CMD(name, num, arg, code) \
    if (cmd_id[0] == num)                \
        {cmd_stringed = #name; argcount = arg;}


void Listing(struct CPU *cpu)
{
    FILE *fp = fopen("Lcpu.txt", "w");
    const char *cmd_stringed = NULL;

    if(fp == NULL)
    {
        return;
    }
    int counter = 0;
    int argcount = 0;
    fprintf(fp, " N   CMD  RAM REG CONST CMD   ARG\n");
    for(int ip = 1; ip < cpu->tokens; ip++, counter++)
    {
        int reg_n_konst = 0;
        char *cmd_id = ((char *)(cpu->machine_inst + ip));
        int FOR_RAM = cmd_id[1];
        int FOR_REG = cmd_id[2];
        int FOR_KONST = cmd_id[3];

        #include "commands.h"

        fprintf(fp, "%04d %3d %3d %3d %3d \t%s ", counter, cmd_id[0], FOR_RAM, FOR_REG, FOR_KONST, cmd_stringed);
        if(argcount > 0)
        {
            ip++;
            if(FOR_RAM) fprintf(fp, "[");
            if(FOR_REG) {fprintf(fp, "r%cx", FOR_REG + 'a' - 1); reg_n_konst = 1;}
            if(FOR_KONST == 1 && reg_n_konst == 1) {fprintf(fp, " + %d", cpu->machine_inst[ip]);} 
            else if(FOR_KONST == 1 && reg_n_konst == 0) {fprintf(fp, "  %d", cpu->machine_inst[ip]);}
            if(FOR_RAM) fprintf(fp, "]");
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}
#undef DEF_CMD


void DEBUGStackPrint(struct Stack *stack)
{
    int i = 0;
    int print_to = stack->capacity;
    #ifdef CANARY_PROT
    i--;
    print_to++;
    #endif// CANARY_PROT
    for(; i < print_to; i++)
    {
        char        in_stack    =   ' ';
        const char *isPOISONED  = "(POISON)";
        const char *notPOISONED = " "; 
        const char *isCANARY    = "(CANARY)";
        const char *situation   = isPOISONED;

        if(*(stack->data + i) == DATA_LEFT_CANARY || *(stack->data + i) == DATA_RIGHT_CANARY)
        {
            in_stack = '#';
            situation = isCANARY;
        }
        else if(stack->data[i] != POISON)
        {
            in_stack = '*';
            situation = notPOISONED;
        }
        printf("\t\t%c[%d] = %d%s\n", in_stack, i, *(stack->data + i), situation);
    }    

}

int CountInts(const char *filename)
{
    struct stat buff = {};
    stat(filename, &buff);

    return buff.st_size / sizeof(int);
}

int *ReadToBuffer(const char *filename, int size)
{    
    FILE *fp = fopen(filename, "r");

    if(fp == NULL)
    {
        printf("Could not open \"%s\"\n", filename);
        return NULL;
    }

    int *buffer = (int *)calloc(size, sizeof(int));

    if(buffer == NULL)
    {
        printf("buffer = NULL (READTOBUFFER)\n");
        return buffer;
    }

    fread(buffer, sizeof(int), size, fp);
    
    fclose(fp);

    return buffer;
}

bool CheckSignature(int *machine_inst)
{ // sign : LILY
    return machine_inst[0] == LILY_REVERTED;
}

int GetArgument(struct CPU *cpu, int *ip)
{
    int arg = 0;
    char *opcode_tmp = (char *)(cpu->machine_inst + (*ip));
    (*ip)++;
    
    if(opcode_tmp[_REG] != 0)
    {
        arg += cpu->reg[(int)(opcode_tmp[_REG])];
    }
    if(opcode_tmp[_KONST] == 1)
    {
        arg += cpu->machine_inst[*ip];
    }
    if(opcode_tmp[_RAM] == 1)
    {
        arg = cpu->RAM[arg];
    }

    return arg;
}

int PopIn(struct Stack *stk1, struct CPU *cpu, int *ip)
{
    char *opcode_tmp = (char *)(cpu->machine_inst + (*ip));
    (*ip)++;

    int err_code = 0; 

    int pop_value = StackPop(stk1, &err_code);

    if(err_code != 0)
    {
        return NOT_OK;
    }

    if(opcode_tmp[_REG] != 0 && opcode_tmp[_RAM] == 1)
    {
        cpu->RAM[(int)opcode_tmp[_REG]] = pop_value;
    }
    else if(opcode_tmp[_REG] != 0)
    {
        cpu->reg[(int)opcode_tmp[_REG]] = pop_value;
    }
    else if(opcode_tmp[_RAM] == 1)
    {
        cpu->RAM[cpu->machine_inst[*ip]] = pop_value;
    }
    else 
    {
        printf("Syntax error in pop [ip == (%d,%d)]\n", *ip - 1, *ip);
    }

    return OK;    
}

#define DEF_CMD(name, num, arg, code)  \
        case CMD_##name:               \
        code;                          \
        ip++;                          \
        break;

void Run(struct Stack *stk1, struct CPU *cpu)
{
    int err_code = 0;
    int ip = 1;
    while (cpu->machine_inst[ip] != CMD_HALT)
    {
        switch(((char *)(cpu->machine_inst + ip))[0])
        {

        #include "commands.h"
        
        default:
            printf("Unknown opcode!\n");
            break;
        }
        
        #undef DEF_CMD
    }

    DEB("End of run\n");
}
