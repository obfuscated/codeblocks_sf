#ifndef MANFRAME_H
#define MANFRAME_H

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/wxhtml.h>
#include <wx/string.h>
#include <vector>

class MANFrame : public wxFrame
{
    private:
        wxTextCtrl *m_entry;
        wxButton *m_search;
        wxHtmlWindow *m_htmlWindow;
        std::vector<wxString> m_dirsVect;

    public:
        MANFrame(wxWindow *parent = 0, wxWindowID id = wxID_ANY, const wxString &title = _("Man pages viewer"));
        bool SearchManPage(const wxString &dirs, const wxString &keyword);

    private:
        wxString GetManPage(const wxString &dirs, const wxString &keyword);
        bool Decompress(const wxString& filename, const wxString& tmpfile);
        void SetPage(const wxString &contents);
        void OnSearch(wxCommandEvent &event);
        void OnClose(wxCloseEvent &event);

    DECLARE_EVENT_TABLE()
};

#endif
