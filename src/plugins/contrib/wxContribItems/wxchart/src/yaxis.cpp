/////////////////////////////////////////////////////////////////////////////
// Name:        yaxis.cpp
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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "yaxis.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "wx/yaxis.h"
#include "wx/chartsizes.h"

//+++-S-cf-------------------------------------------------------------------
//	NAME:		ctor
//	DESC:
//	PARAMETERS:	ChartValue max,
//				ChartValue min
//	RETURN:		None
//----------------------------------------------------------------------E-+++
wxYAxis::wxYAxis(
	ChartValue max,
	ChartValue min
) : wxAxis(max, min)
{
}

//+++-S-cf-------------------------------------------------------------------
//	NAME:		Draw()
//	DESC:		Draw xaxis
//	PARAMETERS:	CHART_HPAINT hp,
//				CHART_HRECT hr
//	RETURN:		None
//----------------------------------------------------------------------E-+++
void wxYAxis::Draw(
	CHART_HPAINT hp,
	CHART_HRECT hr
)
{
	if ( GetVirtualMax() > 0 )
	{
		double range = GetVirtualMax();
		double start = 0;
		double end = range;

		int int_log_range = (int)floor( log10( range ) );
		double step = 1.0;
		if (int_log_range > 0)
		{
			for (int i = 0; i < int_log_range; i++)
				step *= 10;
		}
		if (int_log_range < 0)
		{
			for (int i = 0; i < -int_log_range; i++)
				step /= 10;
		}
		double lower = ceil(start / step) * step;
		double upper = floor(end / step) * step;

		// if too few values, shrink size
		if ((range/step) < 4)
		{
			step /= 2;
			if (lower-step > start) lower -= step;
			if (upper+step < end) upper += step;
		}

		// if still too few, again
		if ((range/step) < 4)
		{
			step /= 2;
			if (lower-step > start) lower -= step;
			if (upper+step < end) upper += step;
		}

		wxChartSizes *sizes = GetSizes();


		wxFont font(8, wxROMAN, wxNORMAL, wxNORMAL);
        hp->SetFont(font);
        hp->SetPen( *wxBLACK_PEN );

		double current = lower;
		while (current < upper+(step/2))
		{
			int y = (int)( (GetVirtualMax()-current) /
                    range * ((double)hr->h - sizes->GetSizeHeight())) - 1;
            if ((y > 10) && (y < hr->h - 7 - sizes->GetSizeHeight()))
			{
                hp->DrawLine( hr->x + hr->w - 15,
                              y + sizes->GetSizeHeight() + hr->y,
                              hr->x + hr->w - 7,
                              y + sizes->GetSizeHeight() + hr->y );
				wxString label;
				if (range < 50)
				{
					label.Printf( wxT("%f"), current );
					while (label.Last() == wxT('0'))
						label.RemoveLast();
					if ((label.Last() == wxT('.')) || (label.Last() == wxT(',')))
						label.Append( wxT('0') );
				}
				else
					label.Printf( wxT("%d"), (int)floor(current) );
                hp->DrawText( label, hr->x + 5,
                              hr->y + y - 7 + sizes->GetSizeHeight() );
			}

			current += step;
		}

        hp->DrawLine( hr->w - 1, 6 + sizes->GetSizeHeight(),
			hr->w - 1, hr->h );

        //hp->DrawLine( hr->w - 7, 6 + sizes.s_height,
		//			  hr->w - 7, hr->h );
        //hp->DrawLine( hr->w - 7, 2 + sizes.s_height,
		//			  hr->w - 13, 8 + sizes.s_height );
        //hp->DrawLine( hr->w - 7, 2 + sizes.s_height,
		//			  hr->w - 2, 8 + sizes.s_height );
	}
}
