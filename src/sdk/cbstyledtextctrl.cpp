/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision: 4722 $
* $Id: cbstyledtextctrl.cpp 4722 2007-12-11 17:04:07Z thomasdenk $
* $HeadURL: svn+ssh://killerbot@svn.berlios.de/svnroot/repos/codeblocks/trunk/src/sdk/cbeditor.cpp $
*/

//#include "sdk_precomp.h"
//#ifndef CB_PRECOMP
//    #include "globals.h"
//#endif

#include <wx/string.h>
#include "cbstyledtextctrl.h"
#include "editorbase.h" // DisplayContextMenu
#include "prep.h" // platform::gtk

BEGIN_EVENT_TABLE(cbStyledTextCtrl, wxScintilla)
    EVT_CONTEXT_MENU(cbStyledTextCtrl::OnContextMenu)
    EVT_KILL_FOCUS(cbStyledTextCtrl::OnKillFocus)
    EVT_MIDDLE_DOWN(cbStyledTextCtrl::OnGPM)
END_EVENT_TABLE()

cbStyledTextCtrl::cbStyledTextCtrl(wxWindow* pParent, int id, const wxPoint& pos, const wxSize& size, long style)
    : wxScintilla(pParent, id, pos, size, style),
    m_pParent(pParent)
{
    //ctor
}

cbStyledTextCtrl::~cbStyledTextCtrl()
{
    //dtor
}

// events

void cbStyledTextCtrl::OnKillFocus(wxFocusEvent& event)
{
    // cancel auto-completion list when losing focus
    if (AutoCompActive())
    {
        AutoCompCancel();
    }
    if (CallTipActive())
    {
        CallTipCancel();
    }
    event.Skip();
} // end of OnKillFocus

void cbStyledTextCtrl::OnContextMenu(wxContextMenuEvent& event)
{
    if ( m_pParent )
    {
        if ( EditorBase* pParent = dynamic_cast<EditorBase*>(m_pParent) )
        {
            const bool is_right_click = event.GetPosition() != wxDefaultPosition;
            const wxPoint mp(is_right_click ? event.GetPosition() : wxDefaultPosition);
            pParent->DisplayContextMenu(mp, mtEditorManager);
        }
        else
        {
            event.Skip();
        }
    }
} // end of OnContextMenu

void cbStyledTextCtrl::OnGPM(wxMouseEvent& event)
{
    if(platform::gtk == false) // only if GPM is not already implemented by the OS
    {
        int pos = PositionFromPoint(wxPoint(event.GetX(), event.GetY()));

        if(pos == wxSCI_INVALID_POSITION)
            return;

        int start = GetSelectionStart();
        int end = GetSelectionEnd();

        const wxString s = GetSelectedText();

        if(pos < GetCurrentPos())
        {
            start += s.length();
            end += s.length();
        }

        InsertText(pos, s);
        SetSelection(start, end);
    }
} // end of OnGPM
