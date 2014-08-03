#ifndef DATAS_H
#define DATAS_H

#define DELETE(str) *(str) = '\0'

#define LENGTH_STACKS 16

typedef char string[64]; /*to avoid allocation all the time*/

#define bool int
#define true 1
#define false 0

struct example_t
{
    string label;
    string *attributes;
    int n_attributes;
};

struct attribute_t
{
    string name;
    string *values;
    int n_values;
};

struct node_t
{
    struct node_t **children;
    int nb_children;
    union
    {
        string label;           /*only if the current node is a leaf*/
        string name_attribute;  /*only if the current node is a node*/
    } property;
};

/*used for most_frequent_label_index and get_first_index function*/
struct counter
{
    string label;
    int count,
        index;
};

#endif
