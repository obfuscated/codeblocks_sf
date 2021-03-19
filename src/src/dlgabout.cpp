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
    #include <wx/button.h>    // wxImage
    #include <wx/image.h>    // wxImage
    #include <wx/intl.h>
    #include <wx/stattext.h>
    #include <wx/string.h>
    #include <wx/textctrl.h>
    #include <wx/xrc/xmlres.h>
    #if wxCHECK_VERSION(3, 0, 0)
        #include <wx/versioninfo.h>
    #endif // wxCHECK_VERSION

    #include "licenses.h"
    #include "configmanager.h"
    #include "wx/wxscintilla.h"
#endif

#include <wx/bitmap.h>
#include <wx/dcmemory.h>    // wxMemoryDC
#include <wx/display.h>
#include <wx/statbmp.h>

#include "appglobals.h"
#include "dlgabout.h" // class's header file
#include "configmanager.h"
#include "splashscreen.h"

// class constructor

dlgAbout::dlgAbout(wxWindow* parent)
{
    if (!wxXmlResource::Get()->LoadObject(this, parent, _T("dlgAbout"), _T("wxScrollingDialog")))
    {
        cbMessageBox(_("There was an error loading the \"About\" dialog from XRC file."),
                     _("Information"), wxICON_EXCLAMATION);
        return;
    }

    wxButton *cancelButton = XRCCTRL(*this, "wxID_CANCEL", wxButton);
    cancelButton->SetDefault();
    cancelButton->SetFocus();

    const wxString description = _("Welcome to ") + appglobals::AppName + _T(" ") +
                                 appglobals::AppVersion + _T("!\n") + appglobals::AppName +
                                 _(" is a full-featured IDE (Integrated Development Environment) "
                                   "aiming to make the individual developer (and the development team) "
                                   "work in a nice programming environment offering everything he/they "
                                   "would ever need from a program of that kind.\n"
                                   "Its pluggable architecture allows you, the developer, to add "
                                   "any kind of functionality to the core program, through the use of "
                                   "plugins...\n");

    wxString file = ConfigManager::ReadDataPath() + _T("/images/splash_1312.png");

    wxImage im;
    im.LoadFile(file, wxBITMAP_TYPE_PNG);
    im.ConvertAlphaToMask();

    wxBitmap bmp(im);
    wxMemoryDC dc;
    dc.SelectObject(bmp);
    cbSplashScreen::DrawReleaseInfo(dc);

    wxStaticBitmap *bmpControl = XRCCTRL(*this, "lblTitle", wxStaticBitmap);
    bmpControl->SetSize(im.GetWidth(),im.GetHeight());
    bmpControl->SetBitmap(bmp);

    XRCCTRL(*this, "lblBuildTimestamp", wxStaticText)->SetLabel(wxString(_("Build: ")) + appglobals::AppBuildTimestamp);

    wxTextCtrl *txtDescription = XRCCTRL(*this, "txtDescription", wxTextCtrl);
    txtDescription->SetValue(description);

    wxTextCtrl *txtThanksTo = XRCCTRL(*this, "txtThanksTo", wxTextCtrl);
    // Note: Keep this is sync with the AUTHORS file in SVN.
    txtThanksTo->SetValue(_(
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
        "Micah Ng            : Developer\n"
        "BlueHazzard         : Developer\n"
        "Ricardo Garcia      : All-hands person\n"
        "Paul A. Jimenez     : Help and AStyle plugins\n"
        "Thomas Lorblanches  : CodeStat and Profiler plugins\n"
        "Bartlomiej Swiecki  : wxSmith RAD plugin\n"
        "Jerome Antoine      : ThreadSearch plugin\n"
        "Pecan Heber         : Keybinder, BrowseTracker, DragScroll\n"
        "                      CodeSnippets plugins\n"
        "Arto Jonsson        : CodeSnippets plugin (passed on to Pecan)\n"
        "Darius Markauskas   : Fortran support\n"
        "Mario Cupelli       : Compiler support for embedded systems\n"
        "                      User's manual\n"
        "Jonas Zinn          : Misc. wxSmith AddOns and plugins\n"
        "Mirai Computing     : cbp2make tool\n"
        "Anders F Bjoerklund : wxMac compatibility\n"
        "\n"
        "Contributors (in no special order):\n"
        "-----------------------------------\n"
        "Daniel Orb          : RPM spec file and packages\n"
        "byo,elvstone, me22  : Conversion to Unicode\n"
        "pasgui              : Providing Ubuntu nightly packages\n"
        "Hakki Dogusan       : DigitalMars compiler support\n"
        "ybx                 : OpenWatcom compiler support\n"
        "Tim Baker           : Patches for the direct-compile-mode\n"
        "                      dependencies generation system\n"
        "David Perfors       : Unicode tester and future documentation writer\n"
        "Sylvain Prat        : Initial MSVC workspace and project importers\n"
        "Chris Raschko       : Design of the 3D logo for Code::Blocks\n"
        "J.A. Ortega         : 3D Icon based on the above\n"
        "Alexandr Efremo     : Providing OpenSuSe packages\n"
        "Huki                : Misc. Code-Completion improvements\n"
        "stahta01            : Misc. patches for several enhancements\n"
        "Miguel Gimenez      : Misc. patches for several enhancements\n"
        "\n"
        "All contributors that provided patches.\n"
        "The wxWidgets project (http://www.wxwidgets.org).\n"
        "wxScintilla (http://sourceforge.net/projects/wxscintilla).\n"
        "TinyXML parser (http://www.grinninglizard.com/tinyxml).\n"
        "Squirrel scripting language (http://www.squirrel-lang.org).\n"
        "The GNU Software Foundation (http://www.gnu.org).\n"
        "Last, but not least, the open-source community."));
    wxTextCtrl *txtLicense = XRCCTRL(*this, "txtLicense", wxTextCtrl);
    txtLicense->SetValue(LICENSE_GPL);

#if wxCHECK_VERSION(3, 0, 0)
    const wxVersionInfo scintillaVersion = wxScintilla::GetLibraryVersionInfo();
    const wxString scintillaStr = wxString::Format(wxT("%d.%d.%d"),
                                                   scintillaVersion.GetMajor(),
                                                   scintillaVersion.GetMinor(),
                                                   scintillaVersion.GetMicro());
#else
    const wxString scintillaStr = wxSCINTILLA_VERSION;
#endif // wxCHECK_VERSION

    struct Item
    {
        wxString name, value;
    };
    std::vector<Item> items;
    items.push_back({_("Name"), appglobals::AppName});
    items.push_back({_("Version"), appglobals::AppActualVersion});
    items.push_back({_("SDK Version"), appglobals::AppSDKVersion});
    items.push_back({_("Scintilla Version"), scintillaStr});

    items.push_back({_("Author"), _("The Code::Blocks Team")});
    items.push_back({_("E-mail"), appglobals::AppContactEmail});
    items.push_back({_("Website"), appglobals::AppUrl});

    const wxPlatformInfo &platform = wxPlatformInfo::Get();

    items.push_back({_("OS"), platform.GetOperatingSystemDescription()});
    const wxString desktopEnv = platform.GetDesktopEnvironment();
    if (!desktopEnv.empty())
        items.push_back({_("Desktop environment"), desktopEnv });

    items.push_back({_("Scaling factor"), wxString::Format("%f", GetContentScaleFactor())});
    items.push_back({_("Detected scaling factor"),
                     wxString::Format("%f", cbGetActualContentScaleFactor(*this))});
    const wxSize displayPPI = wxGetDisplayPPI();
    items.push_back({_("Display PPI"), wxString::Format("%dx%d", displayPPI.x, displayPPI.y)});

    unsigned displays = wxDisplay::GetCount();
    items.push_back({_("Display count"), wxString::Format("%u", displays)});

    for (unsigned ii = 0; ii < displays; ++ii)
    {
        wxDisplay display(ii);

        Item item;
        item.name = wxString::Format(_("Display %u"), ii);

        const wxString &name = display.GetName();
        if (!name.empty())
            item.name += " (" + name + ")";

        const wxRect geometry = display.GetGeometry();
        item.value= wxString::Format(_("XY=[%d,%d]; Size=[%d,%d]; %s"), geometry.GetLeft(),
                                     geometry.GetTop(), geometry.GetWidth(), geometry.GetHeight(),
                                     (display.IsPrimary() ? _("Primary") : wxString()));
        items.push_back(item);
    }

    int maxNameLength = 0;
    for (const Item &item : items)
    {
        maxNameLength = std::max(maxNameLength, int(item.name.length()));
    }

    wxString information;
    for (const Item &item : items)
    {
        information += item.name;
        information += wxString(wxT(' '), maxNameLength - int(item.name.length()));
        information += wxT(": ") + item.value + wxT("\n");
    }

#if wxCHECK_VERSION(3, 0, 0)
    information += wxT("\n") + wxGetLibraryVersionInfo().GetDescription();
#endif // wxCHECK_VERSION(3, 0, 0)

    wxTextCtrl *txtInformation = XRCCTRL(*this, "txtInformation", wxTextCtrl);
    txtInformation->SetValue(information);

#ifdef __WXMAC__
    // Courier 8 point is not readable on Mac OS X, increase font size:
    wxFont font1 = txtThanksTo->GetFont();
    font1.SetPointSize(10);
    txtThanksTo->SetFont(font1);

    wxFont font2 = txtLicense->GetFont();
    font2.SetPointSize(10);
    txtLicense->SetFont(font2);

    wxFont font3 = txtInformation->GetFont();
    font3.SetPointSize(10);
    txtInformation->SetFont(font3);
#endif

    Fit();
    CentreOnParent();
}

// class destructor
dlgAbout::~dlgAbout()
{
    // insert your code here
}
