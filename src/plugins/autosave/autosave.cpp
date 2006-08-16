/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* $Revision$
* $Id $
* $HeadURL$
*/

#include "sdk.h"
#ifndef CB_PRECOMP
    #include <wx/checkbox.h>
    #include <wx/choice.h>
    #include <wx/filefn.h>
    #include <wx/filename.h>
    #include <wx/textctrl.h>
    #include <wx/timer.h>
    #include <wx/xrc/xmlres.h>
    #include "cbeditor.h"
    #include "cbproject.h"
    #include "configmanager.h"
    #include "editormanager.h"
    #include "globals.h"
    #include "licenses.h"
    #include "pluginmanager.h"
    #include "projectmanager.h"
    #include "manager.h"
    #include "sdk_events.h"
#endif

#include "projectloader.h"
#include "autosave.h"

// Implement the plugin's hooks
CB_IMPLEMENT_PLUGIN(Autosave, "Autosave");

BEGIN_EVENT_TABLE(Autosave, cbPlugin)
EVT_TIMER(-1, Autosave::OnTimer)
END_EVENT_TABLE()

Autosave::Autosave()
{
    //ctor
    m_PluginInfo.name = _T("Autosave");
    m_PluginInfo.title = _("Autosave");
    m_PluginInfo.version = _T("0.1");
    m_PluginInfo.description = _("Saves your work in regular intervals");
    m_PluginInfo.author = _T("Thomas Denk");
    m_PluginInfo.authorEmail = _T("");
    m_PluginInfo.authorWebsite = _T("");
    m_PluginInfo.thanksTo = _("");
    m_PluginInfo.license = _T("GPL");

    timer1 = new wxTimer(this, 10000);
    timer2 = new wxTimer(this, 20000);
}

Autosave::~Autosave()
{
    delete timer1;
    delete timer2;
}

void Autosave::OnAttach()
{
    if(!Manager::LoadResource(_T("autosave.zip")))
    {
        NotifyMissingFile(_T("autosave.zip"));
    }
    Start();
}

void Autosave::Start()
{
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("autosave"));
    if(cfg->ReadBool(_T("do_project")))
        timer1->Start(60 * 1000 * cfg->ReadInt(_T("project_mins")));
    else
        timer1->Stop();

    if(cfg->ReadBool(_T("do_sources")))
        timer2->Start(60 * 1000 * cfg->ReadInt(_T("source_mins")));
    else
        timer2->Stop();
}

void Autosave::OnRelease(bool appShutDown)
{}

void Autosave::OnTimer(wxTimerEvent& e)
{
    if(e.GetId() == 10000)
    {
        PluginManager *plm = Manager::Get()->GetPluginManager();
        int method = Manager::Get()->GetConfigManager(_T("autosave"))->ReadInt(_T("method"));
        ProjectManager *pm = Manager::Get()->GetProjectManager();
        if(pm && pm->GetActiveProject())
        {
            cbProject * p = pm->GetActiveProject();
            if(p && p->GetModified())
            {
                switch(method)
                {
                    case 0:
                    {
                        if(::wxRenameFile(p->GetFilename(), p->GetFilename() + _T(".bak")))
                            if(p->Save())
                            {
                                CodeBlocksEvent e(cbEVT_PROJECT_SAVE);
                                plm->NotifyPlugins(e);
                            }
                        break;
                    }
                    case 1:
                    {
                        if(p->Save())
                        {
                            CodeBlocksEvent e(cbEVT_PROJECT_SAVE);
                            plm->NotifyPlugins(e);
                        }
                        break;
                    }
                    case 2:
                    {
                        if (p->IsLoaded() == false)
                            return;
                        ProjectLoader loader(p);
                        if(loader.Save(p->GetFilename() + _T(".save")))
                        {
                            CodeBlocksEvent e(cbEVT_PROJECT_SAVE);
                            plm->NotifyPlugins(e);
                        }
                        p->SetModified(); // the actual project file is still not updated!
                        break;
                    }
                }
            }
        }
    }
else if(e.GetId() == 20000)
    {
        int method = Manager::Get()->GetConfigManager(_T("autosave"))->ReadInt(_T("method"));
        EditorManager* em = Manager::Get()->GetEditorManager();

        if(em)
        {
            for(int i = 0; i < em->GetEditorsCount(); ++i)
            {
                cbEditor* ed = em->GetBuiltinEditor(em->GetEditor(i));
                if(ed && ed->GetModified())
                {
                    wxFileName fn(ed->GetFilename());
                    switch(method)
                    {
                        case 0:
                        {
                            if(::wxRenameFile(fn.GetFullPath(), fn.GetFullPath() + _T(".bak")))
                                cbSaveToFile(fn.GetFullPath(), ed->GetControl()->GetText(), ed->GetEncoding(), ed->GetUseBom());
                            break;
                        }
                        case 1:
                        {
                            cbSaveToFile(fn.GetFullPath(), ed->GetControl()->GetText(), ed->GetEncoding(), ed->GetUseBom());
                            break;
                        }
                        case 2:
                        {
                            cbSaveToFile(fn.GetFullPath() + _T(".save"), ed->GetControl()->GetText(), ed->GetEncoding(), ed->GetUseBom());
                            ed->SetModified(); // the "real" file has not been saved!
                            break;
                        }
                    }
                }

            }
        }
    }

}

cbConfigurationPanel* Autosave::GetConfigurationPanel(wxWindow* parent)
{
    AutosaveConfigDlg* dlg = new AutosaveConfigDlg(parent, this);
    // deleted by the caller

    return dlg;
}


int Autosave::Configure()
{
    return 0;
}



AutosaveConfigDlg::AutosaveConfigDlg(wxWindow* parent, Autosave* plug) : plugin(plug)
{
    wxXmlResource::Get()->LoadPanel(this, parent, _T("dlgAutosave"));

    LoadSettings();
}

void AutosaveConfigDlg::LoadSettings()
{
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("autosave"));

    XRCCTRL(*this, "do_project", wxCheckBox)->SetValue(cfg->ReadBool(_T("do_project")));
    XRCCTRL(*this, "do_sources", wxCheckBox)->SetValue(cfg->ReadBool(_T("do_sources")));
    XRCCTRL(*this, "project_mins", wxTextCtrl)->SetValue(wxString::Format(_T("%d"), cfg->ReadInt(_T("project_mins"))));
    XRCCTRL(*this, "source_mins", wxTextCtrl)->SetValue(wxString::Format(_T("%d"), cfg->ReadInt(_T("source_mins"))));

    XRCCTRL(*this, "method", wxChoice)->SetSelection(cfg->ReadInt(_T("method"), 2));
}

void AutosaveConfigDlg::SaveSettings()
{
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("autosave"));

    cfg->Write(_T("do_project"), (bool) XRCCTRL(*this, "do_project", wxCheckBox)->GetValue());
    cfg->Write(_T("do_sources"), (bool) XRCCTRL(*this, "do_sources", wxCheckBox)->GetValue());

    long pm, sm;

    XRCCTRL(*this, "project_mins", wxTextCtrl)->GetValue().ToLong(&pm);
    XRCCTRL(*this, "source_mins", wxTextCtrl)->GetValue().ToLong(&sm);

    cfg->Write(_T("project_mins"), (int) pm);
    cfg->Write(_T("source_mins"), (int) sm);

    cfg->Write(_T("method"), XRCCTRL(*this, "method", wxChoice)->GetSelection());

    plugin->Start();
}






