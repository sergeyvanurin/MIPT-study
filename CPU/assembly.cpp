#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "command_enum.h"

#define COMMAND_DEFINE(name, num, code, args)                               \
                if (strcmp(string, #name) == 0)                             \
                {                                                           \
                    fprintf(binary_file, "%c", CMD_##name);                 \
                    for (int i = 0; i < args; i++)                          \
                    {                                                       \
                        int argument = atoi(strtok(NULL, " "));             \
                        printf("%d %lu ", argument, sizeof(argument));      \
                        fwrite(&argument, sizeof(argument), 1, binary_file);\
                    }                                                       \
                }                                                           \


void *file_read(char path[30])
{
    FILE *file = fopen(path, "r");
    assert(file);
    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    fseek(file, 0, SEEK_SET);
    void *data = calloc(size + 1, sizeof(char));
    fread(data, sizeof(char), size, file);
    fclose(file);
    return data;
}

int line_counter(void *data)
{
    assert(data);
    char *text = (char*)data;
    int i = 0, strings = 0;
    while (text[i] != '\0')
    {
        if (text[i] == '\n')
        {
            text[i] = '\0';
            strings++;
        }
        i++; 
    }
    return strings + 1; 
}


void *indexing(char *text, int strings)
{
    int i = 0, j = 0, first = 0;

    assert(text);
    assert(strings);
    
    void *index = calloc(strings, sizeof(char*));
    char **indexptr = (char**)index;
    while (i < strings)
    {
        if (text[j] == '\0')
        {
            indexptr[i] = &text[first];
            first = j + 1;
            i++;
        }
        j++;
    }
    return index;
}

void assembly(char **index, int lines)
{
    assert(index);
    char *string;
    FILE *binary_file = fopen("binary.binary", "w");
    assert(binary_file);
    printf("%p\n", binary_file);
    for(int i = 0; i < lines; i++)
    {
        string = strtok(index[i]," ");
        printf("4\n");
        if (string != NULL)
        {
            #include "commands.h"
        }
        else
        {
            #include "commands.h"
        }
    }
}


int main()
{
    char path[30] = "binary.txt";
    void *data = file_read(path);
    int lines = line_counter(data);
    void *index = indexing((char*)data, lines);
    assembly((char**)index, lines);
    free()
}