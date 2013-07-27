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
    #include <wx/fs_zip.h>
    #include <wx/intl.h>
    #include <wx/menu.h>
    #include <wx/string.h>
    #include <wx/utils.h>
    #include <wx/xrc/xmlres.h>
    #include "cbproject.h"
    #include "configmanager.h"
    #include "globals.h"
    #include "logmanager.h"
    #include "manager.h"
    #include "projectmanager.h"
#endif
#include <wx/filesys.h>
#include "classwizard.h"
#include "classwizarddlg.h"

// this auto-registers the plugin
namespace
{
    PluginRegistrant<ClassWizard> reg(_T("ClassWizard"));

    int idLaunch = wxNewId();
}

BEGIN_EVENT_TABLE(ClassWizard, cbPlugin)
	EVT_MENU(idLaunch, ClassWizard::OnLaunch)
END_EVENT_TABLE()

ClassWizard::ClassWizard()
{
    if(!Manager::LoadResource(_T("classwizard.zip")))
    {
        NotifyMissingFile(_T("classwizard.zip"));
    }
}

ClassWizard::~ClassWizard()
{
}

void ClassWizard::OnAttach()
{
	m_FileNewMenu = 0;
	cbPlugin::OnAttach();
}

void ClassWizard::OnRelease(bool appShutDown)
{
	if (m_FileNewMenu)
	{
		m_FileNewMenu->Delete(idLaunch);
		m_FileNewMenu = 0;
	}
	cbPlugin::OnRelease(appShutDown);
}

void ClassWizard::BuildMenu(wxMenuBar* menuBar)
{
	if (m_FileNewMenu)
	{
		m_FileNewMenu->Delete(idLaunch);
		m_FileNewMenu = 0;
	}

    const int pos = menuBar->FindMenu(_("&File"));
    if (pos != wxNOT_FOUND)
    {
        wxMenu* fm = menuBar->GetMenu(pos);
        int id = fm->FindItem(_("New"));
        wxMenuItem* mn = fm->FindItem(id);
        m_FileNewMenu = mn ? mn->GetSubMenu() : 0;
        if (m_FileNewMenu)
        {
			m_FileNewMenu->Insert(2, idLaunch, _("Class..."));
        }
		else
			Manager::Get()->GetLogManager()->DebugLog(_T("Could not find File->New menu!"));
    }
    else
        Manager::Get()->GetLogManager()->DebugLog(_T("Could not find File menu!"));
}

void ClassWizard::OnLaunch(cb_unused wxCommandEvent& event)
{
    ProjectManager* prjMan = Manager::Get()->GetProjectManager();
    cbProject* prj = prjMan->GetActiveProject();

    ClassWizardDlg dlg(Manager::Get()->GetAppWindow());
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        if (!prj)
        {
            cbMessageBox(   _("The new class has been created."),
                            _("Information"),
                            wxOK | wxICON_INFORMATION,
                            Manager::Get()->GetAppWindow());
        }
        else if( cbMessageBox( _("The new class has been created.\n"
                                 "Do you want to add it to the current project?"),
                               _("Add to project?"),
                               wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION,
                               Manager::Get()->GetAppWindow()) == wxID_YES)
        {
            wxArrayInt targets;
            prjMan->AddFileToProject(dlg.GetHeaderFilename(), prj, targets);
            if ( (targets.GetCount() != 0) && (dlg.IsValidImplementationFilename()) )
                prjMan->AddFileToProject(dlg.GetImplementationFilename(), prj, targets);
            if (dlg.AddPathToProject())
            {
                // Add the include Path to the Build targets....
                for (size_t i = 0; i < targets.GetCount(); ++i)
                {
                    ProjectBuildTarget* buildTarget = prj->GetBuildTarget(targets[i]);  // Get the top level build Target
                    if (buildTarget)
                        buildTarget->AddIncludeDir(dlg.GetIncludeDir());
                    else
                    {
                        wxString information;
                        information.Printf(_("Could not find build target ID = %i.\nThe include directory won't be added to this target. Please do it manually"), targets[i]);
                        cbMessageBox(information, _("Information"),
                                     wxOK | wxICON_INFORMATION,
                                     Manager::Get()->GetAppWindow());
                    }
                }
            }
            prjMan->GetUI().RebuildTree();
        }
    }
}
