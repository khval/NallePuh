
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/Amigainput.h>
#include <proto/timer.h>
#include <proto/intuition.h>
#include <proto/icon.h>
#include <proto/graphics.h>
#include <proto/gadtools.h>
#include <proto/asl.h>
#include <proto/requester.h>
#include <proto/commodities.h>

#include <exec/types.h>
#include <exec/ports.h>
#include <exec/interfaces.h>
#include <exec/libraries.h>
#include <exec/devices.h>
#include <devices/input.h>

#define ALL_REACTION_CLASSES
#include <reaction/reaction.h>
#include <reaction/reaction_macros.h>

#include <intuition/intuition.h>
#include <intuition/classes.h>
#include <intuition/icclass.h>
#include <intuition/gadgetclass.h>

int req(const char *title,const  char *body,const char *buttons, ULONG image)
{  
        Object *req = 0;                                // the requester itself
        int reply;                                      // the button that was clicked by the user

	// fill in the requester structure
	req = NewObject(REQUESTER_GetClass(), NULL, 
		REQ_Type,       REQTYPE_INFO,
		REQ_TitleText,  (ULONG)title,
		REQ_BodyText,   (ULONG)body,
		REQ_GadgetText, (ULONG) buttons ,
		REQ_Image,      image,
		TAG_DONE);
        
	if (req) 
	{ 
		reply = IDoMethod(req, RM_OPENREQ, NULL, NULL, NULL);     // ...open it on the default pubscreen
		DisposeObject(req);                                                                 // free the requestr
	}  else printf("[request] Failed to allocate requester\n");

	return( reply ); // give the button number back to the caller
}
