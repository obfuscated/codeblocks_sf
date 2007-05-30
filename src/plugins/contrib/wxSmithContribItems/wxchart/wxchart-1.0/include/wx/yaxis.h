/////////////////////////////////////////////////////////////////////////////
// Name:        yaxis.h
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if !defined( __WXYAXIS_H__ )
#define __WXYAXIS_H__

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

#include "wx/axis.h"

//+++-S-cd-------------------------------------------------------------------
//	NAME:		CWxXAxis
//	DESC:
//	INTERFACE:
//
//----------------------------------------------------------------------E-+++
WXDLLIMPEXP_CHART class wxYAxis : public wxAxis
{

public:
	wxYAxis(ChartValue max = 0, ChartValue min = 0);
	~wxYAxis() {};

	// Draw xaxis area
	//-----------------
	void Draw(CHART_HPAINT hp, CHART_HRECT hr);
};

#endif // __WXXAXIS_H__
