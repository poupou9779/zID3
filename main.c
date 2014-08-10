#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "datas.h"
#include "functions.h"

#define C45

#define SAVE_PATH "tree.NEL"
#ifdef C45
# define DATA_FILE "examples C45.txt"
#else
# define DATA_FILE "examples ID3.txt"
#endif
#define LOG

#define A printf("dbg  -- ");

struct attribute_t *attributes_set;
int n_attr_set;
FILE *log_file;

int main(void)
{
    struct node_t *tree;
    struct example_t *examples;
    int n_ex;
    FILE *fout_tree = fopen(SAVE_PATH, "w+");

#ifdef LOG
    log_file = fopen("progression.log", "w+");
#else
    log_file = stdout;
#endif
    srand((unsigned int)time(NULL));
    get_datas_from_file(DATA_FILE, &examples, &n_ex, &attributes_set, &n_attr_set);
#ifdef C45
    tree = build_C45_tree(examples, n_ex, attributes_set, n_attr_set);
#else
    tree = build_ID3_tree(examples, n_ex, attributes_set, n_attr_set);
#endif
    printf("Tree is built and its size is %d.\n", get_tree_size(tree));
    display_tree(tree);
#ifdef LOG
    fclose(log_file);
#endif
    fsave_tree(fout_tree, tree, attributes_set, n_attr_set);

    delete_tree(&tree);
    free_examples(&examples, n_ex);
    free_attributes(&attributes_set, n_attr_set);
    fclose(fout_tree);
    return EXIT_SUCCESS;
}
