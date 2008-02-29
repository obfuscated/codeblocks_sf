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
#include <wx/image.h>	// wxImage
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#include "licenses.h"
#include "configmanager.h"
#endif
#include <wx/bitmap.h>
#include <wx/dcmemory.h>	// wxMemoryDC
#include <wx/statbmp.h>
#include "appglobals.h"
#include "dlgabout.h" // class's header file
#include "configmanager.h"

// class constructor

dlgAbout::dlgAbout(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgAbout"));

    const wxString description = _("Welcome to ") + appglobals::AppName + _T(" ") +
                                 appglobals::AppVersion + _T("!\n") + appglobals::AppName +
					             _(" is a full-featured IDE (Integrated Development Environment) "
					"aiming to make the individual developer (and the development team) "
					"work in a nice programming environment offering everything he/they "
					"would ever need from a program of that kind.\n"
					"Its pluggable architecture allows you, the developer, to add "
					"any kind of functionality to the core program, through the use of "
					"plugins...\n");

	wxString file = ConfigManager::ReadDataPath() + _T("/images/splash_new.png");


	wxStaticBitmap *bmpControl = XRCCTRL(*this, "lblTitle", wxStaticBitmap);

    wxImage im;
    im.LoadFile(file, wxBITMAP_TYPE_PNG );
    im.ConvertAlphaToMask();
	wxBitmap bmp(im);

    {   // copied from splashscreen.cpp
        const wxString release(wxT(RELEASE));
        const wxString revision = ConfigManager::GetRevisionString();

        wxMemoryDC dc;
        dc.SelectObject(bmp);

        wxFont largeFont(16, wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont smallFont(9,  wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

        wxCoord a, b, c, d;

        dc.GetTextExtent(release,  &a, &b, 0, 0, &largeFont);
        dc.GetTextExtent(revision, &c, &d, 0, 0, &smallFont);

        a >>= 1; c >>=1;
        int y = 180 - ((b + d + 8)>>1);

        dc.SetFont(largeFont);
        dc.DrawText(release,  92 - a, y);
        dc.SetFont(smallFont);
        dc.DrawText(revision, 92 - c, y + b);
    }

	bmpControl->SetBitmap(bmp);



	XRCCTRL(*this, "lblBuildTimestamp", wxStaticText)->SetLabel(wxString(_("Build: ")) + appglobals::AppBuildTimestamp);
	XRCCTRL(*this, "txtDescription", wxTextCtrl)->SetValue(description);
	XRCCTRL(*this, "txtThanksTo", wxTextCtrl)->SetValue(_(
        "Developers:\n"
        "--------------\n"
        "Yiannis Mandravellos: Developer - Project leader\n"
        "Thomas Denk         : Developer\n"
        "Lieven de Cock      : Developer\n"
        "Martin Halle        : Developer\n"
        "Biplab Modak        : Developer\n"
        "Paul A. Jimenez     : Help and AStyle plugins\n"
        "Bartlomiej Swiecki  : wxSmith RAD plugin\n"
        "Jerome Antoine      : ThreadSearch plugin\n"
        "Mario Cupelli       : User's manual\n"
        "\"tiwag\"             : Developer\n"
        "Pecan Heber         : Keybinder, BrowseTracker, DragScroll\n"
        "                      CodeSnippets plugins\n"
        "Arto Jonsson        : CodeSnippets plugin (passed on to Pecan)\n"
        "Thomas Lorblanchès  : CodeStat and Profiler plugins\n"
        "Ricardo Garcia      : All-hands person\n"
        "Anders F Björklund  : wxMac compatibility\n"
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
        "Last, but not least, the open-source community."
        ));
	XRCCTRL(*this, "txtLicense", wxTextCtrl)->SetValue(LICENSE_GPL);
	XRCCTRL(*this, "lblName", wxStaticText)->SetLabel(appglobals::AppName);
	XRCCTRL(*this, "lblVersion", wxStaticText)->SetLabel(appglobals::AppActualVersionVerb);
	XRCCTRL(*this, "lblAuthor", wxStaticText)->SetLabel(_("The Code::Blocks Team"));
	XRCCTRL(*this, "lblEmail", wxStaticText)->SetLabel(appglobals::AppContactEmail);
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
}

// class destructor
dlgAbout::~dlgAbout()
{
	// insert your code here
}
