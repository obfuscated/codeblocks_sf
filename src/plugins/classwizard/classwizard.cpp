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
#include <wx/fs_zip.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/utils.h>
#include <wx/xrc/xmlres.h>
#include "cbproject.h"
#include "configmanager.h"
#include "globals.h"
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
}

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
}

void ClassWizard::OnRelease(bool appShutDown)
{
}

int ClassWizard::Execute()
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
            if ((targets.GetCount() != 0) && (dlg.GetImplementationFilename() != _T("")) )
                prjMan->AddFileToProject(dlg.GetImplementationFilename(), prj, targets);
            prjMan->RebuildTree();
        }
        return 0;
    }

    return -1;
}
