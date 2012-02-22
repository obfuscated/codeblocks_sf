/////////////////////////////////////////////////////////////////////////////
// Name:        xaxis.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id: xaxis.h 7559 2011-11-04 10:19:48Z mortenmacfly $
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
/* C::B begin */
class WXDLLIMPEXP_CHART wxXAxis : public wxAxis
/* C::B end */
{

public:
	~wxXAxis() {};

	// Draw xaxis area
	//-----------------
	void Draw(CHART_HPAINT hp, CHART_HRECT hr);
};

#endif // __WXXAXIS_H__
