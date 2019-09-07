#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
/*  
    read_file
    Функция, которая считывает текст в выделенный блок памяти и возвращает его адрес 
    input: char path[30] - путь у файлу(не длиннее 30 символов)
    output: void *mem_block - указатель к первому элементу прочтенного текста.
*/
void *read_file(char path[30])
{
    char *textptr;
    void *mem_block;
    int size;
    FILE *file;
    file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error reading a file ");
        assert(!(file == NULL));
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    mem_block = calloc(size + 1, sizeof(char)); 
    fread(mem_block, sizeof(char), size, file);
    fclose(file);
    return mem_block;
}

/*
    preprocessing
    Функция, которая заменяет \n на \0 у каждой строки и возвращает количесство строк.
    input: char *array - указатель к первому элементу текста.
    ouput: int strings - колиество строк.
*/

int preprocessing(char *array)
{
    int i, strings;
    strings = 0;
    i = 0;
    while(array[i] != '\0')
    {
        if (array[i] == '\n')
        {
            array[i] = '\0';
            strings++;
        }
        i++; 
    }
    return strings + 1; 
}


void indexing(char *array,char **index, int strings)
{
    int i, j, first;
    first = 0;
    i = 0;
    j = 0;
    while (i < strings)
    {
        if (array[j] == '\0')
        {
            index[i] = &array[first];
            first = j + 1;
            i++;
        }
        j++;
    }
}

/* 0 - base is smaller than array, 1 - array is smaller than base, 2 - base is equal to array */
int comparator(char *array, char *base)
{    
    int i;
    i = 0;
    while(array[i] != 0 || base [i] != 0)
    {
        if (array[i] < base[i])
        {
            return 1;
        }
        else if (array [i] > base[i])
        {
            return 0;
        }
        i++;
    }
    return 2; 
}


void sort(char **index, int first, int last)
{
    int i, j;
    char *temp;
    if(first < last)
    {
        i = first;
        j = last;
        while (i < j)
        {
            while((comparator(index[i], index[first]) == 1 || comparator(index[i], index[first]) == 2) && i < last)
            {
                i++;
            }
            while((comparator(index[j], index[first]) == 0 || comparator(index[j], index[first]) == 2) && j > first)
            {
                j--;
            }
            if(i < j)
            {
                temp = index[i];
                index[i] = index[j];
                index[j] = temp;
            }
        }
        temp = index[first];
        index[first] = index[j];
        index[j] = temp;
        sort(index, first, j - 1);
        sort(index, j + 1, last);
    }
}


int main()
{
    void *index, *text;
    int strings;
    char ch, *textptr, **indexptr, path[30], *line;
    printf("enter file path: ");
    gets(path);
    for (int i = 0; i < 30; i++)
    {
        ch = path[i];
        if (ch == 0)
        {
            break;
        }
        if (ch != 0 && i == 30)
        {
            assert(0);
        }
    }
    text = read_file(path);
    textptr = (char*) text;
    strings = preprocessing(textptr);
    index = calloc(strings, sizeof(char**));
    indexptr = (char**) index;
    indexing(textptr, indexptr, strings);
    sort(indexptr,0,strings - 1);
    for (int i = 0; i < strings; i++)
    {
        line = indexptr[i];
        if (line[0] != 0)
        {
            printf("%s\n", line);
        }
    }
    free(index);
    free(text);
}