#ifndef _TREE_H_
#define _TREE_H_
typedef struct 
{
    int dataNum;
    int** data;
    struct node* parent;
    struct node* kids[3];
} node;
extern node* head;
void node_add2(node*,int[9],node*,node*);
void node_add(node*,int[9]);
int data_relation (int [6],int [6]);
void node_print(node* ,int);
char node_find(node*, int [6]);
void nodeAdd(node*,int [9]);
node* node_create ();

#endif
