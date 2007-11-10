#ifndef __WX_DEBUGGING_H__
#define __WX_DEBUGGING_H__
// RCS-ID:      $Id$

//#include <manager.h>
//#include <logmanager.h>

#if defined(kbLOGGING)
    #define LOGGING 1
#endif
#define wxLogMessage wxLogDebug
#define LOGIT wxLogDebug

#if LOGGING
    #undef wxLogMessage
    #undef LOGIT
   // wxMSW wont write msg to our log window via wxLogDebug
   #ifdef __WXMSW__
    #define TRAP asm("int3")
    #define wxLogDebug wxLogMessage
    #define LOGIT wxLogMessage
   #endif
   // wxGTK wxLogMessage turns into a wxMessage in GTK
   #ifdef __WXGTK__
    #define TRAP asm("int3")
    #define LOGIT wxLogMessage
   #endif
   #ifdef __WXMAC__
    #warning kbLOGGING and __WXMAC__  defined for debugging.h
    #define TRAP asm("trap")
    #define LOGIT wxLogMessage
    #define wxLogDebug wxLogMessage
   #endif
#endif

#endif  //__WX_DEBUGGING_H__
