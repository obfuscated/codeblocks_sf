/////////////////////////////////////////////////////////////////////////////
// Name:        propgrid.i
// Purpose:     Wrappers for the wxPropertyGrid.
//
// Author:      Jaakko Salli
//
// Created:     17-Feb-2005
// RCS-ID:      $Id:
// Copyright:   (c) 2005 by Jaakko Salli
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"The `PropertyGrid` provides a specialized two-column grid for editing
properties such as strings, numbers, colours, and string lists."
%enddef

%module(package="wx", docstring=DOCSTRING) propgrid


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"
#include <wx/propgrid/propgrid.h>

%}

//---------------------------------------------------------------------------

%import core.i
%import misc.i  // for DnD
%pythoncode { wx = _core }
%pythoncode { __docfilter__ = wx.__DocFilter(globals()) }

MAKE_CONST_WXSTRING(PGNameStr);


//%include _propgrid_rename.i

MustHaveApp(wxPropertyGrid);

//---------------------------------------------------------------------------
// Get all our defs from the REAL header file.

%include propgrid.h

//---------------------------------------------------------------------------
// Python functions to act like the event macros

%pythoncode {
EVT_PG_SELECTED = wx.PyEventBinder( wxEVT_PG_CHANGED, 1 )
EVT_PG_HIGHLIGHTED = wx.PyEventBinder( wxEVT_PG_HIGHLIGHTED, 1 )
EVT_PG_RIGHT_CLICK = wx.PyEventBinder( wxEVT_PG_RIGHT_CLICK, 1 )
EVT_PG_PAGE_CHANGED = wx.PyEventBinder( wxEVT_PG_PAGE_CHANGED, 1 )
}

//---------------------------------------------------------------------------

%init %{
%}


//---------------------------------------------------------------------------

