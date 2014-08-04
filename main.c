#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "datas.h"
#include "functions.h"

#define SAVE_PATH "tree.NEL"
#define DATA_FILE "examples.txt"
#define LOG
#define LOAD_TREE

struct attribute_t *attributes_set;
int n_attr_set;
FILE *log_file;

int main(void)
{
    /*struct example_t example_to_label;
    int i*/
    struct node_t *tree;
#if defined (SAVE_TREE)
    struct example_t *examples;
    int n_ex;
    FILE *fout_tree = fopen(SAVE_PATH, "w+");
#endif

#ifdef LOG
    log_file = fopen("progression.log", "w+");
#else
    log_file = stdout;
#endif
    srand((unsigned int)time(NULL));

#if defined (SAVE_TREE)
    get_datas_from_file(DATA_FILE, &examples, &n_ex, &attributes_set, &n_attr_set);
    tree = build_ID3_tree(examples, n_ex, attributes_set, n_attr_set);
    printf("Tree is build and its size is %d.\n", get_tree_size(tree));
#elif defined (LOAD_TREE)
    tree = load_tree("tree.NEL", &attributes_set, &n_attr_set);
    display_tree(tree);
#endif
    display_tree(tree);

#ifdef LOG
    fclose(log_file);
#endif

#if defined (SAVE_TREE)
    fdisplay_tree_tab(fout_tree, tree, 0);
#endif

    /*DELETE(example_to_label.label);
    example_to_label.n_attributes = n_attr_set;
    example_to_label.attributes = malloc(sizeof(*example_to_label.attributes) * example_to_label.n_attributes);
    for(i = 0; i < example_to_label.n_attributes; ++i)
        strcpy(example_to_label.attributes[i], attributes_set[i].values[rand()%attributes_set[i].n_values]);
    display_example(&example_to_label);
    label_example(&example_to_label, tree);
    printf("class is [%s]\n", example_to_label.label);
    free(example_to_label.attributes);*/

    delete_tree(&tree);
#if defined (SAVE_TREE)
    free_examples(&examples, n_ex);
    free_attributes(&attributes_set, n_attr_set);
    fclose(fout_tree);
#endif
    return EXIT_SUCCESS;
}
