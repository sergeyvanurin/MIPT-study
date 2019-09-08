#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#define MAX_PATH_LENGTH 30
#define ASCII_DIFERENCE ('a' - 'A')


/******************************************************************************
 * @brief Read file and put it's content in a memory block
 *
 * Function opens a file from path,
 * allocates memmory, puts data from the
 * file to the allocated memmory and
 * outputs void pointer to the 
 * first element of the memmory block
 * 
 * @param charr array with path value (MAX_PATH_LENGTH long)
 * 
 * 
 * @return void pointer to the memmory block
 *****************************************************************************/
void *read_file(char path[MAX_PATH_LENGTH]);


/******************************************************************************
 * @brief Formats and counts strings of a text
 *
 * Function takes char pointer to the first element
 * of a text, swaps '\n' with '\0' and outputs number of strings 
 *
 * @param pointer to the first element of a text
 * 
 * 
 * @return number of strings
 *****************************************************************************/
int preprocessing(char *text);



/******************************************************************************
 * @brief indexes pointers to a first elements of each string in a text 
 *
 * Function goes through each element of the text and saves 
 * pointers of every first letter of a string into allocated
 * index array
 *
 * @param pointer to the first element of the text
 * @param number of strings
 * 
 * 
 * @return pointer to index
 *****************************************************************************/
void *indexing(char *text, int strings);


/******************************************************************************
 * @brief Compares 2 strings
 *
 * Function compares every char of 2 strings until one of them is later 
 * in alphabet or both strings are the same. Function returns one
 * number based on a list below. Functions ignores non letter ellements
 * 
 * 0 - base_string is smaller than string
 * 1 - string is smaller than base_string
 * 2 - base_string is equal to stray
 *
 * @param pointer to a string
 * @param pointer to a base_string
 * 
 * 
 * @return comparison result
 *****************************************************************************/
int comparator(char *string, char *base_string);


/******************************************************************************
 * @brief sorts strings based on comparator value
 *
 * Function performs quicksort algorithm based on 
 * comparator
 *
 * @param pointer to the index
 * @param first element in a string batch
 * @param last element of a string batch
 *****************************************************************************/
void sort(char **index, int first, int last);


/******************************************************************************
 * @brief Determines if element is a letter
 *
 * Function returns 0 if element is not letter
 * and 1 if it is
 *
 * @param one character
 * 
 * 
 * @output result of evaluation
 *****************************************************************************/
int is_letter(char character);


/******************************************************************************
 * @brief Determines if element is a number
 *
 * Function returns 0 if element is capital
 * and 1 if it is
 *
 * @param one character
 * 
 * 
 * @output result of evaluation
 *****************************************************************************/
int is_capital(char character);


/******************************************************************************
 * @brief swaps 2 strings
 *
 * Function swaps 2 pointers to a string
 *
 * @param pointer to pointer to string 
 * @param pointer to pointer to string
 *****************************************************************************/
void swap_strings(char **first, char **second);


/******************************************************************************
 * @brief checks length
 *
 * Function checks if path is longer than MAX_PATH_LENGTH
 * and asserts if is 
 *
 * @param char array of a path
 *****************************************************************************/
void path_check(char path[MAX_PATH_LENGTH]);


int main()
{
    void *text, **index;
    char path[MAX_PATH_LENGTH], *textptr, **indexptr, *line;
    int strings;

    printf("enter file path: ");
    gets(path);
    path_check(path);
    text = read_file(path);
    textptr = (char*)text;
    strings = preprocessing(textptr);
    index = indexing(textptr, strings);
    indexptr = (char**)index;
    sort(indexptr, 0, strings - 1);
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


void *read_file(char path[MAX_PATH_LENGTH])
{
    void *mem_block;
    int size;

    FILE *file;
    file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Error reading a file");
        exit(0);
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    mem_block = calloc(size + 1, sizeof(char)); 
    fread(mem_block, sizeof(char), size, file);
    fclose(file);
    return mem_block;
}


int preprocessing (char *text)
{
    int i, strings;

    strings = 0;
    i = 0;
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
    void *index;
    int i, j, first;
    char **indexptr;

    first = 0;
    i = 0;
    j = 0;
    index = calloc(strings, sizeof(char**));
    indexptr = (char**)index;
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


int comparator(char *string, char *base_string)
{    
    int i, j;

    i = 0;
    j = 0;
    while (string[i] != 0 || base_string[j] != 0)
    {
        while (is_letter(string[i]) == 0)
        {
            i++;
        }
        while (is_letter(base_string[j]) == 0)
        {
            j++;
        }
        if (is_capital(string[i]) == 1)
        {
            string[i] = string[i] + ASCII_DIFERENCE;
        }
        if (is_capital(base_string[j]) == 1)
        {
            base_string[j] = base_string[j] + ASCII_DIFERENCE;
        }  
        if (string[i] < base_string[j])
        {
            return 1;
        }
        else if (string[i] > base_string[i])
        {
            return 0;
        }
        i++;
        j++;
    }
    return 2; 
}


void sort(char **index, int first, int last)
{
    int i, j;
    char *temp;

    if (first < last)
    {
        i = first;
        j = last;
        while (i < j)
        {
            while ((comparator(index[i], index[first]) == 1 || comparator(index[i], index[first]) == 2) && i < last)
            {
                i++;
            }
            while ((comparator(index[j], index[first]) == 0 || comparator(index[j], index[first]) == 2) && j > first)
            {
                j--;
            }
            if (i < j)
            {
                swap_strings(&index[i], &index[j]);
            }
        }
        swap_strings(&index[first], &index[j]);
        sort(index, first, j - 1);
        sort(index, j + 1, last);
    }
}


int is_letter(char character)
{
    if ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z') || character == '\0')
    {
        return 1;
    }
    return 0;
}


int is_capital(char character)
{
    if (character >= 'A' && character <= 'Z')
    {
        return 1;
    }
    return 0;
}


void swap_strings(char **first, char **second)
{
    char *temp;

    temp = *first;
    *first = *second;
    *second = temp;
}


void path_check(char path[MAX_PATH_LENGTH])
{
    char ch;

    for (int i = 0; i < MAX_PATH_LENGTH; i++)
    {
        ch = path[i];
        if (ch == 0)
        {
            break;
        }
        if (ch != 0 && i == 29)
        {
            printf("Error: Path is too long!");
            exit(0);
        }
    }
}