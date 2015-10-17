/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef XTRA_RES_H
#define XTRA_RES_H

#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_dlg.h>
#include <wx/toolbar.h>


class wxXmlResourceHandler;

class DLLIMPORT wxToolBarAddOnXmlHandler : public wxXmlResourceHandler
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

class DLLIMPORT wxScrollingDialogXmlHandler : public wxDialogXmlHandler
{
    DECLARE_DYNAMIC_CLASS(wxScrollingDialogXmlHandler)

public:
    wxScrollingDialogXmlHandler();
    virtual wxObject *DoCreateResource();
    virtual bool CanHandle(wxXmlNode *node);
};

#endif
