#ifndef FUNCTIONS_H
#define FUCNTIONS_H
#include "datas.h"

#include <stdio.h> /*FILE*/

void get_datas_from_file(const char *path, struct example_t **ex, int *n_ex, struct attribute_t **attr, int *n_attr);

struct node_t *build_ID3_tree(struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr);

#define display_examples(e, n) fdisplay(stdout, e, n)
void fdisplay_examples(FILE *f, const struct example_t *examples, int n_ex);

#define display_example(e) fdisplay_example(stdout, e)
void fdisplay_example(FILE *f, const struct example_t *example);

#define display_attributes(a, n) fdisplay_attributes(stdout, a, n)
void fdisplay_attributes(FILE *f, const struct attribute_t *attributes, int n_attr);

#define display_attribute(a) fdisplay_attribute(stdout, a)
void fdisplay_attribute(FILE *f, const struct attribute_t *attribute);

#define display_tree(n) fdisplay_tree(stdout, n)
#define fdisplay_tree(f, n) fdisplay_tree_tab(f, n, 0)
void fdisplay_tree_tab(FILE *f, struct node_t *tree, int n_tab);

void label_example(struct example_t *example_to_label, struct node_t *tree);
#endif