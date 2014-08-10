#ifndef STATIC_PTOTOTYPES_FUNCTIONS_C_H_
#define STATIC_PROTOTYPES_FUNCTIONS_C_H

static void add_child(struct node_t *node, struct node_t *child, const string attr_value);
static void add_example(struct example_t *tab, int *index, const struct example_t *example);
static void add_node(struct node_t **node, const string attr_name);
static struct attribute_t *create_subset_attribute_without(const struct attribute_t *attributes, int n_attr, int index);
static struct example_t *create_subset_ex_from_attr(const struct example_t *examples, int n_ex, int *len_subset, int index_attribute, string value);
static double entropy(const struct example_t *examples, int n_ex);
static double gain(const struct attribute_t *attribute, const struct example_t *examples, int n_ex);
static double gain_ratio(const struct attribute_t *attribute, const struct example_t *examples, int n_ex);
static void get_double_from_string(const string str, double *value);
static int get_first_index(const struct counter *tab, int len_tab, const string label);
static int get_index_attribute(const string name, const struct attribute_t *attributes, int n_attr);
static bool is_const_label(const struct example_t *examples, int n_ex, string label);
static void leave_memory_error_fl(const string function, int line);
static void load_attributes(FILE *f, struct attribute_t **attributes, int *n_attr);
static struct node_t *load_tree_tab(FILE *f, int n_tab);
static int most_frequent_label_index(const struct example_t *examples, int n_ex);
static struct node_t *new_leaf(const string label);
static struct node_t *new_node(const struct attribute_t *attribute);
static int optimal_attribute_index_gain(const struct attribute_t *attributes, int n_attr, const struct example_t *examples, int n_ex);
static int optimal_attribute_index_gain_ratio(struct attribute_t *attributes, int n_attr, struct example_t *examples, int n_ex);
static char *process(string buffer);

#endif
