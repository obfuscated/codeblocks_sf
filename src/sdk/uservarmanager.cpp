/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is distributed under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
*
* Author: Thomas Denk
*
* $Id$
* $Date$
*/

#include "sdk_precomp.h"
#include "uservarmanager.h"
#include "configmanager.h"
#include "messagemanager.h"
#include "manager.h"
#include <wx/dialog.h>
#include <wx/intl.h>
#include <wx/xrc/xmlres.h>
#include <wx/choice.h>
#include <wx/textctrl.h>

class UsrGlblMgrEditDialog : public wxDialog
{
        wxString curr;
    public:
        UsrGlblMgrEditDialog(wxWindow* parent, const wxString& base);
    private:
        void OnOKClick(wxCommandEvent& event);
        void OnCancelClick(wxCommandEvent& event);
        void OnDelete(wxCommandEvent& event);
        void OnFS(wxCommandEvent& event);
        void OnCB(wxCommandEvent& event);
        void Load(const wxString& s);
        void Save();
        void List();
        void Add(const wxString& base);
        DECLARE_EVENT_TABLE()
};

UserVariableManager* UserVariableManager::instance = 0;

UserVariableManager::UserVariableManager()
{}

UserVariableManager* UserVariableManager::Get()
{
    if(Manager::isappShuttingDown()) // The mother of all sanity checks
        Free();
    else if(!instance)
    {
        instance = new UserVariableManager;
    }
    return instance;
}

void UserVariableManager::Free()
{
    if(instance)
    {
        delete instance;
        instance = 0;
    }
}

void UserVariableManager::Configure()
{
    UsrGlblMgrEditDialog d(Manager::Get()->GetAppWindow(), wxEmptyString);
    d.ShowModal();
}


wxString UserVariableManager::Replace(const wxString& variable)
{
    wxConfigBase * cfg = ConfigManager::Get();

    wxString package = variable.AfterLast(wxT(':')).BeforeFirst(wxT('.')).MakeLower();
    wxString member = variable.AfterFirst(wxT('.')).MakeLower();

    wxString base = cfg->Read(_T("/UserGlobalVars/") + package + _T("/base"), wxEmptyString);

    if(base.IsEmpty())
    {
        wxMessageBox(_("At least one global variable is used but not yet defined.\n"
                        "Please define it now..."), _("Warning"), wxICON_WARNING);
        UsrGlblMgrEditDialog d(Manager::Get()->GetAppWindow(), package);
        d.ShowModal();
    }

    if(member.IsEmpty() || member.IsSameAs(_T("base")) || member.IsSameAs(_T("dir")))
        return base;

    if(member.IsSameAs(_T("include")) || member.IsSameAs(_T("lib")) || member.IsSameAs(_T("obj")))
    {
        wxString ret = cfg->Read(_T("/UserGlobalVars/") + package + _T("/") + member, wxEmptyString);
        if(ret.IsEmpty()
          )
            ret = base + _T("/") + member;
        return ret;
    }
    else if(member.IsSameAs(_T("cflags")) || member.IsSameAs(_T("lflags")))
    {
        return cfg->Read(_T("/UserGlobalVars/") + package + _T("/") + member, wxEmptyString);
    }

    Manager::Get()->GetMessageManager()->DebugLog(_T("Warning: bad member ") + member + _T(" of user variable ") + package);

    return wxEmptyString;
}



//FIXME: Does XRCID already include _T(), or do we need to add that here?
BEGIN_EVENT_TABLE(UsrGlblMgrEditDialog, wxDialog)
EVT_BUTTON(XRCID("wxID_OK"), UsrGlblMgrEditDialog::OnOKClick)
EVT_BUTTON(XRCID("wxID_CANCEL"), UsrGlblMgrEditDialog::OnCancelClick)
EVT_BUTTON(XRCID("delete"), UsrGlblMgrEditDialog::OnDelete)
EVT_BUTTON(XRCID("fs_value"), UsrGlblMgrEditDialog::OnFS)
EVT_BUTTON(XRCID("fs_include"), UsrGlblMgrEditDialog::OnFS)
EVT_BUTTON(XRCID("fs_lib"), UsrGlblMgrEditDialog::OnFS)
EVT_BUTTON(XRCID("fs_obj"), UsrGlblMgrEditDialog::OnFS)
EVT_BUTTON(XRCID("fs_cflags"), UsrGlblMgrEditDialog::OnFS)
EVT_BUTTON(XRCID("fs_lflags"), UsrGlblMgrEditDialog::OnFS)
EVT_COMBOBOX(XRCID("variable"), UsrGlblMgrEditDialog::OnCB)
END_EVENT_TABLE()

UsrGlblMgrEditDialog::UsrGlblMgrEditDialog(wxWindow* parent, const wxString& base)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgUserGlobalVar"));

    List();
    if(!base.IsEmpty())
    {
        Add(base);
        Load(base);
    }
    else
    {
        XRCCTRL(*this, "variable", wxChoice)->SetSelection(0);
        Load(XRCCTRL(*this, "variable", wxChoice)->GetStringSelection());
    }
}

void UsrGlblMgrEditDialog::OnOKClick(wxCommandEvent& event)
{
    Save();
    EndModal(wxID_OK);
}


void UsrGlblMgrEditDialog::Load(const wxString& base)
{
    curr = base;

    XRCCTRL(*this, "variable", wxChoice)->SetStringSelection(base);

    wxConfigBase * cfg = ConfigManager::Get();
    wxString s;

    s = cfg->Read(_T("/UserGlobalVars/") + base + _T("/base"), wxEmptyString);
    XRCCTRL(*this, "value", wxTextCtrl)->SetValue(s);
    s = cfg->Read(_T("/UserGlobalVars/") + base + _T("/include"), wxEmptyString);
    XRCCTRL(*this, "include", wxTextCtrl)->SetValue(s);
    s = cfg->Read(_T("/UserGlobalVars/") + base + _T("/lib"), wxEmptyString);
    XRCCTRL(*this, "lib", wxTextCtrl)->SetValue(s);
    s = cfg->Read(_T("/UserGlobalVars/") + base + _T("/obj"), wxEmptyString);
    XRCCTRL(*this, "obj", wxTextCtrl)->SetValue(s);
    s = cfg->Read(_T("/UserGlobalVars/") + base + _T("/cflags"), wxEmptyString);
    XRCCTRL(*this, "cflags", wxTextCtrl)->SetValue(s);
    s = cfg->Read(_T("/UserGlobalVars/") + base + _T("/lflags"), wxEmptyString);
    XRCCTRL(*this, "lflags", wxTextCtrl)->SetValue(s);
}

void UsrGlblMgrEditDialog::List()
{
    wxChoice * c = XRCCTRL(*this, "variable", wxChoice);
    c->Clear();

    wxConfigBase * cfg = ConfigManager::Get();

    wxString path = cfg->GetPath();
    cfg->SetPath(_T("/UserGlobalVars/"));

    long int i = 0;
    wxString str;

    bool valid = cfg->GetFirstGroup(str, i);
    while ( valid )
    {
        c->Append(str);
        valid = cfg->GetNextGroup(str, i);
    }

    cfg->SetPath(path);
}

void UsrGlblMgrEditDialog::Add(const wxString& base)
{
    XRCCTRL(*this, "variable", wxChoice)->Append(base);
}

void UsrGlblMgrEditDialog::Save()
{
    wxConfigBase * cfg = ConfigManager::Get();

    cfg->Write(_T("/UserGlobalVars/") + curr + _T("/base"),    XRCCTRL(*this, "value", wxTextCtrl)->GetValue());
    cfg->Write(_T("/UserGlobalVars/") + curr + _T("/include"), XRCCTRL(*this, "include", wxTextCtrl)->GetValue());
    cfg->Write(_T("/UserGlobalVars/") + curr + _T("/lib"),     XRCCTRL(*this, "lib", wxTextCtrl)->GetValue());
    cfg->Write(_T("/UserGlobalVars/") + curr + _T("/obj"),     XRCCTRL(*this, "obj", wxTextCtrl)->GetValue());
    cfg->Write(_T("/UserGlobalVars/") + curr + _T("/cflags"),  XRCCTRL(*this, "cflags", wxTextCtrl)->GetValue());
    cfg->Write(_T("/UserGlobalVars/") + curr + _T("/lflags"),  XRCCTRL(*this, "lflags", wxTextCtrl)->GetValue());
}

void UsrGlblMgrEditDialog::OnCB(wxCommandEvent& event)
{
    Save();
    Load(XRCCTRL(*this, "variable", wxChoice)->GetStringSelection());
}

void UsrGlblMgrEditDialog::OnFS(wxCommandEvent& event)
{
    wxTextCtrl* c;
    int id = event.GetId();

    if(id == XRCID("fs_value"))
        c = XRCCTRL(*this, "value", wxTextCtrl);
    else if(id == XRCID("fs_include"))
        c = XRCCTRL(*this, "include", wxTextCtrl);
    else if(id == XRCID("fs_lib"))
        c = XRCCTRL(*this, "lib", wxTextCtrl);
    else if(id == XRCID("fs_obj"))
        c = XRCCTRL(*this, "obj", wxTextCtrl);
    else if(id == XRCID("fs_cflags"))
        c = XRCCTRL(*this, "cflags", wxTextCtrl);
    else if(id == XRCID("fs_lflags"))
        c = XRCCTRL(*this, "lflags", wxTextCtrl);

    wxString path = ChooseDirectory(0,
                        _("Choose a location"),
                        !c->GetValue().IsEmpty() ? c->GetValue() : XRCCTRL(*this, "value", wxTextCtrl)->GetValue());
    if (!path.IsEmpty())
        c->SetValue(path);
}

void UsrGlblMgrEditDialog::OnDelete(wxCommandEvent& event)
{
    wxString g(XRCCTRL(*this, "variable", wxChoice)->GetStringSelection());
    if(wxMessageDialog(Manager::Get()->GetAppWindow(), wxString::Format(_("Delete the global variable %s?"), g.c_str()), _("Delete"), wxYES_NO).ShowModal() == wxID_YES)
        ConfigManager::Get()->DeleteGroup(_T("/UserGlobalVars/") + g);
    List();
}

void UsrGlblMgrEditDialog::OnCancelClick(wxCommandEvent& event)
{
    EndModal(wxID_CANCEL);
}
