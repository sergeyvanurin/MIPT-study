#include <stdio.h>
#include <stdlib.h>
#include <asserth.h>


#define COMMAND_DEFINE(name, num, code, args)
        {
            
        }



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