
#include "musthave_include.h"

const int BUFFER_MAX_SIZE = 200;
const int MAX_DATA_SIZE   = 30;
const int LEFT            = 0;
const int RIGHT           = 1;
int metki = 0;

struct node 
{
    char data[MAX_DATA_SIZE];
    struct node *parent;
    struct node *left;
    struct node *right;
};

typedef struct node node_struct;

int get_func(char **tokens, node_struct *tree, int token_counter);
int get_args(char **tokens, node_struct *tree, int token_counter);
int get_body(char **tokens, node_struct *tree, int token_counter);
int get_op(char **tokens, node_struct *tree, int token_counter);
int get_el(char **tokens, node_struct *tree, int token_counter);


char** detokenize(char *string)
{
    char **tokens = (char**)calloc(BUFFER_MAX_SIZE, sizeof(char *));
    tokens[0] = strtok(string, "@");
    int counter = 1;
    while (char *token = strtok(NULL, "@"))
    {
        tokens[counter++] = token;
    }
    assert(tokens);
    return tokens;

}

char* get_string(const char *path)
{
    FILE *input = fopen(path, "r");
    fseek(input, 0, SEEK_END);
    int size = ftell(input);
    fseek(input, 0, SEEK_SET);
    char *string = (char*)calloc(size + 1, sizeof(char));
    fread(string, size, sizeof(char), input);
    fclose(input);
    return string;
}

char* next_token()
{
    return strtok(NULL, "@");
}

bool node_init(node_struct *node)
{
    if (!node) return 0;
    node->parent = NULL;
    node->left = NULL;
    node->right = NULL;
    return 1;
}

bool deinit(node_struct *tree)
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

node_struct* create_tree()
{
    struct node *new_node = (struct node*)calloc(1, sizeof(struct node));
    node_init(new_node);
    return new_node;
}

node_struct* parse(const char* path)
{
    printf("get parser\n");
    char *string = get_string(path);
    char** detokenized_string = detokenize(string);
    node_struct *tree = create_tree();
    printf("%s", detokenized_string[0]);
    int token_counter = 0;
    token_counter = get_func(detokenized_string, tree, token_counter);
    free(detokenized_string);
    free(string);
    return tree->right;
}

int get_func(char **tokens, node_struct *tree, int token_counter)
{
    printf("get func ");
    printf("%s\n", tokens[token_counter]);
    sprintf(tree->data, ";");
    while (strcmp(tokens[token_counter], "END") != 0)
    {
        add_node(tree, RIGHT);
        tree = tree->right;
        sprintf(tree->data, ";");
        if(*tokens[token_counter + 1] == '(')
        {
            add_node(tree, LEFT);

            sprintf(tree->left->data, "$%s", tokens[token_counter]);
            token_counter++;
            token_counter = get_args(tokens, tree->left, token_counter + 1);
            token_counter = get_body(tokens, tree->left, token_counter); 
        }
    }
    return token_counter;
}

int get_args(char **tokens, node_struct *tree, int token_counter)
{
    printf("get args ");
    printf("%s\n", tokens[token_counter]);
    while (*tokens[token_counter] != ')')
    {
        if (*tokens[token_counter] != ',')
        {
            add_node(tree, LEFT);
            tree = tree->left;
            sprintf(tree->data, ",");
            add_node(tree, RIGHT);
            token_counter = get_el(tokens, tree->right, token_counter);
        }
        else
        {
            token_counter++;
        }
    }
    token_counter++;
    return token_counter;
}

int get_body(char **tokens, node_struct *tree, int token_counter)
{
    printf("get body ");
    printf("%s\n", tokens[token_counter]);
    if (*tokens[token_counter] == '{')
    {
        token_counter++;
        while (*tokens[token_counter] != '}')
        {
            add_node(tree, RIGHT);
            tree = tree->right;
            sprintf(tree->data, "op");
            add_node(tree, LEFT);
            while(*tokens[token_counter] != ';')
            {
                token_counter = get_op(tokens, tree->left, token_counter);
            }
            token_counter++;
        }
        token_counter++;
    }
    return token_counter;
}

int get_op(char **tokens, node_struct *tree, int token_counter)
{
    printf("get op ");
    printf("%s\n", tokens[token_counter]);
    if (strcmp(tokens[token_counter], "if") == 0)
    {
        sprintf(tree->data, "if");
        token_counter++;
        if (*tokens[token_counter] == '(')
        {
            token_counter++;
            add_node(tree, LEFT);
            while (*tokens[token_counter] != ')')
            {
                token_counter = get_op(tokens, tree->left, token_counter);   
            }
            token_counter++;
            if (*tokens[token_counter] == '{')
            {
                add_node(tree, RIGHT);
                sprintf(tree->right->data, "if-else");
                node_struct *new_node = (node_struct*)calloc(1, sizeof(node_struct));
                token_counter = get_body(tokens, new_node, token_counter);
                tree->right->left = new_node->right;
                new_node->left = NULL;
                new_node->right = NULL;
                free(new_node);
            }
            if (strcmp(tokens[token_counter], "else") == 0)
            {
                token_counter++;
                if(*tokens[token_counter] == '{')
                {
                    token_counter = get_body(tokens, tree->right, token_counter);
                }
            }
        }
        printf("if: %s\n", tokens[token_counter]);
        return token_counter;
    }
    if (strcmp(tokens[token_counter], "while") == 0)
    {
        token_counter++;
        sprintf(tree->data, "while");
        if (*tokens[token_counter] == '(')
        {
            token_counter++;
            add_node(tree, LEFT);
            while (*tokens[token_counter] != ')')
            {
                token_counter = get_op(tokens, tree->left, token_counter);   
            }
            token_counter++;
            if (*tokens[token_counter] == '{')
            {
                token_counter = get_body(tokens, tree, token_counter);
            }
        }
        return token_counter;
    }
    if (strcmp(tokens[token_counter], "for") == 0)
    {
        token_counter++;
        sprintf(tree->data, "for");
        if (*tokens[token_counter] == '(')
        {
            printf("bolshoy xuy");
            token_counter++;
            add_node(tree, LEFT);
            node_struct* temp = tree->left;
            for (int i = 0; i < 3; i++)
            {
                sprintf(temp->data, ".");
                add_node(temp, RIGHT);
                while (*tokens[token_counter] != ';')
                {
                    token_counter = get_op(tokens, temp->right, token_counter);   
                }
                token_counter++;
                if (i != 2)
                {
                    add_node(temp, LEFT);
                    temp = temp->left;
                }
            }
            token_counter++;
            if (*tokens[token_counter] == '{')
            {
                token_counter = get_body(tokens, tree, token_counter);
            }
        }
        return token_counter;
    }

    if (strcmp(tokens[token_counter], "ret") == 0)
    {
        sprintf(tree->data, "%s", tokens[token_counter]);
        add_node(tree, LEFT);
        token_counter++;
        token_counter = get_op(tokens, tree->left, token_counter);
        return token_counter;
    }

    if (strcmp(tokens[token_counter], "print") == 0)
    {
        sprintf(tree->data, "%s", tokens[token_counter]);
        add_node(tree, LEFT);
        token_counter++;
        if (*tokens[token_counter] == '(')
        {
            token_counter++;
            add_node(tree, LEFT);
            while (*tokens[token_counter] != ')')
            {
                token_counter = get_op(tokens, tree->left, token_counter);   
            }
            token_counter++;
        }
        return token_counter;
        /*
        sprintf(tree->data, "%s", tokens[token_counter]);
        add_node(tree, LEFT);
        token_counter++;
        token_counter = get_op(tokens, tree->left, token_counter);
        return token_counter;*/
    }

    token_counter = get_el(tokens, tree, token_counter);
    if (*tokens[token_counter] == '=' || *tokens[token_counter] == '+' || *tokens[token_counter] == '-' || *tokens[token_counter] == '*' || *tokens[token_counter] == '/' || *tokens[token_counter] == '>' || *tokens[token_counter] == '<')
    {
        node_struct *new_node = (node_struct*)calloc(1, sizeof(node_struct));
        sprintf(new_node->data, "%s", tree->data);
        new_node->left = tree->left;
        new_node->right = tree->right;
        new_node->parent = tree;
        tree->left = new_node;
        sprintf(tree->data, "%s", tokens[token_counter]);
        token_counter++;
        add_node(tree, RIGHT);
        token_counter = get_op(tokens, tree->right, token_counter);
    }
    printf("%s\n", tokens[token_counter]);
    return token_counter;

    
}

int get_el(char **tokens, node_struct *tree, int token_counter)
{
    printf("get el ");
    printf("%s\n", tokens[token_counter]);
    if (*tokens[token_counter + 1] == '(')
    {
        sprintf(tree->data, "$%s", tokens[token_counter++]);
        token_counter++;
        while(*tokens[token_counter] != ')')
        {
            add_node(tree, LEFT);
            tree = tree->left;
            sprintf(tree->data, ",");
            add_node(tree, RIGHT);
            while (*tokens[token_counter] != ',' && *tokens[token_counter] != ')')
            {
                token_counter = get_op(tokens, tree->right, token_counter);
            }
        }
        token_counter++;
        return token_counter;
    }
    else
    {
        sprintf(tree->data, "%s", tokens[token_counter]);
        token_counter++;
        return token_counter;
    }
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

bool if_var(node_struct *tree)
{
    if (tree->data[0] >= 'a' && tree->data[0] <= 'z') return 1;
    return 0;
}

bool if_num(node_struct *tree)
{
    if (tree->data[0] >= '0' && tree->data[0] <= '9') return 1;
    return 0;
}

void assem(node_struct *tree, FILE *output)
{
    if (!tree) return;
    printf("BLYAAAA %s\n", tree->data);
    if (tree->data[0] == ';')
    {
        fprintf(output, "%s:\n", tree->left->data + 1);
        node_struct *tmp = tree->left;
        while(tmp->left)
        {
            assem(tmp->right, output);
            tmp = tmp->left;
        }
        assem(tree->left->right, output);
        fprintf(output, "RETURN\n");
    }
    else
    if (strcmp(tree->data, "op") == 0)
    {
        assem(tree->left, output);
        assem(tree->right, output);
    }
    else
    if (*tree->data == '+')
    {
        assem(tree->left, output);
        assem(tree->right, output);
        fprintf(output, "ADD\n");
    }
    else
    if (*tree->data == '-')
    {
        assem(tree->left, output);
        assem(tree->right, output);
        fprintf(output, "SUB\n");
    }
    else
    if (*tree->data == '/')
    {
        assem(tree->left, output);
        assem(tree->right, output);
        fprintf(output, "DIV\n");
    }
    else
    if (*tree->data == '*')
    {
        assem(tree->left, output);
        assem(tree->right, output);
        fprintf(output, "PROD\n");
    }
    else
    if (strcmp(tree->data, "if") == 0)
    {

        node_struct *tmp = tree->left;
        assem(tmp->left, output);
        assem(tmp->right, output);
        if (*tmp->data == '=') fprintf(output, "JE if_metka%d\n", metki++);
        else
        if (*tmp->data == '>') fprintf(output, "JA if_metka%d\n", metki++);
        else
        if (*tmp->data == '<') fprintf(output, "JB if_metka%d\n", metki++);

        int if_metka = metki - 1;
        if(tree->right->right)
        {
            assem(tree->right->right, output);

        }
        fprintf(output, "JMP exit_metka%d\n", if_metka);
        fprintf(output, "if_metka%d:\n", if_metka);
        assem(tree->right->left, output);
        fprintf(output, "exit_metka%d:\n", if_metka);

    }
    else
    if (strcmp(tree->data, "while") == 0)
    {
        fprintf(output, "while_metka%d:\n", metki++);
        int while_metka = metki - 1;
        node_struct *tmp = tree->left;
        assem(tmp->left, output);
        assem(tmp->right, output);
        if (*tmp->data == '=') fprintf(output, "JNE exit_metka%d\n", while_metka);
        if (*tmp->data == '>') fprintf(output, "JBE exit_metka%d\n", while_metka);
        if (*tmp->data == '<') fprintf(output, "JAE exit_metka%d\n", while_metka);
        assem(tree->right, output);
        fprintf(output, "JMP while_metka%d:\n", while_metka);
        fprintf(output, "exit_metka%d:\n", while_metka);
    }
    else
    if (*tree->data == '=')
    {
        assem(tree->right, output);
        fprintf(output, "POP [%d]\n", tree->left->data[0]);
    }
    else
    if (tree->data[0] == '$')
    {
        node_struct *tmp = tree->left;
        while (tmp)
        {
            assem(tmp->right, output);
            tmp = tmp->left;
        }
        fprintf(output, "CALL :%s\n", tree->data);

    }
    else
    if (strcmp(tree->data, "print") == 0)
    {
        assem(tree->left, output);
        fprintf(output, "OUT\n");
    }
    else
    if (if_var(tree)) fprintf(output, "PUSH [%d]\n", tree->data[0]);
    else
    if (if_num(tree)) fprintf(output, "PUSH %f\n", atof(tree->data));
}




void compile(node_struct *tree)
{
    FILE *comp = fopen("compiled.txt", "w");
    printf("BLYAAA\n");
    fprintf(comp, "CALL main\nEXIT\n");
    assem(tree, comp);
    fclose(comp);
}


int main()
{

    node_struct* tree = parse("output.txt");
    char *string = get_string("output.txt");
    char **tokens = detokenize(string);
    FILE *output = fopen("tree.txt", "w");
    save_tree(tree, output);
    fclose(output);
    compile(tree);


    int err_code = get_binary_file("compiled.txt", "pls.bin");

    if (!err_code)
        err_code = run_binary_file("pls.bin");


    return err_code;
}