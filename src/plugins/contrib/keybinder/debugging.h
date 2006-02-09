#ifndef __WX_DEBUGGING_H__
#define __WX_DEBUGGING_H__
//#include <manager.h>
//#include <messagemanager.h>
//commit 12/14/2005 9:16 AM
//commit 12/16/2005 8:54 PM




#define eq ==
#define neq !=


#define LOGGING 0

#define wxLogMessage wxLogDebug
#define LOGIT wxLogDebug

#if LOGGING
    #define TRAP asm("int3")
    #undef wxLogMessage
    #undef LOGIT
   // wxMSW wont write msg to our log window via wxLogDebug
   #ifdef __WXMSW__
    #define wxLogDebug wxLogMessage
    #define LOGIT wxLogMessage
   #endif
   // wxGTK wxLogMessage turns into a wxMessage in GTK
   #ifdef __WXGTK__
    #define LOGIT wxLogDebug
   #endif

#endif

#endif  //__WX_DEBUGGING_H__
