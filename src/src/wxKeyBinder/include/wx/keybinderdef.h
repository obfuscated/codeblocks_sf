/////////////////////////////////////////////////////////////////////////////
// Name:        keybinder.h
// Purpose:     shared build defines
// Author:      Francesco Montorsi
// Created:     2005/07/10
// RCS-ID:      $Id$
// Copyright:   (c) Aleksandras Gluchovas and (c) Francesco Montorsi
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////


#ifndef _WX_KEYBINDER_DEFS_H_
#define _WX_KEYBINDER_DEFS_H_

// for shared builds
#ifdef WXMAKINGDLL_KEYBINDER
    #define WXDLLIMPEXP_KEYBINDER                  WXEXPORT
    #define WXDLLIMPEXP_DATA_KEYBINDER(type)       WXEXPORT type

// NOTE: the following block of code commented-out by mandrav@codeblocks.org
//       for usage in Code::Blocks IDE.
//       This allows building wxKeyBinder as a static lib while using wxWidgets DLL.

//#elif defined(WXUSINGDLL)
//    #define WXDLLIMPEXP_KEYBINDER                  WXIMPORT
//    #define WXDLLIMPEXP_DATA_KEYBINDER(type)       WXIMPORT type

#else // not making nor using DLL
    #define WXDLLIMPEXP_KEYBINDER
    #define WXDLLIMPEXP_DATA_KEYBINDER(type)	    type
#endif

#endif // _WX_KEYBINDER_DEFS_H_

