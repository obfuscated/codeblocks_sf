/***************************************************************
 * Name:      [PROJECT_NAME]_pch.h
 * Purpose:   Header to support Pre-Compiled Header
 * Author:    [AUTHOR_NAME] ([AUTHOR_EMAIL])
 * Created:   [NOW]
 * Copyright: [AUTHOR_NAME] ([AUTHOR_WWW])
 * License:   
 **************************************************************/

#ifndef [PROJECT_HDR]_PCH_H_INCLUDED
#define [PROJECT_HDR]_PCH_H_INCLUDED

#if ( defined(USE_PCH) && !defined(WX_PRECOMP) )
    #define WX_PRECOMP
#endif // USE_PCH

// basic wxWidgets headers
#include <wx/wxprec.h>

#ifdef __BORLANDC__
	#pragma hdrstop
#endif

#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#ifdef USE_PCH
	// put here all your rarely-changing header files
#endif // USE_PCH

#endif //[PROJECT_HDR]_PCH_H_INCLUDED
