#ifndef WXSFRAMEPREVIEWFRM_H
#define WXSFRAMEPREVIEWFRM_H

//#include "closepreview.h"
#include <wx/dialog.h>

class wxsResource;

class wxsFramePreviewFrm: public wxFrame
{
    public:

        wxsFramePreviewFrm();

        wxsFramePreviewFrm(
            wxWindow* parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE);

        ~wxsFramePreviewFrm();

        /** Additional initialization
         *
         * This function initializes accelerator table and adds close button
         * when this is borderless dialog. This function must be called after
         * window content is initialized
         */
        void Initialize(wxsResource* Res);

    private:

        bool IsBorderLess();
//        ClosePreviewDlg* CPD;
        wxsResource* Res;

        void OnMove(wxMoveEvent& event);
        void OnSize(wxSizeEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnEscape(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif // WXSFRAMEPREVIEWFRM_H
