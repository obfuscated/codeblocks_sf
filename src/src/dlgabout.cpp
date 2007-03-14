/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
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
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#include "licenses.h"
#include "configmanager.h"
#endif
#include <wx/bitmap.h>
#include <wx/statbmp.h>
#include "appglobals.h"
#include "dlgabout.h" // class's header file
#include "autorevision.h"

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
        const wxString revision(wxT(SVN_REVISION));

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
        "Yiannis Mandravellos: Project leader - Core developer\n"
        "Thomas Denk         : Core developer\n"
        "Ricardo Garcia      : All-hands person\n"
        "\"tiwag\"             : Developer\n"
        "\n"
        "Contributors (in no special order):\n"
        "-----------------------------------\n"
        "byo,elvstone, me22  : Conversion to Unicode\n"
        "byo, cyberkoa       : wxSmith RAD plugin\n"
        "P. A. Jimenez       : Help and AStyle plugins\n"
        "Hakki Dogusan       : DigitalMars compiler support\n"
        "ybx                 : OpenWatcom compiler support\n"
        "Tim Baker           : Patches for the direct-compile-mode\n"
        "                      dependencies generation system\n"
        "Eric Burnett        : GDB debugger plugin patches\n"
        "Chris Raschko       : Design of the 3D logo for Code::Blocks\n"
        "J.A. Ortega         : 3D Icon based on the above\n"
        "\n"
        "The wxWidgets project (http://www.wxwidgets.org).\n"
        "TinyXML parser (http://www.grinninglizard.com/tinyxml).\n"
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
