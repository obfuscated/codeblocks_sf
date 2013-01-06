#ifndef WXSMITHAUIMANAGER_H
#define WXSMITHAUIMANAGER_H

#include <wx/aui/aui.h>
#include <wx/wx.h>
#include <wx/event.h>

#include <prep.h>

class wxSmithAuiManager : public wxAuiManager
{
    public:
        wxSmithAuiManager(wxWindow* managed_wnd = NULL, unsigned int flags = wxAUI_MGR_DEFAULT)
            : wxAuiManager(managed_wnd, flags)
        {
            Connect(wxEVT_DESTROY,(wxObjectEventFunction)&wxSmithAuiManager::OnDestroy);
        }

        virtual ~wxSmithAuiManager() {}

        void OnDestroy(cb_unused wxWindowDestroyEvent& event)
        {
            UnInit();
        }
    protected:
    private:
};

#endif // WXSMITHAUIMANAGER_H
