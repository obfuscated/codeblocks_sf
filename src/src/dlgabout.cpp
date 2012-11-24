/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #ifdef __WXMAC__
        #include <wx/font.h>
    #endif //__WXMAC__
    #include <wx/image.h>    // wxImage
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/string.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>

    #include "licenses.h"
    #include "configmanager.h"
#endif

#include <wx/bitmap.h>
#include <wx/dcmemory.h>    // wxMemoryDC
#include <wx/statbmp.h>

#include "appglobals.h"
#include "dlgabout.h" // class's header file
#include "configmanager.h"

// class constructor

dlgAbout::dlgAbout(wxWindow* parent)
{
    if (!wxXmlResource::Get()->LoadObject(this, parent, _T("dlgAbout"), _T("wxScrollingDialog")))
    {
        cbMessageBox(_("There was an error loading the \"About\" dialog from XRC file."),
                     _("Information"), wxICON_EXCLAMATION);
        return;
    }

    const wxString description = _("Welcome to ") + appglobals::AppName + _T(" ") +
                                 appglobals::AppVersion + _T("!\n") + appglobals::AppName +
                                 _(" is a full-featured IDE (Integrated Development Environment) "
                                   "aiming to make the individual developer (and the development team) "
                                   "work in a nice programming environment offering everything he/they "
                                   "would ever need from a program of that kind.\n"
                                   "Its pluggable architecture allows you, the developer, to add "
                                   "any kind of functionality to the core program, through the use of "
                                   "plugins...\n");

    wxString file = ConfigManager::ReadDataPath() + _T("/images/splash_1005.png");
    wxImage im; im.LoadFile(file, wxBITMAP_TYPE_PNG); im.ConvertAlphaToMask();
    wxBitmap bmp(im);
    wxMemoryDC dc;
    dc.SelectObject(bmp);

    {  // keep this (kind of) in sync with splashscreen.cpp!
        static const wxString release(wxT(RELEASE));
        static const wxString revision = ConfigManager::GetRevisionString();

        wxFont largeFont(16, wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont smallFont(9,  wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

        wxCoord lf_width, lf_heigth, sf_width, sf_heigth, sm_width, sm_height;

        dc.GetTextExtent(release,  &lf_width, &lf_heigth, 0, 0, &largeFont);
        dc.GetTextExtent(revision, &sf_width, &sf_heigth, 0, 0, &smallFont);

        dc.GetTextExtent(_("SAFE MODE"), &sm_width, &sm_height, 0, 0, &largeFont);

        int x_offset = 310;
        int y_offset = 150;

        lf_width >>= 1;
        sf_width >>= 1;
        int y      = y_offset - ((lf_heigth + sf_heigth + 8) >> 1);

        dc.SetTextForeground(*wxBLACK);

        dc.SetFont(largeFont);
#if SVN_BUILD
        dc.DrawText(release,  x_offset - lf_width, y);
        // only render SVN revision when not building official release
        dc.SetFont(smallFont);
        dc.DrawText(revision, x_offset - sf_width, y +  lf_heigth);
#else
        dc.DrawText(release,  x_offset - lf_width, y + (lf_heigth >> 1));
#endif
    }

    wxStaticBitmap *bmpControl = XRCCTRL(*this, "lblTitle", wxStaticBitmap);
    bmpControl->SetSize(im.GetWidth(),im.GetHeight());
    bmpControl->SetBitmap(bmp);

    XRCCTRL(*this, "lblBuildTimestamp", wxStaticText)->SetLabel(wxString(_("Build: ")) + appglobals::AppBuildTimestamp);
    XRCCTRL(*this, "txtDescription",    wxTextCtrl)->SetValue(description);
    XRCCTRL(*this, "txtThanksTo",       wxTextCtrl)->SetValue(_(
        "Developers:\n"
        "--------------\n"
        "Yiannis Mandravellos: Developer - Project leader\n"
        "Thomas Denk         : Developer\n"
        "Lieven de Cock      : Developer\n"
        "\"tiwag\"             : Developer\n"
        "Martin Halle        : Developer\n"
        "Biplab Modak        : Developer\n"
        "Jens Lody           : Developer\n"
        "Yuchen Deng         : Developer\n"
        "Teodor Petrov       : Developer\n"
        "Daniel Anselmi      : Developer\n"
        "Yuanhui Zhang       : Developer\n"
        "Damien Moore        : Developer\n"
        "Ricardo Garcia      : All-hands person\n"
        "Paul A. Jimenez     : Help and AStyle plugins\n"
        "Thomas Lorblanches  : CodeStat and Profiler plugins\n"
        "Bartlomiej Swiecki  : wxSmith RAD plugin\n"
        "Jerome Antoine      : ThreadSearch plugin\n"
        "Pecan Heber         : Keybinder, BrowseTracker, DragScroll\n"
        "                      CodeSnippets plugins\n"
        "Arto Jonsson        : CodeSnippets plugin (passed on to Pecan)\n"
        "Mario Cupelli       : User's manual\n"
        "Anders F Bjoerklund : wxMac compatibility\n"
        "\n"
        "Contributors (in no special order):\n"
        "-----------------------------------\n"
        "Daniel Orb          : RPM spec file and packages\n"
        "Mario Cupelli       : Compiler support for embedded systems\n"
        "byo,elvstone, me22  : Conversion to Unicode\n"
        "pasgui              : Providing linux nightly packages\n"
        "Hakki Dogusan       : DigitalMars compiler support\n"
        "ybx                 : OpenWatcom compiler support\n"
        "Tim Baker           : Patches for the direct-compile-mode\n"
        "                      dependencies generation system\n"
        "David Perfors       : Unicode tester and future documentation writer\n"
        "Sylvain Prat        : Initial MSVC workspace and project importers\n"
        "Chris Raschko       : Design of the 3D logo for Code::Blocks\n"
        "J.A. Ortega         : 3D Icon based on the above\n"
        "\n"
        "All contributors that provided patches.\n"
        "The wxWidgets project (http://www.wxwidgets.org).\n"
        "wxScintilla (http://sourceforge.net/projects/wxscintilla).\n"
        "TinyXML parser (http://www.grinninglizard.com/tinyxml).\n"
        "Squirrel scripting language (http://www.squirrel-lang.org).\n"
        "The GNU Software Foundation (http://www.gnu.org).\n"
        "Last, but not least, the open-source community."));
    XRCCTRL(*this, "txtLicense", wxTextCtrl)->SetValue(LICENSE_GPL);

    XRCCTRL(*this, "lblName",    wxStaticText)->SetLabel(appglobals::AppName);
    XRCCTRL(*this, "lblVersion", wxStaticText)->SetLabel(appglobals::AppActualVersionVerb);
    XRCCTRL(*this, "lblSDK",     wxStaticText)->SetLabel(appglobals::AppSDKVersion);
    XRCCTRL(*this, "lblAuthor",  wxStaticText)->SetLabel(_("The Code::Blocks Team"));
    XRCCTRL(*this, "lblEmail",   wxStaticText)->SetLabel(appglobals::AppContactEmail);
    XRCCTRL(*this, "lblWebsite", wxStaticText)->SetLabel(appglobals::AppUrl);

#ifdef __WXMAC__
    // Courier 8 point is not readable on Mac OS X, increase font size:
    wxFont font1 = XRCCTRL(*this, "txtThanksTo", wxTextCtrl)->GetFont();
    font1.SetPointSize(10);
    XRCCTRL(*this, "txtThanksTo", wxTextCtrl)->SetFont(font1);

    wxFont font2 = XRCCTRL(*this, "txtLicense", wxTextCtrl)->GetFont();
    font2.SetPointSize(10);
    XRCCTRL(*this, "txtLicense", wxTextCtrl)->SetFont(font2);
#endif
    Fit();
    CentreOnParent();
}

// class destructor
dlgAbout::~dlgAbout()
{
    // insert your code here
}
