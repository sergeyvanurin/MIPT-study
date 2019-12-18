#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <strings.h>

const int BUFFER_MAX_SIZE = 10000;
const int MAX_DATA_SIZE   = 300;
const int LEFT            = 0;
const int RIGHT           = 1;

struct node 
{
    char data[MAX_DATA_SIZE];
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
void parse(char* path, node_struct *tree);
char* node_eval(node_struct *tree);
bool node_init(node_struct *node);

bool node_init(node_struct *node)
{
    if (!node) return 0;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    return 1;
}

struct node* create_tree()
{
    struct node *new_node = (struct node*)calloc(1, sizeof(struct node));
    node_init(new_node);
    return new_node;
}

bool deinit(struct node *tree)
{
    if (tree)
    {
        if (tree->left) deinit(tree->left);
        if (tree->right) deinit(tree->right);
        tree->right = NULL;
        tree->left = NULL;
        free(tree);
    }
    return 1;
}

bool make_it_easy(node_struct *tree)
{
    if (tree->data[0] == '*' || tree->data[0] == '/')
    {
        if (tree->left->data[0] == '0' || tree->right->data[0] == '0')
        {
            sprintf(tree->data, "%c", '0');
            deinit(tree->left);
            deinit(tree->right);
            tree->left = NULL;
            tree->right = NULL;
            return 1;
        }
        
        if (tree->left && tree->left->data[0] == '1' && tree->left->data[1] == 0)
        {
            deinit(tree->left);
            node_struct *temp = tree->right;
            sprintf(tree->data, "%s", temp->data);
            tree->left = temp->left;
            tree->right = temp->right;
            temp->left = NULL;
            temp->right = NULL;
            free(temp);
            return 1;
        }
        if (tree->right && tree->right->data[0] == '1' && tree->right->data[1] == 0)
        {
            deinit(tree->right);
            node_struct *temp = tree->left;
            sprintf(tree->data, "%s", temp->data);
            tree->left = temp->left;
            tree->right = temp->right;
            temp->left = NULL;
            temp->right = NULL;
            free(temp);
            return 1;
        }

        if (tree->left->left == NULL && tree->left->right == NULL && tree->right->left == NULL && tree->right->right == NULL)
        {
            if (tree->left->data[0] != 'x' && tree->right->data[0] != 'x')
            {
                if (tree->data[0] == '/') sprintf(tree->data, "%f", atof(tree->left->data) / atof(tree->right->data));
                else sprintf(tree->data, "%f", atof(tree->left->data) * atof(tree->right->data));
                deinit(tree->left);
                deinit(tree->right);
                tree->left = NULL;
                tree->right = NULL;
                return 1;
            }
        }
    }

    if (tree->data[0] == '+' || (tree->data[0] == '-' && tree->data[1] == 0))
    {
        if (tree->left->data[0] == '0')
        {
            deinit(tree->left);
            node_struct *temp = tree->right;
            sprintf(tree->data, "%s", temp->data);
            tree->left = temp->left;
            tree->right = temp->right;
            temp->left = NULL;
            temp->right = NULL;
            free(temp);
            return 1;
        }
        if (tree->right->data[0] == '0')
        {
            deinit(tree->right);
            node_struct *temp = tree->left;
            sprintf(tree->data, "%s", temp->data);
            tree->left = temp->left;
            tree->right = temp->right;
            temp->left = NULL;
            temp->right = NULL;
            free(temp);
            return 1;
        }

        if (tree->left->left == NULL && tree->left->right == NULL && tree->right->left == NULL && tree->right->right == NULL)
        {
            if (tree->left->data[0] != 'x' && tree->right->data[0] != 'x')
            {
                if (tree->data[0] == '+') sprintf(tree->data, "%f", atof(tree->left->data) + atof(tree->right->data));
                else sprintf(tree->data, "%f", atof(tree->left->data) - atof(tree->right->data));
                deinit(tree->left);
                deinit(tree->right);
                tree->left = NULL;
                tree->right = NULL;
                return 1;
            }
        }
    }
    return 0;
}

void easy(node_struct *tree)
{
    if (tree)
    {
        if (tree->left != NULL) easy(tree->left);
        if (tree->right != NULL) easy(tree->right);
        make_it_easy(tree);
    }
}

char *simplify(char *expression)
{
    node_struct *temp = create_tree();
    parse(expression, temp);
    char *result = node_eval(temp);
    deinit(temp);
    return result;
}

char* node_eval(node_struct *tree)
{
    if(tree->left)
    {
        if (tree->right)
        {
            char *a = node_eval(tree->left);
            char *b = node_eval(tree->right);
            char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
            if (*tree->data == '+')
            {
                sprintf(result, "%s%s%s", a, tree->data, b);
            }
            if (tree->data[0] == '-' && tree->data[1] == 0)
            {
                if (*tree->right->data != '*' && *tree->right->data != '/')
                {
                    sprintf(result, "%s%s(%s)", a, tree->data, b);
                }
                else
                {
                    sprintf(result, "%s%s%s", a, tree->data, b);
                }
            }
            if (*tree->data == '*' || *tree->data == '/')
            {
                if ((tree->left->data[0] == '-' && tree->left->data[1] == 0 ) || *tree->left->data == '+' )
                {
                    if ((tree->right->data[0] == '-' && tree->right->data[1] == 0 ) || *tree->right->data == '+')
                    {
                        sprintf(result, "(%s)%s(%s)", a, tree->data, b);
                    }
                    else
                    {
                        sprintf(result, "(%s)%s%s", a, tree->data, b);
                    }
                }
                else
                {
                    if ((tree->right->data[0] == '-' && tree->right->data[1] == 0 ) || *tree->right->data == '+')
                    {
                        sprintf(result, "%s%s(%s)", a, tree->data, b);
                    }
                    else
                    {
                        sprintf(result, "%s%s%s", a, tree->data, b);
                    }
                }
            }
            if (*tree->data == '^')
            {
                sprintf(result, "(%s)^(%s)", a, b);
            }
            free(a);
            free(b);
            return result;
        }
        char *a = node_eval(tree->left);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "%s(%s)", tree->data, a);
        free(a);
        return result;
    }
    char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
    sprintf(result, "%s", tree->data);
    return result;
}


void save_tree(node_struct *tree, FILE *out)
{
    if (!tree)
    {
        fprintf(out, "# ");
    }
    else
    {
        printf("%s\n", tree->data);
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
    string = get_T(string, tree);

    while(*string == '+' || *string == '-')
    {
        node_struct *new_node = (node_struct*)calloc(1, sizeof(node_struct));
        sprintf(new_node->data, "%s", tree->data);

        new_node->left = tree->left;
        new_node->right = tree->right;
        new_node->parent = tree;

        tree->left = new_node;
        sprintf(tree->data, "%c", *string);
        string++;
        add_node(tree, RIGHT);
        string = get_E(string, tree->right);
    }
    return string;
}

char* get_T(char *string, node_struct *tree)
{
    string = get_P(string, tree);
    while(*string == '*' || *string == '/' || *string == '^')
    {
        node_struct *new_node = (node_struct*)calloc(1, sizeof(node_struct));
        sprintf(new_node->data, "%s", tree->data);

        new_node->left = tree->left;
        new_node->right = tree->right;
        new_node->parent = tree;
        
        tree->left = new_node;
        sprintf(tree->data, "%c", *string);
        string++;
        add_node(tree, RIGHT);
        string = get_T(string, tree->right);
    }
    return string;
}

char* get_P(char *string, node_struct *tree)
{
    if (*string == '(')
    {
        string++;
        string = get_E(string, tree);        
        assert(*string == ')');
        string++;
        return string;
    }
    string = get_N(string, tree);
    return string;
}

char* get_N(char *string, node_struct *tree)
{
    if (*string >= 'a' && *string <= 'z' && *string != 'x')
    {
        int scanned = 0;
        sscanf(string, "%[cosintgрhln]%n", tree->data, &scanned);
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
    if ((*string >= '0' && *string <= '9') || *string == 'x' || *string == '-' || *string == 'e')
    {
        int sign = 0;
        int scanned = 0;
        sscanf(string, "%[-]%n", tree->data, &sign);
        string += sign;
        sscanf(string, "%[0-9x.e]%n", tree->data + sign, &scanned);
        string += scanned;
        return string;
    }
    return string;
}

void parse(char* string, node_struct *tree)
{
    get_E(string, tree);
    easy(tree);
}

char* deriv(node_struct *tree)
{
    if (*tree->data == '+' || (tree->data[0] == '-' && tree->data[1] == 0))
    {
        char *a = deriv(tree->left);
        char *b = deriv(tree->right);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "(%s)%s(%s)", a, tree->data, b);
        free(a);
        free(b);
        return result;
    }
    if (*tree->data == '*')
    {
        char *a = deriv(tree->left);
        char *b = deriv(tree->right);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "(%s)*(%s)+(%s)*(%s)", a, node_eval(tree->right), node_eval(tree->left), b);
        free(a);
        free(b);
        return result;
    }
    if (*tree->data == '/')
    {
        char *a = deriv(tree->left);
        char *b = deriv(tree->right);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "((%s)*(%s)-(%s)*(%s))/((%s)^(2))", a, node_eval(tree->right), node_eval(tree->left), b, node_eval(tree->right));
        return result;
    }
    if (tree->data[0] == 'c')
    {
        char *a = deriv(tree->left);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        if (tree->data[1] == 'o')
        {
            sprintf(result, "((-1)*sin(%s))*(%s)", node_eval(tree->left), a);
            free(a);
            return result;
        }
        sprintf(result, "(-1)*(%s)/((sin(%s))^(2))", a, node_eval(tree->left));
        free(a);
        return result;
    }
    if (tree->data[0] == 's')
    {
        char *a = deriv(tree->left);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "(cos(%s))*(%s)", node_eval(tree->left), a);
        free(a);
        return result;
    }
    if (tree->data[0] == 't')
    {
        char *a = deriv(tree->left);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "(%s)/((cos(%s))^(2))", a, node_eval(tree->left));
        free(a);
        return result;
    }
    if ((tree->data[0] >= '0' && tree->data[0] <= '9') || (tree->data[0] == '-' && (tree->data[1] >= '0' && tree->data[1] <= '9')))
    {
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "%c", '0');
        return result;
    }
    if (*tree->data == 'x')
    {
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "%c", '1');
        return result;
    }
    if (*tree->data == '^')
    {
        char *a = deriv(tree->left);
        char *b = deriv(tree->right);
        char *result = (char*)calloc(BUFFER_MAX_SIZE, sizeof(char));
        sprintf(result, "(%s)*((%s)*ln(%s)+(((%s)/(%s))*(%s)))", node_eval(tree), b, node_eval(tree->left), node_eval(tree->right), node_eval(tree->left), a);
        free(a);
        free(b);
        return result;
    }
    return NULL;
}

void latex(const char *path)
{

}

int main()
{
    node_struct *tree = create_tree();
    FILE *output = fopen("output.txt", "w");
    node_init(tree);
    char *expression = get_string("input.txt");
    fprintf(output, "%s\n", expression);
    parse(expression, tree);
    char *derived = deriv(tree);
    char *simplified = simplify(derived);
    fprintf(output, "%s\n", simplified);
    deinit(tree);
    tree = create_tree();
    parse(simplified, tree);
    for(int i = 1; i < 1; i++)
    {
        derived = deriv(tree);
        simplified = simplify(derived);
        fprintf(output, "%s\n", simplified);

        deinit(tree);
        tree = create_tree();
        parse(simplified, tree);
    }

}