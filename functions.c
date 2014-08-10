#include "datas.h"
#include "functions.h"
#include "static_prototypes_functions.c.h"
#include <stdio.h>  /*file management*/
#include <stdlib.h> /*malloc*/
#include <string.h> /*string management*/
#include <math.h>   /*log*/
#include <ctype.h>  /*isalpha*/
#include <float.h>  /*DBL_EPSILON*/

#define LOG
#define C45

#define leave_memory_error(f) leave_memory_error_fl(f, __LINE__)
#define add_attribute add_example
#define is_node(b) (isalpha(b[0]))
#define is_edge(b) (b[0] == '-')
#define is_leaf(b) (b[0] == '.')

extern struct attribute_t *attributes_set;
extern int n_attr_set;
extern FILE *log_file;

#ifdef C45
int index_to_discretize;
#endif

/*
in :
    - node is the node that will be added a child
    - child is the node that's added to node
    - attr_value is the value that's related to the child
out :
    - nothing
*/
static void add_child(struct node_t *node, struct node_t *child, const string attr_value)
{
    void *tmp;
    ++node->nb_children;
    tmp = realloc(node->children, node->nb_children * sizeof(*node->children));
    if(tmp == NULL)
        leave_memory_error("add_child");
    node->children = tmp;
    node->children[node->nb_children-1] = child;
    tmp = realloc(node->attribute_values, node->nb_children * sizeof(*node->attribute_values));
    if(tmp == NULL)
        leave_memory_error("add_child");
    node->attribute_values = tmp;
    strcpy(node->attribute_values[node->nb_children-1], attr_value);
}

/*
in :
    - tab is the tab of examples that's modified
    - index is the pointer on the index value which is incremented
    - example is a pointer on the example to add in <tab>
out :
    - nothing
*/
static void add_example(struct example_t *tab, int *index, const struct example_t *example)
{
    int i;
    tab[*index].tab_values = malloc(sizeof(*tab[*index].tab_values) * example->l_tab);
    for(i = 0; i < example->l_tab; ++i)
        strcpy(tab[*index].tab_values[i], example->tab_values[i]);
    strcpy(tab[*index].property, example->property);
    tab[*index].l_tab = example->l_tab;
    ++(*index);
}

/*
in :
    - node is a pointer of the node that is going to be created
    - attr_name is the attribute that this node shall test
out :
    - nothing
*/
static void add_node(struct node_t **node, const string attr_name)
{
    *node = malloc(sizeof(**node));
    (*node)->nb_children = 0;
    (*node)->children = NULL;
    (*node)->attribute_values = NULL;
    strcpy((*node)->property.name_attribute, attr_name);
}

/*
in :
    - container is a pointer on the container to modify
    - value is the value to add in container
out :
    - nothing
*/
static void add_value(struct container_t *container, const string value)
{
    string *tmp = realloc(container->tab_values, (container->l_tab+1)*sizeof(*container->tab_values));
    if(tmp == NULL)
        leave_memory_error("add_value");
    container->tab_values = tmp;
    strcpy(container->tab_values[container->l_tab], value);
    ++container->l_tab;
}

/*
in :
    - same than build_ID3_tree
out :
    - same than build_ID3_tree
info :
    - code not done 100% and some updates are necessary
*/
struct node_t *build_C45_tree(struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr)
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
        return new_leaf(examples[most_frequent_label_index(examples, n_ex)].property);

    index = optimal_attribute_index_gain_ratio(attributes, n_attr, examples, n_ex);
    to_test = &attributes[index];
    node = new_node(to_test);
    for(i = 0; i < to_test->l_tab; ++i)
    {
        int l_subset_ex;
        struct example_t *subset_ex = create_subset_ex_from_attr(examples, n_ex, &l_subset_ex, get_index_attribute(attributes[index].property, attributes_set, n_attr_set), to_test->tab_values[i]);
        struct attribute_t *subset_attr = create_subset_attribute_without(attributes, n_attr, index);
        node->children[i] = build_C45_tree(subset_ex, l_subset_ex, subset_attr, n_attr-1);
        strcpy(node->attribute_values[i], to_test->tab_values[i]);
        free_attributes(&subset_attr, n_attr-1);
        free_examples(&subset_ex, l_subset_ex);
    }

    return node;
}

/*
in :
    - examples ei the set of pre-labeled examples
    - n_ex is explicit
    - attributes is the list of the unused (yet) attributes. Needed to choose the optimal attribute to test
    - n_attr is explicit
out :
    - a pointer on a node. As the function is recursive, it ends returning the root of the tree but each node is connected to this one
*/
struct node_t *build_ID3_tree(const struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr)
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
        return new_leaf(examples[most_frequent_label_index(examples, n_ex)].property);

    /*find the best attribute to test*/
    index = optimal_attribute_index_gain(attributes, n_attr, examples, n_ex);
    to_test = &attributes[index];

    /*then create a new node*/
    node = new_node(to_test);

    /*and for each node, find the following node*/
    for(i = 0; i < to_test->l_tab; ++i)
    {
        int l_subset_ex;
        struct example_t *subset_ex = create_subset_ex_from_attr(examples, n_ex, &l_subset_ex, get_index_attribute(attributes[index].property, attributes_set, n_attr_set), to_test->tab_values[i]);
        struct attribute_t *subset_attr = create_subset_attribute_without(attributes, n_attr, index);
        /*then use recursion system*/
        node->children[i] = build_ID3_tree(subset_ex, l_subset_ex, subset_attr, n_attr-1);
        strcpy(node->attribute_values[i], to_test->tab_values[i]);
        free_attributes(&subset_attr, n_attr-1);
        free_examples(&subset_ex, l_subset_ex);
    }

    return node;
}

/*
in :
    - s1 is the 1st string containing a number
out :
    - > 0 if s1 > s2
    - 0 if s1 == s2
    - < 0 if s1 < s2
*/
int compare_int_strings(const void *e1, const void *e2)
{
    const struct example_t *el1 = (const struct example_t *)e1,
                           *el2 = (const struct example_t *)e2;
    double a, b;
    sscanf(el1->tab_values[index_to_discretize], "%lf", &a);
    sscanf(el2->tab_values[index_to_discretize], "%lf", &b);
    return a>b ? 1 : b>a ? -1 : 0;
}

/*
in :
    - attributes is the set of attributes where elements of the subset lie
    - n_attr is explicit
    - index is the index of the attributes to not insert in the subset
out :
    - a tab of attributes (that do not contain the <index> attribute)
*/
static struct attribute_t *create_subset_attribute_without(const struct attribute_t *attributes, int n_attr, int index)
{
    int i, index_tab = 0;
    struct attribute_t *ret = malloc(sizeof(*ret) * (n_attr-1));
    for(i = 0; i < n_attr; ++i)
        if(i != index)
            add_attribute(ret, &index_tab, &attributes[i]);
    return ret;
}

/*
in :
    - examples is the set of examples where elements of the subset lie
    - n_ex is explicit
    - len_sebset is a pointer on the length of the subset
    - index_attribute is the index of the attribute that is checked to create the subset
    - value is the value of this attribute
out :
    - a tab of examples
*/
static struct example_t *create_subset_ex_from_attr(const struct example_t *examples, int n_ex, int *len_subset, int index_attribute, string value)
{
    int i;
    void *tmp_ptr;
    struct example_t *ret = NULL;
    *len_subset = 0;
    for(i = 0; i < n_ex; ++i)
        if(strcmp(examples[i].tab_values[index_attribute], value) == 0)
        {
            tmp_ptr = realloc(ret, (*len_subset+1) * sizeof(*ret));
            if(tmp_ptr == NULL)
                leave_memory_error("create_subset_ex_from_attr");
            ret = tmp_ptr;
            add_example(ret, len_subset, &examples[i]);
        }
    return ret;
}

/*
in :
    - node is a pointer on the tree to free
out :
    - nothing
*/
void delete_tree(struct node_t **node)
{
    int i;
    for(i = 0; i < (*node)->nb_children; ++i)
        delete_tree(&(*node)->children[i]);
    free(*node);
    *node = NULL;
}

/*
in :
    - examples is the tab of examples that will have their continous attribute values discretized
    - n_ex is explicit
    - attributes is the tab of attributes that will be modified (aatributes[i].tab_values)
    - n_attr is explicit
out :
    - nothing
*/
void discretize(struct example_t *examples, int n_ex, struct attribute_t *attributes, int n_attr)
{
    int i;
    for(i = 0; i < n_attr; ++i)
        if(attributes[i].l_tab == 0)
        {
            free(attributes[i].tab_values);
            attributes[i].l_tab = 0;
            index_to_discretize = i;
            discretize_attribute(examples, n_ex, &attributes[i]);
        }
}

/*
in :
    - examples is same than above
    - n_ex is explicit
    - attribute is the attribute which is discretized
    - index is the index of the attrbiute in the examples values
out :
    - nothing
*/
void discretize_attribute(struct example_t *examples, int n_ex, struct attribute_t *attribute)
{
    int i;
    double first_value = -INFINITE,
           last_value,
           tmp;
    string tmp_label,
           tmp_value = "";
    qsort((void *)examples,  n_ex, sizeof(*examples), compare_int_strings);
    strcpy(tmp_label, examples[0].property);
    get_double_from_string(examples[0].tab_values[index_to_discretize], &last_value);
    for(i = 1; i < n_ex; ++i)
    {
        if(strcmp(examples[i].property, tmp_label) != 0)
        {
            if(first_value != -INFINITE)
                sprintf(tmp_value, "%s %f", HIGHER, first_value);
            get_double_from_string(examples[i].tab_values[index_to_discretize], &tmp);
            sprintf(tmp_value, "%s %s %f", tmp_value, LESS_OR_EQUAL, (last_value + tmp)/2.);
            add_value(attribute, tmp_value);
            get_double_from_string(examples[i].tab_values[index_to_discretize], &first_value);
            strcpy(tmp_label, examples[i].property);
        }
        else
        {
            get_double_from_string(examples[i].tab_values[index_to_discretize], &last_value);
        }
    }
}

/*
in :
    - examples is the tab of examples to compute the shannon entropy with
    - n_ex is explicit
out :
    - the shannon entropy of the examples set
*/
static double entropy(const struct example_t *examples, int n_ex)
{
    void *tmp_ptr;
    struct counter *tested_labels = NULL;
    double ret = 0.;
    int i,
        tmp,
        index = 0;
    for(i = 0; i < n_ex; ++i)
    {
        if((tmp = get_first_index(tested_labels, index, examples[i].property)) == -1)
        {
            tmp_ptr = realloc(tested_labels, sizeof(*tested_labels) * (index+1));
            if(tmp_ptr == NULL)
                leave_memory_error("entropy");
            tested_labels = tmp_ptr;
            strcpy(tested_labels[index].label, examples[i].property);
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

/*
in :
    - f is same than above
    - attribute is a const pointer on the attribute to display
out :
    - nothing
*/
void fdisplay_attribute(FILE *f, const struct attribute_t *attribute)
{
    int i;
    fprintf(f, "%s ", attribute->property);
    if(attribute->l_tab == 0)
        fputc('C', f);
    else
        fprintf(f, "%d ", attribute->l_tab);
    for(i = 0; i < attribute->l_tab; ++i)
        fprintf(f, "%s ", attribute->tab_values[i]);
    fputc('\n', f);
}

/*
in :
    - f is same than above
    - attributes is a tab of attributes to display
    - n_attr is explicit
out :
    - nothing
*/
void fdisplay_attributes(FILE *f, const struct attribute_t *attributes, int n_attr)
{
    int i;
    fprintf(f, "%d\n", n_attr);
    for(i = 0; i < n_attr; ++i)
        fdisplay_attribute(f, &attributes[i]);
    fputc('\n', f);
}

/*
in :
    - f is same than above
    - example is a const pointer on the example to display
out :
    - nothing
*/
void fdisplay_example(FILE *f, const struct example_t *example)
{
    int i;
    for(i = 0; i < example->l_tab; ++i)
        fprintf(f, "%s ", example->tab_values[i]);
    fprintf(f, "%s\n", example->property);
}

/*
in :
    - f is the file used to write the examples
    - examples is a tab of example
    - n_ex is explicit
out :
    - nothing
*/
void fdisplay_examples(FILE *f, const struct example_t *examples, int n_ex)
{
    int i;
    for(i = 0; i < n_ex; ++i)
        fdisplay_example(f, &examples[i]);
}

/*
in :
    - f is same than above
    - tree is initially the root of the tree. But as function is recursive, it may be a subtree as well
    - n_tab is the recursion depth (usefull to know how many '\t' to print)
out :
    - nothing
*/
void fdisplay_tree_tab(FILE *f, const struct node_t *tree, int n_tab)
{
    int i, j;
    /*if tree is a node*/
    if(tree->nb_children != 0)
    {
        for(i = 0; i < n_tab; ++i) fputc('\t', f);
        /*print details on the tree with a formatted syntax*/
        fprintf(f, "%s\n", tree->property.name_attribute);
        /*then use recursion to explore the rest of it*/
        for(j = 0; j < tree->nb_children; ++j)
        {
            for(i = 0; i < n_tab; ++i) fputc('\t', f);
            fprintf(f, "-%s\n", tree->attribute_values[j]);
            fdisplay_tree_tab(f, tree->children[j], n_tab+1);
        }
    }
    /*otherwise it is a leaf*/
    else
    {
        /*then only show the classification*/
        for(i = 0; i < n_tab; ++i) fputc('\t', f);
        fprintf(f, ".%s\n", tree->property.label);
    }
}

/*
in :
    - cotnainers is a pointer on the tab of containers to free
    - n_cont is explicit
out :
    - nothing
*/
void free_containers(struct container_t **containers, int n_cont)
{
    int i;
    for(i = 0; i < n_cont; ++i)
        free((*containers)[i].tab_values);
    free(*containers);
    *containers = NULL;
}

/*
in :
    - f is the file that will content the tree
    - rest is same than above
out :
    - nothing
*/
void fsave_tree(FILE *f, const struct node_t *tree, const struct attribute_t *attributes, int n_attr)
{
    fprintf(f, "#attributes\n");
    fdisplay_attributes(f, attributes, n_attr);
    fprintf(f, "#tree\n");
    fdisplay_tree(f, tree);
}

/*
in :
    - attribute is the attribute to compute the gain of
    - examples is the set of examples used to compute entropy
    - n_ex is explicit
out :
    - the (optimized) gain value
*/
static double gain(const struct attribute_t *attribute, const struct example_t *examples, int n_ex)
{
    double ret = 0.0;
    struct example_t *ex_set = malloc(sizeof(*ex_set) * n_ex);
    int i, j, index;
    int index_attr = get_index_attribute(attribute->property, attributes_set, n_attr_set);

    for(j = 0; j < attribute->l_tab; ++j)
    {
        index = 0;
        /*ex_set = S_v*/
        for(i = 0; i < n_ex; ++i)
        {
            if(strcmp(examples[i].tab_values[index_attr], attribute->tab_values[j]) == 0)
                add_example(ex_set, &index, &examples[i]);
        }
        ret += index*entropy(ex_set, index)/n_ex;
    }
    free(ex_set);
    return ret;
}

/*
in :
    - attribute is the attribute to compute the gain of
    - examples is a tab of examples used to compute entropy
    - n_ex is explicit
out :
    - the gain ration value
*/
static double gain_ratio(const struct attribute_t *attribute, const struct example_t *examples, int n_ex)
{
    double ret = 0.0;
    struct example_t *ex_set = malloc(sizeof(*ex_set) * n_ex);
    int i, j, index;
    int index_attr = get_index_attribute(attribute->property, attributes_set, n_attr_set);

    for(j = 0; j < attribute->l_tab; ++j)
    {
        index = 0;
        /*ex_set = S_v*/
        for(i = 0; i < n_ex; ++i)
        {
            if(strncmp(attribute->tab_values[j],  HIGHER, sizeof(HIGHER)) == 0)
            {
                double down, up, v;
                get_double_from_string(examples[i].tab_values[index_attr], &v);
                sscanf(attribute->tab_values[j], "%*c %lf %*c%*c %lf", &down, &up);
                if(down < v && (v > up || v - up < DBL_EPSILON))
                    add_example(ex_set, &index, &examples[i]);
            }
            else if(strncmp(attribute->tab_values[j], LESS_OR_EQUAL, sizeof(LESS_OR_EQUAL)) == 0)
            {
                double up, v;
                get_double_from_string(examples[i].tab_values[index_attr], &v);
                sscanf(attribute->tab_values[j], "%*c %lf", &up);
                if(v > up || v - up < DBL_EPSILON)
                    add_example(ex_set, &index, &examples[i]);
            }
            else
            {
                if(strcmp(examples[i].tab_values[index_attr], attribute->tab_values[j]) == 0)
                    add_example(ex_set, &index, &examples[i]);
            }
        }
        ret += entropy(ex_set, index)/log((double)index/n_ex);
    }
    free(ex_set);
    return ret*(-log(2));
}

/*
in :
    - path is the path to the file that contains examples
    - ex is a pointer on the non-allocated yet tab of examples used to create the tree
    - n_ex is a pointer on the ex tab length
    - attr is a pointer on a tab, same than ex
    - n_attr is same than n_ex
out :
    - nothing
*/
void get_datas_from_file(const string path, struct example_t **ex, int *n_ex, struct attribute_t **attr, int *n_attr)
{
    int i, j;
    string buffer;
    FILE *f = fopen(path, "r");

    /*if parameters aren't correct, then*/
    if(ex == NULL || attr == NULL || n_ex == NULL || n_attr == NULL)
        /*leave function*/
        return;

    if(f != NULL)
    {
        /*first, find attributes*/
        while(fgets(buffer, sizeof(buffer), f) != NULL && strstr(buffer, "#attributes") == NULL);
        /*then read and load the set*/
        load_attributes(f, attr, n_attr);
        /*now find the examples*/
        while(fgets(buffer, sizeof(buffer), f) != NULL && strstr(buffer, "#examples") == NULL);

        /*and then load them*/
        fscanf(f, "%d\n", n_ex);
        *ex = malloc(sizeof(**ex) * *n_ex);
        for(i = 0; i < *n_ex; ++i)
        {
            (*ex)[i].l_tab = *n_attr;
            (*ex)[i].tab_values = malloc(sizeof(*(*ex)[i].tab_values) * *n_attr);
            for(j = 0; j < *n_attr; ++j)
                fscanf(f, "%s ", (*ex)[i].tab_values[j]);
            fscanf(f, "%s\n", (*ex)[i].property);
        }
    }
}

/*
in :
    - str is a string containing a number
    - value is a pointer on the double that will contain the number which is written in str
out :
    - nothing
*/
static void get_double_from_string(const string str, double *value)
{
    sscanf(str, "%lf", value);
}

/*
in :
    - tab is the tab you want to get the index in
    - len_tab is explicit
    - label is the value you want the first index of
out :
    - the index value (-1 if there is no <label> in <tab>)
*/
static int get_first_index(const struct counter *tab, int len_tab, const string label)
{
    int i;
    for(i = 0; i < len_tab; ++i)
        if(strcmp(label, tab[i].label) == 0)
            return i;
    return -1;
}

/*
in :
    - name is the name of the attribute to find
    - attributes is the set of attribute to look in
    - n_attr is explicit
out :
    - the index
*/
static int get_index_attribute(const string name, const struct attribute_t *attributes, int n_attr)
{
    int i;
    for(i = 0; i < n_attr; ++i)
        if(strcmp(name, attributes[i].property) == 0)
            return i;
    return -1;
}

/*
in :
    - tree is the tree (or subtree) you want to know the number of non-leaf nodes in
out :
    - the subtree size
*/
int get_tree_size(const struct node_t *tree)
{
    int ret = 0;
    int i;
    if(tree->nb_children == 0)
        return 0;
    for(i = 0; i < tree->nb_children; ++i)
        /*again use recursion to test each node*/
        ret += get_tree_size(tree->children[i]);
    return ret+1;
}

/*
in :
    - examples is a tab of labeled examles
    - n_ex is explicit
    - label is the pseudo-retun value : if each element of the examples tab is labeled the same way, this parameter takes its value
out :
    - true if each element of the tab example is labeled the same
    - false otherwise
*/
static bool is_const_label(const struct example_t *examples, int n_ex, string label)
{
    int i;
    if(n_ex == 0)
    {
        strcpy(label, "ERROR label");
        return true;
    }
    strcpy(label, examples[0].property);
    for(i = 1; i < n_ex; ++i)
        if(strcmp(label, examples[i].property) != 0)
            return false;
    return true;
}

/*
in  :
    - example_to_label is explicit
    - tree is the root of the decision tree
out :
    - nothing
*/
void label_example(struct example_t *example_to_label, const struct node_t *tree)
{
    int i, index;
    if(tree->nb_children == 0)
        strcpy(example_to_label->property, tree->property.label);
    else
    {
        for(i = 0; i < tree->nb_children; ++i)
        {
            index = get_index_attribute(tree->property.name_attribute, attributes_set, n_attr_set);
            if(strcmp(example_to_label->tab_values[index], attributes_set[index].tab_values[i]) == 0)
            {
                /*use recursion to test each node*/
                label_example(example_to_label, tree->children[i]);
                return;
            }
        }
    }
}

/*
in :
    - function is the name of the function that calls this function
    - line is the ID of the line that calls the function
out :
    - nothing (function uses the exit-function)
*/
static void leave_memory_error_fl(const string function, int line)
{
    fprintf(log_file, "Memory error in function %s at line %d\n", function, line);
    exit(ALLOCATION_FAILED);
}

/*
in :
    - f is the file that content the attributes that will be loaded
    - attributes is a pointer on the future tab of examples that are going to be read in the file
    - n_attr is a pointer on an integer which is the size of the tab
out :
    - nothing
*/
static void load_attributes(FILE *f, struct attribute_t **attributes, int *n_attr)
{
    int i, j;
    fscanf(f, "%d\n", n_attr);
    (*attributes) = malloc(sizeof(**attributes) * *n_attr);
    for(i = 0; i < *n_attr; ++i)
    {
        fscanf(f, "%s ", (*attributes)[i].property);
        if(fscanf(f, "%d ", &(*attributes)[i].l_tab) == 0)
        {
            (*attributes)[i].l_tab = 0;
            (*attributes)[i].tab_values = NULL;
            while(fgetc(f) != '\n');
            continue;
        }
        (*attributes)[i].tab_values = malloc(sizeof(*(*attributes)[i].tab_values) * (*attributes)[i].l_tab);
        for(j = 0; j < (*attributes)[i].l_tab; ++j)
            fscanf(f, "%s%*c", (*attributes)[i].tab_values[j]);
    }
}

/*
in :
    - path is the path of the file where datas lie (a *.NEL file)
    - attributes is a pointer on the future tab that will contain the attributes and their values
    - n_attr is explicit
out :
    - the tree that has been loaded grom the file in <path>
*/
struct node_t *load_tree(const string path, struct attribute_t **attributes, int *n_attr)
{
    string buffer;
    struct node_t *ret;
    FILE *f = fopen(path, "r");
    if(f == NULL)
        return NULL;


    while(fgets(buffer, sizeof(buffer), f) != NULL)
    {
        /*on each read line, load the right part of it.*/
        if(strstr(buffer, "#attributes") != NULL)
            load_attributes(f, attributes, n_attr);
        else if(strstr(buffer, "#tree"))
            ret = load_tree_tab(f, 0);
    }
    free(f);
    return ret;
}

/*
in :
    - f is the file that contents the tree
    - n_tab is the recursive depth-value
out :
    - the tree that is in the file
*/
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

/*
in :
    - examples is a tab that's checked to find the most frequent label
    - n_ex is explicit
out :
    - the index
*/
static int most_frequent_label_index(const struct example_t *examples, int n_ex)
{
    void *tmp_ptr;
    struct counter *tab = NULL;
    int i,
        tmp,
        index = 0,
        max_value = -1,
        ret = 0;
    for(i = 0; i < n_ex; ++i)
    {
        if((tmp = get_first_index(tab, index, examples[i].property)) == -1)
        {
            tmp_ptr = realloc(tab, sizeof(*tab) * (index+1));
            if(tmp_ptr == NULL)
                leave_memory_error("most_frequent_label_index");
            tab = tmp_ptr;
            strcpy(tab[index].label, examples[i].property);
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

/*
in :
    - label is the value of the leaf that is going to be created
out :
    - a leaf-node labeled
*/
static struct node_t *new_leaf(const string label)
{
    struct node_t *ret = malloc(sizeof(*ret));
    if(ret == NULL)
        leave_memory_error("new_leaf");
    ret->attribute_values = NULL;
    ret->children = NULL;
    ret->nb_children = 0;
    strcpy(ret->property.label, label);
    return ret;
}

/*
in :
    - attribute is the attribute that is tested in the node that's going to be created
out :
    - the non-leaf node
*/
static struct node_t *new_node(const struct attribute_t *attribute)
{
    int i;
    struct node_t *ret = malloc(sizeof(*ret));
    ret->nb_children = attribute->l_tab;
    ret->children = malloc(sizeof(*ret->children) * ret->nb_children);
    for(i = 0; i < ret->nb_children; ++i)
        ret->children[i] = NULL;
    ret->attribute_values = malloc(sizeof(*ret->attribute_values) * ret->nb_children);
    for(i = 0; i < ret->nb_children; ++i)
        DELETE(ret->attribute_values[i]);
    strcpy(ret->property.name_attribute, attribute->property);
    return ret;
}

/*
in :
    - attributes is the set of attributes to find the optimal one
    - n_attr is explicit
    - examples is the set of examples used to compute entropy
    - n_ex is explicit
out :
    - int the index
*/
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

/*
same than above for in and out
*/
static int optimal_attribute_index_gain_ratio(struct attribute_t *attributes, int n_attr, struct example_t *examples, int n_ex)
{
    int i,
        ret = 0;
    double min_value = 1.,
           tmp;
    bool has_been_discretized = false;
    for(i = 0; i < n_attr; ++i)
    {
        if(attributes[i].l_tab == 0)
        {
            index_to_discretize = get_index_attribute(attributes[i].property, attributes_set, n_attr_set);
            discretize_attribute(examples, n_ex, &attributes[i]);
            has_been_discretized = true;
        }
        if((tmp = gain_ratio(&attributes[i], examples, n_ex)) < min_value)
        {
            min_value = tmp;
            ret = i;
        }
        if(has_been_discretized == true)
        {
            free(attributes[i].tab_values);
            attributes[i].l_tab = 0;
            has_been_discretized = false;
        }
    }

    return ret;
}

/*
in :
    - buffer is the string that's processed (removal of the final '\n' character if it exists)
out :
    - the addresse of the first non-'\t' character of the buffer
*/
static char *process(string buffer)
{
    if(strrchr(buffer, '\n') != NULL)
        *strrchr(buffer, '\n') = '\0';
    return strrchr(buffer, '\t') != NULL ? strrchr(buffer, '\t')+1 : &buffer[0];
}

/*
in :
    - path is the path of the file where the tree will be saved
    - tree is explicit
    - attributes is the set of attributes that have to be saved in the file too
    - n_attr is explicit
out :
    a boolean value for succeeded or failed
*/
bool save_tree(const string path, const struct node_t *tree, const struct attribute_t *attributes, int n_attr)
{
    FILE *f = fopen(path, "w+");
    if(f == NULL)
        return false;
    fsave_tree(f, tree, attributes, n_attr);
    fclose(f);
    return true;
}
