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

bool ERROR_FLAG = 0;
const int MAX_BINARY_SIZE = 10000;
const int MAX_LABEL_COUNT = 50;
const int MAX_BUFFER_SIZE = 50;

#define COMMAND_DEFINE(name, num, code, args)                                                                           \
                if (string != 0 && strcmp(string, #name) == 0)                                                          \
                {                                                                                                       \
                    binary_array[current_pos++] = (char)CMD_##name;                                                     \
                    bytes = bytes + sizeof(char);                                                                       \
                    for (int i = 0; i < args; i++)                                                                      \
                    {                                                                                                   \
                        string = strtok(NULL, " ");                                                                     \
                        if (string == 0)                                                                                \
                        {                                                                                               \
                            binary_array[current_pos++] = '2';\
                            bytes++;                                                          \
                            continue;                                                                                      \
                        }                                                                                               \
                        string[-1] = ' ';                                                                               \
                        if(string[0] == '[')                                                                            \
                        {                                                                                               \
                            binary_array[current_pos++] = '3';                                                          \
                            bytes += 1;\
                            char buffer [MAX_BUFFER_SIZE];\
                            sscanf(string, "%*[[] %[^]]", buffer);                                                                        \
                            string = buffer;                                                                     \
                                                                                                                        \
                        }                                                                                               \
                        if (strcmp(string, "ax") == 0)                                                                  \
                        {                                                                                               \
                            binary_array[current_pos++] = '0';                                                          \
                            binary_array[current_pos++] = (char)AX;                                                     \
                            bytes = bytes + sizeof(char) + sizeof(char);                                                \
                            continue;                                                                                   \
                        }                                                                                               \
                        if (strcmp(string, "bx") == 0)                                                                  \
                        {                                                                                               \
                            binary_array[current_pos++] = '0';                                                          \
                            binary_array[current_pos++] = (char)BX;                                                     \
                            bytes = bytes + sizeof(char) + sizeof(char);                                                \
                            continue;                                                                                   \
                        }                                                                                               \
                        if (strcmp(string, "dx") == 0)                                                                  \
                        {                                                                                               \
                            binary_array[current_pos++] = '0';                                                          \
                            binary_array[current_pos++] = (char)DX;                                                     \
                            bytes = bytes + sizeof(char) + sizeof(char);                                                \
                            continue;                                                                                   \
                        }                                                                                               \
                        if (strcmp(string, "cx") == 0)                                                                  \
                        {                                                                                               \
                            binary_array[current_pos++] = '0';                                                          \
                            binary_array[current_pos++] = (char)CX;                                                     \
                            bytes = bytes + sizeof(char) + sizeof(char);                                                \
                            continue;                                                                                   \
                        }                                                                                               \
                        for (int j = 0; j <= labels_count; j++)                                                         \
                        {                                                                                               \
                            if ((int)vanur_hash((unsigned char*)string) == labels[j])                                   \
                            {                                                                                           \
                                binary_array[current_pos++] = '1';                                                      \
                                int_to_char_array(binary_array, current_pos, labels[j + MAX_LABEL_COUNT]);              \
                                current_pos += 4;                                                                       \
                                bytes = bytes + sizeof(int) + sizeof(char);                                             \
                                continue_flag = 1;                                                                      \
                            }                                                                                           \
                        }                                                                                               \
                        if (continue_flag)                                                                              \
                        {                                                                                               \
                            continue_flag = 0;                                                                          \
                            continue;                                                                                   \
                        }                                                                                               \
                        int argument = atoi(string);                                                                    \
                        printf("1.%d\n", argument);                                                                       \
                        binary_array[current_pos++] = '1';                                                              \
                        int_to_char_array(binary_array, current_pos, argument);                                         \
                        current_pos += 4;                                                                               \
                        bytes = bytes + sizeof(argument) + sizeof(char);                                                \
                    }                                                                                                   \
                }                                                                                                       \


void int_to_char_array(char* array, int current_pos, int number)
{
    for (int i = 0; i < sizeof(number); i++)
    {
        array[current_pos++] = *((char*)(&number) + i);
    }
}

void assembly(char **index, int lines)
{
    assert(index);
    bool continue_flag = 0;
    char *string = 0;
    int bytes = 0;
    int labels[MAX_LABEL_COUNT * 2] = {0};
    char binary_array[MAX_BINARY_SIZE] = {0};
    int current_pos = 0;
    int labels_count = -1;
    for(int i = 0; i < lines; i++)
    {
        string = strtok(index[i], " ");
        #include "commands.h"
        if (string != 0 && string[0] == ':')
        {
            labels_count++;
            labels[labels_count] = (int)vanur_hash((unsigned char*)&string[1]);
            labels[labels_count + MAX_LABEL_COUNT] = bytes;
        }
    }
    current_pos = 0;
    bytes = 0;
    for(int i = 0; i < lines; i++)
    {
        string = strtok(index[i], " ");
        #include "commands.h"
    }
    FILE *binary = fopen("binary.binary", "w");
    fwrite(binary_array, sizeof(char), bytes, binary);
    fclose(binary);   
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