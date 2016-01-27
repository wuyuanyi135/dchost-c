#include "queue.h"
#include "stdint.h"                 
#include "string.h"

uint32_t Enqueue(QueueParameter* Param,void* Source,int32_t Length)
{
	//TAIL will not move until next call; TAIL always indicates the last element;
	int32_t i;
	int32_t offset;
	void* pTail;
	#ifdef Queue_RequireValidate
	if(!QueueValidate(Param))
		return Queue_ParameterError;
	#endif
	if(Param->Count  + Length > Param->BufferSize)
		return Queue_Insufficiency;
	
	
	for(i=0;i<Length;i++)
	{
		offset = Param->UnitSize* ((Param->Count +Param->Offset) % Param->BufferSize);
		pTail =(void*) ((int32_t)Param->QueueBuffer + offset);
		Source = (void*) ((int32_t)Source + Param->UnitSize);				/* the offset is not the same as queue offset! */
		memcpy(
			pTail,
			Source,
			Param->UnitSize
			);
		Param->Count ++;
	}
	return Queue_Success;
}


uint32_t Dequeue(QueueParameter* Param,void* Dest, int32_t Length)
{
	int32_t i;
	void* pHead;
	#ifdef Queue_RequireValidate
	if(!QueueValidate(Param))
		return Queue_ParameterError;
	#endif
	if(Param->Count == 0)
		return Queue_Empty;
	
	if(Length > Param->Count)
		return Queue_Insufficiency;

	for(i=0;i<Length;i++)
	{
		pHead = (void*)((int32_t)Param->QueueBuffer + Param->UnitSize*((Param->Offset) % Param->BufferSize));
		memcpy((void*)((uint32_t)Dest + i*Param->UnitSize) ,pHead,Param->UnitSize);
		Param->Offset = (Param->Offset+1) % Param->BufferSize;
		Param->Count --;
		
	}
	return Queue_Success;
}
uint32_t QueueValidate(QueueParameter* Param)
{
	if(
//	Param->Offset > 0		||
		Param->Offset > Param->BufferSize		||
		Param->Count	> Param->BufferSize	
	)
		return 0;
	return 1;
}
uint32_t QueueVacuate(QueueParameter* Param)
{
	Param->Offset=0;
	Param->Count=0;
	memset(Param->QueueBuffer,0,Param->BufferSize*Param->UnitSize);
	return Queue_Success;
}
uint32_t QueueInit(QueueParameter* QP,void* Buffer,int32_t BufferSize,int32_t UnitSize)
{
	QP->BufferSize = BufferSize;
	QP->Count = 0;
	QP->Offset =0;
	QP->QueueBuffer=Buffer;
	QP->UnitSize = UnitSize;
	return Queue_Success;
}
