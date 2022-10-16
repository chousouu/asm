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

static long unsigned HashCounter(void *data, int len)
{
    unsigned long a = 0 , b = 0, c = 0;
    unsigned int fool      = 0xAF00L;
    unsigned int lightning = 1988  ;

    char *datawalker = (char*)data;
    int hash_counter = 0;
    while (len > 4)
    {
        a += fool++;
        b += fool++;
        c += fool++;

        c = a << 2;
        b = a << 3;
        a = a << 1;
        len -= 4;
    }

    c += datawalker[2] << 16;
    b += datawalker[1] << 8 ;
    a += datawalker[0] << 1 ; 

    hash_counter = a * (fool) + b * (fool + lightning) + c * (fool + 311);

    return hash_counter;
}

char *ReadToBuffer(const char *filename, int size)
{    
    FILE *fp = fopen(filename, "r");

    char *buffer = (char *)calloc(size + 1, sizeof(char));

    fread(buffer, sizeof(char), size, fp);
    buffer[size] = '\0';

    return buffer;
}

void RedundantSpaces(char *buffer, int size)
{
    char c = 0;
    int check = 1;
    int i = 0; //for before array
    int k = 0; // counter for after array
    for(i = 0; i < size; i++) // ?or for(i = 0; i < size, c = buffer[i]; i++)   
    {
        c = buffer[i];
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
    buffer[k] = '\0';    
}

int CountStrings(char *buffer)
{
    int count = 0;
    int i = 0;

    for(i = 0; buffer[i] != '\0'; i++)
    {
        if(buffer[i] == '\n')
        {
            count++;
        }
    }
    if(buffer[i - 1] != '\n')
    {
        count++;
    }

    return count;
}

char **GetString(char* buffer, const int string_count)
{
    char **stringed_buffer = (char**)calloc(string_count, sizeof(char*));
    char *news = strtok(buffer, "\n");

    for(int i = 0; i < string_count; i++)
    {
        stringed_buffer[i] = news;
        news = strtok(NULL, "\n"); 
    }

    return stringed_buffer;
}

void RemoveComments(char *stringed_buffer)
{
    char *semicolon	 = strchr(stringed_buffer, ';');

    if(semicolon != NULL)
    {
        int str_size = strlen(stringed_buffer);
        for(int j = 0; semicolon + j < stringed_buffer + str_size ; j++)
        {
            semicolon[j] = '\0';
        }
    }
}

int SearchLabel(struct Label *labels, unsigned long hash)
{
    for(int i = 0; i < LABEL_MAX; i++)
    {
        if(labels[i].label_hash == hash)
        {
            return i;
        }
    }
    if(hash == LABEL_FREE)
    {
        printf("Could not find free label. Increase LABEL_MAX value in \"asm.h\" if you need more labels.\n");
    }

    return -1;
}

int AddLabel(struct Label *labels, char *str, int ip)
{
    int label_number         = -1;
    unsigned long hash_label =  0;

    hash_label = HashCounter(str, strlen(str));
        
        label_number = SearchLabel(labels, hash_label);
        if(label_number == -1)
        {
            label_number = SearchLabel(labels, LABEL_FREE);
        }
        labels[label_number].label_to   = ip;
        labels[label_number].label_hash = hash_label;

    return label_number;
}

int jmp_to(struct Label *labels, char *str)
{
    if(strchr(str, ':') == NULL) // 1
    {
        int jmp_ip = -1;
        sscanf(str, " %d", &jmp_ip);

        return jmp_ip;
    }
    else 
    {
        unsigned long hash_label = HashCounter(str, strlen(str));

        int index = SearchLabel(labels, hash_label);

        if(index == -1)
        {
            return LABEL_TO_UNTOUCHED; // -1 is label_to value by default 
        }

        return labels[index].label_to;
    }
}

int *Assemble(char **stringed_buffer, struct Label *labels, struct ASM *assembler)
{
    int *opcode_buffer = (int*)calloc(3 * assembler->strings_count, sizeof(int));
// consider resizing array
    int ip = 0;
    int prev_push = 0; //TODO: change name since it will be used for cmds with 2 args, not only push

    for(int i = 0; i < assembler->strings_count; i++)
    {
        char cmd[50] = {}; 
        int cmd_size = 0;

        RemoveComments(stringed_buffer[i]);

        sscanf(stringed_buffer[i], "%[^:-123456789 ]%n", cmd, &cmd_size);

        if(strincmp(cmd, "jmp", cmd_size) == 0)
        {
            opcode_buffer[ip++] = CMD_JMP;
            int jmp_ip = jmp_to(labels, stringed_buffer[i] + cmd_size + 1);
            if(jmp_ip == LABEL_TO_UNTOUCHED)
            {
                assembler->jmp_after[assembler->jmp_after_count++] = i;
                assembler->jmp_after[assembler->jmp_after_count++] = ip;
            }
            opcode_buffer[ip++] = jmp_ip;
        }
        else if(strchr(stringed_buffer[i], ':'))     
        { 
            AddLabel(labels, stringed_buffer[i], ip);
        }
        else if(strincmp(cmd, "push", cmd_size) == 0)
        {
            int push_value = 0;
            sscanf(stringed_buffer[i] + cmd_size, " %d", &push_value);

            opcode_buffer[ip++] = CMD_PUSH;
            opcode_buffer[ip++] = push_value; 
            prev_push = 1;
        }
        else if(strincmp(cmd, "pop", cmd_size) == 0)
        {
            opcode_buffer[ip++] = CMD_POP;
            prev_push = 0;
        }
        else if(strincmp(cmd, "dup", cmd_size) == 0)
        {
            if(prev_push == 1)
            {
                opcode_buffer[ip]     = CMD_PUSH;
                opcode_buffer[ip + 1] = opcode_buffer[ip - 1];
                ip += 2; 
            }
            else 
            {
                opcode_buffer[ip] = opcode_buffer[ip - 1];
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

    for(int i = 0; i < assembler->jmp_after_count; i++)
    {// even elem = i, odd elem = ip, 
        opcode_buffer[assembler->jmp_after[i + 1]] = jmp_to(labels, stringed_buffer[assembler->jmp_after[i]] + 4);
        i++;
    }

    assembler->tokens = ip;

    return opcode_buffer;
}



int main() 
{
    int symbols_count = CountSymbols("bebra.txt");
    char *buffer = ReadToBuffer("bebra.txt", symbols_count);
    RedundantSpaces(buffer, symbols_count); 
    
    struct ASM assembler =
    {
        .opcode_buffer = NULL,
        .strings_count = CountStrings(buffer),
        .tokens = 0,
        .jmp_after = (int*)calloc(LABEL_MAX * 2, sizeof(int)),
        .jmp_after_count = 0,
    };
    
    struct Label labels[LABEL_MAX] = {};

    for(int i = 0; i < LABEL_MAX; i++)
    {
        labels[i].label_to   = LABEL_TO_UNTOUCHED;
        labels[i].label_hash = LABEL_FREE;
    }

    char **string_buffer = GetString(buffer, assembler.strings_count);
    
    assembler.opcode_buffer = Assemble(string_buffer, labels, &assembler);

    
    if(assembler.opcode_buffer == NULL)
    {
        printf("opcode buffer is NULL\n");  
        return 0;
    }


    if(!WriteBinaryFile(assembler.opcode_buffer, assembler.tokens))
    {
        printf("Couldnt write binary file\n");
    }
   
    free(buffer);
    free(string_buffer);
    free(assembler.opcode_buffer);
    free(assembler.jmp_after);

    return 0;
}
