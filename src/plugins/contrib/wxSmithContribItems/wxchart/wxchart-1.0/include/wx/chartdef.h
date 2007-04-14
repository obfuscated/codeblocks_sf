/////////////////////////////////////////////////////////////////////////////
// Name:        chartdef.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: chartdef.h,v 1.1 2006/06/13 12:51:49 pgava Exp $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __CHARTDEF_H__ )
#define __CHARTDEF_H__

// Version
#define wxCHART_MAJOR          0
#define wxCHART_MINOR          1
#define wxCHART_RELEASE        0

// Defines for shared builds
#ifdef WXMAKINGDLL_CHART
    #define WXDLLIMPEXP_CHART                  WXEXPORT
    #define WXDLLIMPEXP_DATA_CHART(type)       WXEXPORT type
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_CHART                  WXIMPORT
    #define WXDLLIMPEXP_DATA_CHART(type)       WXIMPORT type
#else // not making nor using DLL
    #define WXDLLIMPEXP_CHART
    #define WXDLLIMPEXP_DATA_CHART(type)       type
#endif


#endif // __CHARTDEF_H__
