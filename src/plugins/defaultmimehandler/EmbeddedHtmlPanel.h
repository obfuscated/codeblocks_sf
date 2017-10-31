/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU General Public License, version 3
 * http://www.gnu.org/licenses/gpl-3.0.html
 */

#ifndef EMBEDDEDHTMLPANEL_H
#define EMBEDDEDHTMLPANEL_H

#ifndef CB_PRECOMP
    //(*HeadersPCH(EmbeddedHtmlPanel)
    #include <wx/panel.h>
    class wxBitmapButton;
    class wxBoxSizer;
    class wxStaticText;
    //*)
#endif
//(*Headers(EmbeddedHtmlPanel)
class wxHtmlWindow;
//*)

class wxHtmlLinkEvent;

class EmbeddedHtmlPanel: public wxPanel
{
    public:

        EmbeddedHtmlPanel(wxWindow* parent);
        virtual ~EmbeddedHtmlPanel();

        void Open(const wxString& url);

        //(*Declarations(EmbeddedHtmlPanel)
        wxPanel* Panel1;
        wxBitmapButton* btnForward;
        wxBitmapButton* btnBack;
        wxStaticText* lblStatus;
        wxHtmlWindow* winHtml;
        //*)

    protected:

        //(*Identifiers(EmbeddedHtmlPanel)
        static const long ID_BITMAPBUTTON2;
        static const long ID_BITMAPBUTTON3;
        static const long ID_STATICTEXT1;
        static const long ID_PANEL1;
        static const long ID_HTMLWINDOW1;
        //*)

    private:
        void OnUpdateUI(wxUpdateUIEvent& event);
        void OnLinkClicked(wxHtmlLinkEvent &event);

        //(*Handlers(EmbeddedHtmlPanel)
        void OnbtnBackClick(wxCommandEvent& event);
        void OnbtnForwardClick(wxCommandEvent& event);
        //*)

        DECLARE_EVENT_TABLE()
};

#endif
