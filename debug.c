
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define __USE_INLINE__

#include <proto/exec.h>
#include <proto/dos.h>

extern struct Hook *stackDump_hook;

int32 printStack(struct Hook *hook, struct Task *task, struct StackFrameMsg *frame)
{
	switch (frame->State)
	{
		case STACK_FRAME_DECODED:

			{
				struct DebugSymbol *symbol = ObtainDebugSymbol(frame->MemoryAddress,NULL);

				if (symbol)
				{
					Printf("Stack %p -> Address: %p, offset %p, SegmentNumber %p, SegmentOffset %p, Name: %s, BaseName: %s,FunctionName: %s\n", 
							frame->StackPointer, 
							frame->MemoryAddress,
							symbol -> Offset,
							symbol -> SegmentNumber,
							symbol -> SegmentOffset	,
							symbol -> Name ? symbol -> Name : "NULL",
							symbol -> SourceBaseName ? symbol -> SourceBaseName : "NULL" ,
							symbol -> SourceFunctionName ? symbol -> SourceFunctionName : "NULL" );

					ReleaseDebugSymbol(symbol);
				}
				else
				{
					Printf("Stack %p -> Address: %p\n",frame->StackPointer, frame->MemoryAddress);
				}
			}

			break;

		case STACK_FRAME_INVALID_BACKCHAIN_PTR:
			Printf("(%p) invalid backchain pointer\n",
			frame->StackPointer);
			break;

		case STACK_FRAME_TRASHED_MEMORY_LOOP:
			Printf("(%p) trashed memory loop\n",
			frame->StackPointer);
			break;

		case STACK_FRAME_BACKCHAIN_PTR_LOOP:
			Printf("(%p) backchain pointer loop\n",
			frame->StackPointer);
			break;

		default:
			Printf("Unknown state=%lu\n", frame->State);
	}

	return 0;  // Continue tracing.
}


void StackDump(  struct Task *task )
{
	if (task)
	{
		Printf("Task: %08lx\n",task);
		uint32 result = StackTrace(task, stackDump_hook);	
	}
}

