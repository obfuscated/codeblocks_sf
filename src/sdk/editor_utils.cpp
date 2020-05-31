/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#include "sdk_precomp.h"
#include "editor_utils.h"
#ifndef CB_PRECOMP
    #include <wx/wxscintilla.h>
#endif // CB_PRECOMP

namespace cb
{

static void SetMarkerStyle(wxScintilla *stc, int marker, int markerType, wxColor fore, wxColor back)
{
    stc->MarkerDefine(marker, markerType);
    stc->MarkerSetForeground(marker, fore);
    stc->MarkerSetBackground(marker, back);
}

void SetFoldingMarkers(wxScintilla *stc, int id)
{
    wxColor f(0xff, 0xff, 0xff); // foreground colour
    wxColor b(0x80, 0x80, 0x80); // background colour
    // Arrow
    if (id == 0)
    {
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_ARROWDOWN,  f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_ARROW,      f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_ARROW,      f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_ARROWDOWN,  f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, f, b);
    }
    // Circle
    else if (id == 1)
    {
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_CIRCLEMINUS,          f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_CIRCLEPLUS,           f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_VLINE,                f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_LCORNERCURVE,         f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_CIRCLEPLUSCONNECTED,  f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_CIRCLEMINUSCONNECTED, f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER,              f, b);
    }
    // Square
    else if (id == 2)
    {
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_BOXMINUS,          f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_BOXPLUS,           f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_VLINE,             f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_LCORNER,           f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_BOXPLUSCONNECTED,  f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_BOXMINUSCONNECTED, f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_TCORNER,           f, b);
    }
    // Simple
    else if (id == 3)
    {
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPEN,    wxSCI_MARK_MINUS,      f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDER,        wxSCI_MARK_PLUS,       f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERSUB,     wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERTAIL,    wxSCI_MARK_BACKGROUND, f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEREND,     wxSCI_MARK_PLUS,       f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDEROPENMID, wxSCI_MARK_MINUS,      f, b);
        SetMarkerStyle(stc, wxSCI_MARKNUM_FOLDERMIDTAIL, wxSCI_MARK_BACKGROUND, f, b);
    }
}

void UnderlineFoldedLines(wxScintilla *stc, bool underline)
{
    stc->SetFoldFlags(underline ? wxSCI_FOLDFLAG_LINEAFTER_CONTRACTED : 0);
}

} // namespace cb
