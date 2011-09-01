#include "GenericMessageBox.h"

/////////////////////////////////////////////////////////////////////////////
// Name:        GenericMessageDialog (derived from src/generic/msgdlgg.cpp)
// Purpose:     Centering wxMessageBox
// Author:      Julian Smart, Robert Roebling
// Modified by: Pecan 2009/06/5
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

//-#if wxUSE_MSGDLG && (!defined(__WXGTK20__) || defined(__WXUNIVERSAL__) || defined(__WXGPE__))

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/button.h"
    #include "wx/stattext.h"
    #include "wx/statbmp.h"
    #include "wx/layout.h"
    #include "wx/intl.h"
    #include "wx/icon.h"
    #include "wx/sizer.h"
    #include "wx/app.h"
    #include "wx/settings.h"
#endif

#include <stdio.h>
#include <string.h>

#define __WX_COMPILING_MSGDLGG_CPP__ 1
#include "wx/msgdlg.h"
#include "wx/artprov.h"

#if wxUSE_STATLINE
    #include "wx/statline.h"
#endif

// ----------------------------------------------------------------------------
// icons
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(GenericMessageDialog, wxScrollingDialog)
        EVT_BUTTON(wxID_YES, GenericMessageDialog::OnYes)
        EVT_BUTTON(wxID_NO, GenericMessageDialog::OnNo)
        EVT_BUTTON(wxID_CANCEL, GenericMessageDialog::OnCancel)
END_EVENT_TABLE()

IMPLEMENT_CLASS(GenericMessageDialog, wxScrollingDialog)

// ----------------------------------------------------------------------------
GenericMessageDialog::GenericMessageDialog( wxWindow *parent,
                                                const wxString& message,
                                                const wxString& caption,
                                                long style,
                                                const wxPoint& pos)
// ----------------------------------------------------------------------------
    : wxScrollingDialog( parent, wxID_ANY, caption, pos, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{

    SetMessageDialogStyle(style);

    bool is_pda = (wxSystemSettings::GetScreenType() <= wxSYS_SCREEN_PDA);

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *icon_text = new wxBoxSizer( wxHORIZONTAL );

//-#if wxUSE_STATBMP
    // 1) icon
    if (style & wxICON_MASK)
    {
        wxBitmap bitmap;
        switch ( style & wxICON_MASK )
        {
            default:
                wxFAIL_MSG(_T("incorrect log style"));
                // fall through

            case wxICON_ERROR:
                bitmap = wxArtProvider::GetIcon(wxART_ERROR, wxART_MESSAGE_BOX);
                break;

            case wxICON_INFORMATION:
                bitmap = wxArtProvider::GetIcon(wxART_INFORMATION, wxART_MESSAGE_BOX);
                break;

            case wxICON_WARNING:
                bitmap = wxArtProvider::GetIcon(wxART_WARNING, wxART_MESSAGE_BOX);
                break;

            case wxICON_QUESTION:
                bitmap = wxArtProvider::GetIcon(wxART_QUESTION, wxART_MESSAGE_BOX);
                break;
        }
        wxStaticBitmap *icon = new wxStaticBitmap(this, wxID_ANY, bitmap);
        if (is_pda)
            topsizer->Add( icon, 0, wxTOP|wxLEFT|wxRIGHT | wxALIGN_LEFT, 10 );
        else
            icon_text->Add( icon, 0, wxCENTER );
    }
//-#endif // wxUSE_STATBMP

//-#if wxUSE_STATTEXT
    // 2) text
    icon_text->Add( CreateTextSizer( message ), 0, wxALIGN_CENTER | wxLEFT, 10 );

    topsizer->Add( icon_text, 1, wxCENTER | wxLEFT|wxRIGHT|wxTOP, 10 );
//-#endif // wxUSE_STATTEXT

    // 3) buttons
    int center_flag = wxEXPAND;
    if (style & wxYES_NO)
        center_flag = wxALIGN_CENTRE;

    // make OK button center also
    center_flag = wxALIGN_CENTRE;       //(pecan 2009/6/05)

    wxSizer *sizerBtn = CreateSeparatedButtonSizer(style & ButtonSizerFlags);
    if ( sizerBtn )
        topsizer->Add(sizerBtn, 0, center_flag | wxALL, 10 );

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );
    wxSize size( GetSize() );

    // This original code is causing multiline text dialogs to be way too WIDE !! //(pecan 2009/6/06)
    // Better to control text with \n's.
    //if (size.x < size.y*3/2)
    //{
    //    size.x = size.y*3/2;
    //    SetSize( size );
    //}

    Centre( wxBOTH | wxCENTER_FRAME);
}

void GenericMessageDialog::OnYes(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_YES );
}

void GenericMessageDialog::OnNo(wxCommandEvent& WXUNUSED(event))
{
    EndModal( wxID_NO );
}

void GenericMessageDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
    // Allow cancellation via ESC/Close button except if
    // only YES and NO are specified.
    const long style = GetMessageDialogStyle();
    if ( (style & wxYES_NO) != wxYES_NO || (style & wxCANCEL) )
    {
        EndModal( wxID_CANCEL );
    }
}
// ----------------------------------------------------------------------------
int GenericMessageBox(const wxString& messageIn, const wxString& captionIn, long style,
                 wxWindow *parent, int x, int y )
// ----------------------------------------------------------------------------
{
    long decorated_style = style | wxCENTER;

    if ( ( style & ( wxICON_EXCLAMATION | wxICON_HAND | wxICON_INFORMATION | wxICON_QUESTION ) ) == 0 )
    {
        decorated_style |= ( style & wxYES ) ? wxICON_QUESTION : wxICON_INFORMATION ;
    }

    // Substitute blanks for \t (tabs) which the wxGenericMessageDialog can't handle
    wxString message = messageIn; message.Replace( _T("\t"), _T("    ") );
    wxString caption = captionIn; caption.Replace( _T("\t"), _T("    ") );

    GenericMessageDialog dialog(parent, message, caption, decorated_style, wxPoint(x,y));

    int ans = dialog.ShowModal();
    switch ( ans )
    {
        case wxID_OK:
            return wxOK;
        case wxID_YES:
            return wxYES;
        case wxID_NO:
            return wxNO;
        case wxID_CANCEL:
            return wxCANCEL;
    }

    wxFAIL_MSG( _T("unexpected return code from GenericMessageDialog") );

    return wxCANCEL;
}
//-#endif // wxUSE_MSGDLG && !defined(__WXGTK20__)
