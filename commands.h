// TODO: deal with dup : need to add prev_cmd_arg or use third arg. in DEF_CMD;

DEF_CMD(PUSH, 1, 1,          
{   {                                  
    int argument = GetArgument(cpu, &ip);  
    StackPush(stk1, argument);        
    }
})


DEF_CMD(POP, 2, 1,               
{      
    {  
    if(PopIn(stk1, cpu, &ip) != OK)
    {
        printf("ERROR in pop\n");
        return;
    }     
    }                         
})

DEF_CMD(ADD, 3, 0,     
{      
    {    
        int first = StackPop(stk1, &err_code);
        int second = StackPop(stk1, &err_code);

        StackPush(stk1, first + second);
    }
})

DEF_CMD(SUB, 4, 0,     
{     
    {     
        int subtracted = StackPop(stk1, &err_code);
        int reduced = StackPop( stk1, &err_code);

        StackPush(stk1, (reduced - subtracted));   
    }
})

DEF_CMD(MUL, 5, 0,     
{                                 
    {
        int multiplier = StackPop(stk1, &err_code);
        int multiplicand = StackPop(stk1, &err_code);

        StackPush(stk1, multiplier * multiplicand);   
    }
})

DEF_CMD(DIV, 6, 0,                      
{                                                  
    {
        int denominator = StackPop(stk1, &err_code);   
        int numerator   = StackPop(stk1, &err_code);

        StackPush(stk1, numerator / denominator);    
    }
})

DEF_CMD(JMP, 7, 12,              
{                                          
    {
        ip = cpu->machine_inst[ip + 1];
    }      
})

DEF_CMD(OUT, 8, 0, 
{
    {
        printf("StackTOP = %d\n", StackTop(stk1, &err_code));
    }
})

DEF_CMD(HALT, 9, 0, {{printf("Program completed\n");}})
    
DEF_CMD(IN, 10, 0, {{int InVal = 0; scanf("%d", &InVal); StackPush(stk1, InVal);}})

DEF_CMD(CALL, 11, 12, {{StackPush(stk1, ip + 2); ip = cpu->machine_inst[ip + 1];}})

DEF_CMD(RET, 12, 0, {{ip = StackPop(stk1, &err_code); ip--;}})

DEF_CMD(JA, 13, 12, 
{
    int a = StackPop(stk1, &err_code);
    int b = StackPop(stk1, &err_code);
    if(a > b)
    {
        ip = cpu->machine_inst[ip + 1];
    }
    else 
    {
        ip++;
    }
})

DEF_CMD(JB, 14, 12, 
{
    int a = StackPop(stk1, &err_code);
    int b = StackPop(stk1, &err_code);

    if(a < b)
    {
        ip = cpu->machine_inst[ip + 1];
    }
    else 
    {
        ip++;
    }
})

DEF_CMD(JE, 15, 12, 
{
    int a = StackPop(stk1, &err_code);
    int b = StackPop(stk1, &err_code);

    if(a == b)
    {
        ip = cpu->machine_inst[ip + 1];
    }
    else 
    {
        ip++;
    }
})

DEF_CMD(JNE, 16, 12, 
{
    int a = StackPop(stk1, &err_code);
    int b = StackPop(stk1, &err_code);

    if(a != b)
    {
        ip = cpu->machine_inst[ip + 1];
    }
    else 
    {
        ip++;
    }
})

DEF_CMD(SQRT, 17, 0, 
{
    int a = StackPop(stk1, &err_code);

    StackPush(stk1, sqrt(a));
})

DEF_CMD(PRROOT, 18, 0, 
{
    int root_number = StackPop(stk1, &err_code);
    int root = StackPop(stk1, &err_code);

    printf("x%d = %d\n", root_number, root);
})