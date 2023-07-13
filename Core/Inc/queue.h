/*
 * queue.h
 *
 *  Created on: Apr 19, 2021
 *      Author: admin
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#include "main.h"

#define MAX_LIVE_QUEUE_SIZE         (10)
#define MAX_BUFF_QUEUE_SIZE         (9)


typedef struct
{
	uint32_t u32MaxStackDepth;
	int32_t u32Stacktop;
}strctPayloadStack;

typedef struct{
    int head;
    int tail;
    char* data[MAX_LIVE_QUEUE_SIZE];
}strctQUEUE;


typedef struct{
    int head;
    int tail;
    char* data[MAX_BUFF_QUEUE_SIZE];
}strctBuffQUEUE;


/* Queue Function Prototypes */
void Queue_InitLiveQ(strctQUEUE *queue);
void Queue_LiveEnequeue(strctQUEUE * queue,char * data);
void Queue_LiveDequeue(strctQUEUE * queue);
uint32_t Queue_IsLiveQueueEmpty(strctQUEUE * queue);
uint32_t Queue_IsLiveQueueFull(strctQUEUE * queue);

/*For Buff Que*/
void Queue_InitBuffQ(strctBuffQUEUE *queue);
void Queue_BuffEnequeue(strctBuffQUEUE * queue,char * data);
void Queue_BuffDequeue(strctBuffQUEUE * queue);
uint32_t Queue_IsBuffQueueEmpty(strctBuffQUEUE * queue);
uint32_t Queue_IsBuffQueueFull(strctBuffQUEUE * queue);





#endif /* INC_QUEUE_H_ */
