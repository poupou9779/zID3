#ifndef DATAS_H
#define DATAS_H

/*macro to clarify string initialisation*/
#define DELETE(str) *(str) = '\0'

typedef char string[64]; /*to avoid allocation all the time*/

/*as bool isn't a normalized type in C89, defines should be enough here*/
#define bool int
#define true 1
#define false 0

struct example_t
{
    /*label is the classification obtained with the attributes*/
    string label;
    /*the pointer is used to create a tab of strings which contains different attributes values (one per attribute)*/
    string *attributes;
    /*'n' means 'number'. So this is example_t.attributes size.*/
    int n_attributes;
};

struct attribute_t
{
    /*name is the name/title of the algorithm (for instance : Humidity)*/
    string name;
    /*same than above about pointer. This tab contains all of the possible values for each attribute*/
    string *values;
    /*same than above, this is the size of attribute_t.values.*/
    int n_values;
};

struct node_t
{
    /*pointer on pointer is used here because I have a tab of pointers.*/
    struct node_t **children;
    /*explicit*/
    int nb_children;
    /*union is used to save a few memory bytes (only 64 bytes are used instead of 128)*/
    union
    {
        string label;           /*only if the current node is a leaf*/
        string name_attribute;  /*only if the current node is a node*/
    } property;
    /*attribute_values*/
    string *attribute_values;    /*only if the current node is a node*/
};

/*used for most_frequent_label_index and get_first_index function*/
struct counter
{
    /*keep the label in memory to check if next are the same*/
    string label;
    /*explicit*/
    int count,
        index;
};

#endif
