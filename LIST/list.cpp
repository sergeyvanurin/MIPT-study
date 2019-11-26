#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
typedef int list_t;
const int DEFAULT_LIST_SIZE = 10;
const int POISON            = 666;
const int RESIZE_CONSTANT   =

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
};

bool add_to_end(struct list *list, list_t element);


bool list_init(struct list* list)
{
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
    return true;
}


bool add(struct list *list, list_t element)
{
    if (list->next_free == POISON) return false;
    list->data[list->next_free] = element;
    if (list->next_free == list->max_size - 1)
    {
        list->next_free = POISON;
    }
    else
    {
        list->next_free = -1 * list->next[list->next_free];
    }
    list->size += 1;
    return true;
}

bool add_after(struct list *list, list_t element, int index)
{
    if (index > list->tail || index < list->head || list->next[index] < 0) return false;
    int new_index = list->next_free;
    int old_index = list->next[index];
    if (index == list->tail) return add_to_end(list, element);
    if (!add(list, element)) return false;
    list->prev[old_index] = new_index;
    list->prev[new_index] = index;
    list->next[index] = new_index;
    list->next[new_index] = old_index;
    return true;
}

bool add_before(struct list *list, list_t element, int index)
{
    if (index > list->tail || index <= list->head || list->next[index] < 0) return false;
    int new_index = list->next_free;
    int old_index = list->prev[index];
    if (!add(list, element)) return false;
    list->next[old_index] = new_index;
    list->next[new_index] = index;
    list->prev[new_index] = old_index;
    list->prev[index] = new_index;
    return true;
}

bool remove(struct list *list, int index)
{
    if (index > list->tail || index < list->head || list->next[index] < 0) return false;
    int next_index = list->next[index];
    int prev_index = list->prev[index];
    list->next[prev_index] = next_index;
    list->prev[next_index] = prev_index;
    list->next[index] = -1 * list->next_free;
    list->next_free = index;
    list->size -= 1;
    return true;
}

bool sort(struct list *list)
{
    list_t *buffer = (list_t*)calloc(list->max_size, sizeof(list_t));
    buffer[0] = list->data[list->head];
    int counter = list->next[0];
    for (int i = 1; i < list->size; i++)
    {
        buffer[i] = list->data[counter];
        counter =  list->next[counter];
    }
    for (int i =  1; i < list->size; i++)
    {
        list->next[i] = i + 1;
        list->prev[i + 1] = i;
    }
    list->tail = list->size - 1;
    list->next[list->tail] = POISON;
    list->prev[list->size] = -1 * (list->tail);
    list->next_free = list->size;
    for (int i = list->size + 1; i < list->max_size; i++)
    {
        list->next[i - 1] = -1 * i;
        list->prev[i] = -1 * (i - 1);
    }
    list->next[list->max_size] = POISON;

    free(list->data);
    list->data = buffer;
    return true;
}

bool add_to_end(struct list *list, list_t element)
{
    int new_element = list->next_free;
    if (!add(list, element)) return false;
    list->next[new_element] = POISON;
    if (list->tail != new_element) list->next[list->tail] = new_element;
    if (list->head != new_element) list->prev[new_element] = list->tail;
    list->tail = new_element;
    return true;
}

bool print_element(struct list *list, int index)
{
    if (index >= list->head && index <= list->tail) printf("%d\n", list->data[index]);
    return true;
}

bool dump(struct list *list)
{
    printf("-------------------------------------\n");
    printf("|   data     |    next     |    prev |\n");
    printf("-------------------------------------\n");
    for(int i = 0; i < list->max_size; i++)
    {
        printf("|%d. %-5d    |    %-5d    |    %-5d|\n",i,  list->data[i], list->next[i], list->prev[i]);
    }
    printf("-------------------------------------\n");
    return true;
}

bool build_graph(struct list *list)
{
    FILE *graph = fopen("graph.dot", "w");
    fprintf(graph, "digraph {\nnode[shape=record, fontname=\"Gapstown AH\"];\n");
    fprintf(graph, "0 [xlabel=\"%d\", label = \"<f0> Prev: %d|<f1> data:%d |<f2> next:%d\"];\n",list->head, list->prev[list->head], list->data[list->head], list->next[list->head]);
    int prev_node = list->head;
    int current_node = list->next[prev_node];
    while (current_node != POISON)
    {
        fprintf(graph, "%d [xlabel=\"%d\", label = \"<f0> Prev: %d|<f1> data:%d |<f2> next:%d\"];\n",current_node,  current_node, list->prev[current_node], list->data[current_node], list->next[current_node]);
        fprintf(graph, "%d:<f2> -> %d:<f0>;\n", prev_node, current_node);
        prev_node = current_node;
        current_node = list->next[current_node];
    }
    fprintf(graph, "}");
    return true;
}

bool resize(struct list *list, int resize_const)
{
    list->data = (list_t*)realloc(list->data, sizeof(list_t) * (DEFAULT_LIST_SIZE + resize_const));
    list->next = (int*)realloc(list->data, sizeof(list_t) * (DEFAULT_LIST_SIZE + resize_const));
    list->prev = (int*)realloc(list->data, sizeof(list_t) * (DEFAULT_LIST_SIZE + resize_const));
    for (int i = list->max_size; i < list->max_size)

}


int main()
{
struct list list;
list_init(&list);
for (int i = 0; i < 5; i++)
{
    assert(add_to_end(&list, i));
    print_element(&list, i);
}
add_after(&list, 10, 2);
for (int i = 0; i < 6; i++)
{
    printf("%d\n", list.next[i]);
}
assert(add_before(&list, 20, 4));
//sort(&list);
dump(&list);
build_graph(&list);
}