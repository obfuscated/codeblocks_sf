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
#include <sdk.h>
#ifndef CB_PRECOMP
#include <wx/arrstr.h>
#include <wx/file.h>
#include <wx/filename.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/string.h>
#include "cbproject.h"
#include "globals.h"
#include "licenses.h"
#include "manager.h"
#include "messagemanager.h"
#include "projectbuildtarget.h"
#include "projectmanager.h"
#endif
#include "windowsxplooknfeel.h"

namespace
{
    PluginRegistrant<WindowsXPLookNFeel> reg(_T("WindowsXPLookNFeel"));
}

WindowsXPLookNFeel::WindowsXPLookNFeel()
{
	//ctor
}

WindowsXPLookNFeel::~WindowsXPLookNFeel()
{
	//dtor
}

void WindowsXPLookNFeel::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void WindowsXPLookNFeel::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// NOTE: after this function, the inherited member variable
	// IsAttached() will be FALSE...
}

int WindowsXPLookNFeel::Execute()
{
	if (!IsAttached())
		return -1;

	cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
	if (!project)
	{
		wxString msg = _("No active project!");
		cbMessageBox(msg, _("Error"), wxICON_ERROR | wxOK);
		Manager::Get()->GetMessageManager()->DebugLog(msg);
		return -1;
	}

	wxArrayString targetNames;
	ProjectBuildTarget* target = 0L;
	for (int i = 0; i < project->GetBuildTargetsCount(); ++i)
	{
		ProjectBuildTarget* tgt = project->GetBuildTarget(i);
		if (tgt)
		{
			if (tgt->GetTargetType() != ttExecutable)
			{
				Manager::Get()->GetMessageManager()->DebugLog(_T("WindowsXPLookNFeel: Ignoring target '%s'"), tgt->GetTitle().c_str());
				continue;
			}
			targetNames.Add(tgt->GetTitle());
			target = tgt;
		}
	}

	if (!target)
	{
		// not even one executable target...
		Manager::Get()->GetMessageManager()->DebugLog(_T("WindowsXPLookNFeel: No executable targets in project"));
		return -1;
	}
	else if (targetNames.GetCount() > 1)
	{
		// more than one executable target... ask...
		target = 0L;
		int targetIndex = project->SelectTarget(-1, true);
		if (targetIndex > -1)
			target = project->GetBuildTarget(targetIndex);
	}


	if (target)
	{
		if (cbMessageBox(_("Do you want to create the manifest file?"),
						_("Confirmation"),
						wxYES_NO | wxICON_QUESTION) == wxID_NO)
			return -2;
		wxString filename = target->GetOutputFilename();
		filename << _T(".Manifest");
		wxFileName fname(filename);
		fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, project->GetBasePath());
		filename = fname.GetFullPath();
		Manager::Get()->GetMessageManager()->DebugLog(_T("WindowsXPLookNFeel: Creating Manifest '%s'"), filename.c_str());

		wxString buffer;
		buffer << _T("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>") << _T('\n');
		buffer << _T("<assembly") << _T('\n');
		buffer << _T("  xmlns=\"urn:schemas-microsoft-com:asm.v1\"") << _T('\n');
		buffer << _T("  manifestVersion=\"1.0\">") << _T('\n');
		buffer << _T("<assemblyIdentity") << _T('\n');
		buffer << _T("    name=\"");
		buffer << project->GetTitle() << _T(".") << target->GetTitle() << _T(".App");
		buffer << _T("\"") << _T('\n');
		buffer << _T("    processorArchitecture=\"x86\"") << _T('\n');
		buffer << _T("    version=\"1.0.0.0\"") << _T('\n');
		buffer << _T("    type=\"win32\"/>") << _T('\n');
		buffer << _T("<description>Executable</description>") << _T('\n');
		buffer << _T("<dependency>") << _T('\n');
		buffer << _T("    <dependentAssembly>") << _T('\n');
		buffer << _T("        <assemblyIdentity") << _T('\n');
		buffer << _T("            type=\"win32\"") << _T('\n');
		buffer << _T("            name=\"Microsoft.Windows.Common-Controls\"") << _T('\n');
		buffer << _T("            version=\"6.0.0.0\"") << _T('\n');
		buffer << _T("            processorArchitecture=\"x86\"") << _T('\n');
		buffer << _T("            publicKeyToken=\"6595b64144ccf1df\"") << _T('\n');
		buffer << _T("            language=\"*\"") << _T('\n');
		buffer << _T("        />") << _T('\n');
		buffer << _T("    </dependentAssembly>") << _T('\n');
		buffer << _T("</dependency>") << _T('\n');
		buffer << _T("</assembly>") << _T('\n');

		wxFile file(filename, wxFile::write);
		cbWrite(file,buffer);

		cbMessageBox(_("Manifest file created"), _("Information"), wxICON_INFORMATION | wxOK);
	}

	return 0;
}

