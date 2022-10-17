// TODO: deal with dup : need to add prev_cmd_arg or use third arg. in DEF_CMD;
// TODO: remove err_code's?

DEF_CMD(PUSH, 1, 1,          
{   {                                  
    int argument = machine_code_buffer[++ip];  
    printf("push arg = %d\n", argument);
    StackPush(stk1, argument);        
    printf("DEF CMD STACK PUSH pushed\n");  
    }
})


DEF_CMD(POP, 2, 1,               
{      
    {                           
    StackPop(stk1, &err_code);     
    }                         
})

DEF_CMD(ADD, 3, 0,     
{      
    {    
        int first = StackPop(stk1, &err_code);
        int second = StackPop(stk1, &err_code);
        StackPush(stk1, first + second);
        DEB("added = %d + %d\n", first, second);   
    }
})

DEF_CMD(SUB, 4, 0,     
{     
    {     
    int subtracted = StackPop(stk1, &err_code);
    int reduced  = StackPop( stk1, &err_code);
    StackPush(stk1, (reduced - subtracted));   
    }
})

DEF_CMD(MUL, 5, 0,     
{                                 
    {
    StackPush(stk1, StackPop(stk1, &err_code) * StackPop(stk1, &err_code));   
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
    ip = machine_code_buffer[ip + 1];
    }      
})

DEF_CMD(OUT, 8, 0, 
{
    {
    printf("StackTOP = %d\n", StackTop(stk1, &err_code));
    }
})

DEF_CMD(HALT, 9, 0, {printf("Program completed\n");})
    
DEF_CMD(IN, 10, 0, {int InVal = 0; scanf("%d", &InVal); StackPush(stk1, InVal);})

DEF_CMD(CALL, 11, 12, {})

DEF_CMD(RET, 12, 12, {})