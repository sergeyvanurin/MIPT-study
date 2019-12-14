#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const int BUFFER_MAX_SIZE = 300;
const int MAX_DATA_SIZE   = 300;
const int LEFT            = 0;
const int RIGHT           = 1;

struct node 
{
    char data[MAX_DATA_SIZE] = {}
    ;
    struct node *parent;
    struct node *left;
    struct node *right;
};

typedef struct node node_struct;

bool add_node(node_struct *node, int direction);
node_struct* upper_add(node_struct *node, int direction);
char* get_string(const char *path);
char* get_E(char *string, node_struct *tree);
char* get_T(char *string, node_struct *tree);
char* get_P(char *string, node_struct *tree);
char* get_N(char *string, node_struct *tree);
char* get_G(char* string, node_struct *tree);
void parse(const char* path, node_struct *tree);
bool node_init(node_struct *node);

bool node_init(node_struct *node)
{
    if (!node) return 0;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    return 1;
}

void save_tree(node_struct *tree, FILE *out)
{
    if (!tree)
    {
        fprintf(out, "# ");
    }
    else
    {
        printf("%s %p", tree->data, tree);
        fprintf(out, "%s ", tree->data);
        save_tree(tree->left, out);
        save_tree(tree->right, out);
    }
}

bool add_node(node_struct *node, int direction)
{
    if (!node) return 0;
    node_struct *new_node = (node_struct*)calloc(1, sizeof(node_struct));
    node_init(new_node);
    if (direction == 0)
    {
        node->left = new_node;
    }
    else
    {
        node->right = new_node;
    }
    new_node->parent = node;
    return 1;
}

node_struct* upper_add(node_struct *node, int direction)
{
    node_struct *new_node = (node_struct*)calloc(1, sizeof(node_struct));
    node_init(new_node);
    if (direction == 0)
    {
        new_node->left = node;
    }
    else
    {
        new_node->right = node;
    }
    if (node->parent != NULL)
    {
        if (node->parent->left == node)
        {
            node->parent->left = new_node;
        }
        else
        {
            node->parent->right = new_node;
        }
        new_node->parent = node->parent;
    }
    node->parent = new_node;
    return new_node;
}

char* get_string(const char *path)
{
    FILE *input = fopen(path, "r");
    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, SEEK_SET);
    char *string = (char*)calloc(size + 1, sizeof(char));
    fread(string, size, sizeof(char), input);
    int counter = 0;
    for (int i = 0; i < size + 1; i++)
    {
        if (string[i] != ' ')
        {
            string[counter++] = string[i];
        }
    }
    assert(realloc(string, sizeof(char) * counter));
    fclose(input);
    return string;
}

char* get_E(char *string, node_struct *tree)
{
    printf("E");
    string = get_T(string, tree);
    while(*string == '+' || *string == '-')
    {
        tree = upper_add(tree, LEFT);
        sprintf(tree->data, "%c", *string);
        string++;
        add_node(tree, RIGHT);
        string = get_T(string, tree->right);
    }
    return string;
}

char* get_T(char *string, node_struct *tree)
{
    printf("T");
    string = get_P(string, tree);
    while(*string == '*' || *string == '/')
    {
        printf("t");
        tree = upper_add(tree, LEFT);
        sprintf(tree->data, "%c", *string);
        string++;
        add_node(tree, RIGHT);
        string = get_E(string, tree->right);
    }
    return string;
}

char* get_P(char *string, node_struct *tree)
{
    printf("P");
    if (*string == '(')
    {
        string++;
        node_struct *new_node = (node_struct*)calloc(1, sizeof(node_struct));
        node_init(new_node);
        add_node(new_node, LEFT);
        string = get_E(string, new_node->left);
        sprintf(tree->data, "%s", new_node->left->data);
        tree->left = new_node->left->left;
        tree->right = new_node->left->right;
        assert(*string == ')');
        string++;
        free(new_node);
        return string;
    }
    string = get_N(string, tree);
    return string;
}

char* get_N(char *string, node_struct *tree)
{
    printf("N");
    if (*string > 'a' && *string < 'z' && *string != 'x')
    {
        int scanned = 0;
        sscanf(string, "%[cosintgрh]%n", tree->data, &scanned);
        printf("%s %d ", tree->data, scanned);
        string += scanned;
        add_node(tree, LEFT);
        string = get_P(string, tree->left);
        return string;
    }
    string = get_G(string, tree);
    return string;
}

char* get_G(char* string, node_struct *tree)
{
    printf("G");
    if ((*string > '0' && *string < '9') || *string == 'x')
    {
        int scanned = 0;
        sscanf(string, "%[0-9x]%n", tree->data, &scanned);
        printf("%s %d ", tree->data, scanned);
        string += scanned;
        return string;
    }
    return string;
}

void parse(const char* path, node_struct *tree)
{
    node_init(tree);
    char *string = get_string(path);
    printf("%s", string);
    assert(*get_E(string, tree) == 0);
}



int main()
{
    node_struct tree;
    node_struct *parent = &tree;
    node_init(&tree);
    const char path[] = "input.txt";
    FILE *output = fopen("output.txt", "w");
    parse(path, &tree);
    while (parent->parent != NULL)
    {
        parent = parent->parent;
    }
    save_tree(parent, output);
    return 0;
    fclose(output);
}