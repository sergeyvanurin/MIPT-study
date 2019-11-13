#include <stdio.h>
#include <stdlib.h>

unsigned long hash_function(void *first_ptr, void *last_ptr)
{
    unsigned long hash = 0;
    int i = 0;
    while((char*)first_ptr + i != (char*)last_ptr)
    {
        printf("%lu\n", hash);
        hash = hash + (unsigned long)*((char*)first_ptr + i);
        i++;
    }
    return hash;
}

int main()
{
    int string[15];
    for (int i = 0; i < 15; i++)
    {
        string[i] = 1;
    }
    unsigned long hash = hash_function(&string[0], &string[9]) + hash_function(&string[9] + 1, &string[15]);
    printf("%lu\n", hash);
}