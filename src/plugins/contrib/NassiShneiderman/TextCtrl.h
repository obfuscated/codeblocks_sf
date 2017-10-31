#ifndef TEXTCTRL_H
#define TEXTCTRL_H

// For compilers that support precompilation, includes <wx/wx.h>
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class TextCtrl : public wxTextCtrl
{
public:
        TextCtrl(wxWindow* parent, wxWindowID id, const wxString& value = _T(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
        virtual ~TextCtrl();

        void SetOrigSize(wxSize size);
protected:
private:
    void OnText(wxCommandEvent &event);
    void OnMouseWheel(wxMouseEvent &event);

    wxSize m_origSize;
protected:
    //DECLARE_DYNAMIC_CLASS(NassiDiagramWindow)
    DECLARE_EVENT_TABLE()
};

#endif // TEXTCTRL_H
