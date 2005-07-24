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
* $Id$
* $Date$
*/

#include "dlgabout.h" // class's header file
#include "globals.h"
#include "../sdk/licenses.h"
#include "../sdk/configmanager.h"

#define DESCR		"Welcome to "APP_NAME" v"APP_VERSION"!\n" \
					APP_NAME" is a full-featured IDE (Integrated Development Environment) " \
					"aiming to make the individual developer (and the development team) " \
					"work in a nice programming environment offering everything he/they " \
					"would ever need from a program of that kind.\n" \
					"Its pluggable architecture allows you, the developer, to add " \
					"any kind of functionality to the core program, through the use of " \
					"plugins...\n"

// class constructor
dlgAbout::dlgAbout(wxWindow* parent)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _("dlgAbout"));

	wxBitmap bmp;
	wxString file = ConfigManager::Get()->Read("data_path") + "/images/splash.png"; 
	bmp.LoadFile(file, wxBITMAP_TYPE_PNG);
	XRCCTRL(*this, "lblTitle", wxStaticBitmap)->SetBitmap(bmp);
	XRCCTRL(*this, "lblBuildTimestamp", wxStaticText)->SetLabel("Build: "APP_BUILD_TIMESTAMP);
	XRCCTRL(*this, "txtDescription", wxTextCtrl)->SetValue(DESCR);
	XRCCTRL(*this, "txtThanksTo", wxTextCtrl)->SetValue(
        _("The wxWidgets project (http://www.wxwidgets.org)\n"
        "TinyXML parser (http://http://www.grinninglizard.com/tinyxml)\n"
        "The GNU Software Foundation (http://www.gnu.org)\n"
        "Last, but not least, the open-source community\n\n"
        "Co-developers:\n"
        "--------------\n"
        "Ricardo Garcia: All-hands person\n"
        "\n"
        "Contributors (in no special order):\n"
        "-----------------------------------\n"
        "byo, cyberkoa : wxSmith RAD plugin (WIP)\n"
        "P. A. Jimenez : Help and AStyle plugins\n"
        "Hakki Dogusan : DigitalMars compiler support\n"
        "ybx           : OpenWatcom compiler support\n"
        "Tim Baker     : Patches for the direct-compile-mode\n"
        "                dependencies generation system\n"
        "Eric Burnett  : GDB debugger plugin patches\n"
        "Chris Raschko : Design of the 3D logo for Code::Blocks\n"
        "J.A. Ortega   : 3D Icon based on the above"
        ));
	XRCCTRL(*this, "txtLicense", wxTextCtrl)->SetValue(LICENSE_GPL);
	XRCCTRL(*this, "lblName", wxStaticText)->SetLabel(APP_NAME);
	XRCCTRL(*this, "lblVersion", wxStaticText)->SetLabel(APP_ACTUAL_VERSION);
	XRCCTRL(*this, "lblAuthor", wxStaticText)->SetLabel("Yiannis An. Mandravellos");
	XRCCTRL(*this, "lblEmail", wxStaticText)->SetLabel(APP_CONTACT_EMAIL);
	XRCCTRL(*this, "lblWebsite", wxStaticText)->SetLabel(APP_URL);
}

// class destructor
dlgAbout::~dlgAbout()
{
	// insert your code here
}
