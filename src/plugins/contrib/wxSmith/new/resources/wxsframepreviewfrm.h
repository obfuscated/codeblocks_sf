#ifndef WXSFRAMEPREVIEWFRM_H
#define WXSFRAMEPREVIEWFRM_H

#include <wx/frame.h>

class wxsFrameRes;

class wxsFramePreviewFrm: public wxFrame
{
    public:

        wxsFramePreviewFrm(wxsFrameRes* Resource);

        ~wxsFramePreviewFrm();

    private:

        wxsFrameRes* Res;
        void OnEscape(wxCommandEvent& event);

        DECLARE_EVENT_TABLE()
};

#endif
