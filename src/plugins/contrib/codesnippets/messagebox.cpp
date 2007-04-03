/*
	This file is part of SnipList, program to store information snippets.
	Copyright (C) 2006 Pecan Heber

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifdef WX_PRECOMP
    #include "wx_pch.h"
#else
    #include <wx/wx.h>
    #include <wx/event.h>
    #include <wx/dcclient.h>
#endif

#include "messageBoxForm.h"
#include "messagebox.h"
#include "snippetsconfig.h"
// ----------------------------------------------------------------------------
class messageBoxCtrl : public messageBoxForm
// ----------------------------------------------------------------------------
{
   public:
    messageBoxCtrl( wxWindow* parent, int id = -1,
                    wxString title = wxT("Properties"),
                    wxPoint pos = wxDefaultPosition,
                    wxSize size = wxSize( 527,120 ),
                    int dialogStyle = wxCAPTION|wxDEFAULT_DIALOG_STYLE
                                    |wxRESIZE_BORDER|wxSTAY_ON_TOP,
                    int textStyle = wxTE_CENTRE )
        :messageBoxForm( parent, id, title, pos, size,
                         dialogStyle, textStyle )
        {;}

    void OnYes(wxCommandEvent& event) { EndModal(wxYES);}
    void OnNo (wxCommandEvent& event) { EndModal(wxNO );}
    void OnOk (wxCommandEvent& event) { EndModal(wxOK );}
    void OnCancel (wxCommandEvent& event) { EndModal(wxCANCEL );}

    DECLARE_EVENT_TABLE()

};
// ----------------------------------------------------------------------------
   BEGIN_EVENT_TABLE(messageBoxCtrl, messageBoxForm)
    EVT_BUTTON(wxID_YES,             messageBoxCtrl::OnYes)
    EVT_BUTTON(wxID_NO,              messageBoxCtrl::OnNo)
    EVT_BUTTON(wxID_OK,              messageBoxCtrl::OnOk)
    EVT_BUTTON(wxID_CANCEL,          messageBoxCtrl::OnCancel)
   END_EVENT_TABLE()

// ----------------------------------------------------------------------------
int messageBox( const wxString& message, const wxString& title, long dialogStyle, long textStyle)
// ----------------------------------------------------------------------------
{
    // Avoid infinite loop when main app window has already been destroyed
    // this doesn't work during debugging, causing an exit loop

    // Race condition:
    // This doesn't work well. wxWidgets reports the window and even IsShown()
    //  even when the debugger shows the window is already destroyed.
    if (!wxTheApp->GetTopWindow()){
        return wxMessageBox(message, title, dialogStyle);
    }

    int dialogStyleDefault = wxCAPTION|wxDEFAULT_DIALOG_STYLE |wxRESIZE_BORDER|wxSTAY_ON_TOP ;
    if (-1 == dialogStyle ) dialogStyle = dialogStyleDefault;
    else dialogStyle |= dialogStyleDefault;

    //int textStyleDefault = wxTE_CENTRE|wxTE_MULTILINE|wxTE_READONLY|wxNO_BORDER| wxTE_RICH;
    int textStyleDefault = wxTE_CENTRE|wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH;
    if (not (wxBORDER_MASK & textStyle)) textStyleDefault |= wxNO_BORDER;
    textStyle |= textStyleDefault;

    wxTextAttr textAttr(wxNullColour, wxNullColour,  wxNullFont, wxTEXT_ALIGNMENT_JUSTIFIED);
    //SetLeftIndent() sets the left indent in tenths of a millimetre
    int indentMMAmt  = 25/7 ; // 1/7 inch * 25 mm/inch
    textAttr.SetLeftIndent(10*indentMMAmt);

    int h; int w;
    int x; int y;
    int displayX; int displayY;
    wxWindow* mainFrame = GetConfig()->pMainFrame;
    // move dialog box underneath the mouse pointer
    wxPoint movePosn = ::wxGetMousePosition();
    // move upper left dialog corner to center of parent
    ::wxDisplaySize(&displayX, &displayY);
    mainFrame->GetPosition(&x, &y );
    mainFrame->GetClientSize(&w,&h);
    movePosn.x = x+(w>>2);
    movePosn.y = y+(h>>2);
    //wxWindow* pWin = pBox->GetParent();

    // Get button sizes
    wxSize buttonSize( 300, -1 );

    int lines = message.Freq('\n');
    if (not lines) ++lines;
    // adjust the width to account for carriage returns
    size_t i=0, lth=0, longest=0, bigposn = 0;
    for ( i=0; i<message.Length(); ++i )
    {
        lth = message.Mid(i).Find('\n');
        if (-1 == (int)lth) break;
        if (lth > longest){
             bigposn=i;
             longest = lth;
             //#ifdef LOGGING
             // LOGIT( _T("longest[%s]"),message.Mid(bigposn,longest).GetData() );
             //#endif //LOGGING
        }
    }
    // check from last \n to end of string for possible longer line
    if (i < message.Length() )
    {   lth = message.Mid(i).Length();
        if ( lth > longest)
        {    bigposn = i;
             longest = lth;
        }
    }
    #ifdef LOGGING
     //LOGIT( _T("Longest line[%s]"),message.Mid(bigposn, longest).GetData() );
    #endif //LOGGING

    // Get info to determine message width
    wxClientDC dc(mainFrame);
    wxFont font = dc.GetFont();
    wxSize PixelsPerInch = dc.GetPPI();
    int PixelsPerMM = PixelsPerInch.GetWidth()/25;
        // Get longest message width
    w = -1;
    dc.GetTextExtent(message.Mid(bigposn,longest), &w, &h);
    w+= (PixelsPerInch.GetWidth()/7);    // add 1/7 inch spacer pixels
        // get largest width of buttons or message
    wxSize size = wxSize( wxMax(w, buttonSize.GetWidth()), 120 );
        // add left indent width 100/25 millimeters
    size.SetWidth(size.GetWidth() + (PixelsPerMM*indentMMAmt) );

    // now get the height of lines + height of buttons + spacers
    int height = (h*lines)+120;//+(PixelsPerMM*indentMMAmt);
    size.SetHeight(height);

    // Make sure box is not off the screen
    if ( (movePosn.x+size.GetWidth()) > displayX)
        movePosn.x = displayX-size.GetWidth();
    if ( (movePosn.y+size.GetHeight()) > displayY)
        movePosn.y = displayY-size.GetHeight();

    wxSystemSettings sysSettings;
    wxColour newTextBkgnd = sysSettings.GetColour(wxSYS_COLOUR_BTNFACE);

    // Create the custom message box
    messageBoxForm* pBox = new messageBoxCtrl( mainFrame, -1,
        title, movePosn, size, dialogStyle, textStyle);
    pBox->m_messageBoxTextCtrl->SetBackgroundColour(newTextBkgnd);
    pBox->m_messageBoxTextCtrl->Clear();
    pBox->m_messageBoxTextCtrl->SetDefaultStyle( textAttr);
    pBox->m_messageBoxTextCtrl->WriteText(message);

    // This routine returns successful results, but the caret is still
    // shown in the textCtrl. ShowModal must turn it back on.
    //HWND handle = (HWND)pBox->m_messageBoxTextCtrl->GetHandle();
    //int bresult = ::HideCaret(0);
    //if (not bresult)
    //{ bresult =  GetLastError();
    //    #ifdef LOGGING
    //     LOGIT( _T("GetLastError[%d]"), bresult);
    //    #endif //LOGGING
    //}

    // return results
    int result = pBox->ShowModal();
    delete pBox;
    return result;
}
// ----------------------------------------------------------------------------

