/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
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
