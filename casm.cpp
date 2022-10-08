#include "asm.h"
#include "stack.h"

FILE *WriteBinaryFile(int *machine_code_buffer, int machine_symbols_count)
{
    FILE *fp = fopen("asm.bin", "wb");
    if(fp == NULL)
    {
        printf("Could not open the file!\n");
        return NULL;
    } 

    fwrite("CP", sizeof(char), 2, fp); //signature

    fwrite(machine_code_buffer, sizeof(char), machine_symbols_count * sizeof(int), fp);

    return fp;
}

int CountSymbols(const char *filename)
{
    struct stat buff = {};
    stat(filename, &buff);

    return buff.st_size;
}

char *ReadToBuffer(const char *filename)
{
    int size = CountSymbols(filename);

    FILE *fp = fopen(filename, "r");

    char *buffer = (char *)calloc(size, sizeof(char));

    int PutZero = fread(buffer, sizeof(char), size, fp);
    buffer[PutZero] = '\0';

    return buffer;
}
void OddSpaceRemoveArray(char *buffer)
{
    char c = 0;
    int check = 1;
    int i = 0; //for before array
    int k = 0; // counter for after array

    while((c = buffer[i++]) != '\0')
    {
        if(c == '\n' && check == 0)
        {
            buffer[k] = c;
            k++;
            check = 1;
        }
        else if(c == '\n' && check == 1)
        {
            continue;
        }
        else if(c != '\n')
        {
            if (check == 1 && c == ' ')
            {
                continue;
            }
            else
            {
                buffer[k] = c;
                k++;
                check = 0;
            }
        }
    }

    buffer[k] = '\n';
    buffer[k + 1] = '\0';
}

int CountString(char *buffer)
{
    int count = 0;

    for(int i = 0; buffer[i] != '\0'; i++)
    {
        if(buffer[i] == '\n')
        {
            count++;
        }
    }
    return count;
}

int strincmp(const char *str1, const char *str2, int n)
{
    for(int i = 0; i < n; i++)
    {
        int razn = tolower(str1[i]) - tolower(str2[i]);

        if(razn)
        {
            return razn;
        }
    }
    return 0;
}


char **GetString(char* buffer, const int string_count)
{
    char **stringed_buffer = (char**)calloc(string_count, sizeof(char*));
    char *news = buffer;

    for(int i = 0; i < string_count; i++)
    {
        stringed_buffer[i] = news;
        news = strchr(news, '\n');
        *news++ = '\0';      
    }

    return stringed_buffer;
}

int *Assembly(char **stringed_buffer, int strings_count, int *machine_symbols_count)
{
    int *machine_code_buffer = (int*)calloc(2 * strings_count, sizeof(int)); // ocenka sverxu (cmd_amount* (max amount of args per cmd))
    struct Label labels[LABEL_AMOUNT] = {};

    for(int i = 0; i < LABEL_AMOUNT; i++)
    {
        labels[i].label_to   = -1;
        labels[i].label_name = NULL;
    }

    int ip = 0;
    int prev_push = 0;

    for(int i = 0; i < strings_count; i++)
    {
        char cmd[100] = {}; 
        int cmd_size = 0;
        sscanf(stringed_buffer[i], "%s%n", cmd, &cmd_size);

//; comments
        char *semicolon	 = strchr(stringed_buffer[i], ';');

        if(semicolon != NULL)
        {
            int str_size = strlen(stringed_buffer[i]);
            for(int j = 0; semicolon + j < stringed_buffer[i] + str_size ; j++)
            {
                semicolon[j] = '\0';
            }
        }

// : labels 
// TODO: label with names 
        char *colon = strchr(stringed_buffer[i], ':');
        int label_number = 0;
        
        if(colon != NULL) // if != NULL => something about labels
        {
            if(strincmp(cmd, "jmp", cmd_size) == 0)
            {
                char *is_jmp_label = strchr(stringed_buffer[i], ':'); 
                int jmp_to = 0;
                
                if(is_jmp_label == NULL)
                {
                    sscanf(stringed_buffer[i] + cmd_size + 1, " %d", &jmp_to);

                    machine_code_buffer[ip++] = CMD_JMP;
                    machine_code_buffer[ip++] = jmp_to;
                }
                else 
                {
                    sscanf(stringed_buffer[i] + cmd_size + 2, " %d", &jmp_to);

                    machine_code_buffer[ip++] = CMD_JMP;
                    machine_code_buffer[ip++] = labels[jmp_to].label_to;
                }
            }
            else 
            {
                if(sscanf(colon, ":%d", &label_number) == 1)
                {
                    labels[label_number].label_to = ip;
                }
                // HERE: else : for named labels
            }
        }
        else
        { 
        if(strincmp(cmd, "push", cmd_size) == 0)
        {
            int push_value = 0;
            sscanf(stringed_buffer[i] + cmd_size + 1, " %d", &push_value);
            machine_code_buffer[ip++] = CMD_PUSH;
            machine_code_buffer[ip++] = push_value; 
            prev_push = 1;
        }
        else if(strincmp(cmd, "pop", cmd_size) == 0)
        {
            machine_code_buffer[ip++] = CMD_POP;
            prev_push = 0;
        }
        else if(strincmp(cmd, "dup", cmd_size) == 0)
        {
            if(prev_push == 1)
            {
                machine_code_buffer[ip]     = CMD_PUSH;
                machine_code_buffer[ip + 1] = machine_code_buffer[ip - 1];
                ip += 2; 
            }
            else 
            {
                machine_code_buffer[ip] = machine_code_buffer[ip - 1];
                ip++;  
            }
            prev_push = 0;
        } 
        ELSE_IF_CMP(ADD)
        ELSE_IF_CMP(MUL)
        ELSE_IF_CMP(DIV)
        ELSE_IF_CMP(OUT)
        ELSE_IF_CMP(HALT)
        }
    
    }
    *machine_symbols_count = ip;

    return machine_code_buffer;
}



int main() // TODO: if bebra.txt has odd empty stroki, the result of the program is unexpected
{
    char *buffer = ReadToBuffer("bebra.txt");
    OddSpaceRemoveArray(buffer);
    int strings_count = CountString(buffer);
    char **string_buffer = GetString(buffer, strings_count);

    for(int i = 0; i < strings_count; i++)
    {
        DEB("%s\n", string_buffer[i]);
    }
    int machine_symbols_count = 0;

    int *machined_buffer = Assembly(string_buffer, strings_count, &machine_symbols_count);

    WriteBinaryFile(machined_buffer, machine_symbols_count);
}
