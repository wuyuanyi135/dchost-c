#ifndef _QUEUE_H_
#define _QUEUE_H_
#include "stdint.h"                   

#define		Queue_ParameterError			0x05
#define		Queue_Success							0x01
#define		Queue_Insufficiency				0x06			
#define		Queue_Empty								0x04
typedef struct 
{
	void* 		QueueBuffer;
	int32_t 	UnitSize;
	int32_t		BufferSize;
	int32_t		Offset;
	int32_t		Count;
}QueueParameter; 

	uint32_t Enqueue(QueueParameter* Param,void* Source,int32_t Length);
	uint32_t Dequeue(QueueParameter* Param,void* Dest, int32_t Length);
	uint32_t QueueValidate(QueueParameter* Param);
	uint32_t QueueVacuate(QueueParameter* Param);
	uint32_t QueueInit(QueueParameter* QP,void* Buffer,int32_t BufferSize,int32_t UnitSize);
#endif
