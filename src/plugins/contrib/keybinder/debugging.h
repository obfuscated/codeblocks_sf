#ifndef DEBUGLOGGING_H
#define DEBUGLOGGING_H

// ---------------------------------------------------------------------------
// Logging / debugging
// ---------------------------------------------------------------------------
//debugging control
#include <wx/log.h>

#if wxVERSION_NUMBER > 3000
    #define LOGIT wxLogDebug
#endif
#if defined(LOGGING)
 #define LOGGING 1
 #undef LOGIT
 #define LOGIT wxLogMessage
 #define TRAP asm("int3")
#endif

// ----------------------------------------------------------------------------
	extern wxLogWindow*    m_pAppLog;
// ----------------------------------------------------------------------------

#endif // DEBUGLOGGING_H
