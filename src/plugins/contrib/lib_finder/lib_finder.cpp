/***************************************************************
 * Name:      lib_finder.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    BYO<byo.spoon@gmail.com>
 * Copyright: (c) BYO
 * License:   GPL
 **************************************************************/

#include "lib_finder.h"
#include "libraryconfigmanager.h"
#include "resultmap.h"
#include "dirlistdlg.h"
#include "processingdlg.h"

// Register the plugin
namespace
{
  PluginRegistrant<lib_finder> reg(_T("lib_finder"));
};

lib_finder::lib_finder()
{
}

lib_finder::~lib_finder()
{
}

void lib_finder::OnAttach()
{
}

void lib_finder::OnRelease(bool appShutDown)
{
}

int lib_finder::Execute()
{
    LCM()->Clear();
    RM()->Clear();

    wxString resPath = ConfigManager::GetDataFolder();
    LCM()->LoadXmlConfig(resPath + wxFileName::GetPathSeparator() + _T("lib_finder"));

    if ( LCM()->GetLibraryCount() == 0 )
    {
        ::wxMessageBox(_("Didn't find any library :("));
        return -1;
    }

    DirListDlg Dlg(0L);
    if ( Dlg.ShowModal() == wxID_CANCEL ) return 0;

    FileNamesMap FNMap;
    ProcessingDlg PDlg(0L);
    PDlg.Show();
    PDlg.MakeModal(true);
    if ( PDlg.ReadDirs(Dlg.Dirs) &&
         PDlg.ProcessLibs() )
    {
        PDlg.Hide();
        ResultArray Results;
        RM()->GetAllResults(Results);
        if ( Results.Count() == 0 )
        {
            ::wxMessageBox(_("Didn't find any library"));
        }
        else
        {
            wxArrayString Names;
            wxArrayInt Selected;
            wxString PreviousVar;
            for ( size_t i=0; i<Results.Count(); ++i )
            {
                Names.Add(
                    wxString::Format(_T("%s : %s"),
                        Results[i]->GlobalVar.c_str(),
                        Results[i]->LibraryName.c_str()));
                if ( PreviousVar != Results[i]->GlobalVar )
                {
                    Selected.Add((int)i);
                    PreviousVar = Results[i]->GlobalVar;
                }
            }
            wxMultiChoiceDialog Dlg(NULL,_("Select libraries You want to set up."),_("Setting up libraries"),Names);
            Dlg.SetSelections(Selected);
            if ( Dlg.ShowModal() == wxID_OK )
            {
                Selected = Dlg.GetSelections();
                for ( size_t i = 0; i<Selected.Count(); i++ )
                {
                    SetGlobalVar(Results[Selected[i]]);
                }
            }
        }
    }
    PDlg.MakeModal(false);

	return -1;
}

void lib_finder::SetGlobalVar(LibraryResult* Res)
{
    ConfigManager * cfg = Manager::Get()->GetConfigManager(_T("gcv"));
    wxString activeSet = cfg->Read(_T("/active"));
    wxString curr = _T("/sets/") + activeSet + _T("/") + Res->GlobalVar;

    cfg->Write(curr + _T("/base"),    Res->BasePath);
    cfg->Write(curr + _T("/include"), Res->IncludePath);
    cfg->Write(curr + _T("/lib"),     Res->LibPath);
    cfg->Write(curr + _T("/obj"),     Res->ObjPath);
    cfg->Write(curr + _T("/cflags"),  Res->CFlags);
    cfg->Write(curr + _T("/lflags"),  Res->LFlags);
}
