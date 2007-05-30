/////////////////////////////////////////////////////////////////////////////
// Name:        xaxis.cpp
// Purpose:     wxChart
// Author:      Paolo Gava
// Modified by:
// Created:
// Copyright:   (C) 2006, Paolo Gava
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// Headers
//----------------------------------------------------------------------------

// c++
#include <cmath>

// wx

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/xaxis.h"

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		Draw xaxis
//	PARAMETERS:	CHART_HPAINT hp,
//				CHART_HRECT hr
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxXAxis::Draw(
	CHART_HPAINT hp,
	CHART_HRECT hr
)
{

	if ( GetVirtualMax() > 0 )
	{
		wxFont font(8, wxROMAN, wxNORMAL, wxNORMAL);
        hp->SetFont(font);
        hp->SetPen( *wxBLACK_PEN );

        int iNodes = static_cast<int>(ceil( GetVirtualMax() ));
		ChartSizes sizes = GetSizes();

		double x;

		//-------------------------------------------------------------------
		// If hr->x != 0 means we are drawing the axis on file. So the
		// following condition prevent from drawing only part of the axis ie
		// ignor any scroll position
		// TODO: any better idea?!
		//-------------------------------------------------------------------
		if ( hr->x == 0 )
		{
			hr->xscroll *= sizes.scroll;
			x = 0 - hr->xscroll;
		}
		else
			x = 0;

		for ( int iNode = 0; iNode <= iNodes; ++ iNode )
		{
			if ( x >= 0 )
			{
				wxString label;

                hp->DrawLine( static_cast<int>(ceil(x)) + hr->x, 5 + hr->y,
                              static_cast<int>(ceil(x)) + hr->x, 15 + hr->y );

                label.Printf( wxT("%d"), iNode );
                hp->DrawText( label, static_cast<int>(ceil(x)) +
                              hr->x, 20 + hr->y );
			}
			x +=  GetZoom() * ( sizes.wbar * sizes.nbar +
								sizes.wbar3d * sizes.nbar3d +
								sizes.gap );
		}

        hp->DrawLine( hr->x, hr->y + 1, hr->x + static_cast<int>(ceil(x)),
                      hr->y + 1 );

	}
}
