#ifndef PROJECTDEPSDLG_H
#define PROJECTDEPSDLG_H

#include <wx/dialog.h>

class cbProject;

class ProjectDepsDlg : public wxDialog
{
    public:
        ProjectDepsDlg(wxWindow* parent, cbProject* sel = 0);
        virtual ~ProjectDepsDlg();
        virtual void EndModal(int retCode);
    protected:
        void FillList();
        bool SaveList();
        void OnProjectChange(wxCommandEvent& event);
        int m_LastSel;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // PROJECTDEPSDLG_H
