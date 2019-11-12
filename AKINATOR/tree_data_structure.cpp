#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef char tree_type;

const int LEFT  = 0;
const int RIGHT = 1;
const int MAX_DATA_SIZE = 200;

struct node 
{
    int question;
    char data[MAX_DATA_SIZE];
    struct node *left;
    struct node *right;
};

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

bool node_init(struct node *node)
{
    if (!node) return 0;
    node->question = 0;
    node->left = NULL;
    node->right = NULL;
    return 1;
}

bool add_node(struct node *node, int direction)
{
    if (!node) return 0;
    struct node *new_node = (struct node*)calloc(1, sizeof(struct node));
    node_init(new_node);
    if (direction == 0)
    {
        node->left = new_node;
    }
    else
    {
        node->right = new_node;
    }
    return 1;
}

void ask_questions(struct node *tree)
{
    int answer = 0;
    printf("Is it %s? ", tree->data);
    printf("1 - yes , 0 - no:\n");
    scanf("%d", &answer);
    if (answer == 1)
    {
        if (tree->left != NULL)
        {
            ask_questions(tree->left);
        }
        else
        {
            if (tree->question == 0) printf("I knew it!\n");
            else
            {
                add_node(tree, LEFT);
                printf("I don't know what you mean :(\nWhat or who did you mean?: ");
                scanf("%s", tree->left->data);
            }
            
        }
    }
    if (answer == 0)
    {
        if (tree->right != NULL)
        {
            ask_questions(tree->right);
        }
        else
        {
            if (tree->question == 0)
            {
                add_node(tree, LEFT);
                add_node(tree, RIGHT);
                printf("I was close! What did you pick?:");
                scanf("%s", tree->left->data);
                strcpy(tree->right->data, tree->data);
                printf("what characterstic differ %s from %s: ",tree->left->data, tree->data);
                scanf("%s", tree->data);
                tree->question = 1;
            }
            else
            {
                add_node(tree, RIGHT);
                printf("I don't know what you mean :(\n What or who did you mean?:");
                scanf("%s", tree->right->data);
            }
        }
    }
}


void save_tree(struct node *tree, FILE *out)
{
    if (!tree)
    {
        fprintf(out, "# ");
    }
    else
    {
        fprintf(out, "%s %d ", tree->data, tree->question);
        save_tree(tree->left, out);
        save_tree(tree->right, out);
    }
}

void load_tree(struct node *tree, FILE *in, char *buffer)
{
    strcpy(tree->data, buffer);
    fscanf(in, "%d", &tree->question);
    fscanf(in, "%s", buffer);
    if (strcmp(buffer, "#") != 0)
    {
        add_node(tree, LEFT);
        node_init(tree->left);
        load_tree(tree->left, in, buffer);
    }
    fscanf(in, "%s", buffer);
    if (strcmp(buffer, "#") != 0)
    {
        add_node(tree, RIGHT);
        node_init(tree->right);
        load_tree(tree->right, in, buffer);
    }
}

void akinator(struct node *tree)
{
    char init_question[] = "ALIVE";
    strcpy(tree->data, init_question);
    tree->question = 1;
    int i = 1;
    while (i)
    {
        ask_questions(tree);
        printf("Do you want to play again? 1 - yes, 0 - no\n");
        scanf("%d", &i);
    }
}

int main()
{
    char buffer[500];
    struct node tree;
    char *data = (char*)file_read("akinator.txt");
    node_init(&tree);
    FILE *in = fopen("akinator.txt", "r");
    fscanf(in, "%s", buffer);
    load_tree(&tree, in, buffer);
    akinator(&tree);
    FILE *out = fopen("akinator.txt", "w");
    save_tree(&tree, out);
}