/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
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
#include "manager.h"
#include "logmanager.h"
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
		Manager::Get()->GetLogManager()->DebugLog(msg);
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
				Manager::Get()->GetLogManager()->DebugLog(F(_T("WindowsXPLookNFeel: Ignoring target '%s'"), tgt->GetTitle().wx_str()));
				continue;
			}
			targetNames.Add(tgt->GetTitle());
			target = tgt;
		}
	}

	if (!target)
	{
		// not even one executable target...
		Manager::Get()->GetLogManager()->DebugLog(_T("WindowsXPLookNFeel: No executable targets in project"));
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
		Manager::Get()->GetLogManager()->DebugLog(F(_T("WindowsXPLookNFeel: Creating Manifest '%s'"), filename.wx_str()));

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

