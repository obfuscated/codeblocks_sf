#include <sdk.h>
#include <wx/xrc/xmlres.h>
#include <manager.h>
#include <configmanager.h>
#include <pluginmanager.h>
#include <cbplugin.h>
#include <wx/intl.h>
#include <wx/listbook.h>

#include "compilersettingsdlg.h"
#include "appglobals.h"
#ifdef __WXMSW__
    #include "associations.h"
#endif

// images by order of pages
const wxString base_imgs[] =
{
    _T("batch"),
};
const int IMAGES_COUNT = 1; // keep this in sync!

BEGIN_EVENT_TABLE(CompilerSettingsDlg, wxDialog)
    EVT_LISTBOOK_PAGE_CHANGING(XRCID("nbMain"), CompilerSettingsDlg::OnPageChanging)
    EVT_LISTBOOK_PAGE_CHANGED(XRCID("nbMain"), CompilerSettingsDlg::OnPageChanged)
END_EVENT_TABLE()

// This dialog initially contains only the batch-build settings.
// So we 'll add all compiler-related configuration before it
// and all debugger-related after it...

CompilerSettingsDlg::CompilerSettingsDlg(wxWindow* parent)
{
    wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgCompilerSettings"));
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    wxImageList* images = new wxImageList(80, 80);
    lb->AssignImageList(images);
    ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));

    // tab "Batch builds"
#ifdef __WXMSW__
    XRCCTRL(*this, "txtBatchBuildsCmdLine", wxTextCtrl)->SetValue(cfg->Read(_T("/batch_build_args"), g_DefaultBatchBuildArgs));
#endif

    // add all plugins configuration panels
    AddPluginPanels();

    // make sure everything is laid out properly
    GetSizer()->SetSizeHints(this);
    CenterOnParent();
}

CompilerSettingsDlg::~CompilerSettingsDlg()
{
    //dtor
}

void CompilerSettingsDlg::AddPluginPanels()
{
    const wxString base = ConfigManager::GetDataFolder() + _T("/images/settings/");
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);

    // we 'll remove the existing page and add it when appropriate
    wxWindow* existingPage = lb->GetPage(0);
    wxString existingTitle = lb->GetPageText(0);
    lb->RemovePage(0);

    // we 'll do this in two steps:
    // 1) get all compiler-related panels and put them before batch-builds
    // 2) get all debugger panels and put them after batch-builds

    ConfigurationPanelsArray local;

    // step 1
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(cgCompiler, lb, local);
    for (size_t i = 0; i < local.GetCount(); ++i)
    {
        cbConfigurationPanel* panel = local[i];
        lb->AddPage(panel, panel->GetTitle());

        lb->GetImageList()->Add(LoadPNGWindows2000Hack(base + panel->GetBitmapBaseName() + _T(".png")));
        lb->GetImageList()->Add(LoadPNGWindows2000Hack(base + panel->GetBitmapBaseName() + _T("-off.png")));
        lb->SetPageImage(lb->GetPageCount() - 1, lb->GetImageList()->GetImageCount() - 2);

        // add it in our central container too
        m_PluginPanels.Add(panel);
    }

    // now load the builtin pages' images
    lb->AddPage(existingPage, existingTitle);
    wxBitmap bmp;
    bmp.LoadFile(base + base_imgs[0] + _T(".png"), wxBITMAP_TYPE_PNG);
    lb->GetImageList()->Add(bmp);
    bmp.LoadFile(base + base_imgs[0] + _T("-off.png"), wxBITMAP_TYPE_PNG);
    lb->GetImageList()->Add(bmp);
    lb->SetPageImage(lb->GetPageCount() -1, lb->GetImageList()->GetImageCount() - 2);

    // step 2
    Manager::Get()->GetPluginManager()->GetConfigurationPanels(cgDebugger, lb, local);
    for (size_t i = 0; i < local.GetCount(); ++i)
    {
        cbConfigurationPanel* panel = local[i];
        lb->AddPage(panel, panel->GetTitle());

        lb->GetImageList()->Add(LoadPNGWindows2000Hack(base + panel->GetBitmapBaseName() + _T(".png")));
        lb->GetImageList()->Add(LoadPNGWindows2000Hack(base + panel->GetBitmapBaseName() + _T("-off.png")));
        lb->SetPageImage(lb->GetPageCount() - 1, lb->GetImageList()->GetImageCount() - 2);

        // add it in our central container too
        m_PluginPanels.Add(panel);
    }

    UpdateListbookImages();
}

void CompilerSettingsDlg::UpdateListbookImages()
{
    wxListbook* lb = XRCCTRL(*this, "nbMain", wxListbook);
    int sel = lb->GetSelection();
    // set page images according to their on/off status
    for (size_t i = 0; i < IMAGES_COUNT + m_PluginPanels.GetCount(); ++i)
    {
        lb->SetPageImage(i, (i * 2) + (sel == (int)i ? 0 : 1));
    }

    // the selection color is ruining the on/off effect,
    // so make sure no item is selected ;)
    lb->GetListView()->Select(sel, false);

    // update the page title
    wxString label = lb->GetPageText(sel);
    // replace any stray & with && because label makes it an underscore
    while (label.Replace(_T(" & "), _T(" && ")))
        ;
    XRCCTRL(*this, "lblBigTitle", wxStaticText)->SetLabel(label);
}

void CompilerSettingsDlg::OnPageChanging(wxListbookEvent& event)
{
}

void CompilerSettingsDlg::OnPageChanged(wxListbookEvent& event)
{
    UpdateListbookImages();
}

void CompilerSettingsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        ConfigManager *cfg = Manager::Get()->GetConfigManager(_T("app"));

        // tab "Batch builds"
#ifdef __WXMSW__
        wxString bbargs = XRCCTRL(*this, "txtBatchBuildsCmdLine", wxTextCtrl)->GetValue();
        if (bbargs != cfg->Read(_T("/batch_build_args"), g_DefaultBatchBuildArgs))
        {
            cfg->Write(_T("/batch_build_args"), bbargs);
            Associations::SetBatchBuildOnly();
        }
#endif

        // finally, apply settings in all plugins' panels
        for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
        {
            cbConfigurationPanel* panel = m_PluginPanels[i];
            panel->OnApply();
        }
    }
    else
    {
        // finally, cancel settings in all plugins' panels
        for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
        {
            cbConfigurationPanel* panel = m_PluginPanels[i];
            panel->OnCancel();
        }
    }

    wxDialog::EndModal(retCode);
}
