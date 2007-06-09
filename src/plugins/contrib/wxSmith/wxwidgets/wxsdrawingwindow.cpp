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

    /** \brief Identifier used by refresh timer */
    const int RefreshTimerId = wxNewId();

    /** \brief Types of paint delays */
    enum RepaintDelayType
    {
        None,
        Yield,
        TimerFast,
        TimerNormal
    };

    inline RepaintDelayType GetDelayType()
    {
        // Looks like this gives best results so far on both linux and windows
        return TimerNormal;
    }
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
        DrawingPanel(wxsDrawingWindow* Parent): wxPanel(Parent,DrawingPanelId), m_Parent(Parent)
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

        /** \brief Dctor */
        virtual ~DrawingPanel()
        {
            m_Parent->Panel = NULL;
        }

        void OnEraseBack(wxEraseEvent& event)
        {
        }

        wxsDrawingWindow* m_Parent;
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
    WasContentChanged(false),
    IsDestroyed(false),
    RefreshTimer(this,RefreshTimerId)
{
    // Strange - it seems that by declaring this event in event table, it's not processed
    Connect(-1,wxEVT_FETCH_SEQUENCE,(wxObjectEventFunction)&wxsDrawingWindow::OnFetchSequence);
    Connect(RefreshTimerId,wxEVT_TIMER,(wxObjectEventFunction)&wxsDrawingWindow::OnRefreshTimer);
    Panel = new DrawingPanel(this);
    Panel->Hide();
    SetScrollbars(5,5,1,1,0,0,true);
}

wxsDrawingWindow::~wxsDrawingWindow()
{
    IsDestroyed = true;
    if ( Bitmap ) delete Bitmap;
    Panel = NULL;
}

void wxsDrawingWindow::BeforeContentChanged()
{
    if ( !DuringChangeCnt++ && Panel )
    {
        Panel->Hide();
    }
}

void wxsDrawingWindow::AfterContentChanged()
{
    if ( !--DuringChangeCnt && Panel )
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
    if ( !Panel ) return;

    wxPaintDC PaintDC(Panel);
    if ( !DuringFetch )
    {
        if ( IsBlockFetch || NoNeedToRefetch() )
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
    if ( !Panel ) return;
    if ( IsDestroyed ) return;

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

    // Here we have requested to hide panel and show children
    // But since wxWidges does some updates on events, we
    // have to introduce some delay and allow wxWidgets to
    // do it's stuff before we can read bitmap directly from screen
    switch ( GetDelayType() )
    {
        case None:
            // We don't wait at all assuming that everything is
            // shown now
            FetchSequencePhase2();
            break;

        case Yield:
            // We call Yield() to let wxWidgets process all messages
            // This can be dangerous in some environments when calling
            // Yield() may internally destroy this class (happens on Linux)
            Manager::Yield();
            FetchSequencePhase2();
            break;

        case TimerNormal:
            // We start timer that will send event after some time.
            // We assume here that before timer event is processed,
            // all events used to udpate screen will be processed.
            RefreshTimer.Start(50,true);
            break;

        case TimerFast:
            // This version is simillar to TimerNormal, but with the difference is
            // that here's almost no gap between refresh request and refresh
            // execution. This may not be preffered on some platforms where
            // timer events have high priority and are called before processing
            // any pending events on queue. In such situation, it may lead to
            // some unprocessed events which should update screen's content
            // while fetching bitmap.
            RefreshTimer.Start(1,true);
            break;
    }

}

void wxsDrawingWindow::OnRefreshTimer(wxTimerEvent& event)
{
    FetchSequencePhase2();
}

void wxsDrawingWindow::FetchSequencePhase2()
{
    if ( !Panel ) return;
    if ( IsDestroyed ) return;
    FetchScreen();
    HideChildren();
    DuringFetch = false;
    Panel->Show();
}

void wxsDrawingWindow::FetchScreen()
{
    if ( !Bitmap ) return;

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
    if ( !Panel ) return;
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
    // Testing if this window is disabled which usually means
    // that there's some dialog shown in modal
    // modal dialogs may cause some artefacts on editor so we will
    // block any refetching while they're shown and will force
    // updating content when dialog will be hidden
    for ( wxWindow* Window = this; Window; Window = Window->GetParent() )
    {
        if ( !Window->IsEnabled() || !Window->IsShown() )
        {
            WasContentChanged = true;
            return true;
        }
    }

    // If there's no risk that some dialog is shown in modal,
    // we check if WasContentChanged flag is set or position / size
    // of area is changed.
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

bool wxsDrawingWindow::Destroy()
{
    IsDestroyed = true;
    return wxScrolledWindow::Destroy();
}
