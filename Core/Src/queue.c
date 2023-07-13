/*
  *****************************************************************************
  * @file    queue.c
  * @author  KloudQ Team
  * @version
  * @date
  * @brief   Queue for GSM and Debug Payload and utility Functions
*******************************************************************************
*/
/******************************************************************************

            Copyright (c) by KloudQ Technologies Limited.

  This software is copyrighted by and is the sole property of KloudQ
  Technologies Limited.
  All rights, title, ownership, or other interests in the software remain the
  property of  KloudQ Technologies Limited. This software may only be used in
  accordance with the corresponding license agreement. Any unauthorized use,
  duplication, transmission, distribution, or disclosure of this software is
  expressly forbidden.

  This Copyright notice may not be removed or modified without prior written
  consent of KloudQ Technologies Limited.

  KloudQ Technologies Limited reserves the right to modify this software
  without notice.

*/
#include "main.h"
#include "stm32l433xx.h"
#include "applicationdefines.h"
#include <string.h>
#include <stdlib.h>
#include "queue.h"

uint32_t GSMQueueEmptyFlag = 0;
uint32_t GSMQueueFullFlag = 0;

/****************************************************************************
 Function: Queue_InitLiveQ
 Purpose: Initialize Queue
 Input:	strctQUEUE *queue
 Return value: None

 Note(s)(if-any) :

 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/05/19			initial code
******************************************************************************/
void Queue_InitLiveQ(strctQUEUE *queue)
{
    queue->head = 0;
    queue->tail = 0;
}

/****************************************************************************
 Function: Queue_LiveEnequeue
 Purpose: Add element to Queue
 Input:	strctQUEUE *queue
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/05/19			initial code
******************************************************************************/
void Queue_LiveEnequeue(strctQUEUE * queue,char * data)
{

	/* if data is completely transfered, reset the queue */
	if(queue->head <= queue->tail)
	{
		queue->head = 0;
		queue->tail = 0;
	}

	/* If queue has data clear it */
	if(queue->data[queue->head] != NULL)
	{
		free(queue->data[queue->head]);
		queue->data[queue->head] = NULL;
	}

	if(data != NULL) /*malloc successfully done*/
	{
		/* Write data to queue head */
		queue->data[queue->head]= data;
		/* move to head to next location */
		queue->head++;
	}

	/* If queue is full */
	if(queue->head > (MAX_LIVE_QUEUE_SIZE - 1))
	{
		/*Raise Queue Overflow flag */
		GSMQueueFullFlag = 1;
		queue->head = (MAX_LIVE_QUEUE_SIZE - 1);
	}
}

/****************************************************************************
 Function: Queue_LiveDequeue
 Purpose: Remove element from Queue
 Input:	strctQUEUE *queue
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/05/19			initial code
******************************************************************************/
void Queue_LiveDequeue(strctQUEUE * queue)
{

    /* After data is uploaded free the pointer in queue->data */
        if(((queue->tail == 0) && (queue->head == 0)) || (queue->tail == queue->head))
        {

            if((queue->tail >= queue->head) && (queue->tail != 0)  && (queue->head != 0))
            {
            	if(queue->data[queue->tail] != NULL)
				{
					free(queue->data[queue->tail]);
					queue->data[queue->tail] = NULL;
				}

            	queue->head = 0;
    			queue->tail = 0;
            }
        }
        else
        {
            free(queue->data[queue->tail]);
            queue->data[queue->tail] = NULL;
            queue->tail++;
            if(queue->tail > (MAX_LIVE_QUEUE_SIZE-1))
    		{
    			queue->tail=0;
    		}
        }
}

/****************************************************************************
 Function: displayQueue
 Purpose: Check is queue is empty
 Input:	strctQUEUE *queue
 Return value: uint32_t status,TRUE if queue is empty


 Note(s)(if-any) :

 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/05/19			initial code
******************************************************************************/
uint32_t Queue_IsLiveQueueEmpty(strctQUEUE * queue)
{
	if((queue->head == queue->tail) && (queue->data[queue->tail] == NULL))
		return 1;
	else
		return 0;
	//	return((queue->head) == (queue->tail));
}

uint32_t Queue_IsLiveQueueFull(strctQUEUE * queue)
{
	return(queue->head == (MAX_LIVE_QUEUE_SIZE));
}

/*********************************************BUFFER QUEUE FUNCTIONS************************/


/****************************************************************************
 Function: Queue_InitBuffQ
 Purpose: Initialize Queue
 Input:	strctBuffQUEUE *queue
 Return value: None

 Note(s)(if-any) :

 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/05/19			initial code
******************************************************************************/
void Queue_InitBuffQ(strctBuffQUEUE *queue)
{
    queue->head = 0;
    queue->tail = 0;
}

/****************************************************************************
 Function: Queue_BuffEnequeue
 Purpose: Add element to Queue
 Input:	strctBuffQUEUE *queue
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/05/19			initial code
******************************************************************************/
void Queue_BuffEnequeue(strctBuffQUEUE * queue,char * data)
{


	/* if data is completely transfered, reset the queue */
	if(queue->head <= queue->tail)
	{
		queue->head = 0;
		queue->tail = 0;
	}

	/* If queue has data clear it */
	if(queue->data[queue->head] != NULL)
	{
		free(queue->data[queue->head]);
		queue->data[queue->head] = NULL;
	}

	if(data != NULL) /*malloc successfully done*/
	{
		/* Write data to queue head */
		queue->data[queue->head]= data;
		/* move to head to next location */
		queue->head++;
	}

	/* If queue is full */
	if(queue->head > (MAX_BUFF_QUEUE_SIZE - 1))
	{
		queue->head = (MAX_BUFF_QUEUE_SIZE - 1);
	}
}

/****************************************************************************
 Function: Queue_BuffDequeue
 Purpose: Remove element from Queue
 Input:	strctBuffQUEUE *queue
 Return value: None


 Note(s)(if-any) :


 Change History:
 Author            	Date                Remarks
 KloudQ Team        22/05/19			initial code
******************************************************************************/
void Queue_BuffDequeue(strctBuffQUEUE * queue)
{

    /* After data is uploaded free the pointer in queue->data */
        if(((queue->tail == 0) && (queue->head == 0)) || (queue->tail == queue->head))
        {
        	if((queue->tail >= queue->head) && (queue->tail != 0)  && (queue->head != 0))
            {
    			if(queue->data[queue->tail] != NULL)
				{
					free(queue->data[queue->tail]);
					queue->data[queue->tail] = NULL;
				}
    			queue->head = 0;
				queue->tail = 0;
            }
        }
        else
        {
            free(queue->data[queue->tail]);
            queue->data[queue->tail] = NULL;
            queue->tail++;
            if(queue->tail > (MAX_BUFF_QUEUE_SIZE-1))
    		{
    			queue->tail=0;
    		}
        }
}



uint32_t Queue_IsBuffQueueEmpty(strctBuffQUEUE * queue)
{
	if((queue->head == queue->tail) && (queue->data[queue->tail] == NULL))
		return 1;
	else
		return 0;
}

uint32_t Queue_IsBuffQueueFull(strctBuffQUEUE * queue)
{
	return(queue->head == (MAX_BUFF_QUEUE_SIZE));
}

//******************************* End of File *******************************************************************
