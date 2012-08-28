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

#ifndef WXSDRAWINGWINDOW_H
#define WXSDRAWINGWINDOW_H

#include <wx/scrolwin.h>
#include <wx/timer.h>
#include <wx/event.h>

/** \brief Class allowing drawing over it's surface.
 *
 * This window is upgrade of standard wxScrolledWindow.
 * It allows painting over it's surface and over surface
 * of it's children. It also handles all mouse and keyboard
 * events.
 */
class wxsDrawingWindow: public wxScrolledWindow
{
    public:

        /** \brief Ctor */
        wxsDrawingWindow(wxWindow* Parent,wxWindowID id,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize,long style=wxHSCROLL|wxVSCROLL,const wxString& name=_T("wxsDrawingWindow"));

        /** \brief Dctor */
        virtual ~wxsDrawingWindow();

        /** \brief Function fetching bitmap from this window
         *
         * \note Regions which were not visible to the screen
         *       won't be valid in this bitmap.
         */
        const wxBitmap& GetBitmap() { return *m_Bitmap; }

        /** \brief Function notifying that window's content is going to change
         *
         * This function should be called just before changing content of
         * this window (to be more exact: it's child items). It makes
         * wxsDrawingWindow to prepare for change.
         *
         * \warning Remember to call AfterContentChanged after change
         */
        void BeforeContentChanged();

        /** \brief Function notifying that window's content has been changed
         *
         * This function should be called right after change of window content
         * (to be more exact: after changing window's children). It makes
         * wxsDrawingWindow to fetch children preview and repaint itself
         *
         * \warning Call to this function must be preceded with call to
         *          BeforeContentChange function.
         *
         */
        void AfterContentChanged();

        /** \brief Function repainting window
         *
         * Repainting window content may be done in two ways.
         * First is by raising paint event (so by calling wxWindow::Refresh),
         * in current implementation this require refetching bitmap from
         * window area (just like calling AfterContentChanged). Second way is by
         * calling this function. It does not raise any events if not needed
         * and draws using wxClientDC object. It may be used when extra
         * graphics has been changed only.
         */
        void FastRepaint();

        /** \brief Function (un)blocking background fetching */
        inline void BlockFetch(bool Block=true) { m_IsBlockFetch = Block; }

    protected:

        /** \brief Function used for drawing additional data
         *
         * You should override this function and paint additional
         * graphics inside this.
         */
        virtual void PaintExtra(wxDC* DC) = 0;

        /** \brief Function called right after taking screen shoot and just before hiding children */
        virtual void ScreenShootTaken() {}

    private:

        void OnPaint(wxPaintEvent& event);
        void OnEraseBack(wxEraseEvent& event);
        void OnFetchSequence(wxCommandEvent& event);
        void OnRefreshTimer(wxTimerEvent& event);

        virtual bool Destroy();

        /** \brief Function stating sequence fetching editor's background
         *
         * This sequence may be splitted into few smaller events so it's not
         * granted that after finishing StartFetchingSequence, the content of
         * Bitmap is valid.
         */
        void StartFetchingSequence();

        /** \brief Starting second step of fetch sequence
         *
         * In this step, the screen content is grabbed from the screen
         * and put into internal bitmap.
         */
        void FetchSequencePhase2();

        /** \brief Function copying screen data into bitmap */
        void FetchScreen();

        /** \brief Hiding all children except Panel to avoid some random repaints */
        void HideChildren();

        /** \brief Showing all children except Panel just before fetching preview */
        void ShowChildren();

        /** \brief Checking if there's need to refetch background
         *
         * Screen is not fetched when size of drawing window is
         * the same, sliders were not shifted and there was no
         * call to ContentChanged()
         */
        bool NoNeedToRefetch();

        wxBitmap* m_Bitmap;             ///< \brief Bitmap with fetched window content (may be valid partially)
        bool m_IsBlockFetch;            ///< \brief Flag used to block fetching background (may be set by user)
        bool m_DuringFetch;             ///< \brief Set to true if we're during fetching sequence
        int m_DuringChangeCnt;          ///< \brief When >0, window's content is changed somewhere
        int m_LastSizeX, m_LastSizeY;   ///< \brief client size during last fetch
        int m_LastVirtX, m_LastVirtY;   ///< \brief virtual area shift relative to client area during last fetch
        bool m_WasContentChanged;       ///< \brief If there was a call to WasContentChanged from last fetch
        bool m_IsDestroyed;             ///< \brief Set to true after calling Destroy(), used to track destroyed but not yet deleted windows

        wxTimer m_RefreshTimer;         ///< \brief Timer used while fetching screen content

        DECLARE_EVENT_TABLE()
};

#endif
