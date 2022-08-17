#include "my_structure.h"

/**
	*���г�ʼ��
	*@pram q ��Ҫ��ʼ���Ķ���
	*			 pArray  �������е�����
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
	*���
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
	*����
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
