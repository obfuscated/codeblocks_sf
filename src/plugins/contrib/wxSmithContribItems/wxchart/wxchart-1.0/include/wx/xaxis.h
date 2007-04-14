/////////////////////////////////////////////////////////////////////////////
// Name:        xaxis.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: xaxis.h,v 1.1 2006/06/13 12:51:50 pgava Exp $
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __WXXAXIS_H__ )
#define __WXXAXIS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "xaxis.h"
#endif

#include "wx/axis.h"

//+++-S-cd-------------------------------------------------------------------
//	NAME:		wxXAxis
//	DESC:		
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxXAxis : public wxAxis
{

public:
	~wxXAxis() {};

	// Draw xaxis area
	//-----------------
	void Draw(CHART_HPAINT hp, CHART_HRECT hr);
};

#endif // __WXXAXIS_H__
