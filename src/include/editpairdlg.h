#ifndef EDITPAIRDLG_H
#define EDITPAIRDLG_H

#include <wx/intl.h>
#include <wx/dialog.h>

class EditPairDlg : public wxDialog
{
    public:
        enum BrowseMode
        {
            bmDisable = 0,
            bmBrowseForFile,
            bmBrowseForDirectory
        };

        EditPairDlg(wxWindow* parent, wxString& key, wxString& value, const wxString& title = _("Edit pair"), BrowseMode allowBrowse = bmDisable);
        virtual ~EditPairDlg();
		EditPairDlg& operator=(const EditPairDlg&){ return *this; } // just to satisfy script bindings (never used)
		virtual void EndModal(int retCode);
    protected:
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnBrowse(wxCommandEvent& event);

        wxString& m_Key;
        wxString& m_Value;
        BrowseMode m_BrowseMode;
    private:
        DECLARE_EVENT_TABLE()
};

#endif // EDITPAIRDLG_H
