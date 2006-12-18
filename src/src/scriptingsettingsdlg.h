#ifndef SCRIPTINGSETTINGSDLG_H
#define SCRIPTINGSETTINGSDLG_H

#include <vector>
#include <wx/dialog.h>

class ScriptEntry// : public ISerializable (<-- can't make it work)
{
public:
    wxString SerializeOut() const
    {
        return wxString::Format(_T("%s;%d;%d;%s"), script.c_str(), enabled ? 1 : 0, registered ? 1 : 0, menu.c_str());
    }
    void SerializeIn(const wxString& s)
    {
        wxString tmp = s;
        script = tmp.BeforeFirst(_T(';'));
        tmp = tmp.AfterFirst(_T(';'));
        enabled = tmp.BeforeFirst(_T(';')) == _T("1") ? true : false;
        tmp = tmp.AfterFirst(_T(';'));
        registered = tmp.BeforeFirst(_T(';')) == _T("1") ? true : false;
        tmp = tmp.AfterFirst(_T(';'));
        menu = tmp.BeforeFirst(_T(';'));
    }

    wxString script;
    bool enabled;
    bool registered;
    wxString menu;
};
typedef std::vector<ScriptEntry> ScriptsVector;

class ScriptingSettingsDlg : public wxDialog
{
    public:
        ScriptingSettingsDlg(wxWindow* parent);
        ~ScriptingSettingsDlg();

        void EndModal(int retCode);
    protected:
        void FillScripts();
        void UpdateState();
        void LoadItem(long item);
        void SaveItem(long item);

        void OnListSelection(wxListEvent& event);
        void OnListDeselection(wxListEvent& event);
        void OnAddScript(wxCommandEvent& event);
        void OnRemoveScript(wxCommandEvent& event);
        void OnScriptChanged(wxCommandEvent& event);
        void OnScriptMenuChanged(wxCommandEvent& event);
        void OnEnable(wxCommandEvent& event);
        void OnRegister(wxCommandEvent& event);
        void OnBrowse(wxCommandEvent& event);

        ScriptsVector m_ScriptsVector;
        bool m_IgnoreTextEvents;
    private:
        DECLARE_EVENT_TABLE()
};



#endif // SCRIPTINGSETTINGSDLG_H
