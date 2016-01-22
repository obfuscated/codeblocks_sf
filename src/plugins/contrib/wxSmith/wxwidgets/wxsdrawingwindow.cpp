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

#include "wxsdrawingwindow.h"

#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>
#include <wx/dcscreen.h>
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>
#include <wx/app.h>

#include <manager.h>
#include <logmanager.h>
#include <prep.h>

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
        return TimerNormal;
    }
}

BEGIN_EVENT_TABLE(wxsDrawingWindow,wxScrolledWindow)
    EVT_PAINT(wxsDrawingWindow::OnPaint)
    EVT_ERASE_BACKGROUND(wxsDrawingWindow::OnEraseBack)
END_EVENT_TABLE()

wxsDrawingWindow::wxsDrawingWindow(wxWindow* Parent,wxWindowID id,const wxPoint& pos,const wxSize& size,long style,const wxString& name):
    wxScrolledWindow(Parent,id,pos,size,style,name),
    m_Bitmap(0),
    m_IsBlockFetch(false),
    m_DuringFetch(false),
    m_DuringChangeCnt(0),
    m_LastSizeX(0),
    m_LastSizeY(0),
    m_LastVirtX(0),
    m_LastVirtY(0),
    m_WasContentChanged(false),
    m_IsDestroyed(false),
    m_RefreshTimer(this,RefreshTimerId)
{
    // Strange - it seems that by declaring this event in event table, it's not processed
    Connect(-1,wxEVT_FETCH_SEQUENCE,(wxObjectEventFunction)&wxsDrawingWindow::OnFetchSequence);
    Connect(RefreshTimerId,wxEVT_TIMER,(wxObjectEventFunction)&wxsDrawingWindow::OnRefreshTimer);
    SetScrollbars(5,5,1,1,0,0,true);
}

wxsDrawingWindow::~wxsDrawingWindow()
{
    m_IsDestroyed = true;
    delete m_Bitmap;
    m_Bitmap = 0;
}

void wxsDrawingWindow::BeforeContentChanged()
{
    m_DuringChangeCnt++;
}

void wxsDrawingWindow::AfterContentChanged()
{
    if ( !--m_DuringChangeCnt )
    {
        m_WasContentChanged = true;
        wxSize Size = GetVirtualSize();

        // Generating new bitmap
        delete m_Bitmap;
        m_Bitmap = new wxBitmap(Size.GetWidth(),Size.GetHeight());

        // Resizing panel to cover whole window
        int X, Y;
        CalcScrolledPosition(0,0,&X,&Y);
        StartFetchingSequence();
    }
}

void wxsDrawingWindow::OnPaint(wxPaintEvent& event)
{
    if ( !m_DuringFetch )
    {
        wxPaintDC PaintDC(this);
        PrepareDC(PaintDC);
        if ( m_IsBlockFetch || NoNeedToRefetch() )
        {
            if ( m_Bitmap )
            {
                wxBitmap BmpCopy = m_Bitmap->GetSubBitmap(wxRect(0,0,m_Bitmap->GetWidth(),m_Bitmap->GetHeight()));
                wxBufferedDC DC(&PaintDC,BmpCopy);
                PaintExtra(&DC);
            }
        }
        else
        {
            StartFetchingSequence();
        }
    }
    else
    {
        event.Skip();
    }
}

void wxsDrawingWindow::OnEraseBack(wxEraseEvent& event)
{
    // Let the background be cleared when screenshoot is in progress
    if ( m_DuringFetch ) event.Skip();
}

void wxsDrawingWindow::StartFetchingSequence()
{
    if ( m_DuringFetch )
    {
        return;
    }
    m_DuringFetch = true;

    // Fetching sequence will end after quitting
    // this event handler. This will be done
    // by adding some pending event
    wxCommandEvent event(wxEVT_FETCH_SEQUENCE,GetId());
    event.SetEventObject(this);
    GetEventHandler()->AddPendingEvent(event);
}

void wxsDrawingWindow::OnFetchSequence(cb_unused wxCommandEvent& event)
{
    if ( m_IsDestroyed ) return;

    ShowChildren();
    Refresh();
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
            m_RefreshTimer.Start(50,true);
            break;

        case TimerFast:
            // This version is simillar to TimerNormal, but with the difference is
            // that here's almost no gap between refresh request and refresh
            // execution. This may not be preffered on some platforms where
            // timer events have high priority and are called before processing
            // any pending events on queue. In such situation, it may lead to
            // some unprocessed events which should update screen's content
            // while fetching bitmap.
            m_RefreshTimer.Start(1,true);
            break;

        default:
            break;
    }

}

void wxsDrawingWindow::OnRefreshTimer(cb_unused wxTimerEvent& event)
{
    FetchSequencePhase2();
}

void wxsDrawingWindow::FetchSequencePhase2()
{
    if ( m_IsDestroyed ) return;
    FetchScreen();
    ScreenShootTaken();
    HideChildren();
    m_DuringFetch = false;
}

void wxsDrawingWindow::FetchScreen()
{
    if ( !m_Bitmap ) return;

    wxClientDC DC(this);
    wxMemoryDC DestDC;
    int X = 0, Y = 0;
    int DX = 0, DY = 0;
    CalcUnscrolledPosition(0,0,&DX,&DY);
    DestDC.SelectObject(*m_Bitmap);
    DestDC.Blit(DX,DY,GetSize().GetWidth(),GetSize().GetHeight(),&DC,X,Y);
    DestDC.SelectObject(wxNullBitmap);
}

void wxsDrawingWindow::FastRepaint()
{
    wxClientDC ClientDC(this);
    PrepareDC(ClientDC);
    wxBitmap BmpCopy = m_Bitmap->GetSubBitmap(wxRect(0,0,m_Bitmap->GetWidth(),m_Bitmap->GetHeight()));
    wxBufferedDC DC(&ClientDC,BmpCopy);
    PaintExtra(&DC);
}

void wxsDrawingWindow::ShowChildren()
{
    wxWindowList& Children = GetChildren();
    for ( size_t i=0; i<Children.GetCount(); i++ )
    {
        Children[i]->Show();
    }
}

void wxsDrawingWindow::HideChildren()
{
    wxWindowList& Children = GetChildren();
    for ( size_t i=0; i<Children.GetCount(); i++ )
    {
        Children[i]->Hide();
    }
}

bool wxsDrawingWindow::NoNeedToRefetch()
{
    // Testing current application is not active, If it's not,
    // We block any fetches because fetching may read part
    // of other application
    if ( wxTheApp && !wxTheApp->IsActive() )
    {
        m_WasContentChanged = true;
        return true;
    }

    // Testing if this window is disabled which usually means
    // that there's some dialog shown in modal
    // modal dialogs may cause some artefacts on editor so we will
    // block any refetching while they're shown and will force
    // updating content when dialog will be hidden
    for ( wxWindow* Window = this; Window; Window = Window->GetParent() )
    {
        if ( !Window->IsEnabled() || !Window->IsShown() )
        {
            m_WasContentChanged = true;
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

    if ( m_WasContentChanged ||
         NewSizeX != m_LastSizeX ||
         NewSizeY != m_LastSizeY ||
         NewVirtX != m_LastVirtX ||
         NewVirtY != m_LastVirtY )
    {
        m_WasContentChanged = false;
        m_LastSizeX = NewSizeX;
        m_LastSizeY = NewSizeY;
        m_LastVirtX = NewVirtX;
        m_LastVirtY = NewVirtY;
        return false;
    }

    return true;
}

bool wxsDrawingWindow::Destroy()
{
    m_IsDestroyed = true;
    return wxScrolledWindow::Destroy();
}
