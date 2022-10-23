#include "stack.h"
#include "cpu.h"
#include "enum.h"
#include "vidmem.h"

int main()
{
    struct CPU cpu = 
    {
        .machine_inst = NULL,
        .tokens = 0,
        .reg = {},
        .RAM = {},
    };

    Stack stk1 = {};
    StackCtor_(stk1, 10);
    
    cpu.tokens = CountInts("asm.bin");

    DEB("%d token\n", cpu.tokens);
    cpu.machine_inst = ReadToBuffer("asm.bin", cpu.tokens);

    if(cpu.machine_inst == NULL)
    {
        printf("opcode is NULL");
        return 0;
    }

    if(CheckSignature(cpu.machine_inst))
    {
        FillRam(&cpu);
        
        Run(&stk1, &cpu);
        
        // DEB("stackprint: \n");
        // DEBUGStackPrint(&stk1);

        // DEB("RAM :\n");
        // for(int i = 0; i < RAM_MAX; i++)
        //     DEB("%d ", cpu.RAM[i]);
        
        // DEB("REG:\n");
        // for(int i = 1; i < REG_COUNT; i++)
        //     DEB("%d ", cpu.reg[i]);
        
        Listing(&cpu);
    }
    else
    {
        printf("Signature did not match\n");
    }

    free(cpu.machine_inst);
    StackDtor(&stk1);
    printf("done\n");

    return 0;
}