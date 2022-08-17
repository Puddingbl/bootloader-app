#include "my_structure.h"

/**
	*队列初始化
	*@pram q 需要初始化的队列
	*			 pArray  传给队列的数组
**/
Q_Status queue_init(Queue_t *q, QElemType *pArray)
{
	q->front = 0;
	q->rear = 0;
	q->data = pArray;
	
	return Q_OK;
}

uint8_t queue_length(Queue_t q)
{
	return (q.rear + Queue_Size - q.front) % Queue_Size;
}

void queue_clear(Queue_t *q)
{
	q->front = 0;
	q->rear = 0;
}

/**
	*入队
**/
Q_Status queue_insert(Queue_t *q, QElemType e)
{
	if((q->rear+1)%Queue_Size == q->front)
	{
		return Q_ERROR;
	}
	
	q->data[q->rear] = e;
	q->rear = (q->rear+1) % Queue_Size;
	
	return Q_OK;
}

/**
	*出队
**/
Q_Status queue_extract(Queue_t *q, QElemType *e)
{
	if(q->front == q->rear)
	{
		return Q_ERROR;
	}
	
	*e = q->data[q->front];
	q->front = (q->front+1)%Queue_Size;
	
	return Q_OK;
}
