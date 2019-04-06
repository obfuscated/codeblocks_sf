/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef XTRA_RES_H
#define XTRA_RES_H

#include <wx/xrc/xmlres.h>
#include <wx/xrc/xh_dlg.h>
#include <wx/toolbar.h>

#include "settings.h" // DLLIMPORT

class wxXmlResourceHandler;

class DLLIMPORT wxToolBarAddOnXmlHandler : public wxXmlResourceHandler
{
    public:
        wxToolBarAddOnXmlHandler();
        wxObject *DoCreateResource() override;
        bool CanHandle(wxXmlNode *node) override;

        void SetToolbarImageSize(int size);

    protected:
        bool m_isInside;
        bool m_isAddon;
        wxToolBar *m_toolbar;
        int m_ImageSize;
        wxString m_PathReplaceString;

        wxBitmap GetCenteredBitmap(const wxString& param, wxSize size, double scaleFactor);
        wxBitmap LoadBitmap(const wxString& name, double scaleFactor);
};

class DLLIMPORT wxScrollingDialogXmlHandler : public wxDialogXmlHandler
{
    DECLARE_DYNAMIC_CLASS(wxScrollingDialogXmlHandler)

public:
    wxScrollingDialogXmlHandler();
    wxObject *DoCreateResource() override;
    bool CanHandle(wxXmlNode *node) override;
};

#endif
