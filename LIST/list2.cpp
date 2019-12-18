#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

typedef int list_t;
const int DEFAULT_LIST_SIZE = 10;
const int POISON            = 666;
const int RESIZE_CONSTANT   = 10;
int       ERROR_CODE        = 0;

struct list
{
    list_t *data;
    int *next;
    int  *prev;
    int head;
    int tail;
    int next_free;
    int size;
    int max_size;
    bool is_sorted;
};

enum ERROR_CODES
{
    OK,
    LIST_IS_CORRUPTED
};

bool list_init(struct list* list);
bool add(struct list *list, list_t element);
bool add_after(struct list *list, list_t element, int index);
bool add_before(struct list *list, list_t element, int index);
bool remove(struct list *list, int index);
bool sort(struct list *list);
bool add_to_end(struct list *list, list_t element);
bool print_element(struct list *list, int index);
bool dump(struct list *list);
bool build_graph(struct list *list, const char *path);
bool resize(struct list *list, int resize_const);
bool deinit(struct list *list);
bool list_validation(struct list *list);
void swap(struct list *list, int a, int b);
bool test1(struct list *list);
bool test2(struct list *list);
bool test3(struct list *list);
bool test4(struct list *list);
bool test5(struct list *list);

int main()
{
    struct list list = {};
    list_init(&list);

    assert(test1(&list));

    deinit(&list);
    list_init(&list);

    assert(test2(&list));

    deinit(&list);
    list_init(&list);

    assert(test3(&list));

    deinit(&list);
    list_init(&list);

    assert(test4(&list));
    deinit(&list);

    return 0;
}

bool list_init(struct list* list)
{
    assert(list);

    list->data = (list_t*)calloc(DEFAULT_LIST_SIZE, sizeof(list_t));
    list->next = (int*)calloc(DEFAULT_LIST_SIZE, sizeof(int));
    list->prev = (int*)calloc(DEFAULT_LIST_SIZE, sizeof(int));

    for (int i = 1; i < DEFAULT_LIST_SIZE; i++)
    {
        list->next[i - 1] = -1 * i;
        list->prev[DEFAULT_LIST_SIZE - i] = -1 * (DEFAULT_LIST_SIZE - i - 1);
    }

    list->next[DEFAULT_LIST_SIZE - 1] = POISON;
    list->prev[0] = POISON;
    list->head = 0;
    list->tail = 0;
    list->next_free = 0;
    list->size = 0;
    list->max_size = DEFAULT_LIST_SIZE;
    list->is_sorted = true;
    return true;
}

bool add(struct list *list, list_t element)
{
    assert(list);
    if (!list_validation(list)) return false;

    if (list->next_free == POISON) return false;

    if (list->size == list->max_size ) resize(list, RESIZE_CONSTANT);

    list->data[list->next_free] = element;
    list->next_free = -1 * list->next[list->next_free];
    list->size++;
    
    if (!list_validation(list)) return false;
    return true;
}

bool add_after(struct list *list, list_t element, int index)
{
    assert(list);
    if (!list_validation(list)) return false;

    if (list->next[index] < 0 || list->prev[index] < 0 || index == POISON || index >= list->max_size) return false;

    if (list->size == list->max_size) resize(list, RESIZE_CONSTANT);

    int new_index = list->next_free;
    int old_index = list->next[index];

    if (index == list->tail) return add_to_end(list, element);

    if (!add(list, element)) return false;

    list->prev[old_index] = new_index;
    list->prev[new_index] = index;
    list->next[index] = new_index;
    list->next[new_index] = old_index;
    list->is_sorted = false;
    
    if (!list_validation(list)) return false;
    return true;
}

bool add_before(struct list *list, list_t element, int index)
{
    assert(list);

    return add_after(list, element, list->prev[index]);
}

bool remove(struct list *list, int index)
{
    assert(list);
    if (!list_validation(list)) return false;

    if (list->next[index] < 0 || list->prev[index] < 0) return false;

    int next_index = list->next[index];
    int prev_index = list->prev[index];

    if (list->head == index) list->head = list->next[list->head];

    if (prev_index != POISON) list->next[prev_index] = next_index;

    if (next_index != POISON) list->prev[next_index] = prev_index;

    list->next[index] = -1 * list->next_free;
    list->data[index] = POISON;
    list->next_free = index;
    list->size--;

    if (index != list->tail ) list->is_sorted = false;

    else list->tail = list->prev[list->tail];
    
    if (!list_validation(list)) return false;
    return true;
}

bool sort(struct list *list)
{
    assert(list);
    if (!list_validation(list)) return false;

    list->prev[0] = 0;
    int prev = 0;

    for (int i = 1; i < list->size; i++)
    {
        list->prev[list->next[prev]] = i;
        prev = list->next[prev];
    }

    for (int i = 1; i < list->size; i++)
    {
        if (list->prev[i] != i)
        { 
            swap(list, i, list->prev[i]);
            i--;
        }
    }

    list->prev[list->head] = POISON;

    for (int i =  0; i < list->size - 1; i++)
    {
        list->next[i] = i + 1;
        list->prev[i + 1] = i;
    }

    list->tail = list->size - 1;
    list->next[list->tail] = POISON;

    if(list->size != list->max_size) list->next_free = list->size;

    else list->next_free = POISON;

    for (int i = list->size; i < list->max_size; i++)
    {
        list->next[i] = -1 * (i + 1);
        list->prev[i + 1] = -1 * i;
    }

    list->next[list->max_size - 1] = POISON;
    list->is_sorted = true;

    if (!list_validation(list)) return false;
    return true;
}

bool add_to_end(struct list *list, list_t element)
{
    assert(list);
    if (!list_validation(list)) return false;

    if (list->size == list->max_size) resize(list, RESIZE_CONSTANT);

    int new_element = list->next_free;

    if (!add(list, element)) return false;

    list->next[new_element] = POISON;

    if (list->tail != new_element) list->next[list->tail] = new_element;

    if (list->head != new_element) list->prev[new_element] = list->tail;

    list->tail = new_element;
    
    if (!list_validation(list)) return false;
    return true;
}

bool print_element(struct list *list, int index)
{
    assert(list);
    if (!list_validation(list)) return false;

    if (index >= list->head && index <= list->tail) printf("%d\n", list->data[index]);

    return true;
}

bool dump(struct list *list)
{
    assert(list);

    printf("-------------------------------------\n");
    printf("|STATUS_CODE %d|next_free = %d|      \n", ERROR_CODE,  list->next_free);
    printf("-------------------------------------\n");
    printf("|   data     |    next     |    prev |\n");
    printf("-------------------------------------\n");

    for (int i = 0; i < list->max_size; i++)
    {
        printf("|%-2d. %-5d    |    %-5d    |    %-5d|\n",i,  list->data[i], list->next[i], list->prev[i]);
    }

    printf("-------------------------------------\n");
    return true;
}

bool build_graph(struct list *list, const char *path)
{
    assert(list);
    if (!list_validation(list)) return false;

    FILE *graph = fopen(path, "w");
    fprintf(graph, "digraph {\nnode[shape=record, fontname=\"Lobster\"];\n");
    fprintf(graph, "meta [xlabel=\"meta data\", label = \"<f0> head|<f1> size:%d|<f2> max size:%d|<f3> Error code %d\"];\n", list->size, list->max_size, ERROR_CODE);
    fprintf(graph, "%d [xlabel=\"%d\", label = \"<f0> data:%d |<f1> next:%d\"];\n",list->head, list->head, list->data[list->head], list->next[list->head]);
    fprintf(graph, "meta:<f0> -> %d;", list->head);
    int prev_node = list->head;
    int current_node = list->next[prev_node];

    while (current_node != POISON)
    {
        if (list->next[current_node] == POISON) fprintf(graph, "%d [xlabel=\"%d\", label = \"<f0> Prev: %d|<f1> data:%d\"];\n",current_node,  current_node, list->prev[current_node], list->data[current_node]);
        else fprintf(graph, "%d [xlabel=\"%d\", label = \"<f0> Prev: %d|<f1> data:%d |<f2> next:%d\"];\n",current_node,  current_node, list->prev[current_node], list->data[current_node], list->next[current_node]);
        fprintf(graph, "%d:<f2> -> %d:<f0>;\n", prev_node, current_node);
        prev_node = current_node;
        current_node = list->next[current_node];
    }

    fprintf(graph, "tail [label = \"<f0> tail\"];\n");
    fprintf(graph, "tail -> %d;\n", prev_node);
    fprintf(graph, "}");
    fclose(graph);
    return true;
}

bool resize(struct list *list, int resize_const)
{
    assert(list);
    if (!list_validation(list)) return false;

    list->data = (list_t*)realloc(list->data, sizeof(list_t) * (list->max_size + resize_const));
    list->next = (int*)realloc(list->next, sizeof(int) * (list->max_size + resize_const));
    list->prev = (int*)realloc(list->prev, sizeof(int) * (list->max_size + resize_const));

    list->next_free = list->size;

    for (int i = list->max_size; i < list->max_size + resize_const; i++)
    {
        list->data[i] = 0;
        list->next[i] = -1 * (i + 1);
        list->prev[i] = -1 * (i - 1);
    }

    list->next[list->max_size + resize_const - 1] = POISON;
    list->max_size = list->max_size + resize_const;
    
    if (!list_validation(list)) return false;
    return true;
}

bool deinit(struct list *list)
{
    assert(list); 
    if (list->data != NULL)
    {
        free(list->data);
        free(list->next);
        free(list->prev);
    }
    list->data = NULL;
    list->next = NULL;
    list->prev = NULL;
    return true;
}

bool list_validation(struct list *list)
{
    int *positions = new int[list->max_size]();
    int current_pos = list->head;
    int counter = 0;

    for (int i = 0; i < list->max_size; i++)
    {
        if (list->next[current_pos] > 0)
        { 
            positions[current_pos] = 1;
            counter++;
        }

        if (list->next[current_pos] == POISON) break;
        current_pos = list->next[current_pos];
    }

    if (counter != list->size)
    { 
        return false;
        ERROR_CODE = 1;
    }

    counter = 0;

    for (int i = 0; i < list->max_size; i++)
    {
        if (!positions[i]) counter++;
    }

    if (counter != (list->max_size - list->size))
    {
        return false;
        ERROR_CODE = 1;
    }

    delete[](positions);

    return true;
}

bool  get_element(struct list *list, int index, list_t *element)
{
    assert(list);
    if (!list_validation(list)) return false;

    if(!list->is_sorted) sort(list);
    *element = list->data[index];
    return true;
}

void swap(struct list *list, int a, int b)
{
    assert(list);
    
    list_t buffer = 0;
    int buffer2 = 0;
    buffer = list->data[a];
    list->data[a] = list->data[b];
    list->data[b] = buffer;
    buffer2 = list->prev[a];
    list->prev[a] = list->prev[b];
    list->prev[b] = buffer2;
}

bool test1(struct list *list)
{
    for (int i = 0; i < 6; i++)
    {
        if (!add_to_end(list , i)) return false;
        dump(list);
    }

    if(!add_after(list, 10, 2)) return false;

    if(!add_before(list, 20, 5)) return false;

    build_graph(list, "test1.dot");
    dump(list);
    return true;
}

bool test2(struct list *list)
{
    for (int i = 0; i < 15; i++)
    {
        if (!add_to_end(list , i)) return false;
    }
    build_graph(list, "test2.dot");
    dump(list);
    return true;
}

bool test3(struct list *list)
{
    for (int i = 0; i < 6; i++)
    {
        if (!add_to_end(list , i)) return false;
    }
    add_after(list, 10, 1);
    add_after(list, 20, 3);
    add_after(list, 30, 5);
    sort(list);

    add_before(list, 40, 4);
    add_before(list, 50, 4);
    add_before(list, 60, list->tail);
    sort(list);

    build_graph(list, "test3.dot");
    dump(list);
    return true;
}

bool test4(struct list *list)
{
    for (int i = 0; i < 6; i++)
    {
        if (!add_to_end(list , i)) return false;
    }

    remove(list, 0);
    remove(list, 4);
    add_to_end(list, 10);
    add_to_end(list, 20);
    remove(list, list->tail);
    build_graph(list, "test4.dot");
    list->next[2] = -5;
    assert(!list_validation(list));
    dump(list);
    return true;
}