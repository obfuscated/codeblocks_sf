#include "wxscustomwidgetxmlhandler.h"

#include <wx/stattext.h>

wxsCustomWidgetXmlHandler::wxsCustomWidgetXmlHandler()
{
}

wxObject* wxsCustomWidgetXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(wnd,wxStaticText);
    wnd->Create(m_parentAsWindow,
                GetID(),
                _T("???"),
                wxDefaultPosition, wxDefaultSize,
                wxST_NO_AUTORESIZE|wxALIGN_CENTRE,
                GetName());
    if (HasParam(wxT("size")))
        wnd->SetClientSize(GetSize(wxT("size"),wnd));
    if (HasParam(wxT("pos")))
        wnd->Move(GetPosition());
    wnd->SetForegroundColour(wxColour(0xFF,0xFF,0xFF));
    wnd->SetBackgroundColour(wxColour(0x00,0x00,0x00));
    SetupWindow(wnd);
    return wnd;
}

bool wxsCustomWidgetXmlHandler::CanHandle(wxXmlNode *node)
{
    // This handler will be called as last one, we handle
    // everything that has not been handled yet
    return true;
}
