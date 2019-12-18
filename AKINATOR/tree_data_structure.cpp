#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

const int LEFT  = 0;
const int RIGHT = 1;
const int MAX_DATA_SIZE = 200;
const int MAX_BUFFER_SIZE = 100;

#define max(x,y) ((x) >= (y)) ? (x) : (y)

#define min(x,y) ((x) <= (y)) ? (x) : (y)

#define say(string, ...)                                    \
        printf(string, ## __VA_ARGS__);                     \
        sprintf(tts_buffer + 4, string, ## __VA_ARGS__);    \
        system(tts_buffer);                                 \

struct node 
{
    char data[MAX_DATA_SIZE];
    struct node *left;
    struct node *right;
};

void *file_read(const char *path);
struct node* create_tree();
bool node_init(struct node *node);
bool add_node(struct node *node, int direction);
void ask_questions(struct node *tree);
void save_tree(struct node *tree, FILE *out);
int load_tree(struct node *tree, char *string, int offset);
bool akinator(struct node *tree,const char *path);
bool search(const char *key_word, struct node *tree, int path[MAX_BUFFER_SIZE], int counter);
int rec_graph(struct node *tree, int prev, int counter, FILE *graph, bool ans);
void graph(struct node *tree, const char *path);
bool compare(const char* a, const char* b, struct node *tree);
bool deinit(struct node *tree);

int main()
{
    
    struct node *tree = create_tree();
    akinator(tree, "akinator.txt");
    deinit(tree);
    return 1;
    
}

void *file_read(const char *path)
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

struct node* create_tree()
{
    struct node *new_node = (struct node*)calloc(1, sizeof(struct node));
    node_init(new_node);
    return new_node;
}

bool node_init(struct node *node)
{
    if (!node) return 0;
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
    char tts_buffer[MAX_DATA_SIZE] = {0};
    sprintf(tts_buffer, "say ");
    int answer = 0;

    say("Is it %s? ", tree->data)
    say("1 - yes , 0 - no:\n")
    
    scanf("%d", &answer);
    if (answer == 1)
    {
        if (tree->left != NULL) ask_questions(tree->left);
        else
        {
            if (tree->left == NULL && tree->right == NULL)
            {
                say("I knew it!\n")
            } 
            else
            {
                add_node(tree, LEFT);

                say("I don't know what you mean :(\nWhat or who did you pick?: ")

                scanf(" %[^\n]", tree->left->data);
            }
        }
    }

    if (answer == 0)
    {
        if (tree->right != NULL) ask_questions(tree->right);
        else
        {
            if (tree->left == NULL && tree->right == NULL)
            {
                add_node(tree, LEFT);
                add_node(tree, RIGHT);

                say("I was close! What did you pick?:");

                scanf(" %[^\n]", tree->left->data);
                sprintf(tree->right->data, "%s", tree->data);

                say("What characterstic differ %s from %s: ", tree->left->data, tree->data);

                scanf(" %[^\n]", tree->data);
            }
            else
            {
                add_node(tree, RIGHT);

                say("I don't know what you mean :(\n What or who did you mean?:");

                scanf(" %[^\n]", tree->right->data);
            }
        }
    }
}

void save_tree(struct node *tree, FILE *out)
{
    if (tree)
    {
        fprintf(out, "{%s", tree->data);

        save_tree(tree->left, out);
        save_tree(tree->right, out);

        fprintf(out, "}");
    }
}

int load_tree(struct node *tree, char *string, int offset)
{
    if (*(string + offset) == '{')
    {
        sscanf(string + offset + 1, "%[^{}]", tree->data);
        offset += strlen(tree->data) + 1;

        if (*(string + offset) == '}') return offset + 1;

        add_node(tree, LEFT);
        add_node(tree, RIGHT);

        offset = load_tree(tree->left, string, offset);
        offset = load_tree(tree->right, string, offset);
    }
    return offset + 1;
}

bool akinator(struct node *tree, const char *path)
{
    char tts_buffer[MAX_BUFFER_SIZE] = {0};
    sprintf(tts_buffer, "say ");

    char *string = (char*)file_read(path);
    load_tree(tree, string,  0);
    free(string);

    int i = 1;
    while (i)
    {
        say("Choose gamemode\n")
        say("1 The Akinator game\n")
        say("2 Show tree\n")
        say("3 Compare two entities\n")
        say("0 exit\n")
        scanf("%d", &i);
        char a[MAX_BUFFER_SIZE] = {0};
        char b[MAX_BUFFER_SIZE] = {0};
        switch (i)
        {
            case 1:
                ask_questions(tree);
                break;
            case 2:
                graph(tree, "akinator.dot");
                break;
            case 3:
                say("Enter two entities\n")
                scanf("%s %s", a, b);
                compare(a, b, tree);
                break;
            case 0:
                say("see you next time\n")
                return 1;
            default:
                say("Invalid choice!\n")
                break;
        }
        say("Do you want to play again? 1 - yes, 0 - no\n");
        scanf("%d", &i);
    }
    return 1;
}

bool search(const char *key_word, struct node *tree, int path[MAX_BUFFER_SIZE], int counter)
{
    if (!tree)
    {    
        return 0;
    }
    if (tree->data != NULL && strcmp(key_word, tree->data) != 0)
    {
        if (search(key_word, tree->left, path, counter + 1))
        {
            path[counter + 1] = 1; 
            return 1;
        }
        if (search(key_word, tree->right, path, counter + 1))
        {
            path[counter + 1] = 0;
            return 1;
        }
        return 0;
    }
    if (strcmp(key_word, tree->data) == 0)
    {
        if (tree->left == NULL && tree->right == NULL)
        {
            path[0] = counter;
            return 1;
        }
        return 0;
    }
    return 0;
}



int rec_graph(struct node *tree, int prev, int counter, FILE *graph, bool ans)
{
    if (tree)
    {
        fprintf(graph, "%d [label = \"%s\"];\n",counter++, tree->data);
        fprintf(graph, "%d -> %d", prev, counter - 1);
        if (ans) fprintf(graph, "[label = \"yes\"];\n");
        else fprintf(graph, "[label = \"no\"];\n");
        prev = counter - 1;
        counter = rec_graph(tree->left, prev, counter, graph, 1);
        counter = rec_graph(tree->right, prev, counter, graph, 0);
    }
    return counter;
}

void graph(struct node *tree, const char *path)
{
    int counter = 0;
    FILE *graph = fopen(path, "w");
    fprintf(graph, "digraph {\nnode[shape = ellipse, fontname=\"Lobster\"];\n");
    fprintf(graph, "%d [label = \"%s\"];\n", counter++, tree->data);
    counter = rec_graph(tree->left, 0, counter, graph, 1);
    rec_graph(tree->right, 0, counter, graph, 0);
    fprintf(graph, "}");
    fclose(graph);
}

bool deinit(struct node *tree)
{
    if (tree)
    {
        if (tree->left) deinit(tree->left);
        if (tree->right) deinit(tree->right);
        tree->left = NULL;
        tree->right = NULL;
        free(tree);
        return 1;
    }
    return 0;
}

bool compare(const char* a, const char* b, struct node *tree)
{
    char tts_buffer[MAX_BUFFER_SIZE] = {0};
    sprintf(tts_buffer, "say ");
    int path_a[MAX_BUFFER_SIZE] = {0};
    int path_b[MAX_BUFFER_SIZE] = {0};

    if (!search(a, tree, path_a, 0)) 
    {
        printf("Couldn't find first entity\n");
        return 0;
    }

    if (!search(b, tree, path_b, 0))
    {   
        printf("Couldn't find second entity\n");
        return 0;
    }

    if (path_a[1] != path_b[1])
    {
        say("They are completely different\n");
    }

    struct node *temp = tree;
    int i = 1;

    for (; i <= min(path_a[0], path_b[0]); i++)
    {
        if (path_a[i] != path_b[i]) break;
        if(path_a[i])
        {
            say("They are both %s\n", tree->data);
            tree = tree->left;
        }
        else
        {
            say("They are both not %s\n", tree->data);
            tree = tree->right;
        }
    }
    int break_point = i;
    i = 1;
    if (break_point != path_a[0])
    { 
        tree = temp;
        say("They differ in\n")
        say("first entity is\n")
        while (i <= path_a[0])
        {
            if(path_a[i])
            {
                if (i >= break_point) 
                {
                    say("%s\n", tree->data)
                }
                tree = tree->left;
            }
            else
            {
                if (i >= break_point) 
                {
                    say("not %s\n", tree->data)
                }
                tree = tree->right;
            }
            i++;
        }
        i = 1;
        tree = temp;
        say("second entity is\n")
        while (i <= path_b[0])
        {
            if(path_b[i])
            {
                if (i >= break_point) 
                {
                    say("%s\n", tree->data)
                }
                tree = tree->left;
            }
            else
            {
                if (i >= break_point) 
                {
                    say("not %s\n", tree->data)
                }
                tree = tree->right;
            }
            i++;
        }
    }
    return 1;
}
