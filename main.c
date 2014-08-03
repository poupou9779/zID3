#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "datas.h"
#include "functions.h"

#define DATA_FILE "examples.txt"
#define LOG

#ifdef LOG
FILE *log_file;
#endif

struct attribute_t *attributes_set;
int n_attr_set;

int main(void)
{
    struct example_t *examples;
    struct example_t example_to_label;
    struct node_t *tree;
    int n_ex, i;
    srand((unsigned int)time(NULL));
#ifdef LOG
    log_file = fopen("progression.log", "w+");
#endif

    get_datas_from_file(DATA_FILE, &examples, &n_ex, &attributes_set, &n_attr_set);
    tree = build_ID3_tree(examples, n_ex, attributes_set, n_attr_set);

#ifdef LOG
    fclose(log_file);
#endif

    display_tree(tree);

    DELETE(example_to_label.label);
    example_to_label.n_attributes = n_attr_set;
    example_to_label.attributes = malloc(sizeof(*example_to_label.attributes) * example_to_label.n_attributes);
    for(i = 0; i < example_to_label.n_attributes; ++i)
        strcpy(example_to_label.attributes[i], attributes_set[i].values[rand()%attributes_set[i].n_values]);
    display_example(&example_to_label);
    label_example(&example_to_label, tree);
    printf("class is [%s]\n", example_to_label.label);

    free(example_to_label.attributes);
    /*delete_tree(&tree);*/
    return EXIT_SUCCESS;
}
