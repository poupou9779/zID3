#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "datas.h"

#include <stdio.h> /*FILE*/

#define HIGHER ">"
#define LESS_OR_EQUAL "<="

#define ALLOCATION_FAILED -1

#define display_example(e) fdisplay_example(stdout, e)
#define display_examples(e, n) fdisplay_examples(stdout, e, n)
#define display_attribute(a) fdisplay_attribute(stdout, a)
#define display_attributes(a, n) fdisplay_attributes(stdout, a, n)
#define display_tree(n) fdisplay_tree(stdout, n)
#define fdisplay_tree(f, n) fdisplay_tree_tab(f, n, 0)
#define free_examples free_attributes
#define free_attributes free_containers

struct node_t *build_ID3_tree(const struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr);
struct node_t *build_C45_tree(struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr);
int compare_int_strings(const void *s1, const void *s2);
void delete_tree(struct node_t **node);
void discretize(struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr);
void discretize_attribute(struct example_t *examples, int n_ex, struct attribute_t *attributes);
void fdisplay_attribute(FILE *f, const struct attribute_t *attribute);
void fdisplay_attributes(FILE *f, const struct attribute_t *attributes, int n_attr);
void fdisplay_example(FILE *f, const struct example_t *example);
void fdisplay_examples(FILE *f, const struct example_t *examples, int n_ex);
void fdisplay_tree_tab(FILE *f, const struct node_t *tree, int n_tab);
void free_containers(struct container_t **containers, int n_cont);
void fsave_tree(FILE *f, const struct node_t *tree, const struct attribute_t *attributes, int n_attr);
void get_datas_from_file(const string path, struct example_t **ex, int *n_ex, struct attribute_t **attr, int *n_attr);
int get_tree_size(const struct node_t *tree);
void label_example(struct example_t *example_to_label, const struct node_t *tree);
struct node_t *load_tree(const string path, struct attribute_t **attributes, int *n_attr);
int save_tree(const string path, const struct node_t *tree, const struct attribute_t *attributes, int n_attr);

#endif
