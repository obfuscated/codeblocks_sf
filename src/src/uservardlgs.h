/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef USERVARDLGS_H
#define USERVARDLGS_H

#include "sdk_precomp.h"

#ifndef CB_PRECOMP
    #include "uservarmanager.h"
    #include "configmanager.h"
    #include "logmanager.h"
    #include "projectmanager.h"
    #include "macrosmanager.h"
    #include "manager.h"
    #include "infowindow.h"

    #include <wx/button.h>
    #include "scrollingdialog.h"
    #include <wx/intl.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/textctrl.h>
    #include <wx/textdlg.h>
    #include <wx/splitter.h>
    #include <wx/choice.h>
    #include <wx/listbox.h>
#endif


class GetUserVariableDialog : public wxScrollingDialog
{
public:
    GetUserVariableDialog(wxWindow *parent, const wxString &old);

    wxString GetVariable() { return m_SelectedVar; }
private:
    void OnOK(cb_unused wxCommandEvent& event);
    void OnCancel(cb_unused wxCommandEvent& event);
    void OnConfig(cb_unused wxCommandEvent& event);
    void OnActivated(wxTreeEvent& event);

    void Load();

    wxString GetSelectedVariable();
private:
    wxTreeCtrl *m_treectrl;
    wxString m_SelectedVar;
    wxString m_old;

    DECLARE_EVENT_TABLE()
};

class UsrGlblMgrEditDialog : public wxScrollingDialog
{
    wxString m_CurrentSet;
    wxString m_CurrentVar;

    wxChoice *m_SelSet;
    wxListBox *m_SelVar;

    wxButton *m_DeleteSet;

    wxTextCtrl *m_Base;
    wxTextCtrl *m_Include;
    wxTextCtrl *m_Lib;
    wxTextCtrl *m_Obj;
    wxTextCtrl *m_Bin;

    wxTextCtrl *m_Name[7];
    wxTextCtrl *m_Value[7];

    ConfigManager *m_CfgMan;

    void Help(wxCommandEvent& event);
    void DoClose();
    void OnOK(cb_unused wxCommandEvent& event)       { DoClose(); };
    void OnCancel(cb_unused wxCommandEvent& event)   { DoClose(); };
    void CloseHandler(cb_unused wxCloseEvent& event) { DoClose(); };

    void CloneVar(wxCommandEvent&  event);
    void CloneSet(wxCommandEvent&  event);
    void NewVar(wxCommandEvent&    event);
    void NewSet(wxCommandEvent&    event);
    void DeleteVar(wxCommandEvent& event);
    void DeleteSet(wxCommandEvent& event);
    // handler for the folder selection button
    void OnFS(wxCommandEvent& event);

    void SelectSet(wxCommandEvent& event);
    void SelectVar(wxCommandEvent& event);

    void Load();
    void Save();
    void UpdateChoices();
    void AddVar(const wxString& var);
    void Sanitise(wxString& s);

    DECLARE_EVENT_TABLE()

public:
    UsrGlblMgrEditDialog(const wxString& var = wxEmptyString);
    friend class UserVarManagerGUI;
};

class UserVarManagerGUI : public UserVarManagerUI
{
    public:
    void DisplayInfoWindow(const wxString &title, const wxString &msg) override;
    void OpenEditWindow(const std::set<wxString> &var) override;
    wxString GetVariable(wxWindow* parent, const wxString &old) override;
};

#endif // USERVARDLGS_H
