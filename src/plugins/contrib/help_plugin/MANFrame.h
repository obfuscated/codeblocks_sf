#ifndef MANFRAME_H
#define MANFRAME_H

#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/bmpbuttn.h>
#include <wx/wxhtml.h>
#include <wx/string.h>
#include <wx/html/htmlwin.h>
#include <wx/bitmap.h>
#include <vector>

class MANFrame : public wxPanel
{
    private:
        wxTextCtrl *m_entry;
        wxButton *m_search;
        wxBitmapButton *m_zoomIn;
        wxBitmapButton *m_zoomOut;
        wxHtmlWindow *m_htmlWindow;
        std::vector<wxString> m_dirsVect;
        int m_baseFontSize;

    public:
        MANFrame(wxWindow *parent = 0, wxWindowID id = wxID_ANY, const wxBitmap &zoomInBmp = wxNullBitmap, const wxBitmap &zoomOutBmp = wxNullBitmap);
        bool SearchManPage(const wxString &dirs, const wxString &keyword);
        void SetBaseFontSize(int newsize);
        int GetBaseFontSize() const { return m_baseFontSize; }

    private:
        wxString GetManPage(const wxString &dirs, const wxString &keyword);
        bool Decompress(const wxString& filename, const wxString& tmpfile);
        void SetPage(const wxString &contents);
        void OnSearch(wxCommandEvent &event);
        void OnZoomIn(wxCommandEvent &event);
        void OnZoomOut(wxCommandEvent &event);
        void OnLinkClicked(wxHtmlLinkEvent &event);

    DECLARE_EVENT_TABLE()
};

#endif
