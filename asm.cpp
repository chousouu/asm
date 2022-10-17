#include "asm.h"
#include "enum.h"

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
    if(n == 0)
    {
        return 0;
    }
    int i = 0, j = 0;
    
    while(i < n && j < n)
    {
        char s1 = str1[i];
        char s2 = str2[j];

        if(tolower(s1) != tolower(s2))
        {
           return s1 - s2;
        }
        if(s1 == '\0' && s2 != '\0') 
        {
            return -1;
        }
        if (s2 == '\0' && s1 != '\0')
        {
            return 1;
        }
        i++;
        j++;
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

    if(buffer == NULL)
    {
        printf("buffer = NULL (READTOBUFFER)\n");
    }

    fread(buffer, sizeof(char), size, fp);
    buffer[size] = '\0';
    
    fclose(fp);

    return buffer;
}

void RemoveSpaces(char *buffer, int size)
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
    if(strchr(str, ':') == NULL) 
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

int ChooseArgs(char *opcode, char *str)
{
    int push_value = 0;
    char reg = '!';    
    char bracket = '!';

    if((sscanf(str, " [r%cx + %d %c", &reg, &push_value, &bracket)) == 3) 
    {                                                                                               
        IF_NOT_BRACKET;                                                                             
                                                                                                    
        opcode_tmp[_RAM]   = 1;                                                                     
        opcode_tmp[_REG]   = reg - 'a' + 1;                                                         
        opcode_tmp[_KONST] = 1;                                                                     
    }                                                                                               
    else if((sscanf(str, " [%d %c", &push_value, &bracket)) == 2)         
    {                                                                                               
        IF_NOT_BRACKET;                                                                             
                                                                                                    
        opcode_tmp[_RAM]   = 1;                                                                     
        opcode_tmp[_KONST] = 1;                                                                     
    }                                                                                               
    else if((sscanf(str, " [r%cx %c", &reg, &bracket)) == 2)              
    {                                                                                               
        IF_NOT_BRACKET;                                                                             
                                                                                                    
        opcode_tmp[_RAM]   = 1;                                                                     
        opcode_tmp[_REG] = reg - 'a' + 1;                                                           
    }                                                                                               
    else if(sscanf(str, " r%cx + %d", &reg, &push_value) == 2)            
    {                                                                                               
        opcode_tmp[_REG] = reg - 'a' + 1;                                                           
        opcode_tmp[_KONST] = 1;                                                                     
    }                                                                                               
    else if(sscanf(str, " r%cx", &reg) == 1)                              
    {                                                                                               
        opcode_tmp[_REG] = reg - 'a' + 1;                                                           
    }                                                                                               
    else if(sscanf(str, " %d", &push_value) == 1)                         
    {                                                                                               
        opcode_tmp[_KONST] = 1;                                                                     
    }
    if(reg != '!')
    {
        opcode_tmp[_REG] = reg - 'a' + 1;
    }
    if(bracket != '!')
    {
        opcode_tmp[_RAM]   = 1;                                                                     
    }
}


#define DEF_CMD(name, num, arg, code)                                                                   \
else if(strincmp(cmd, #name, cmd_size) == 0)                                                            \
{                                                                                                       \
    if(arg > 0)                                                                                    \
    {                                                                                                   \
        if(arg == CMD_USES_LABEL)                                                                                       \
        {                                                                                                   \
                opcode_buffer[ip++] = CMD_##name;                                                           \
                int jmp_ip = jmp_to(labels, stringed_buffer[i] + cmd_size + 1);                             \
                if(jmp_ip == LABEL_TO_UNTOUCHED)                                                            \
                {                                                                                           \
                    assembler->jmp_after[assembler->jmp_after_count++] = i;                                 \
                    assembler->jmp_after[assembler->jmp_after_count++] = ip;                                \
                }                                                                                           \
                opcode_buffer[ip++] = jmp_ip;                                                               \
        }                                                                                                   \
        else                                                                                                \
        {                                                                                                   \
            char *opcode_tmp = (char *)(opcode_buffer + ip);                                                \
            opcode_tmp[_CMD] = CMD_##name;                                                                  \
            ip++;                                                                                           \
            ChooseArgs(opcode_tmp, stringed_buffer[i] + cmd_size);\
            opcode_buffer[ip++] = push_value;                                                               \
            prev_two_arg = 1;                                                                                  \
        }                                                                                                   \
    }                                                                                                   \
    else                                                                                                \
    {                                                                                                   \
        opcode_buffer[ip++] = CMD_##name;                                                               \
        prev_two_arg = 0;                                                                                  \
    }                                                                                                   \
}                                   



int *Assemble(char **stringed_buffer, struct Label *labels, struct ASM *assembler)
{//TODO strincmp counts ("dup", "dup", 3) == ("d", "dup", 1)
    int *opcode_buffer = (int*)calloc(3 * assembler->strings_count, sizeof(int));
// consider resizing array
    int ip = 0;
    int prev_two_arg = 0; //TODO: change name since it will be used for cmds with 2 args, not only push

    for(int i = 0; i < assembler->strings_count; i++)
    {
        char cmd[50] = {}; 
        int cmd_size = 0;

        RemoveComments(stringed_buffer[i]);

        sscanf(stringed_buffer[i], "%[^:-123456789 ]%n", cmd, &cmd_size);

        if(strincmp(cmd, "dup", cmd_size) == 0)
        {
            if(prev_two_arg == 1)
            {
                opcode_buffer[ip]     = opcode_buffer[ip - 2];
                opcode_buffer[ip + 1] = opcode_buffer[ip - 1];
                ip += 2; 
            }
            else 
            {
                opcode_buffer[ip] = opcode_buffer[ip - 1];
                ip++;  
            }
            prev_two_arg = 0;
        }

        #include "commands.h"
        
        else if(strchr(stringed_buffer[i], ':'))     
        { 
            AddLabel(labels, stringed_buffer[i], ip);
        }
    }

        #undef DEF_CMD

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
    RemoveSpaces(buffer, symbols_count); 
    
    struct ASM assembler =
    {
        .opcode_buffer = NULL,
        .strings_count = CountStrings(buffer),
        .tokens = 0,
        .jmp_after = (int*)calloc(LABEL_MAX * 2, sizeof(int)),
        .jmp_after_count = 0,
    };

    if(assembler.jmp_after == NULL)
    {
        printf("assembler.jmp_after == NULL\n");
        return 0;
    }
    
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
        // все равно есть мемори лик внутри ассембла, там тоже добавлять свой фри? free(opcode == NULL) makes no sense 
        free(buffer);
        free(string_buffer);
        free(assembler.jmp_after);
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
    printf("done\n");
    return 0;
}