///////////////////////////////////////////////////////////////////////////////
// Name:        steart.h
// Purpose:     wxSTEditorMenuManager
// Author:      John Labenski, parts taken from wxGuide by Otto Wyss
// Modified by:
// Created:     29/05/2010
// Copyright:   (c) John Labenski, Troels K, Otto Wyss
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _STEART_H_
#define _STEART_H_

//-----------------------------------------------------------------------------
// wxSTEditorArtProvider - a holding place for our art for menu items and
//  toolbar tools.
//
// The XPM files located in the "art" directory contain the images used by
//  this art provider. If you wish to use you own images then just call
//  wxArtProvider::PushProvider(new myArtProvider);
//  and have it return your own bitmaps using the wxArtIDs set below.
//-----------------------------------------------------------------------------
#include <wx/artprov.h>

#define wxART_STEDIT                wxART_MAKE_CLIENT_ID(wxART_STEDIT)

#define wxART_STEDIT_APP            wxART_MAKE_ART_ID(wxART_STEDIT_APP)
#define wxART_STEDIT_NEW            wxART_MAKE_ART_ID(wxART_STEDIT_NEW)
#define wxART_STEDIT_OPEN           wxART_MAKE_ART_ID(wxART_STEDIT_OPEN)
#define wxART_STEDIT_SAVE           wxART_MAKE_ART_ID(wxART_STEDIT_SAVE)
#define wxART_STEDIT_SAVEALL        wxART_MAKE_ART_ID(wxART_STEDIT_SAVEALL)
#define wxART_STEDIT_SAVEAS         wxART_MAKE_ART_ID(wxART_STEDIT_SAVEAS)
#define wxART_STEDIT_PRINT          wxART_MAKE_ART_ID(wxART_STEDIT_PRINT)
#define wxART_STEDIT_PRINTPREVIEW   wxART_MAKE_ART_ID(wxART_STEDIT_PRINTPREVIEW)
#define wxART_STEDIT_PRINTSETUP     wxART_MAKE_ART_ID(wxART_STEDIT_PRINTSETUP)
#define wxART_STEDIT_PRINTPAGESETUP wxART_MAKE_ART_ID(wxART_STEDIT_PRINTPAGESETUP)
#define wxART_STEDIT_EXIT           wxART_MAKE_ART_ID(wxART_STEDIT_EXIT)
#define wxART_STEDIT_CUT            wxART_MAKE_ART_ID(wxART_STEDIT_CUT)
#define wxART_STEDIT_COPY           wxART_MAKE_ART_ID(wxART_STEDIT_COPY)
#define wxART_STEDIT_PASTE          wxART_MAKE_ART_ID(wxART_STEDIT_PASTE)
#define wxART_STEDIT_FIND           wxART_MAKE_ART_ID(wxART_STEDIT_FIND)
#define wxART_STEDIT_FINDNEXT       wxART_MAKE_ART_ID(wxART_STEDIT_FINDNEXT)
#define wxART_STEDIT_FINDUP         wxART_MAKE_ART_ID(wxART_STEDIT_FINDUP)
#define wxART_STEDIT_FINDDOWN       wxART_MAKE_ART_ID(wxART_STEDIT_FINDDOWN)
#define wxART_STEDIT_REPLACE        wxART_MAKE_ART_ID(wxART_STEDIT_REPLACE)
#define wxART_STEDIT_UNDO           wxART_MAKE_ART_ID(wxART_STEDIT_UNDO)
#define wxART_STEDIT_REDO           wxART_MAKE_ART_ID(wxART_STEDIT_REDO)
#define wxART_STEDIT_CLEAR          wxART_MAKE_ART_ID(wxART_STEDIT_CLEAR)

// Art for each preference dialog page
#define wxART_STEDIT_PREFDLG_VIEW       wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_VIEW)
#define wxART_STEDIT_PREFDLG_TABSEOL    wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_TABSEOL)
#define wxART_STEDIT_PREFDLG_FOLDWRAP   wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_FOLDWRAP)
#define wxART_STEDIT_PREFDLG_PRINT      wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_PRINT)
#define wxART_STEDIT_PREFDLG_LOADSAVE   wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_LOADSAVE)
#define wxART_STEDIT_PREFDLG_HIGHLIGHT  wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_HIGHLIGHT)
#define wxART_STEDIT_PREFDLG_STYLES     wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_STYLES)
#define wxART_STEDIT_PREFDLG_LANGS      wxART_MAKE_ART_ID(wxART_STEDIT_PREFDLG_LANGS)

#define wxDialogIconSize wxIconSize_SystemSmall
#define wxMenuIconSize   wxIconSize_SystemSmall

class WXDLLIMPEXP_STEDIT wxSTEditorArtProvider : public wxArtProvider
{
public:
    wxSTEditorArtProvider();

    static wxArtClient m_default_client; // default is wxART_STEDIT

    virtual wxBitmap CreateBitmap(const wxArtID& id,
                                  const wxArtClient& client,
                                  const wxSize& size);

    static wxBitmap GetBitmap(const wxArtID& id,
                              const wxSize& size = wxDefaultSize)
    {
       return wxArtProvider::GetBitmap(id, m_default_client, size);
    }

    static wxIcon GetIcon(const wxArtID& id,
                          const wxSize& size = wxDefaultSize)
    {
       return wxArtProvider::GetIcon(id, m_default_client, size);
    }
    static wxIcon GetDefaultDialogIcon();

protected:
    wxBitmap m_app_large;
    wxBitmap m_app_small;
};

#define STE_ARTBMP(id) wxSTEditorArtProvider::GetBitmap(id)

#endif  // _STEART_H_
