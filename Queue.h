#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>
#include "p.h"

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
