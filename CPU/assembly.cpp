#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "command_enum.h"
#include "vanur_lib.h"


enum REGISTERS
{
    AX = 100,
    BX = 101,
    CX = 102,
    DX = 103
};



#define COMMAND_DEFINE(name, num, code, args)                               \
                if (strcmp(string, #name) == 0)                             \
                {                                                           \
                    fprintf(binary_file, "%c", CMD_##name);                 \
                    for (int i = 0; i < args; i++)                          \
                    {                                                       \
                        string = strtok(NULL, " ");                         \
                        if (strcmp(string, "ax") == 0)                        \
                        {                                                   \
                            fprintf(binary_file, "0%c", (unsigned char)AX);           \
                            continue;                                       \
                        }                                                   \
                        if (strcmp(string, "bx") == 0)                        \
                        {                                                   \
                            fprintf(binary_file, "0%c", (unsigned char)BX);           \
                            continue;                                       \
                        }                                                   \
                        if (strcmp(string, "dx") == 0)                        \
                        {                                                   \
                            fprintf(binary_file, "0%c", (unsigned char)DX);           \
                            continue;                                       \
                        }                                                   \
                        if (strcmp(string, "cx") == 0)                        \
                        {                                                   \
                            fprintf(binary_file, "0%c", (unsigned char)CX);           \
                            continue;                                       \
                        }                                                   \
                        int argument = atoi(string);                       \
                        fprintf(binary_file, "1");             \
                        fwrite(&argument, sizeof(argument), 1, binary_file);\
                    }                                                       \
                }                                                           \




void assembly(char **index, int lines)
{
    assert(index);
    char *string;
    FILE *binary_file = fopen("binary.binary", "w");
    assert(binary_file);
    fwrite(&lines, sizeof(lines), 1, binary_file);
    for(int i = 0; i < lines; i++)
    {
        string = strtok(index[i]," ");
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
    int lines = line_counter(data, 1);
    void *index = indexing((char*)data, lines);
    assembly((char**)index, lines);
    free(index);
    free(data);
}
