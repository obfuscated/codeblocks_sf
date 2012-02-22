///////////////////////////////////////////////////////////////////////////////
// Name:        steart.cpp
// Purpose:     wxSTEditorArtProvider
// Author:      John Labenski, parts taken from wxGuide by Otto Wyss
// Modified by:
// Created:     29/05/2010
// RCS-ID:
// Copyright:   (c) John Labenski, Troels K, Otto Wyss
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

#include "precomp.h"

#include <wx/stedit/stedefs.h>
#include <wx/stedit/steart.h>
#include "wxext.h"

//-----------------------------------------------------------------------------
// wxSTEditorArtProvider
//-----------------------------------------------------------------------------
#include <wx/image.h>

// Bitmaps used for the toolbar in the wxSTEditorFrame
#include "../art/pencil16.xpm"
#include "../art/pencil32.xpm"
#include "../art/new.xpm"
#include "../art/open.xpm"
#include "../art/save.xpm"
#include "../art/saveall.xpm"
#include "../art/saveas.xpm"
#include "../art/print.xpm"
#include "../art/print_preview.xpm"
#include "../art/print_setup.xpm"
#include "../art/print_page_setup.xpm"
#include "../art/x_red.xpm"

#include "../art/cut.xpm"
#include "../art/copy.xpm"
#include "../art/paste.xpm"
#include "../art/find.xpm"
#include "../art/findnext.xpm"
#include "../art/finddown.xpm"
#include "../art/findup.xpm"
#include "../art/replace.xpm"
#include "../art/undo.xpm"
#include "../art/redo.xpm"
#include "../art/cross.xpm"

static wxBitmap DoGetBitmap(const wxArtID& id, const wxArtClient& client, const wxSize& reqSize)
{
    static const struct art_item
    {
        wxArtID ste_id;
        const char* const* xpm;
        wxArtID wx_id;
    } array[] =
    {
        { wxART_STEDIT_NEW,            new_xpm,             wxART_NEW          },
        { wxART_STEDIT_OPEN,           open_xpm,            wxART_FILE_OPEN    },
        { wxART_STEDIT_SAVE,           save_xpm,            wxART_FILE_SAVE    },
        { wxART_STEDIT_SAVEALL,        saveall_xpm,         wxEmptyString},
        { wxART_STEDIT_SAVEAS,         saveas_xpm,          wxART_FILE_SAVE_AS },
        { wxART_STEDIT_PRINT,          print_xpm,           wxART_PRINT        },
        { wxART_STEDIT_PRINTPREVIEW,   print_preview_xpm,   wxEmptyString},
        { wxART_STEDIT_PRINTSETUP,     print_setup_xpm,     wxEmptyString},
        { wxART_STEDIT_PRINTPAGESETUP, print_page_setup_xpm, wxEmptyString },
        { wxART_STEDIT_EXIT,           x_red_xpm,           wxART_QUIT         },
        { wxART_STEDIT_CUT,            cut_xpm,             wxART_CUT   },
        { wxART_STEDIT_COPY,           copy_xpm,            wxART_COPY   },
        { wxART_STEDIT_PASTE,          paste_xpm,           wxART_PASTE },
        { wxART_STEDIT_FIND,           find_xpm,            wxART_FIND   },
        { wxART_STEDIT_FINDNEXT,       findnext_xpm,        wxEmptyString },
        { wxART_STEDIT_FINDUP,         findup_xpm,          wxEmptyString },
        { wxART_STEDIT_FINDDOWN,       finddown_xpm,        wxEmptyString },
        { wxART_STEDIT_REPLACE,        replace_xpm,         wxART_FIND_AND_REPLACE },
        { wxART_STEDIT_UNDO,           undo_xpm,            wxART_UNDO  },
        { wxART_STEDIT_REDO,           redo_xpm,            wxART_REDO },
        { wxART_STEDIT_CLEAR,          cross_xpm,           wxART_DELETE }
    };

    for (size_t i = 0; i < WXSIZEOF(array); i++)
    {
        if (array[i].ste_id == id)
        {
            if ( (wxART_STEDIT == client) || array[i].wx_id.empty() )
            {
                return wxBitmap(array[i].xpm);
            }
            else
            {
                return wxArtProvider::GetBitmap(array[i].wx_id, client, reqSize);
            }
        }
    }
    return wxNullBitmap;
}

wxSTEditorArtProvider::wxSTEditorArtProvider() : wxArtProvider(), m_app_large(pencil32_xpm), m_app_small(pencil16_xpm)
{
}

/*static*/ wxArtClient wxSTEditorArtProvider::m_default_client = wxART_STEDIT;

wxBitmap wxSTEditorArtProvider::CreateBitmap(const wxArtID& id,
                                             const wxArtClient& client,
                                             const wxSize& reqSize_)
{
    wxBitmap bmp;
    wxSize reqSize = wxIconSize_System;

    if (id == wxART_STEDIT_PREFDLG_VIEW)
        bmp = wxArtProvider::GetBitmap(wxART_FIND, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_PREFDLG_TABSEOL)
        bmp = wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_PREFDLG_FOLDWRAP)
        bmp = wxArtProvider::GetBitmap(wxART_COPY, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_PREFDLG_PRINT)
        bmp = wxArtProvider::GetBitmap(wxART_PRINT, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_PREFDLG_LOADSAVE)
        bmp = wxArtProvider::GetBitmap(wxART_FILE_SAVE, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_PREFDLG_HIGHLIGHT)
        bmp = wxArtProvider::GetBitmap(wxART_TIP, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_PREFDLG_STYLES)
        bmp = wxArtProvider::GetBitmap(wxART_HELP_BOOK, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_PREFDLG_LANGS)
        bmp = wxArtProvider::GetBitmap(wxART_HELP_SETTINGS, wxART_OTHER, reqSize);
    else if (id == wxART_STEDIT_APP)
    {
        // this logic has room for improvement
        bmp = (reqSize_ == wxSize(m_app_small.GetWidth(), m_app_small.GetHeight()))
           ? m_app_small
           : m_app_large;
    }
    else
    {
        bmp = DoGetBitmap(id, client, reqSize_);
        reqSize = reqSize_;
    }

#if wxUSE_IMAGE
    if (bmp.IsOk())
    {
        // fit into transparent image with desired size hint from the client
        if (reqSize == wxDefaultSize)
        {
            // find out if there is a desired size for this client
            wxSize bestSize = GetSizeHint(client);
            if (bestSize != wxDefaultSize)
            {
                int bmp_w = bmp.GetWidth();
                int bmp_h = bmp.GetHeight();
                // want default size but it's smaller, paste into transparent image
                if ((bmp_h < bestSize.x) && (bmp_w < bestSize.y))
                {
                    wxPoint offset((bestSize.x - bmp_w)/2, (bestSize.y - bmp_h)/2);
                    wxImage img = bmp.ConvertToImage();
                    img.Resize(bestSize, offset);
                    bmp = wxBitmap(img);
                }
            }
        }
    }
#endif // wxUSE_IMAGE

    return bmp;
}

/*static*/ wxIcon wxSTEditorArtProvider::GetDefaultDialogIcon()
{
    return GetIcon(wxART_STEDIT_APP, wxDialogIconSize);
}
