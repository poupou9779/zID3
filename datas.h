#ifndef DATAS_H
#define DATAS_H

/*macro to clarify string initialisation*/
#define DELETE(str) *(str) = '\0'

#define INFINITE DBL_MAX

typedef char string[64]; /*to avoid allocation all the time*/

/*as bool isn't a normalized type in C89, defines should be enough here*/
#define bool int
#define true 1
#define false 0

#define attribute_t example_t
#define example_t container_t
struct container_t
{
    /*property = (container == example_t ? label : name)*/
    string property;
    /*the pointer is used to create a tab of strings which contains different attributes values (one per attribute)
      if container_t == example_t, then tab_values contains the value of each attribute
      else if container_t == attribute_t, then tab_values contains all of the different values that the attribute can have*/
    string *tab_values;
    /*'l' means length. So l_tab is the length of <tab_values>*/
    int l_tab;
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
