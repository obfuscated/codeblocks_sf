#include "wxsheaders.h"
#include "wxsmithwizard.h"
#include "wxswizard.h"
#include "wxsmith.h"
#include <configmanager.h>

wxSmithWizard::wxSmithWizard()
{
    Bitmap.LoadFile(
        ConfigManager::GetDataFolder() +
        _T("/images/wxsmith/wxsmith32x32.png"),
        wxBITMAP_TYPE_PNG);
}

wxSmithWizard::~wxSmithWizard()
{
}

CompileTargetBase* wxSmithWizard::Launch(int Index, wxString* pFilename)
{
    if ( !wxsPLUGIN() )
    {
        wxMessageBox(
            _("wxSmith plugin is not enable. Please enable it before running wizard"));
            return 0;
    }
    wxsWizard Dlg(0L);
    Dlg.ShowModal();
    return 0;
}
