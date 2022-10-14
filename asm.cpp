#include "asm.h"
#include "enum.h"

// TODO: split into files !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

bool WriteBinaryFile(int *opcode_buffer, int tokens)
{
    FILE *fp = fopen("asm.bin", "wb");
    if(fp == NULL)
    {
        printf("Could not open the file!\n");
        return 0;
    } 

    fwrite("LILY", sizeof(char), 4, fp); //signature

    fwrite(opcode_buffer, sizeof(char), tokens * sizeof(int), fp);

    fclose(fp);

    return 1;
}

int CountSymbols(const char *filename)
{
    struct stat buff = {};
    stat(filename, &buff);

    return buff.st_size;
}

int strincmp(const char *str1, const char *str2, int n)
{
    if (n == 0)
    {
        return -1;
    }
    for(int i = 0; i < n; i++)
    {
        // make variable for str[i]
        // check logic

        // try:

            // 'hsup'
            // ' pop'
            // ' pmj'

        if(str1[i] == ' ' || str2[i] == ' ')
        {
            continue;
        }
        if(tolower(str1[i]) != tolower(str2[i]))
        {
           return str1[i] - str2[i];
        }
        if(str1[i] == '\0' && str2[i] != '\0') 
        {
            return -1;
        }
        if (str2[i] == '\0' && str1[i] != '\0')
        {
            return 1;
        }
    }
    return 0;
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
void RedundantSpaces(char *buffer)
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

int CountStrings(char *buffer)
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
    // strtok ^^^^^^^^^^^^

    return stringed_buffer;
}

int *Assemble(char **stringed_buffer, struct Label *labels, int strings_count, int *machine_symbols_count)
{
    int *machine_code_buffer = (int*)calloc(3 * strings_count, sizeof(int));
// consider resizing array
    int ip = 0;
    int prev_push = 0; //TODO: change name since it will be used for cmds with 2 args, not only push

    for(int i = 0; i < strings_count; i++)
    {
        char cmd[100] = {}; 
        int cmd_size = 0;
        int cmd_colon_size = 0;
        char colon      = '?';

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

        sscanf(stringed_buffer[i], "%[^:-123456789 ]%n %[:]%n", cmd, &cmd_size, &colon, &cmd_colon_size);

        DEB("[%s / %d / %c / %d ]\n", cmd, cmd_size, colon, cmd_colon_size);
// : labels 
//  ̶T̶O̶D̶O̶: label with names  
//  TODO: make hash

        if(strincmp(cmd, "jmp", cmd_size) == 0)
        {  // dealing with labels
            DEB("joined jmp with command : %s\n", cmd);
            int jmp_to = 0;
            
            if(colon != ':') // jmp to not label  + to named label
            {
                if(sscanf(stringed_buffer[i] + cmd_size, " %d", &jmp_to))
                {
                    DEB("[%s]colon != : %d\n", cmd, jmp_to);
                    machine_code_buffer[ip++] = CMD_JMP;
                    machine_code_buffer[ip++] = jmp_to;
                }
                else
                {
                    char labels_name[20] = ""; //ig 
                    int labels_size = 0;
                    sscanf(stringed_buffer[i] + cmd_size + 1, " %[^:]%n", labels_name, &labels_size);
                    DEB("scanned LABEL  = %s [%d]\n", labels_name, labels_size);
                    machine_code_buffer[ip++] = CMD_JMP;
                    DEB("m_c_b : JMP [ip = %d]\n", ip - 1);
                    for(int j = 0; j < LABEL_MAX; j++)
                    {
                        DEB("comparing %s, %s, %d\n", labels[j].label_name, labels_name, labels_size);
                        if(strincmp(labels[j].label_name, labels_name, labels_size) == 0)
                        {
                            DEB("found in jmp%d\n", j);
                            machine_code_buffer[ip++] = labels[j].label_to;
                            DEB("ip = %d", ip);
                            break;
                        }
                    }
                }
            }
            else 
            {
                sscanf(stringed_buffer[i] + cmd_size, " :%d", &jmp_to);
                DEB("<colon ==  :> %d\n", jmp_to);
                machine_code_buffer[ip++] = CMD_JMP;
                machine_code_buffer[ip++] = labels[jmp_to].label_to;
                DEB("machine_code_buffer[%d] = labels[%d].label_to = %d\n", ip - 1, jmp_to, labels[jmp_to].label_to);
            }
        }
        else if(char *colonn = strchr(stringed_buffer[i], ':'))  // ну это хуйня блять сканфная не работает если строчка ":цифра"    
        { // adding labels; 
            DEB("ebanaya metka : ");
            int label_number = 0;
            if(sscanf(colonn, ":%d", &label_number) == 1)
            {
                DEB("label_number = %d. = %d ; \n",label_number, ip);
                labels[label_number].label_to = ip;
            }
            else 
            {
                char *labels_name = cmd;
                DEB("label name : %s\n", labels_name);
                for(int k = 0; k < LABEL_MAX; k++)
                {
                    if(labels[k].label_name == NULL && labels[k].label_to == -1)
                    {
                        labels[k].label_to = ip;
                        labels[k].label_name = labels_name;
                        DEB("found free label for \"%s\"on %d\n",labels_name, k); // хуйня нерабочая
                        DEB("labels[i].label_name: %s\n", labels[k].label_name);
                        break;
                    }
                }
            }
        }
        else if(strincmp(cmd, "push", cmd_size) == 0)
        {
            int push_value = 0;
            sscanf(stringed_buffer[i] + cmd_size, " %d", &push_value);
            DEB("push value : %d\n", push_value);
            machine_code_buffer[ip++] = CMD_PUSH;
            machine_code_buffer[ip++] = push_value; 
            prev_push = 1;
        }
        else if(strincmp(cmd, "pop", cmd_size) == 0)
        {
            DEB("popping\n");
            machine_code_buffer[ip++] = CMD_POP;
            prev_push = 0;
        }
        else if(strincmp(cmd, "dup", cmd_size) == 0)
        {
            DEB("in dup..\n");
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
    *machine_symbols_count = ip;

    return machine_code_buffer;
}



int main() // TODO: if bebra.txt has empty string, the result of the program is unexpected???????? *(check again, ive might been drunk)
{
    char *buffer = ReadToBuffer("bebra.txt");
    RedundantSpaces(buffer);
    int strings_count = CountStrings(buffer);
    char **string_buffer = GetString(buffer, strings_count);

    int tokens = 0;

    struct Label labels[LABEL_MAX] = {};

    for(int i = 0; i < LABEL_MAX; i++)
    {
        labels[i].label_to   = -1;
        labels[i].label_name = NULL;
    }

    int *opcode_buffer = Assemble(string_buffer, labels, strings_count, &tokens);
    //   ^^^^^^
    // check if null
    if(opcode_buffer == NULL)
    {
        printf("opcode buffer is NULL\n");  
        return 0;
    }


    WriteBinaryFile(opcode_buffer, tokens); // check return value

    DEB("tokens :%d\n", tokens);
   
    DEB("done");

    return 0;
}
