#ifndef STACK_H_
#define STACK_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef struct QuadtreeNode {
    unsigned char blue, green, red ;
    /*nr de pixeli din blocul descris de nod*/
    uint32_t area;
    int32_t top_left , top_right ;
    int32_t bottom_left , bottom_right ;
} __attribute__ ( ( packed ) ) QuadtreeNode ;


typedef struct CompressedFile {
    /*nr de blocuri, cu informaţie utilă,
    utilizate în cadrul procesului de compresie 
    (numărul de frunze ale arborelui cuaternar de compresie) */
    uint32_t numar_culori;

    /*nr total de noduri ale arborelui cuaternar*/
    uint32_t numar_noduri;

    /*un vector care conţine număr_noduri elemente de tip structură QuadtreeNode,
    fiecare element reprezentând un nod din arborele de compresie creat */
    QuadtreeNode *vector;
} CompressedFile;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} Color;

typedef struct Node {
    Color pixel;
    uint32_t s_size;
    struct Node* q1;
    struct Node* q2;
    struct Node* q3;
    struct Node* q4;
    int isLeaf;
    int offset;

    int q;
    int h_start;
    int h_end;
    int w_start;
    int w_end;
    int64_t mean;

    int* size_of_tree;
    int* nr_leafs;
} Node;



typedef struct StackNode{
	Node* elem;
	struct StackNode *next;
}StackNode;

typedef StackNode TCelSt; // Redenumirea unui nod la notatiile din curs 
typedef StackNode *ACelSt; // Redenumirea unui pointer la nod la notatiile din curs

typedef struct Stack{
	StackNode* head;  // Varful stivei
	long size; // Numarul de elemente din stiva
}Stack;

typedef Stack TStiva; // Redenumirea stivei la notatiile din curs
typedef Stack *ASt;//  // Redenumirea unui pointer la stiva la notatiile din curs

Stack* createStack(void){
	
	Stack* stack = malloc(sizeof(Stack));
	stack->head = NULL;
	stack->size = 0;
	return stack;
}

int isStackEmpty(Stack* stack){
	
	if (stack->head == NULL)
		return 1;
	return 0;
}
//adauga un element in stiva
void push(Stack *stack, Node* elem){
	StackNode* newelem = malloc(sizeof(StackNode));
	newelem->elem = elem;
	if (isStackEmpty(stack))
		{
			newelem->next = NULL;
			stack->head = newelem;
			stack->size++;
			return;
		}
	newelem->next = stack->head;
	stack->head = newelem;
	stack->size++;
}
//returneaza varful stivei
Node* top(Stack *stack){	
	if (isStackEmpty(stack))
		return NULL;
	return stack->head->elem;
} 

//elimina varful stivei din stiva
void pop(Stack *stack){
	if (isStackEmpty(stack))
		return;
	StackNode* aux = stack->head;
	stack->head = stack->head->next;
	free(aux);
	stack->size--;
}

//dezaloca stiva
void destroyStack(Stack *stack){
	while (!isStackEmpty(stack))
		{
			pop(stack);
		}
		free(stack);
}

#endif 