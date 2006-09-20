#include "wxsheaders.h"
#include "wxsmithwizard.h"
#include "wxswizard.h"
#include <configmanager.h>
#include <licenses.h>

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
    wxsWizard Dlg(0L);
    Dlg.ShowModal();
    return 0;
}
