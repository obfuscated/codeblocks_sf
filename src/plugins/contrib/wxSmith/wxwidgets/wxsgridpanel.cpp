/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include <wx/dcclient.h> // wxPaintDC
#include "wxsgridpanel.h"

#include <configmanager.h>
#include <manager.h>

IMPLEMENT_CLASS(wxsGridPanel,wxPanel)

BEGIN_EVENT_TABLE(wxsGridPanel,wxPanel)
    EVT_PAINT(wxsGridPanel::OnPaint)
END_EVENT_TABLE()

wxsGridPanel::wxsGridPanel()
{
}

wxsGridPanel::wxsGridPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size,long style,const wxString& name):
    wxPanel(parent,id,pos,size,style,name)
{
}

wxsGridPanel::~wxsGridPanel()
{
}

void wxsGridPanel::OnPaint(cb_unused wxPaintEvent& event)
{
    wxPaintDC DC(this);

    int Width, Height;
    GetClientSize(&Width,&Height);
    DC.SetPen(wxPen(*wxBLACK));
    DC.SetBrush(*wxTRANSPARENT_BRUSH);

    int GridSize = GetGridSize();

    if ( GridSize > 1 )
    {
        for ( int Y=0; Y<Height; Y+=GridSize )
        {
            for ( int X=0; X<Width; X+=GridSize )
            {
                DC.DrawPoint(X,Y);
            }
        }
    }

    if ( DrawBorder() )
    {
        DC.DrawRectangle(0,0,Width,Height);
    }
}

int wxsGridPanel::GetGridSize()
{
    return Manager::Get()->GetConfigManager(_T("wxsmith"))->ReadInt(_T("/gridsize"),8);
}
