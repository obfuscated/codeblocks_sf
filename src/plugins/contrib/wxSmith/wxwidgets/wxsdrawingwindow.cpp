/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* $Revision$
* $Id$
* $HeadURL$
*/

#include "wxsdrawingwindow.h"

#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/dcscreen.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>

#include <manager.h>
#include <messagemanager.h>

namespace
{
    /** \brief Event type for background-fetching system */
    const int wxEVT_FETCH_SEQUENCE = wxNewEventType();

    /** \brief Identifier used by internal DrawingPanel class inside wxsDrawingWindow */
    const int DrawingPanelId = wxNewId();
}


/** \brief Drawing panel
 *
 * This panel is put over all other items in wxsDrawingWindow class. It's
 * responsible for fetching background and handling mouse and keyboard events
 */
class wxsDrawingWindow::DrawingPanel: public wxPanel
{
    public:

        /** \brief Ctor */
        DrawingPanel(wxsDrawingWindow* Parent): wxPanel(Parent,DrawingPanelId)
        {
            // Connecting event handlers of drawing window
            Connect(DrawingPanelId,wxEVT_PAINT,(wxObjectEventFunction)&wxsDrawingWindow::PanelPaint,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEFT_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEFT_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEFT_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MIDDLE_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MIDDLE_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MIDDLE_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_RIGHT_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_RIGHT_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_RIGHT_DCLICK,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MOTION,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_ENTER_WINDOW,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_LEAVE_WINDOW,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_MOUSEWHEEL,(wxObjectEventFunction)&wxsDrawingWindow::PanelMouse,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_KEY_DOWN,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_KEY_UP,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);
            Connect(DrawingPanelId,wxEVT_CHAR,(wxObjectEventFunction)&wxsDrawingWindow::PanelKeyboard,NULL,Parent);

            // Connecting handler for empty erase event handler
            Connect(DrawingPanelId,wxEVT_ERASE_BACKGROUND,(wxObjectEventFunction)&wxsDrawingWindow::DrawingPanel::OnEraseBack);
        }

        void OnEraseBack(wxEraseEvent& event)
        {
        }

};

BEGIN_EVENT_TABLE(wxsDrawingWindow,wxScrolledWindow)
END_EVENT_TABLE()

wxsDrawingWindow::wxsDrawingWindow(wxWindow* Parent,wxWindowID id):
    wxScrolledWindow(Parent,id),
    Panel(NULL),
    Bitmap(NULL),
    IsBlockFetch(false),
    DuringFetch(false),
    DuringChangeCnt(0),
    LastSizeX(0),
    LastSizeY(0),
    LastVirtX(0),
    LastVirtY(0),
    WasContentChanged(false)
{
    // Strange - it seems that by declaring this event in event table, it's not processed
    Connect(-1,-1,wxEVT_FETCH_SEQUENCE,(wxObjectEventFunction)&wxsDrawingWindow::OnFetchSequence);
    Panel = new DrawingPanel(this);
    Panel->Hide();
    SetScrollbars(5,5,1,1,0,0,true);
}

wxsDrawingWindow::~wxsDrawingWindow()
{
    if ( Bitmap ) delete Bitmap;
}

void wxsDrawingWindow::BeforeContentChanged()
{
    if ( !DuringChangeCnt++ )
    {
        Panel->Hide();
    }
}

void wxsDrawingWindow::AfterContentChanged()
{
    if ( !--DuringChangeCnt )
    {
        WasContentChanged = true;
        wxSize Size = GetVirtualSize();

        // Generating new bitmap
        if ( Bitmap ) delete Bitmap;
        Bitmap = new wxBitmap(Size.GetWidth(),Size.GetHeight());

        // Resizing panel to cover whole window
        int X, Y;
        CalcScrolledPosition(0,0,&X,&Y);
        Panel->SetSize(X,Y,Size.GetWidth(),Size.GetHeight());
        StartFetchingSequence();
    }
}

void wxsDrawingWindow::PanelPaint(wxPaintEvent& event)
{
    wxPaintDC PaintDC(Panel);
    if ( !DuringFetch )
    {
        if ( IsBlockFetch ||  NoNeedToRefetch() )
        {
            wxBitmap BmpCopy = Bitmap->GetSubBitmap(wxRect(0,0,Bitmap->GetWidth(),Bitmap->GetHeight()));
            wxBufferedDC DC(&PaintDC,BmpCopy);
            PaintExtra(&DC);
        }
        else
        {
            StartFetchingSequence();
        }
    }
}

void wxsDrawingWindow::PanelMouse(wxMouseEvent& event)
{
    event.SetEventObject(this);
    event.SetId(GetId());
    MouseExtra(event);
}

void wxsDrawingWindow::PanelKeyboard(wxKeyEvent& event)
{
    event.SetEventObject(this);
    event.SetId(GetId());
    ProcessEvent(event);
}

void wxsDrawingWindow::StartFetchingSequence()
{
    if ( DuringFetch )
    {
        return;
    }
    DuringFetch = true;

    // Fetching sequence will end after quitting
    // this event handler. This will be done
    // by adding some pending event
    wxCommandEvent event(wxEVT_FETCH_SEQUENCE,GetId());
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}

void wxsDrawingWindow::OnFetchSequence(wxCommandEvent& event)
{
    // Hiding panel to show content under it
    // If panel is hidden, there's no need to hide it and show children
    // because fetching sequence has been raised from AfterContentChanged()
    // and is actually correct
    if ( Panel->IsShown() )
    {
        Panel->Hide();
        ShowChildren();
    }
    Update();

    // Processing all pending events, it MUST be done
    // to repaint the content of window
    Manager::Yield();
    FetchScreen();
    HideChildren();
    Panel->Show();
    Manager::Yield();
    Panel->Update();
    Manager::Yield();

    FastRepaint();
    Manager::Yield();
    DuringFetch = false;
}

void wxsDrawingWindow::FetchScreen()
{
    // Fetching preview directly from screen
	wxScreenDC DC;
	wxMemoryDC DestDC;
    int X = 0, Y = 0;
    int DX = 0, DY = 0;
    ClientToScreen(&X,&Y);
    CalcUnscrolledPosition(0,0,&DX,&DY);
    DestDC.SelectObject(*Bitmap);
    DestDC.Blit(DX,DY,GetSize().GetWidth(),GetSize().GetHeight(),&DC,X,Y);
}

void wxsDrawingWindow::FastRepaint()
{
    wxClientDC ClientDC(Panel);
    wxBitmap BmpCopy = Bitmap->GetSubBitmap(wxRect(0,0,Bitmap->GetWidth(),Bitmap->GetHeight()));
    wxBufferedDC DC(&ClientDC,BmpCopy);
    PaintExtra(&DC);
}

void wxsDrawingWindow::ShowChildren()
{
    wxWindowList& Children = GetChildren();
    for ( size_t i=0; i<Children.GetCount(); i++ )
    {
        if ( Children[i] != Panel )
        {
            Children[i]->Show();
        }
    }
}

void wxsDrawingWindow::HideChildren()
{
    wxWindowList& Children = GetChildren();
    for ( size_t i=0; i<Children.GetCount(); i++ )
    {
        if ( Children[i] != Panel )
        {
            Children[i]->Hide();
        }
    }
}

bool wxsDrawingWindow::NoNeedToRefetch()
{
    int NewSizeX=0, NewSizeY=0;
    int NewVirtX=0, NewVirtY=0;

    GetClientSize(&NewSizeX,&NewSizeY);
    GetViewStart(&NewVirtX,&NewVirtY);

    if ( WasContentChanged ||
         NewSizeX != LastSizeX ||
         NewSizeY != LastSizeY ||
         NewVirtX != LastVirtX ||
         NewVirtY != LastVirtY )
    {
        WasContentChanged = false;
        LastSizeX = NewSizeX;
        LastSizeY = NewSizeY;
        LastVirtX = NewVirtX;
        LastVirtY = NewVirtY;
        return false;
    }

    return true;
}
