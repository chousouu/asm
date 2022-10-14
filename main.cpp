#include "asm.h"

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

    struct Label labels[LABEL_AMOUNT] = {};

    for(int i = 0; i < LABEL_AMOUNT; i++)
    {
        labels[i].label_to   = -1;
        labels[i].label_name = NULL;
    }


    int *machined_buffer = Assembly(string_buffer, strings_count, &machine_symbols_count);

    WriteBinaryFile(machined_buffer, machine_symbols_count);
}