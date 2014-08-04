#include "datas.h"
#include "functions.h"
#include "static_prototypes_functions.c.h"
#include <stdio.h>  /*file management*/
#include <stdlib.h> /*malloc*/
#include <string.h> /*string management*/
#include <math.h>   /*log*/
#include <ctype.h>  /*isalpha*/

#define LOG

extern struct attribute_t *attributes_set;
extern int n_attr_set;
extern FILE *log_file;

void get_datas_from_file(const char *path, struct example_t **ex, int *n_ex, struct attribute_t **attr, int *n_attr)
{
    int i, j;
    FILE *f = fopen(path, "r");

    if(ex == NULL || attr == NULL)
        return;

    if(f != NULL)
    {
        fscanf(f, "%d\n", n_attr);
        (*attr) = malloc(sizeof(**attr) * *n_attr);
        for(i = 0; i < *n_attr; ++i)
        {
            fscanf(f, "%d %s", &(*attr)[i].n_values, (*attr)[i].name);
            (*attr)[i].values = malloc(sizeof(*(*attr)[i].values) * (*attr)[i].n_values);
            for(j = 0; j < (*attr)[i].n_values; ++j)
                fscanf(f, "%s%*c", (*attr)[i].values[j]);
        }

        fscanf(f, "%d\n", n_ex);
        *ex = malloc(sizeof(**ex) * *n_ex);
        for(i = 0; i < *n_ex; ++i)
        {
            (*ex)[i].n_attributes = *n_attr;
            (*ex)[i].attributes = malloc(sizeof(*(*ex)[i].attributes) * *n_attr);
            for(j = 0; j < *n_attr; ++j)
                fscanf(f, "%s ", (*ex)[i].attributes[j]);
            fscanf(f, "%s\n", (*ex)[i].label);
        }
    }
}

struct node_t *build_ID3_tree(struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr)
{
    string label;
    struct attribute_t *to_test;
    struct node_t *node;
    int i, index;

    fprintf(log_file, "\t\texamples :\n");
    fdisplay_examples(log_file, examples, n_ex);
    fprintf(log_file, "\t\tattributes :\n");
    fdisplay_attributes(log_file, attributes, n_attr);
    fprintf(log_file, "\n");

    /*if there is no example left, then*/
    if(n_ex == 0)
        /*leave without creating a node*/
        return NULL;
    /*if every piece of examples has the very same label, then*/
    if(is_const_label(examples, n_ex, label))
        /*return the so called label in a leaf*/
        return new_leaf(label);
    /*if every attribute has already been tested, then*/
    if(n_attr == 0)
        /*return a leaf with */
        return new_leaf(examples[most_frequent_label_index(examples, n_ex)].label);

    index = optimal_attribute_index_gain(attributes, n_attr, examples, n_ex);
    to_test = &attributes[index];
    node = new_node(to_test);

    for(i = 0; i < to_test->n_values; ++i)
    {
        int l_subset_ex;
        struct example_t *subset_ex = create_subset_ex_from_attr(examples, n_ex, &l_subset_ex, get_index_attribute(attributes[index].name, attributes_set, n_attr_set), to_test->values[i]);
        struct attribute_t *subset_attr = create_subset_attribute_without(attributes, n_attr, index);
        node->children[i] = build_ID3_tree(subset_ex, l_subset_ex, subset_attr, n_attr-1);
        strcpy(node->attribute_value[i], to_test->values[i]);
        free_attributes(&subset_attr, n_attr-1);
        free_examples(&subset_ex, l_subset_ex);
    }

    return node;
}

void fdisplay_examples(FILE *f, const struct example_t *examples, int n_ex)
{
    int i;
    for(i = 0; i < n_ex; ++i)
        fdisplay_example(f, &examples[i]);
}

void fdisplay_example(FILE *f, const struct example_t *example)
{
    int i;
    for(i = 0; i < example->n_attributes; ++i)
        fprintf(f, "%s ", example->attributes[i]);
    fprintf(f, "%s\n", example->label);
}

void fdisplay_attributes(FILE *f, const struct attribute_t *attributes, int n_attr)
{
    int i;
    for(i = 0; i < n_attr; ++i)
        fprintf(f, "%s ", attributes[i].name);
    fputc('\n', f);
}

void fdisplay_attribute(FILE *f, const struct attribute_t *attribute)
{
    fprintf(f, "%s ", attribute->name);
}

void fdisplay_tree_tab(FILE *f, struct node_t *tree, int n_tab)
{
    int i, j;
    if(tree->nb_children != 0)
    {
        for(i = 0; i < n_tab; ++i) fputc('\t', f);
        fprintf(f, "%s\n", tree->property.name_attribute);
        for(j = 0; j < tree->nb_children; ++j)
        {
            for(i = 0; i < n_tab; ++i) fputc('\t', f);
            fprintf(f, "-%s\n", tree->attribute_value[j]);
            fdisplay_tree_tab(f, tree->children[j], n_tab+1);
        }
    }
    else
    {
        for(i = 0; i < n_tab; ++i) fputc('\t', f);
        fprintf(f, ".%s\n", tree->property.label);
    }
}

void label_example(struct example_t *example_to_label, struct node_t *tree)
{
    int i, index;
    if(tree->nb_children == 0)
        strcpy(example_to_label->label, tree->property.label);
    else
    {
        for(i = 0; i < tree->nb_children; ++i)
        {
            index = get_index_attribute(tree->property.name_attribute, attributes_set, n_attr_set);
            if(strcmp(example_to_label->attributes[index], attributes_set[index].values[i]) == 0)
            {
                label_example(example_to_label, tree->children[i]);
                return;
            }
        }
    }
}

struct node_t *build_C45_tree(struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr)
{
    string label;
    struct attribute_t *to_test;
    struct node_t *node;
    int i, index;

#ifdef LOG
    fprintf(log_file, "\t\texamples :\n");
    fdisplay_examples(log_file, examples, n_ex);
    fprintf(log_file, "\t\tattributes :\n");
    fdisplay_attributes(log_file, attributes, n_attr);
    fprintf(log_file, "\n");
#endif

    /*if there is no example left, then*/
    if(n_ex == 0)
        /*leave without creating a node*/
        return NULL;
    /*if every piece of examples has the very same label, then*/
    if(is_const_label(examples, n_ex, label))
        /*return the so called label in a leaf*/
        return new_leaf(label);
    /*if every attribute has already been tested, then*/
    if(n_attr == 0)
        /*return a leaf with */
        return new_leaf(examples[most_frequent_label_index(examples, n_ex)].label);

    index = optimal_attribute_index_gain_ratio(attributes, n_attr, examples, n_ex);
    to_test = &attributes[index];
    node = new_node(to_test);

    for(i = 0; i < to_test->n_values; ++i)
    {
        int l_subset_ex;
        struct example_t *subset_ex = create_subset_ex_from_attr(examples, n_ex, &l_subset_ex, get_index_attribute(attributes[index].name, attributes_set, n_attr_set), to_test->values[i]);
        struct attribute_t *subset_attr = create_subset_attribute_without(attributes, n_attr, index);
        node->children[i] = build_ID3_tree(subset_ex, l_subset_ex, subset_attr, n_attr-1);
    }

    return node;
}

void free_examples(struct example_t **examples, int n_ex)
{
    int i;
    for(i = 0; i < n_ex; ++i)
        free((*examples)[i].attributes);
    free(*examples);
    *examples = NULL;
}

void free_attributes(struct attribute_t **attributes, int n_attr)
{
    int i;
    for(i = 0; i < n_attr; ++i)
        free((*attributes)[i].values);
    free(*attributes);
    *attributes = NULL;
}

void delete_tree(struct node_t **node)
{
    int i;
    for(i = 0; i < (*node)->nb_children; ++i)
        delete_tree(&(*node)->children[i]);
    free(*node);
    *node = NULL;
}

int get_tree_size(const struct node_t *tree)
{
    int ret = 0;
    int i;
    if(tree->nb_children == 0)
        return 0;
    for(i = 0; i < tree->nb_children; ++i)
        ret += get_tree_size(tree->children[i]);
    return ret+1;
}

struct node_t *load_tree(const string path)
{
    struct node_t *ret;
    FILE *f = fopen(path, "r");
    if(f == NULL)
        return NULL;
    ret = load_tree_tab(f, 0);
    free(f);
    return ret;
}


/*static function*/
#define leave_memory_error(f) leave_memory_error_fl(f, __LINE__)
static void leave_memory_error_fl(const string function, int line)
{
    fprintf(log_file, "Memory error in function %s at line %d\n", function, line);
    exit(ALLOCATION_FAILED);
}

static void add_child(struct node_t *node, struct node_t *son, const string attr_value)
{
    void *tmp;
    ++node->nb_children;
    tmp = realloc(node->children, node->nb_children * sizeof(*node->children));
    if(tmp == NULL)
        leave_memory_error("add_child");
    node->children = tmp;
    node->children[node->nb_children-1] = son;
    tmp = realloc(node->attribute_value, node->nb_children * sizeof(*node->attribute_value));
    if(tmp == NULL)
        leave_memory_error("add_child");
    node->attribute_value = tmp;
    strcpy(node->attribute_value[node->nb_children-1], attr_value);
}

static void add_node(struct node_t **node, const string attr_name)
{
    *node = malloc(sizeof(**node));
    (*node)->nb_children = 0;
    (*node)->children = NULL;
    (*node)->attribute_value = NULL;
    strcpy((*node)->property.name_attribute, attr_name);
}

static char *process(string buffer)
{
    if(strrchr(buffer, '\n') != NULL)
        *strrchr(buffer, '\n') = '\0';
    return strrchr(buffer, '\t') != NULL ? strrchr(buffer, '\t')+1 : &buffer[0];
}

#define is_node(b) (isalpha(b[0]))
#define is_edge(b) (b[0] == '-')
#define is_leaf(b) (b[0] == '.')
static struct node_t *load_tree_tab(FILE *f, int n_tab)
{
    struct node_t *ret = NULL;
    string buffer;
    char *ptr;
    fpos_t mem;

    fprintf(log_file, "%d tabs\n", n_tab);

    while(fgets(buffer, sizeof(buffer), f) != NULL)
    {
        ptr = process(buffer);
        if((ptr - buffer) == n_tab-1)
        {
            fsetpos(f, &mem);
            break;
        }

        if(is_node(ptr))
        {
            fprintf(log_file, "creating a new node for %s\n", ptr);
            add_node(&ret, ptr);
        }
        else if(is_edge(ptr))
        {
            fprintf(log_file, "exploring [%s in %s]\n", ptr, ret->property.name_attribute);
            add_child(ret, load_tree_tab(f, n_tab+1), ptr+1);
        }
        else if(is_leaf(ptr))
        {
            fprintf(log_file, "creating a \"%s\" node\n", ptr+1);
            return new_leaf(ptr+1);
        }
        fgetpos(f, &mem);
    }
    return ret;
}

static bool is_const_label(const struct example_t *examples, int n_ex, string label)
{
    int i;
    if(n_ex == 0)
    {
        strcpy(label, "ERROR label");
        return true;
    }
    strcpy(label, examples[0].label);
    for(i = 1; i < n_ex; ++i)
        if(strcmp(label, examples[i].label) != 0)
            return false;
    return true;
}

static struct node_t *new_leaf(const string label)
{
    struct node_t *ret = malloc(sizeof(*ret));
    if(ret == NULL)
        leave_memory_error("new_leaf");
    ret->attribute_value = NULL;
    ret->children = NULL;
    ret->nb_children = 0;
    strcpy(ret->property.label, label);
    return ret;
}

static int get_first_index(const struct counter *tab, int len_tab, const string label)
{
    int i;
    for(i = 0; i < len_tab; ++i)
        if(strcmp(label, tab[i].label) == 0)
            return i;
    return -1;
}

static int most_frequent_label_index(const struct example_t *examples, int n_ex)
{
    struct counter *tab = malloc(sizeof(*tab) * n_ex);
    int i,
        tmp,
        index = 0,
        max_value = -1,
        ret = 0;
    for(i = 0; i < n_ex; ++i)
    {
        if((tmp = get_first_index(tab, index, examples[i].label)) == -1)
        {
            strcpy(tab[index].label, examples[i].label);
            tab[index].count = 0;
            tab[index].index = i;
            ++index;
        }
        else
        {
            ++tab[tmp].count;
            if(tab[tmp].count > max_value)
            {
                max_value = tab[tmp].count;
                ret = i;
            }
        }
    }
    free(tab);
    return ret;
}

static double entropy(const struct example_t *examples, int n_ex)
{
    struct counter *tested_labels = malloc(sizeof(*tested_labels) * n_ex);
    double ret = 0.;
    int i,
        tmp,
        index = 0;
    for(i = 0; i < n_ex; ++i)
    {
        if((tmp = get_first_index(tested_labels, index, examples[i].label)) == -1)
        {
            strcpy(tested_labels[index].label, examples[i].label);
            tested_labels[index].count = 1;
            ++index;
        }
        else
        {
            ++tested_labels[tmp].count;
        }
    }
    for(i = 0; i < index; ++i)
        ret -= tested_labels[i].count * log((double)tested_labels[i].count/n_ex);
    free(tested_labels);
    return ret/(log(2) * n_ex);
}

static void add_example(struct example_t *tab, int *index, const struct example_t *example)
{
    int i;
    tab[*index].attributes = malloc(sizeof(*tab[*index].attributes) * example->n_attributes);
    for(i = 0; i < example->n_attributes; ++i)
        strcpy(tab[*index].attributes[i], example->attributes[i]);
    strcpy(tab[*index].label, example->label);
    tab[*index].n_attributes = example->n_attributes;
    ++(*index);
}

static void add_attribute(struct attribute_t *tab, int *index, const struct attribute_t *attribute)
{
    int i;
    strcpy(tab[*index].name, attribute->name);
    tab[*index].n_values = attribute->n_values;
    tab[*index].values = malloc(sizeof(*tab[*index].values) * tab[*index].n_values);
    for(i = 0; i < tab[*index].n_values; ++i)
        strcpy(tab[*index].values[i], attribute->values[i]);
    ++(*index);
}

static int get_index_attribute(const string name, const struct attribute_t *attributes, int n_attr)
{
    int i;
    for(i = 0; i < n_attr; ++i)
        if(strcmp(name, attributes[i].name) == 0)
            return i;
    return -1;
}

static double gain(const struct attribute_t *attribute, const struct example_t *examples, int n_ex)
{
    double ret = 0.0;
    struct example_t *ex_set = malloc(sizeof(*ex_set) * n_ex);
    string v;
    int i, j, index;
    int index_attr = get_index_attribute(attribute->name, attributes_set, n_attr_set);

    for(j = 0; j < attribute->n_values; ++j)
    {
        strcpy(v, attribute->values[j]);
        index = 0;
        /*ex_set = S_v*/
        for(i = 0; i < n_ex; ++i)
        {
            if(strcmp(examples[i].attributes[index_attr], v) == 0)
                add_example(ex_set, &index, &examples[i]);
        }
        ret += index*entropy(ex_set, index)/n_ex;
    }
    free(ex_set);
    return ret;
}

static int optimal_attribute_index_gain(const struct attribute_t *attributes, int n_attr, const struct example_t *examples, int n_ex)
{
    int i,
        ret = 0;
    double min_value = 1.,
           tmp;
    for(i = 0; i < n_attr; ++i)
    {
        if((tmp = gain(&attributes[i], examples, n_ex)) < min_value)
        {
            min_value = tmp;
            ret = i;
        }
    }
    return ret;
}

static int optimal_attribute_index_gain_ratio(const struct attribute_t *attributes, int n_attr, const struct example_t *examples, int n_ex)
{
    int i,
        ret = 0;
    double min_value = 1.,
           tmp;
    for(i = 0; i < n_attr; ++i)
    {
        if((tmp = gain_ratio(&attributes[i], examples, n_ex)) < min_value)
        {
            min_value = tmp;
            ret = i;
        }
    }
    return ret;
}

static struct node_t *new_node(const struct attribute_t *attribute)
{
    int i;
    struct node_t *ret = malloc(sizeof(*ret));
    ret->nb_children = attribute->n_values;
    ret->children = malloc(sizeof(*ret->children) * ret->nb_children);
    for(i = 0; i < ret->nb_children; ++i)
        ret->children[i] = NULL;
    ret->attribute_value = malloc(sizeof(*ret->attribute_value) * ret->nb_children);
    for(i = 0; i < ret->nb_children; ++i)
        DELETE(ret->attribute_value[i]);
    strcpy(ret->property.name_attribute, attribute->name);
    return ret;
}

static struct example_t *create_subset_ex_from_attr(const struct example_t *examples, int n_ex, int *len_subset, int index_attribute, string value)
{
    int i;
    struct example_t *ret = malloc(sizeof(*ret) * n_ex);
    *len_subset = 0;
    for(i = 0; i < n_ex; ++i)
        if(strcmp(examples[i].attributes[index_attribute], value) == 0)
            add_example(ret, len_subset, &examples[i]);
    return ret;
}

static struct attribute_t *create_subset_attribute_without(const struct attribute_t *attributes, int n_attr, int index)
{
    int i, index_tab = 0;
    struct attribute_t *ret = malloc(sizeof(*ret) * (n_attr-1));
    for(i = 0; i < n_attr; ++i)
        if(i != index)
            add_attribute(ret, &index_tab, &attributes[i]);
    return ret;
}

static double gain_ratio(const struct attribute_t *attribute, const struct example_t *examples, int n_ex)
{
    double ret = 0.0;
    struct example_t *ex_set = malloc(sizeof(*ex_set) * n_ex);
    string v;
    int i, j, index;
    int index_attr = get_index_attribute(attribute->name, attributes_set, n_attr_set);

    for(j = 0; j < attribute->n_values; ++j)
    {
        strcpy(v, attribute->values[j]);
        index = 0;
        /*ex_set = S_v*/
        for(i = 0; i < n_ex; ++i)
        {
            if(strcmp(examples[i].attributes[index_attr], v) == 0)
                add_example(ex_set, &index, &examples[i]);
        }
        ret += entropy(ex_set, index)/log((double)index/n_ex);
    }
    free(ex_set);
    return ret*(-log(2));
}
