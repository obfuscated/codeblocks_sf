#ifndef XTRA_RES_H
#define XTRA_RES_H

#include <wx/wxprec.h>
#include <wx/xrc/xmlres.h>
#include <wx/toolbar.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif


class wxXmlResourceHandler;

class wxToolBarAddOnXmlHandler : public wxXmlResourceHandler
{
    public:
        wxToolBarAddOnXmlHandler();
        virtual wxObject *DoCreateResource();
        virtual bool CanHandle(wxXmlNode *node);

    protected:
        bool m_isInside;
        bool m_isAddon;
        wxToolBar *m_toolbar;

        wxBitmap GetCenteredBitmap(const wxString& param = wxT("bitmap"),
            const wxArtClient& defaultArtClient = wxART_OTHER,
            wxSize size = wxDefaultSize);
};

#endif
