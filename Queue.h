#ifndef QUEUE_H_
#define QUEUE_H_

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


typedef struct QueueNode{
	Node* elem;
	struct QueueNode *next;
}QueueNode;

typedef QueueNode TcelQ;
typedef QueueNode *ACelQ;

typedef struct Queue{
	QueueNode *front;
	QueueNode *rear;
	long size;
}Queue;

typedef Queue TCoada;
typedef Queue *AQ;


Queue* createQueue(void){
	Queue* q = malloc(sizeof(Queue));
	q->front = NULL;
	q->rear = NULL;
	q->size = 0;
	return q;
} 

int isQueueEmpty(Queue *q){
	return !(q->rear);
}

void enqueue(Queue *q, Node* elem){
	QueueNode* newnode = malloc(sizeof(QueueNode));
	newnode->elem = elem;
	if (isQueueEmpty(q))
		{
			newnode->next = NULL;
			q->front = newnode;
			q->rear = newnode;
			q->size++;
		}
		else
		{
			q->rear->next = newnode;
			q->rear = newnode;
			newnode->next = NULL;
			q->size++;
		}
		
}

Node* front(Queue* q){
	if (isQueueEmpty(q))
		return NULL;
	return q->front->elem;
}

void dequeue(Queue* q){
	if (isQueueEmpty(q))
		return;
	if (q->front == q->rear)
		{
			free(q->front);
			q->front = NULL;
			q->rear = NULL;
			q->size--;
			return;
		}
	QueueNode* aux = q->front;
	q->front = q->front->next;
	free(aux);
	q->size--;

}

void destroyQueue(Queue *q){
	while (!isQueueEmpty(q))
	{
		dequeue(q);
	}
	free(q);
}

#endif
