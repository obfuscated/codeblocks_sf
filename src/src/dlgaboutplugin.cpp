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
#include <wx/intl.h>
#include <wx/stattext.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/xrc/xmlres.h>
#endif

#include "cbplugin.h"
#include "dlgaboutplugin.h" // class's header file

// class constructor
dlgAboutPlugin::dlgAboutPlugin(wxWindow* parent, const PluginInfo* pi)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgAboutPlugin"));

	XRCCTRL(*this, "lblTitle", wxStaticText)->SetLabel(pi->title);
	XRCCTRL(*this, "txtDescription", wxTextCtrl)->SetValue(pi->description);
	XRCCTRL(*this, "txtThanksTo", wxTextCtrl)->SetValue(pi->thanksTo);
	XRCCTRL(*this, "txtLicense", wxTextCtrl)->SetValue(pi->license);
	XRCCTRL(*this, "lblName", wxStaticText)->SetLabel(pi->name);
	XRCCTRL(*this, "lblVersion", wxStaticText)->SetLabel(pi->version);
	XRCCTRL(*this, "lblAuthor", wxStaticText)->SetLabel(pi->author);
	XRCCTRL(*this, "lblEmail", wxStaticText)->SetLabel(pi->authorEmail);
	XRCCTRL(*this, "lblWebsite", wxStaticText)->SetLabel(pi->authorWebsite);
}

// class destructor
dlgAboutPlugin::~dlgAboutPlugin()
{
	// insert your code here
}
