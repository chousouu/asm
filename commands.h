// TODO: deal with dup : need to add prev_cmd_arg or use third arg. in DEF_CMD;
// TODO: remove err_code's?

DEF_CMD(PUSH, 0x11, 1,          
{   {                                  
    int argument = machine_code_buffer[++ip];  
    printf("push arg = %d\n", argument);
    StackPush(stk1, argument);        
    printf("DEF CMD STACK PUSH pushed\n");  
    }
})

DEF_CMD(POP, 0x21, 1,               
{      
    {                           
    StackPop(stk1, &err_code);     
    }                         
})

DEF_CMD(ADD, 0x01, 1,     
{      
    {    
        int first = StackPop(stk1, &err_code);
        int second = StackPop(stk1, &err_code);
        StackPush(stk1, first + second);
        DEB("added = %d + %d\n", first, second);   
    }
})

DEF_CMD(SUB, 0x02, 1,     
{     
    {     
    int jojo = StackPop(stk1, &err_code);
    int dio  = StackPop( stk1, &err_code);
    StackPush( stk1, (dio - jojo));   
    }
})

DEF_CMD(MUL, 0x03, 1,     
{                                 
    {
    StackPush(stk1, StackPop(stk1, &err_code) * StackPop(stk1, &err_code));   
    }
})

DEF_CMD(DIV, 0x04, 1,                      
{                                                  
    {
    int denominator = StackPop(stk1, &err_code);   
    int numerator   = StackPop(stk1, &err_code);     
    StackPush(stk1, numerator / denominator);    
    }
})

DEF_CMD(JMP, 0x05, 1,              
{                                          
    {
    ip = machine_code_buffer[ip + 1];
    }      
})

DEF_CMD(OUT, 0x06, 1, 
{
    {
    printf("StackTOP = %d\n", StackTop(stk1, &err_code));
    }
})

DEF_CMD(HALT, 0x07, 0, {printf("Program completed\n");})
    // CMD_DUP   = 6,
    // CMD_OUT   = 10,
    // CMD_HALT  = 11,