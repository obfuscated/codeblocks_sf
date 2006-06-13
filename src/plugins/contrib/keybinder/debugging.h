#ifndef __WX_DEBUGGING_H__
#define __WX_DEBUGGING_H__
//#include <manager.h>
//#include <messagemanager.h>


#define eq ==
#define neq !=


//#define LOGGING 0
#if defined(kbLOGGING)
    #define LOGGING 1
#endif
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
