/***************************************************************
 * Purpose:   Tracing in an external log window to make
 *            debugging easier.
 *
 * Author:    Pecan
 * Created:   2007-07-26
 * Copyright: Pecan
 * License:   GPL
 **************************************************************/
 #ifndef LOGGING_H_INCLUDED
#define LOGGING_H_INCLUDED

// ---------------------------------------------------------------------------
// Logging / debugging
// ---------------------------------------------------------------------------
//debugging control
#include <wx/log.h>

#define LOGIT wxLogDebug
#if defined(LOGGING)
 #define LOGGING 1
 #undef LOGIT
 #define LOGIT wxLogMessage
 #define TRAP asm("int3")
#endif

// ----------------------------------------------------------------------------
   #if LOGGING
	extern wxLogWindow*    m_pLog;
   #endif
// ----------------------------------------------------------------------------


#endif // LOGGING_H_INCLUDED
