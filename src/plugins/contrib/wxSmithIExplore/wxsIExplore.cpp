/*
* This file is part of a wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* wxSmith and this file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
* Ron Collins
* rcoll@theriver.com
* 4-Feb-2010
*
*/

#include "wxsIExplore.h"

#include "IEHtmlWin.h"

//------------------------------------------------------------------------------

namespace
{
// Loading images from xpm files
    #include "IE16.xpm"
    #include "IE32.xpm"

    wxsRegisterItem<wxsIExplore> Reg(
        _T("wxIEHtmlWin"),              // Class name
        wxsTWidget,                     // Item type
        _T("wxWindows"),                // License
        _T("Ron Collins"),              // Author
        _T("rcoll@theriver.com"),       // Author's email
        _T(""),                         // Item's homepage
        _T("IExplore"),                 // Category in palette
        90,                             // Priority in palette
        _T("IE"),                       // Base part of names for new items
        wxsCPP,                         // List of coding languages supported by this item
        1, 0,                           // Version
        wxBitmap(IE32_xpm),             // 32x32 bitmap
        wxBitmap(IE16_xpm),             // 16x16 bitmap
        false);                         // We do not allow this item inside XRC files

    WXS_ST_BEGIN(wxsIExploreStyles,_T("wxRAISED_BORDER|wxTAB_TRAVERSAL"))
        WXS_ST_CATEGORY("wxIEHtmlWin")
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsIExploreEvents)
        WXS_EVI(EVT_MSHTML_BEFORENAVIGATE2,  wxEVT_COMMAND_MSHTML_BEFORENAVIGATE2,  wxMSHTMLEvent, BeforeNavigate2)
        WXS_EVI(EVT_MSHTML_NEWWINDOW2,       wxEVT_COMMAND_MSHTML_NEWWINDOW2,       wxMSHTMLEvent, NewWindow2)
        WXS_EVI(EVT_MSHTML_DOCUMENTCOMPLETE, wxEVT_COMMAND_MSHTML_DOCUMENTCOMPLETE, wxMSHTMLEvent, DocumentComplete)
        WXS_EVI(EVT_MSHTML_PROGRESSCHANGE,   wxEVT_COMMAND_MSHTML_PROGRESSCHANGE,   wxMSHTMLEvent, ProgressChange)
        WXS_EVI(EVT_MSHTML_STATUSTEXTCHANGE, wxEVT_COMMAND_MSHTML_STATUSTEXTCHANGE, wxMSHTMLEvent, StatusTextChange)
        WXS_EVI(EVT_MSHTML_TITLECHANGE,      wxEVT_COMMAND_MSHTML_TITLECHANGE,      wxMSHTMLEvent, TitleChange)
    WXS_EV_END()
}

//------------------------------------------------------------------------------

wxsIExplore::wxsIExplore(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsIExploreEvents,
        wxsIExploreStyles)
{
// start with a clean slate
    mStartPage = _T("");
}

//------------------------------------------------------------------------------

void wxsIExplore::OnBuildCreatingCode()
{
// valid language?
    if (GetLanguage() != wxsCPP) wxsCodeMarks::Unknown(_T("wxsIExplore::OnBuildCreatingCode"),GetLanguage());

// who we are
    wxString vname = GetVarName(); // name of this var

// include files
    AddHeader(_("<IEHtmlWin.h>"), GetInfo().ClassName, 0);

// create the explorer
    Codef(_T("%C(%W, %I, %P, %S, %T, %N);\n"));
    BuildSetupWindowCode();

// start page (if given) must have a protocol
// if none given, use "http://"
    if ( !mStartPage.IsEmpty())
    {
        int i = mStartPage.Find(_T("//"));
        if (i == wxNOT_FOUND) mStartPage = _T("http://") + mStartPage;
        Codef(_T("%s->LoadUrl(%t);\n"), vname.c_str(), mStartPage.c_str());
    }
}



//------------------------------------------------------------------------------

wxObject* wxsIExplore::OnBuildPreview(wxWindow* Parent, long Flags)
{
// make it
    wxIEHtmlWin *ie = new wxIEHtmlWin(Parent, GetId(), Pos(Parent), Size(Parent), Style(), _T("IExplore"));
    SetupWindow(ie, Flags);

// start page
    if (! mStartPage.IsEmpty()) {
        int i = mStartPage.Find(_T("//"));
        if (i == wxNOT_FOUND) mStartPage = _T("http://") + mStartPage;
        if ((Flags & pfExact) != 0) ie->LoadUrl(mStartPage);
    };

// done
    return ie;
}

//------------------------------------------------------------------------------

void wxsIExplore::OnEnumWidgetProperties(long Flags)
{
    WXS_STRING(wxsIExplore, mStartPage,      _("Start Page"),               _T("startpage"),      _T(""), false);
};



