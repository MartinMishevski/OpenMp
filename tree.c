#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
node* node_create ()
{
    node* self = malloc (sizeof(node));
    self->dataNum = 0;
    self->data = malloc (2*sizeof(int*));
    self->parent = NULL;
    self->kids[0] = NULL;
    return self;
}

node* head;
/*
 * output:
 *  if first < second return -1
 *  if first == second return 0
 *  else 1
 */
int data_relation (int left[6],int right[6])
{
    int i;
    for (i = 0; i != 6; ++i)
        if (left[i] != right[i])
            if (left[i] < right[i])
                return -1;
            else
                return 1;
    return 0;
}

void node_print(node* self,int depth)
{
    int i,j;
    char space[depth+1];
    for(i=0;i<depth;++i)
        space[i]=' ';
    space[i] = 0;
    if (self == NULL)
        return;
    if (self->kids[0]!=NULL)
    {
        for(i = 0; i < self->dataNum;++i)
        {    
            node_print(self->kids[i],depth+3);
            printf ("%s||",space);
            for (j = 0;j < 6;++j)
            {
                printf("%d ",self->data[i][j]);
            
            }
            printf("-------------------------\n%s\n",space);
        }
        node_print(self->kids[i],depth+3);

    }
    else
        for(i = 0; i < self->dataNum;++i)
        {
            printf ("%s||",space);
            for (j = 0; j < 6; ++j)
            {
                printf ("%d ",self->data[i][j]);
            }
            printf("-------------------------\n%s|\n",space);
        }
}
/*
 * input:
 *  self is node that we are on
 *  data is what we are checking to see if in tree
 * output:
 *  1: data is not unique
 *  0: data is unique
 */
char node_find(node* self, int data[6])
{
    int i,j;
    if (self==NULL)
        return 0;
    for (i = 0; i != self->dataNum;++i)
    {
        j = data_relation (data,self->data[i]);
        if (j==-1)
            return node_find(self->kids[i],data);
        else if (j==0)
            return 1;
    }
    return node_find(self->kids[i],data);
}
/*
 * input:
 *  self is node that we are working in
 *  data is what data we are adding
 * outcome:
 *  adds a node to the tree
 */
void nodeAdd(node* self,int data[9])
{
    if (self->kids[0]==NULL)
    {
        node_add(self,data);
        return;
    }
    int i;
    for(i = 0;i < self->dataNum;++i)
    {
        if (data_relation(data,self->data[i])==-1)
        {
            nodeAdd(self->kids[i],data);
            return;
        }
    }
    nodeAdd(self->kids[i],data);
}

void node_add (node* self, int data[6])
{
    int i,j; 
    if (self->dataNum < 2)//No need to split
    {
        if (self->dataNum == 1)//data already exists
        {
            i = data_relation(data,self->data[0]);
            if (i == -1)
            {
                self->data[1] = self->data[0];
                self->data[0] = data;
            }
            else
            {
                self->data[1] = data;
            }
        }
        else
            self->data[0] = data;
        self->dataNum ++;
    }
    else// need to split
    {
        int* left; 
        int* right;
        int* temp;
        left = self->data[0];
        right = self->data[1];
        node* rightN = node_create();
        node* leftN = node_create();
        
        if (data_relation(data,left)==-1)
        {
            temp = left;
            left = data;
            data = temp;
        }
        else
            if (data_relation(data,right)==1)
            {
                temp = right;
                right = data;
                data = temp;
            }
        if (self->parent == NULL)
        {
            self->parent = node_create(0);
            if (head == self)
                head = self->parent;
        }
        node_add(leftN,left);
        node_add(rightN,right);
        node_add2(self->parent, data,leftN,rightN);
        free(self);
    }
}
void node_add2(node* self, int data[6],node* passL, node* passR)
{
    int i;
    passR->parent = self;
    passL->parent = self;

    if (self->dataNum < 2)
    {
        if (self->dataNum == 0)//Create a node that is not a root
        {
            self->data[0] = data;
            self->kids[0]=passL;
            self->kids[1]=passR;

        }
        else//There is already something in this node
        {
            i = data_relation(data,self->data[0]);
            if (i == -1)//add to left side
            {
                self->data[1] = self->data[0];
                self->data[0] = data;

                self->kids[2]=self->kids[1];
                self->kids[0]=passL;
                self->kids[1] = passR;
            }
            else //add to right side
            {
                self->data[1]=data;
                self->kids[1]=passL;
                self->kids[2]=passR;
            }
        }
        self->dataNum ++;
    }
    else//Need to split
    {
        int* left; 
        int* right;
        left = self->data[0];
        right = self->data[1];
        node* leftN;
        node* rightN;
        leftN = node_create();
        rightN = node_create();

        if (self->parent == NULL)//No parent yet
        {
            self->parent = node_create();
            if (self == head)
                head = self->parent; 
        }

        //Split nodes and give parent data
        if (data_relation (data,self->data[0])==-1)//Come from left kid
        {
            node_add2(leftN,data,passL,passR);
            node_add2(rightN,self->data[1],self->kids[1],self->kids[2]); 
            node_add2(self->parent,self->data[0],leftN,rightN);
        }
        else if (data_relation (data,self->data[1])==-1)//Comes from middle kid
        {
            node_add2(leftN,self->data[0],self->kids[0],passL);
            node_add2(rightN,self->data[1],passR,self->kids[2]);
            node_add2(self->parent,data,leftN,rightN);
        }
        else//Comes from right kid
        {
            node_add2(leftN,self->data[0],self->kids[0],self->kids[1]);
            node_add2(rightN,data,passL,passR);
            node_add2(self->parent,self->data[1],leftN,rightN);
        }
        //free(self);

    }
}


