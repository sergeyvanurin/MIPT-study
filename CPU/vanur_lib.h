#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "stack_lib.h"

void *file_read(char *path)
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


int line_counter(void *data, bool mode)
{
    assert(data);
    char *text = (char*)data;
    int i = 0, strings = 0;
    while (text[i] != '\0')
    {
        if (text[i] == '\n')
        {
            if (mode) text[i] = '\0';
            strings++;
        }
        i++; 
    }
    return strings + 1; 
}


void *indexing(char *data, int strings)
{
    int i = 0, j = 0, first = 0;

    assert(data);
    assert(strings);
    
    void *index = calloc(strings, sizeof(char*));
    char **indexptr = (char**)index;
    while (i < strings)
    {
        if (data[j] == '\0')
        {
            indexptr[i] = &data[first];
            first = j + 1;
            i++;
        }
        j++;
    }
    return index;
}
