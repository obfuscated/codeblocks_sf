#ifndef __WX_DEBUGGING_H__
#define __WX_DEBUGGING_H__
//#include <manager.h>
//#include <messagemanager.h>
//commit 12/14/2005 9:16 AM

#define LOGGING 0
#define TRAP asm("int3")

#define wxLogMessage wxLogDebug
#define LOGIT wxLogDebug

#if LOGGING
    #undef wxLogMessage
    #undef LOGIT
    #define wxLogDebug wxLogMessage
    #define LOGIT wxLogMessage
#endif

#endif  //__WX_DEBUGGING_H__
