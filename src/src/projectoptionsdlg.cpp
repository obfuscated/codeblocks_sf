/*
 * This file is part of the Code::Blocks IDE and licensed under the GNU Lesser General Public License, version 3
 * http://www.gnu.org/licenses/lgpl-3.0.html
 *
 * $Revision$
 * $Id$
 * $HeadURL$
 */

#include "sdk.h"

#ifndef CB_PRECOMP
    #include "manager.h"
    #include "logmanager.h"
    #include "macrosmanager.h"
    #include "pluginmanager.h"
    #include "projectmanager.h"
    #include "pluginmanager.h"
    #include "scriptingmanager.h"
    #include "compilerfactory.h"
    #include "globals.h"
    #include "cbproject.h"
    #include "cbplugin.h"
    #include "sdk_events.h"

    #include <wx/button.h>
    #include <wx/checkbox.h>
    #include <wx/checklst.h>
    #include <wx/combobox.h>
    #include <wx/filedlg.h>
    #include <wx/filefn.h> // wxMatchWild
    #include <wx/notebook.h>
    #include <wx/sizer.h>
    #include <wx/spinctrl.h>
    #include <wx/stattext.h>
    #include <wx/treectrl.h>
    #include <wx/xrc/xmlres.h>
#endif

#include <wx/radiobox.h>

#include "scripting/sqplus/sqplus.h"

#include "annoyingdialog.h"
#include "configurationpanel.h"
#include "editarrayorderdlg.h"
#include "editarrayfiledlg.h"
#include "editpathdlg.h"
#include "externaldepsdlg.h"
#include "filefilters.h"
#include "multiselectdlg.h"
#include "projectdepsdlg.h"
#include "projectloader.h"
#include "projectoptionsdlg.h" // class's header file
#include "virtualbuildtargetsdlg.h"


BEGIN_EVENT_TABLE(ProjectOptionsDlg, wxScrollingDialog)
    EVT_UPDATE_UI( -1,                                          ProjectOptionsDlg::OnUpdateUI)
    EVT_BUTTON(    XRCID("wxID_OK"),                            ProjectOptionsDlg::OnOK)
    EVT_BUTTON(    XRCID("btnPlatform"),                        ProjectOptionsDlg::OnPlatform)
    EVT_BUTTON(    XRCID("btnPlatformProj"),                    ProjectOptionsDlg::OnPlatform)
    EVT_BUTTON(    XRCID("btnProjectBuildOptions"),             ProjectOptionsDlg::OnProjectBuildOptionsClick)
    EVT_BUTTON(    XRCID("btnProjectDeps"),                     ProjectOptionsDlg::OnProjectDepsClick)
    EVT_BUTTON(    XRCID("btnTargetBuildOptions"),              ProjectOptionsDlg::OnTargetBuildOptionsClick)
    EVT_LISTBOX_DCLICK(XRCID("lstBuildTarget"),                 ProjectOptionsDlg::OnTargetBuildOptionsClick)
    EVT_BUTTON(    XRCID("btnBuildOrder"),                      ProjectOptionsDlg::OnBuildOrderClick)
    EVT_BUTTON(    XRCID("btnAddBuildTarget"),                  ProjectOptionsDlg::OnAddBuildTargetClick)
    EVT_BUTTON(    XRCID("btnEditBuildTarget"),                 ProjectOptionsDlg::OnEditBuildTargetClick)
    EVT_BUTTON(    XRCID("btnCopyBuildTarget"),                 ProjectOptionsDlg::OnCopyBuildTargetClick)
    EVT_BUTTON(    XRCID("btnDelBuildTarget"),                  ProjectOptionsDlg::OnRemoveBuildTargetClick)
    EVT_BUTTON(    XRCID("btnBrowseOutputFilename"),            ProjectOptionsDlg::OnBrowseOutputFilenameClick)
    EVT_BUTTON(    XRCID("btnBrowseImportLibraryFilename"),     ProjectOptionsDlg::OnBrowseImportLibraryFilenameClick)
    EVT_BUTTON(    XRCID("btnBrowseDefinitionFileFilename"),    ProjectOptionsDlg::OnBrowseDefinitionFileFilenameClick)
    EVT_BUTTON(    XRCID("btnBrowseWorkingDir"),                ProjectOptionsDlg::OnBrowseDirClick)
    EVT_BUTTON(    XRCID("btnBrowseObjectDir"),                 ProjectOptionsDlg::OnBrowseDirClick)
    EVT_BUTTON(    XRCID("btnExecutionDir"),                    ProjectOptionsDlg::OnBrowseDirClick)
    EVT_BUTTON(    XRCID("btnVirtualBuildTargets"),             ProjectOptionsDlg::OnVirtualTargets)
    EVT_BUTTON(    XRCID("btnExternalDeps"),                    ProjectOptionsDlg::OnEditDepsClick)
    EVT_BUTTON(    XRCID("btnExportTarget"),                    ProjectOptionsDlg::OnExportTargetClick)
    EVT_LISTBOX_DCLICK(XRCID("lstFiles"),                       ProjectOptionsDlg::OnFileOptionsClick)
    EVT_BUTTON(    XRCID("btnFileOptions"),                     ProjectOptionsDlg::OnFileOptionsClick)
    EVT_BUTTON(    XRCID("btnToggleCheckmarks"),                ProjectOptionsDlg::OnFileToggleMarkClick)
    EVT_BUTTON(    XRCID("btnCheckmarksOn"),                    ProjectOptionsDlg::OnFileMarkOnClick)
    EVT_BUTTON(    XRCID("btnCheckmarksOff"),                   ProjectOptionsDlg::OnFileMarkOffClick)
    EVT_LISTBOX(   XRCID("lstBuildTarget"),                     ProjectOptionsDlg::OnBuildTargetChanged)
    EVT_COMBOBOX(  XRCID("cmbProjectType"),                     ProjectOptionsDlg::OnProjectTypeChanged)
    EVT_CHECKBOX(  XRCID("chkCreateStaticLib"),                 ProjectOptionsDlg::OnCreateImportFileClick)
    EVT_CHECKBOX(  XRCID("chkCreateDefFile"),                   ProjectOptionsDlg::OnCreateDefFileClick)

    EVT_TREE_SEL_CHANGED(XRCID("tcOverview"),                   ProjectOptionsDlg::OnScriptsOverviewSelChanged)
    EVT_BUTTON(XRCID("btnCheckScripts"),                        ProjectOptionsDlg::OnCheckScripts)
    EVT_BUTTON(XRCID("btnAddPreScripts"),                       ProjectOptionsDlg::OnAddScript)
    EVT_BUTTON(XRCID("btnRemovePreScripts"),                    ProjectOptionsDlg::OnRemoveScript)
    EVT_SPIN_UP(XRCID("spnPreScripts"),                         ProjectOptionsDlg::OnScriptMoveUp)
    EVT_SPIN_DOWN(XRCID("spnPreScripts"),                       ProjectOptionsDlg::OnScriptMoveDown)
END_EVENT_TABLE()

// class constructor
ProjectOptionsDlg::ProjectOptionsDlg(wxWindow* parent, cbProject* project)
    : m_Project(project),
    m_Current_Sel(-1),
    m_pCompiler(nullptr)
{
    wxXmlResource::Get()->LoadObject(this, parent, _T("dlgProjectOptions"),_T("wxScrollingDialog"));
    XRCCTRL(*this, "wxID_OK", wxButton)->SetDefault();

    wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);

    for (int i=0; i< m_Project->GetFilesCount(); ++i)
        list->Append(m_Project->GetFile(i)->relativeFilename);

    // this fixes the minsize of the file list
    // which becomes huge when we add items in it (!)
    list->SetMinSize(wxSize(50,50));

    // general
    XRCCTRL(*this, "txtProjectName", wxTextCtrl)->SetValue(m_Project->GetTitle());
    XRCCTRL(*this, "txtProjectFilename", wxStaticText)->SetLabel(m_Project->GetFilename());
    XRCCTRL(*this, "txtPlatformProj", wxTextCtrl)->SetValue(GetStringFromPlatforms(m_Project->GetPlatforms()));
    XRCCTRL(*this, "txtProjectMakefile", wxTextCtrl)->SetValue(m_Project->GetMakefile());
    XRCCTRL(*this, "chkCustomMakefile", wxCheckBox)->SetValue(m_Project->IsMakefileCustom());
    XRCCTRL(*this, "txtExecutionDir", wxTextCtrl)->SetValue(m_Project->GetMakefileExecutionDir());
    XRCCTRL(*this, "rbPCHStrategy", wxRadioBox)->SetSelection((int)m_Project->GetModeForPCH());

    Compiler* compiler = CompilerFactory::GetCompiler(project->GetCompilerID());
    bool hasPCH = compiler && compiler->GetSwitches().supportsPCH;
    XRCCTRL(*this, "rbPCHStrategy", wxRadioBox)->Enable(hasPCH);

    XRCCTRL(*this, "chkExtendedObjNames", wxCheckBox)->SetValue(m_Project->GetExtendedObjectNamesGeneration());

    XRCCTRL(*this, "chkShowNotes", wxCheckBox)->SetValue(m_Project->GetShowNotesOnLoad());
    XRCCTRL(*this, "txtNotes", wxTextCtrl)->SetValue(m_Project->GetNotes());

    XRCCTRL(*this, "chkCheckFiles", wxCheckBox)->SetValue(m_Project->GetCheckForExternallyModifiedFiles());

    FillBuildTargets();

    PluginsArray plugins = Manager::Get()->GetPluginManager()->GetCompilerOffers();
    if (plugins.GetCount())
        m_pCompiler = (cbCompilerPlugin*)plugins[0];

    XRCCTRL(*this, "txtNotes", wxTextCtrl)->Connect(wxEVT_KEY_DOWN,wxKeyEventHandler(ProjectOptionsDlg::OnKeyDown),nullptr,this);

    // scripts
    BuildScriptsTree();

    // make sure everything is laid out properly
    // before adding panels from plugins
    // we don't want the dialog to become huge ;)
    // note that a similar situation in editor settings had been solved by commenting
    // the following line out...
    GetSizer()->SetSizeHints(this);

    // other plugins configuration
    AddPluginPanels();

    CentreOnParent();
}

// class destructor
ProjectOptionsDlg::~ProjectOptionsDlg()
{
    // insert your code here
    XRCCTRL(*this, "txtNotes", wxTextCtrl)->Disconnect(wxEVT_KEY_DOWN,wxKeyEventHandler(ProjectOptionsDlg::OnKeyDown),nullptr,this);
}

void ProjectOptionsDlg::OnKeyDown(wxKeyEvent& event)
{
    if (event.ControlDown())
    {
        if (event.GetKeyCode() == 'J')
        {
            wxString buffer = XRCCTRL(*this, "txtNotes", wxTextCtrl)->GetValue();
            Manager::Get()->GetMacrosManager()->ReplaceMacros(buffer,nullptr);
            XRCCTRL(*this, "txtNotes", wxTextCtrl)->SetValue(buffer);
        }
    }
    event.Skip();
}

void ProjectOptionsDlg::BuildScriptsTree()
{
    wxTreeCtrl* tc = XRCCTRL(*this, "tcOverview", wxTreeCtrl);
    tc->DeleteAllItems();

    wxTreeItemId root = tc->AddRoot(m_Project->GetTitle());
    for (int x = 0; x < m_Project->GetBuildTargetsCount(); ++x)
    {
        ProjectBuildTarget* target = m_Project->GetBuildTarget(x);
        tc->AppendItem(root, target->GetTitle());
    }
    tc->Expand(root);
    tc->SelectItem(root);
    FillScripts();
}

void ProjectOptionsDlg::AddPluginPanels()
{
    wxNotebook* nb = XRCCTRL(*this, "nbMain", wxNotebook);

    Manager::Get()->GetPluginManager()->GetProjectConfigurationPanels(nb, m_Project, m_PluginPanels);

    for (size_t i = 0; i < m_PluginPanels.GetCount(); ++i)
    {
        cbConfigurationPanel* panel = m_PluginPanels[i];
        panel->SetParentDialog(this);
        nb->AddPage(panel, panel->GetTitle());
    }
}

void ProjectOptionsDlg::FillScripts()
{
    wxTreeCtrl* tc = XRCCTRL(*this, "tcOverview", wxTreeCtrl);
    wxTreeItemId sel = tc->GetSelection();
    CompileOptionsBase* base = sel == tc->GetRootItem()
                                ? static_cast<CompileOptionsBase*>(m_Project)
                                : static_cast<CompileOptionsBase*>(m_Project->GetBuildTarget(tc->GetItemText(sel)));

    wxListBox* lstScripts = XRCCTRL(*this, "lstPreScripts", wxListBox);
    lstScripts->Clear();
    for (size_t i = 0; i < base->GetBuildScripts().GetCount(); ++i)
        lstScripts->Append(base->GetBuildScripts().Item(i));
}

void ProjectOptionsDlg::FillBuildTargets()
{
    // add build targets
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    lstTargets->Clear();
    for (int i = 0; i < m_Project->GetBuildTargetsCount(); ++i)
        lstTargets->Append(m_Project->GetBuildTarget(i)->GetTitle());
    lstTargets->SetSelection(0);
    DoTargetChange(false);
}

void ProjectOptionsDlg::DoTargetChange(bool saveOld)
{
    if (saveOld)
        DoBeforeTargetChange();

    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);

    if (lstTargets->GetSelection() == -1)
        lstTargets->SetSelection(0);
    ProjectBuildTarget* target = m_Project->GetBuildTarget(lstTargets->GetSelection());
    if (!target)
        return;

    // global project options
    wxTextCtrl* txtP = XRCCTRL(*this, "txtPlatform", wxTextCtrl);
    wxComboBox* cmb = XRCCTRL(*this, "cmbProjectType", wxComboBox);
    wxCheckBox* chkCR = XRCCTRL(*this, "chkUseConsoleRunner", wxCheckBox);
    wxCheckBox* chkSL = XRCCTRL(*this, "chkCreateStaticLib", wxCheckBox);
    wxCheckBox* chkCD = XRCCTRL(*this, "chkCreateDefFile", wxCheckBox);
    wxTextCtrl* txt = XRCCTRL(*this, "txtOutputFilename", wxTextCtrl);
    wxTextCtrl* txtI = XRCCTRL(*this, "txtImportLibraryFilename", wxTextCtrl);
    wxTextCtrl* txtD = XRCCTRL(*this, "txtDefinitionFileFilename", wxTextCtrl);
    wxTextCtrl* txtW = XRCCTRL(*this, "txtWorkingDir", wxTextCtrl);
    wxTextCtrl* txtO = XRCCTRL(*this, "txtObjectDir", wxTextCtrl);
    wxButton* browse = XRCCTRL(*this, "btnBrowseOutputFilename", wxButton);
    wxButton* browseI = XRCCTRL(*this, "btnBrowseImportLibraryFilename", wxButton);
    wxButton* browseD = XRCCTRL(*this, "btnBrowseDefinitionFileFilename", wxButton);
    wxButton* browseW = XRCCTRL(*this, "btnBrowseWorkingDir", wxButton);
    wxButton* browseO = XRCCTRL(*this, "btnBrowseObjectDir", wxButton);

    wxString platforms = GetStringFromPlatforms(target->GetPlatforms());
    txtP->SetValue(platforms);

    chkCR->SetValue(false);
    chkCD->SetValue(target->GetCreateDefFile());
    chkSL->SetValue(target->GetCreateStaticLib());

    TargetFilenameGenerationPolicy prefixPolicy;
    TargetFilenameGenerationPolicy extensionPolicy;
    target->GetTargetFilenameGenerationPolicy(prefixPolicy, extensionPolicy);
    XRCCTRL(*this, "chkAutoGenPrefix", wxCheckBox)->SetValue(prefixPolicy == tgfpPlatformDefault);
    XRCCTRL(*this, "chkAutoGenExt", wxCheckBox)->SetValue(extensionPolicy == tgfpPlatformDefault);

    chkCR->Enable(false);
    chkSL->Enable(target->GetTargetType() == ttDynamicLib);
    chkCD->Enable(target->GetTargetType() == ttDynamicLib);
    if (cmb && chkCR && txt && browse)
    {
        cmb->SetSelection(target->GetTargetType());
//        Compiler* compiler = CompilerFactory::Compilers[target->GetCompilerIndex()];
        switch ((TargetType)cmb->GetSelection())
        {
            case ttConsoleOnly:
                chkCR->Enable(true);
                chkCR->SetValue(target->GetUseConsoleRunner());
                // purposely fall-through

            case ttExecutable:
            case ttDynamicLib:
            case ttNative:
            case ttStaticLib:
                txt->SetValue(target->GetOutputFilename());
                txt->Enable(true);
                txtI->SetValue(target->GetDynamicLibImportFilename());
                txtI->Enable(chkSL->IsChecked() && (TargetType)cmb->GetSelection() == ttDynamicLib);
                txtD->SetValue(target->GetDynamicLibDefFilename());
                txtD->Enable(chkCD->IsChecked() && (TargetType)cmb->GetSelection() == ttDynamicLib);
                txtW->SetValue(target->GetWorkingDir());
                txtW->Enable((TargetType)cmb->GetSelection() == ttExecutable ||
                            (TargetType)cmb->GetSelection() == ttConsoleOnly ||
                            (TargetType)cmb->GetSelection() == ttNative ||
                            (TargetType)cmb->GetSelection() == ttDynamicLib);
                txtO->SetValue(target->GetObjectOutput());
                txtO->Enable(true);
                browse->Enable(true);
                browseI->Enable(chkSL->IsChecked() && (TargetType)cmb->GetSelection() == ttDynamicLib);
                browseD->Enable(chkCD->IsChecked() && (TargetType)cmb->GetSelection() == ttDynamicLib);
                browseW->Enable((TargetType)cmb->GetSelection() == ttExecutable ||
                                (TargetType)cmb->GetSelection() == ttConsoleOnly ||
                                (TargetType)cmb->GetSelection() == ttNative ||
                                (TargetType)cmb->GetSelection() == ttDynamicLib);
                browseO->Enable(true);
                break;

            case ttCommandsOnly: // fall-through
            default: // for commands-only targets
                txt->SetValue(_T(""));
                txt->Enable(false);
                txtI->SetValue(_T(""));
                txtI->Enable(false);
                txtD->SetValue(_T(""));
                txtD->Enable(false);
                txtW->SetValue(_T(""));
                txtW->Enable(false);
                txtO->SetValue(_T(""));
                txtO->Enable(false);
                browse->Enable(false);
                browseI->Enable(false);
                browseD->Enable(false);
                browseW->Enable(false);
                browseO->Enable(false);
                break;
        }
    }

    // files options
    wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);
    int count = list->GetCount();
    for (int i = 0; i < count; ++i)
    {
        ProjectFile* pf = m_Project->GetFileByFilename(list->GetString(i));
        if (!pf)
            break;

        bool doit = pf->buildTargets.Index(target->GetTitle()) >= 0;
        list->Check(i, doit);
    }

    // update currently selected target
    m_Current_Sel = lstTargets->GetSelection();
}

void ProjectOptionsDlg::DoBeforeTargetChange(bool force)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);

    // if no previously selected target, exit
    if (m_Current_Sel == -1)
        return;

    if (force || lstTargets->GetSelection() != m_Current_Sel)
    {
        // selected another build target
        // save changes to the previously selected target
        wxArrayString array;
        ProjectBuildTarget* target = m_Project->GetBuildTarget(m_Current_Sel);
        if (!target)
            return;

        wxString platforms = XRCCTRL(*this, "txtPlatform", wxTextCtrl)->GetValue();
        int p = GetPlatformsFromString(platforms);
        target->SetPlatforms(p);

        target->SetTargetFilenameGenerationPolicy(
            XRCCTRL(*this, "chkAutoGenPrefix", wxCheckBox)->GetValue() ? tgfpPlatformDefault : tgfpNone,
            XRCCTRL(*this, "chkAutoGenExt", wxCheckBox)->GetValue() ? tgfpPlatformDefault : tgfpNone);

        target->SetTargetType(TargetType(XRCCTRL(*this, "cmbProjectType", wxComboBox)->GetSelection()));
        wxFileName fname(XRCCTRL(*this, "txtOutputFilename", wxTextCtrl)->GetValue());
        target->SetOutputFilename(fname.GetFullPath());

        fname.Assign(XRCCTRL(*this, "txtImportLibraryFilename", wxTextCtrl)->GetValue());
        target->SetImportLibraryFilename(fname.GetFullPath());

        fname.Assign(XRCCTRL(*this, "txtDefinitionFileFilename", wxTextCtrl)->GetValue());
        target->SetDefinitionFileFilename(fname.GetFullPath());

        fname.Assign(XRCCTRL(*this, "txtWorkingDir", wxTextCtrl)->GetValue());
        target->SetWorkingDir(fname.GetFullPath());

        fname.Assign(XRCCTRL(*this, "txtObjectDir", wxTextCtrl)->GetValue());
        target->SetObjectOutput(fname.GetFullPath());

        target->SetUseConsoleRunner(XRCCTRL(*this, "chkUseConsoleRunner", wxCheckBox)->GetValue());
        target->SetCreateDefFile(XRCCTRL(*this, "chkCreateDefFile", wxCheckBox)->GetValue());
        target->SetCreateStaticLib(XRCCTRL(*this, "chkCreateStaticLib", wxCheckBox)->GetValue());

        // files options
        wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);
        size_t count = list->GetCount();
        for (size_t i = 0; i < count; ++i)
        {
            ProjectFile* pf = m_Project->GetFileByFilename(list->GetString(i));
            if (!pf)
                break;

            if (list->IsChecked(i))
                pf->AddBuildTarget(target->GetTitle());
            else
                pf->RemoveBuildTarget(target->GetTitle());
        }
    }
}

// events

void ProjectOptionsDlg::OnProjectTypeChanged(cb_unused wxCommandEvent& event)
{
    ProjectBuildTarget* target = m_Project->GetBuildTarget(m_Current_Sel);
    if (!target)
        return;

    wxComboBox* cmb = XRCCTRL(*this, "cmbProjectType", wxComboBox);
    wxCheckBox* chkSL = XRCCTRL(*this, "chkCreateStaticLib", wxCheckBox);
    wxCheckBox* chkCD = XRCCTRL(*this, "chkCreateDefFile", wxCheckBox);
    wxTextCtrl* txt = XRCCTRL(*this, "txtOutputFilename", wxTextCtrl);
    wxTextCtrl* txtI = XRCCTRL(*this, "txtImportLibraryFilename", wxTextCtrl);
    wxTextCtrl* txtD = XRCCTRL(*this, "txtDefinitionFileFilename", wxTextCtrl);
    wxTextCtrl* txtW = XRCCTRL(*this, "txtWorkingDir", wxTextCtrl);
    wxTextCtrl* txtO = XRCCTRL(*this, "txtObjectDir", wxTextCtrl);
    wxButton* browse = XRCCTRL(*this, "btnBrowseOutputFilename", wxButton);
    wxButton* browseI = XRCCTRL(*this, "btnBrowseImportLibraryFilename", wxButton);
    wxButton* browseD = XRCCTRL(*this, "btnBrowseDefinitionFileFilename", wxButton);
    wxButton* browseW = XRCCTRL(*this, "btnBrowseWorkingDir", wxButton);
    wxButton* browseO = XRCCTRL(*this, "btnBrowseObjectDir", wxButton);
    if (!cmb || !txt || !browse)
        return;

    XRCCTRL(*this, "chkUseConsoleRunner", wxCheckBox)->Enable(cmb->GetSelection() == ttConsoleOnly);
    XRCCTRL(*this, "chkCreateDefFile", wxCheckBox)->Enable(cmb->GetSelection() == ttDynamicLib);
    XRCCTRL(*this, "chkCreateStaticLib", wxCheckBox)->Enable(cmb->GetSelection() == ttDynamicLib);

    txt->Enable(true);
    txtI->Enable(chkSL->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    txtD->Enable(chkCD->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    txtW->SetValue(target->GetWorkingDir());
    txtW->Enable((TargetType)cmb->GetSelection() == ttExecutable ||
                (TargetType)cmb->GetSelection() == ttConsoleOnly ||
                (TargetType)cmb->GetSelection() == ttDynamicLib);
    txtO->Enable(true);
    txtO->SetValue(target->GetObjectOutput());
    browse->Enable(true);
    browseI->Enable(chkSL->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    browseD->Enable(chkCD->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    browseW->Enable((TargetType)cmb->GetSelection() == ttExecutable ||
                    (TargetType)cmb->GetSelection() == ttConsoleOnly ||
                    (TargetType)cmb->GetSelection() == ttDynamicLib);
    browseO->Enable(true);

    Compiler* compiler = CompilerFactory::GetCompiler(target->GetCompilerID());

    wxFileName fname = target->GetOutputFilename();
    wxFileName fnameI = target->GetDynamicLibImportFilename();
    wxFileName fnameD = target->GetDynamicLibDefFilename();
    wxString name = fname.GetName();
    wxString nameI = fnameI.GetName();
    wxString nameD = fnameD.GetName();
    wxString ext = fname.GetExt();
    wxString extI = fname.GetExt();
    wxString extD = fname.GetExt();
    wxString libext = compiler ? compiler->GetSwitches().libExtension : _T("");
    wxString libextI = compiler ? compiler->GetSwitches().libExtension : _T(""); // TODO: add specialized compiler option for this
    wxString libextD = _T("def");                                                // TODO: add specialized compiler option for this
    wxString libpre = compiler ? compiler->GetSwitches().libPrefix : _T("");
    wxString libpreI = compiler ? compiler->GetSwitches().libPrefix : _T("");    // TODO: add specialized compiler option for this
    wxString libpreD = compiler ? compiler->GetSwitches().libPrefix : _T("");    // TODO: add specialized compiler option for this

    switch ((TargetType)cmb->GetSelection())
    {
        case ttConsoleOnly:
        case ttExecutable:
            if (ext != FileFilters::EXECUTABLE_EXT)
                fname.SetExt(FileFilters::EXECUTABLE_EXT);
            if (!libpre.IsEmpty() && name.StartsWith(libpre))
            {
                name.Remove(0, libpre.Length());
                fname.SetName(name);
            }
            txt->SetValue(fname.GetFullPath());
            txtI->SetValue(_T(""));
            txtD->SetValue(_T(""));
            break;
        case ttDynamicLib:
            if (ext != FileFilters::DYNAMICLIB_EXT)
                fname.SetExt(FileFilters::DYNAMICLIB_EXT);
            if (extI != FileFilters::STATICLIB_EXT)
                fnameI.SetExt(FileFilters::STATICLIB_EXT);
            if (extD != _T("def"))
                fnameD.SetExt(_T("def"));
            if (!libpre.IsEmpty() && name.StartsWith(libpre))
            {
                name.Remove(0, libpre.Length());
                fname.SetName(name);
            }
            if (!libpreI.IsEmpty() && nameI.StartsWith(libpreI))
            {
                nameI.Remove(0, libpreI.Length());
                fnameI.SetName(nameI);
            }
            if (!libpreD.IsEmpty() && nameD.StartsWith(libpreD))
            {
                nameD.Remove(0, libpreD.Length());
                fnameD.SetName(nameD);
            }
            txt->SetValue(fname.GetFullPath());
            txtI->SetValue(fnameI.GetFullPath());
            txtD->SetValue(fnameD.GetFullPath());
            break;
        case ttStaticLib:
            if (ext != libext)
                fname.SetExt(libext);
            if (!libpre.IsEmpty() && !name.StartsWith(libpre))
            {
                name.Prepend(libpre);
                fname.SetName(name);
            }
            txt->SetValue(fname.GetFullPath());
            txtI->SetValue(_T(""));
            txtD->SetValue(_T(""));
            break;
        case ttNative:
            if (ext != FileFilters::NATIVE_EXT)
                fname.SetExt(FileFilters::NATIVE_EXT);
            if (!libpre.IsEmpty() && name.StartsWith(libpre))
            {
                name.Remove(0, libpre.Length());
                fname.SetName(name);
            }
            txt->SetValue(fname.GetFullPath());
            txtI->SetValue(_T(""));
            txtD->SetValue(_T(""));
            break;
        case ttCommandsOnly: // fall-through
        default:
            txt->SetValue(_T(""));
            txtI->SetValue(_T(""));
            txtD->SetValue(_T(""));
            txtW->SetValue(_T(""));
            txtO->SetValue(_T(""));
            txt->Enable(false);
            txtI->Enable(false);
            txtD->Enable(false);
            txtW->Enable(false);
            txtO->Enable(false);
            browse->Enable(false);
            browseI->Enable(false);
            browseD->Enable(false);
            browseW->Enable(false);
            browseO->Enable(false);
            break;
    }
}

void ProjectOptionsDlg::OnBuildTargetChanged(cb_unused wxCommandEvent& event)
{
    DoTargetChange();
    CodeBlocksEvent e(cbEVT_PROJECT_TARGETS_MODIFIED);
    e.SetProject(m_Project);
    Manager::Get()->GetPluginManager()->NotifyPlugins(e);
}

void ProjectOptionsDlg::OnBuildOrderClick(cb_unused wxCommandEvent& event)
{
    wxArrayString array;
    for (int i = 0; i < m_Project->GetBuildTargetsCount(); ++i)
        array.Add(m_Project->GetBuildTarget(i)->GetTitle());

    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    wxString ActiveTarget;
    if (lstTargets->GetSelection() != wxNOT_FOUND)
        ActiveTarget = lstTargets->GetString(lstTargets->GetSelection());

    EditArrayOrderDlg dlg(this, array);
    PlaceWindow(&dlg);
    if (dlg.ShowModal() == wxID_OK)
    {
        // Save changes in current target.
        DoBeforeTargetChange(true);
        m_Project->ReOrderTargets(dlg.GetArray());
        FillBuildTargets();

        // Retrieve active target.
        if (!ActiveTarget.IsEmpty())
            lstTargets->SetSelection(lstTargets->FindString(ActiveTarget, true));
        DoTargetChange(false);

        CodeBlocksEvent e(cbEVT_PROJECT_TARGETS_MODIFIED);
        e.SetProject(m_Project);
        Manager::Get()->GetPluginManager()->NotifyPlugins(e);
    }
}

void ProjectOptionsDlg::OnProjectDepsClick(cb_unused wxCommandEvent& event)
{
    Manager::Get()->GetProjectManager()->GetUI().ConfigureProjectDependencies(m_Project);
}

void ProjectOptionsDlg::OnProjectBuildOptionsClick(cb_unused wxCommandEvent& event)
{
    if (m_pCompiler)
        m_pCompiler->Configure(m_Project);
}

void ProjectOptionsDlg::OnTargetBuildOptionsClick(cb_unused wxCommandEvent& event)
{
    if (m_pCompiler)
    {
        wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
        int targetIdx = lstTargets->GetSelection();

        ProjectBuildTarget* target = m_Project->GetBuildTarget(targetIdx);
        if (target)
            m_pCompiler->Configure(m_Project, target);
    }
}

void ProjectOptionsDlg::OnAddBuildTargetClick(cb_unused wxCommandEvent& event)
{
    wxString targetName = cbGetTextFromUser(_("Enter the new build target name:"),
                                            _("New build target"));
    if (!ValidateTargetName(targetName))
        return;

    ProjectBuildTarget* target = m_Project->AddBuildTarget(targetName);
    if (!target)
    {
        cbMessageBox(_("The new target could not be added..."),
                     _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    // add target to targets combo
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    lstTargets->Append(targetName);
    lstTargets->SetSelection(lstTargets->GetCount() - 1);
    DoTargetChange();
    BuildScriptsTree();
    CodeBlocksEvent e(cbEVT_PROJECT_TARGETS_MODIFIED);
    e.SetProject(m_Project);
    Manager::Get()->GetPluginManager()->NotifyPlugins(e);
}

void ProjectOptionsDlg::OnEditBuildTargetClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    int targetIdx = lstTargets->GetSelection();

    ProjectBuildTarget* target = m_Project->GetBuildTarget(targetIdx);
    if (!target)
    {
        cbMessageBox(_("Could not locate target..."),
                     _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    wxString oldTargetName = target->GetTitle();
    wxString newTargetName = cbGetTextFromUser(_("Change the build target name:"),
                                               _("Rename build target"),
                                              oldTargetName);
    if (newTargetName == oldTargetName || !ValidateTargetName(newTargetName))
        return;

    m_Project->RenameBuildTarget(targetIdx, newTargetName);
    lstTargets->SetString(targetIdx, newTargetName);
    lstTargets->SetSelection(targetIdx);
    BuildScriptsTree();
    CodeBlocksEvent e(cbEVT_PROJECT_TARGETS_MODIFIED);
    e.SetProject(m_Project);
    Manager::Get()->GetPluginManager()->NotifyPlugins(e);
}

void ProjectOptionsDlg::OnCopyBuildTargetClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    int targetIdx = lstTargets->GetSelection();

    ProjectBuildTarget* target = m_Project->GetBuildTarget(targetIdx);
    if (!target)
    {
        cbMessageBox(_("Could not locate target..."),
                     _("Error"), wxOK | wxICON_ERROR, this);
        return;
    }

    wxString newTargetName = cbGetTextFromUser(_("Enter the duplicated build target's name:"),
                                               _("Duplicate build target"),
                                              _("Copy of ") + target->GetTitle());
    if (!ValidateTargetName(newTargetName))
        return;
    if (!m_Project->DuplicateBuildTarget(targetIdx, newTargetName))
    {
        cbMessageBox(_("Failed to duplicate this build target..."), _("Error"), wxICON_ERROR, this);
        return;
    }

    // add target to targets combo
    lstTargets->Append(newTargetName);
    lstTargets->SetSelection(lstTargets->GetCount() - 1);
    DoTargetChange();
    BuildScriptsTree();
    CodeBlocksEvent e(cbEVT_PROJECT_TARGETS_MODIFIED);
    e.SetProject(m_Project);
    Manager::Get()->GetPluginManager()->NotifyPlugins(e);
}

void ProjectOptionsDlg::OnRemoveBuildTargetClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    int targetIdx = lstTargets->GetSelection();
    if (targetIdx == wxNOT_FOUND)
    {
        Manager::Get()->GetLogManager()->Log(_("No target removed, because nothing is selected in the Build target list box!"));
        return;
    }

    wxString caption;
    caption.Printf(_("Are you sure you want to delete build target \"%s\"?"), lstTargets->GetStringSelection().c_str());
    if (cbMessageBox(caption, _("Confirmation"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, this) == wxID_NO)
        return;

    lstTargets->Delete(targetIdx);
    if (lstTargets->GetCount() > 0)
    {
        if (static_cast<unsigned>(targetIdx) >= lstTargets->GetCount())
            lstTargets->SetSelection(lstTargets->GetCount() - 1);
        else
            lstTargets->SetSelection(targetIdx);
    }
    // the target name in the units is changed by the project...
    m_Project->RemoveBuildTarget(targetIdx);
    m_Current_Sel = -1;
    DoTargetChange();
    BuildScriptsTree();
    CodeBlocksEvent e(cbEVT_PROJECT_TARGETS_MODIFIED);
    e.SetProject(m_Project);
    Manager::Get()->GetPluginManager()->NotifyPlugins(e);
}

void ProjectOptionsDlg::OnVirtualTargets(cb_unused wxCommandEvent& event)
{
    VirtualBuildTargetsDlg dlg(this, -1, m_Project);
    dlg.ShowModal();
}

void ProjectOptionsDlg::OnEditDepsClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    ProjectBuildTarget* target = m_Project->GetBuildTarget(lstTargets->GetSelection());
    if (!target)
        return;

    ExternalDepsDlg dlg(this, m_Project, target);
    PlaceWindow(&dlg);
    dlg.ShowModal();
}

void ProjectOptionsDlg::OnExportTargetClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    ProjectBuildTarget* target = m_Project->GetBuildTarget(lstTargets->GetSelection());
    if (!target)
        return;

    AnnoyingDialog dlg(_("Create project from target confirmation"),
                        _("This project will be saved before exporting the build target.\n"
                        "Are you sure you want to export the selected "
                        "build target to a new project?"),
                        wxART_QUESTION);
    if (dlg.ShowModal() == AnnoyingDialog::rtYES)
    {
        if (m_Project->ExportTargetAsProject(target->GetTitle()))
            cbMessageBox(_("New project created successfully!"), _("Information"), wxICON_INFORMATION, this);
    }
}

void ProjectOptionsDlg::OnBrowseDirClick(wxCommandEvent& event)
{
    wxTextCtrl* targettext = nullptr;
    if (event.GetId() == XRCID("btnBrowseWorkingDir"))
        targettext = XRCCTRL(*this, "txtWorkingDir", wxTextCtrl);
    else if (event.GetId() == XRCID("btnBrowseObjectDir"))
        targettext = XRCCTRL(*this, "txtObjectDir", wxTextCtrl);
    else if (event.GetId() == XRCID("btnExecutionDir"))
        targettext = XRCCTRL(*this, "txtExecutionDir", wxTextCtrl);
    else
        return;

    wxFileName fname(targettext->GetValue() + wxFileName::GetPathSeparator());
    fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_Project->GetBasePath());

    wxString path = ChooseDirectory(this,
                                    _("Select directory"),
                                    fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR),
                                    m_Project->GetBasePath(),
                                    true,
                                    true);
    if (path.IsEmpty())
        return;

    fname.Assign(path);
    targettext->SetValue(path);
}

void ProjectOptionsDlg::OnBrowseOutputFilenameClick(cb_unused wxCommandEvent& event)
{
    wxFileName fname;
    fname.Assign(XRCCTRL(*this, "txtOutputFilename", wxTextCtrl)->GetValue());
    fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_Project->GetBasePath());
    wxFileDialog dlg(this,
                     _("Select output filename"),
                     fname.GetPath(),
                     fname.GetFullName(),
                     FileFilters::GetFilterAll(),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    fname.Assign(dlg.GetPath());
    fname.MakeRelativeTo(m_Project->GetBasePath());
    XRCCTRL(*this, "txtOutputFilename", wxTextCtrl)->SetValue(fname.GetFullPath());
}

void ProjectOptionsDlg::OnBrowseImportLibraryFilenameClick(cb_unused wxCommandEvent& event)
{
    wxFileName fname;
    fname.Assign(XRCCTRL(*this, "txtImportLibraryFilename", wxTextCtrl)->GetValue());
    fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_Project->GetBasePath());
    wxFileDialog dlg(this,
                     _("Select import library filename"),
                     fname.GetPath(),
                     fname.GetFullName(),
                     FileFilters::GetFilterAll(),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    fname.Assign(dlg.GetPath());
    fname.MakeRelativeTo(m_Project->GetBasePath());
    XRCCTRL(*this, "txtImportLibraryFilename", wxTextCtrl)->SetValue(fname.GetFullPath());
}

void ProjectOptionsDlg::OnBrowseDefinitionFileFilenameClick(cb_unused wxCommandEvent& event)
{
    wxFileName fname;
    fname.Assign(XRCCTRL(*this, "txtDefinitionFileFilename", wxTextCtrl)->GetValue());
    fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_Project->GetBasePath());
    wxFileDialog dlg(this,
                     _("Select definition file filename"),
                     fname.GetPath(),
                     fname.GetFullName(),
                     FileFilters::GetFilterAll(),
                     wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;
    fname.Assign(dlg.GetPath());
    fname.MakeRelativeTo(m_Project->GetBasePath());
    XRCCTRL(*this, "txtDefinitionFileFilename", wxTextCtrl)->SetValue(fname.GetFullPath());
}

void ProjectOptionsDlg::OnFileOptionsClick(cb_unused wxCommandEvent& event)
{
    wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);

    if (list->GetSelection() >= 0)
    {
        // show file options dialog
        ProjectFile* pf = m_Project->GetFileByFilename(list->GetString(list->GetSelection()));
        pf->ShowOptions(this);
    }
}

void ProjectOptionsDlg::OnFileToggleMarkClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    int targetIdx = lstTargets->GetSelection();
    ProjectBuildTarget* target = m_Project->GetBuildTarget(targetIdx);
    if (!target)
        return;

    wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);
    for (unsigned int i = 0; i < list->GetCount(); ++i)
    {
        ProjectFile* pf = m_Project->GetFileByFilename(list->GetString(i));
        if (pf)
        {
            list->Check(i, !list->IsChecked(i));
            if (list->IsChecked(i))
                pf->AddBuildTarget(target->GetTitle());
            else
                pf->RemoveBuildTarget(target->GetTitle());
        }
    }
}

void ProjectOptionsDlg::OnFileMarkOnClick(cb_unused wxCommandEvent& event)
{
    wxString wildcard = cbGetTextFromUser(_("Select wildcard (file mask) to toggle on:"),
                                          _("Select files"), _T("*.*"));
    if (wildcard.IsEmpty()) return; // user pressed Cancel

    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    int targetIdx = lstTargets->GetSelection();
    ProjectBuildTarget* target = m_Project->GetBuildTarget(targetIdx);
    if (!target)
        return;

    wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);
    for (unsigned int i=0; i<list->GetCount(); ++i)
    {
        ProjectFile* pf = m_Project->GetFileByFilename(list->GetString(i));
        if (pf && wxMatchWild(wildcard, list->GetString(i), false))
        {
            list->Check(i, true);
            pf->AddBuildTarget(target->GetTitle());
        }
    }
}

void ProjectOptionsDlg::OnFileMarkOffClick(cb_unused wxCommandEvent& event)
{
    wxString wildcard = cbGetTextFromUser(_("Select wildcard (file mask) to toggle off:"),
                                          _("Select files"), _T("*.*"));
    if (wildcard.IsEmpty()) return; // user pressed Cancel

    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    int targetIdx = lstTargets->GetSelection();
    ProjectBuildTarget* target = m_Project->GetBuildTarget(targetIdx);
    if (!target)
        return;

    wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);
    for (unsigned int i=0; i<list->GetCount(); ++i)
    {
        ProjectFile* pf = m_Project->GetFileByFilename(list->GetString(i));
        if (pf && wxMatchWild(wildcard, list->GetString(i), false))
        {
            list->Check(i, false);
            pf->RemoveBuildTarget(target->GetTitle());
        }
    }
}

void ProjectOptionsDlg::OnScriptsOverviewSelChanged(cb_unused wxTreeEvent& event)
{
    FillScripts();
}

bool ProjectOptionsDlg::IsScriptValid(ProjectBuildTarget* target, const wxString& script)
{
    static const wxString clearout_buildscripts = _T("SetBuildOptions <- null;");
    try
    {
        wxString script_nomacro = script;
        Manager::Get()->GetMacrosManager()->ReplaceMacros(script_nomacro, target);
        script_nomacro = wxFileName(script_nomacro).IsAbsolute() ? script_nomacro : m_Project->GetBasePath() + wxFILE_SEP_PATH + script_nomacro;
        Manager::Get()->GetScriptingManager()->LoadBuffer(clearout_buildscripts); // clear previous script's context
        Manager::Get()->GetScriptingManager()->LoadScript(script_nomacro);
        SqPlus::SquirrelFunction<void> setopts("SetBuildOptions");

        if (setopts.func.IsNull())
            return false;

        return true;
    }
    catch (SquirrelError& e)
    {
        Manager::Get()->GetScriptingManager()->DisplayErrors(&e);
        return false;
    }
}

bool ProjectOptionsDlg::ValidateTargetName(const wxString& name)
{
    if (name.IsEmpty())
        return false;

    if (m_Project->GetBuildTarget(name))
    {
        cbMessageBox(_("A target with this name already exists in this project!"),
                     _("Error"), wxOK | wxCENTRE | wxICON_ERROR, this);
        return false;
    }

    if (m_Project->HasVirtualBuildTarget(name))
    {
        cbMessageBox(_("A virtual target with this name already exists in this project!"),
                     _("Error"), wxOK | wxCENTRE | wxICON_ERROR, this);
        return false;
    }

    const wxString forbidden = _T(";,!@#$%^&*\"':`~=?\\><");
    if (name.find_first_of(forbidden, 0) != wxString::npos)
    {
        cbMessageBox(_("The name contains at least one invalid character:\n\n") + forbidden,
                     _("Error"), wxOK | wxCENTRE | wxICON_ERROR, this);
        return false;
    }

    return true;
}

bool ProjectOptionsDlg::DoCheckScripts(CompileTargetBase* base)
{
    const wxArrayString& scripts = base->GetBuildScripts();
    for (size_t i = 0; i < scripts.GetCount(); ++i)
    {
        ProjectBuildTarget* bt = dynamic_cast<ProjectBuildTarget*>(base);
        if (!IsScriptValid(bt, scripts[i]))
        {
            wxString msg;
            msg << _("Invalid build script: ") + scripts[i] << _T('\n');
            msg << _("First seen in: ") + base->GetTitle() << _T('\n');
            cbMessageBox(msg, _("Error"), wxICON_ERROR, this);
            return false;
        }
    }
    return true;
}

void ProjectOptionsDlg::OnCheckScripts(cb_unused wxCommandEvent& event)
{
    if (!DoCheckScripts(m_Project))
        return;

    for (int i = 0; i < m_Project->GetBuildTargetsCount(); ++i)
    {
        if (!DoCheckScripts(m_Project->GetBuildTarget(i)))
            return;
    }

    cbMessageBox(_("All scripts seem to be valid!"), _("Information"), wxICON_INFORMATION, this);
}

void ProjectOptionsDlg::OnAddScript(cb_unused wxCommandEvent& event)
{
    wxListBox* ctrl = XRCCTRL(*this, "lstPreScripts", wxListBox);
    if (!ctrl)
        return;

    wxFileName fname;
    if (ctrl->GetSelection())
        fname.Assign(ctrl->GetStringSelection());
    else if (ctrl->GetCount())
        fname.Assign(ctrl->GetString(ctrl->GetCount() - 1));
    fname.Normalize(wxPATH_NORM_ALL & ~wxPATH_NORM_CASE, m_Project->GetBasePath());

    EditPathDlg dlg(this,
                    fname.GetFullName(),
                    wxEmptyString,//fname.GetPath(),
                    _("Add script(s)"),
                    _("Select script file(s)"),
                    false,
                    true,
                    _("Script files (*.script)|*.script"));

    PlaceWindow(&dlg);
    if (dlg.ShowModal() != wxID_OK)
        return;

    wxArrayString paths = GetArrayFromString(dlg.GetPath());
    for (size_t i = 0; i < paths.GetCount(); ++i)
    {
        fname.Assign(paths[i]);
        fname.MakeRelativeTo(m_Project->GetBasePath());
        ctrl->Append(fname.GetFullPath());
        ctrl->SetSelection(ctrl->GetCount()-1);

        wxTreeCtrl* tc = XRCCTRL(*this, "tcOverview", wxTreeCtrl);
        wxTreeItemId sel = tc->GetSelection();
        CompileOptionsBase* base = sel == tc->GetRootItem()
                                    ? static_cast<CompileOptionsBase*>(m_Project)
                                    : static_cast<CompileOptionsBase*>(m_Project->GetBuildTarget(tc->GetItemText(sel)));
        base->AddBuildScript(fname.GetFullPath());
    }
}

void ProjectOptionsDlg::OnRemoveScript(cb_unused wxCommandEvent& event)
{
    wxListBox* ctrl = XRCCTRL(*this, "lstPreScripts", wxListBox);
    if (!ctrl || ctrl->GetSelection() == -1)
        return;

    wxString script = ctrl->GetStringSelection();
    if (script.IsEmpty())
        return;

    wxTreeCtrl* tc = XRCCTRL(*this, "tcOverview", wxTreeCtrl);
    wxTreeItemId sel = tc->GetSelection();
    CompileOptionsBase* base = sel == tc->GetRootItem()
                                ? static_cast<CompileOptionsBase*>(m_Project)
                                : static_cast<CompileOptionsBase*>(m_Project->GetBuildTarget(tc->GetItemText(sel)));
    base->RemoveBuildScript(script);
    int isel = ctrl->GetSelection();
    ctrl->Delete(isel);
    ctrl->SetSelection(isel < (int)(ctrl->GetCount()) ? isel : --isel );
}

void ProjectOptionsDlg::OnPlatform(wxCommandEvent& event)
{
    wxTextCtrl* txtP;
    if (event.GetId() == XRCID("btnPlatform"))
        txtP = XRCCTRL(*this, "txtPlatform",     wxTextCtrl);
    else
        txtP = XRCCTRL(*this, "txtPlatformProj", wxTextCtrl);
    bool isAll = txtP->GetValue().Contains(_("All"));

    wxArrayString arr = GetArrayFromString(GetStringFromPlatforms(spAll, true));
    MultiSelectDlg dlg(this, arr, isAll, _("Select supported platforms:"), _("Build target platforms"));
    if (!isAll)
    {
        wxArrayString sel = GetArrayFromString(txtP->GetValue());
        for (size_t i = 0; i < sel.GetCount(); ++i)
            dlg.SelectWildCard(sel[i].Lower(), true, false);
    }

    if (dlg.ShowModal() == wxID_OK)
    {
        wxArrayString ret = dlg.GetSelectedStrings();
        if (ret.GetCount() == 3)
            txtP->SetValue(_("All"));
        else
        {
            wxString platform = GetStringFromArray(ret);
            txtP->SetValue(platform);
        }
    }
}

void ProjectOptionsDlg::OnScriptMoveUp(cb_unused wxSpinEvent& event)
{
    wxListBox* ctrl = XRCCTRL(*this, "lstPreScripts", wxListBox);
    if (!ctrl || ctrl->GetSelection() <= 0)
        return;

    wxTreeCtrl* tc = XRCCTRL(*this, "tcOverview", wxTreeCtrl);
    wxTreeItemId sel = tc->GetSelection();
    CompileOptionsBase* base = sel == tc->GetRootItem()
                                ? static_cast<CompileOptionsBase*>(m_Project)
                                : static_cast<CompileOptionsBase*>(m_Project->GetBuildTarget(tc->GetItemText(sel)));

    int isel = ctrl->GetSelection();
    wxString ssel = ctrl->GetStringSelection();
    wxArrayString scripts = base->GetBuildScripts();
    scripts.RemoveAt(isel);
    ctrl->Delete(isel);
    --isel;
    ctrl->Insert(ssel, isel);
    scripts.Insert(ssel, isel);
    ctrl->SetSelection(isel);
    base->SetBuildScripts(scripts);
}

void ProjectOptionsDlg::OnScriptMoveDown(cb_unused wxSpinEvent& event)
{
    wxListBox* ctrl = XRCCTRL(*this, "lstPreScripts", wxListBox);
    if (!ctrl || ctrl->GetSelection() == (int)(ctrl->GetCount()) - 1)
        return;

    wxTreeCtrl* tc = XRCCTRL(*this, "tcOverview", wxTreeCtrl);
    wxTreeItemId sel = tc->GetSelection();
    CompileOptionsBase* base = sel == tc->GetRootItem()
                                ? static_cast<CompileOptionsBase*>(m_Project)
                                : static_cast<CompileOptionsBase*>(m_Project->GetBuildTarget(tc->GetItemText(sel)));

    int isel = ctrl->GetSelection();
    wxString ssel = ctrl->GetStringSelection();
    wxArrayString scripts = base->GetBuildScripts();
    scripts.RemoveAt(isel);
    ctrl->Delete(isel);
    ++isel;
    ctrl->Insert(ssel, isel);
    scripts.Insert(ssel, isel);
    ctrl->SetSelection(isel);
    base->SetBuildScripts(scripts);
}

void ProjectOptionsDlg::OnUpdateUI(cb_unused wxUpdateUIEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);
    wxCheckListBox* list = XRCCTRL(*this, "lstFiles", wxCheckListBox);

    bool filesEn = list->GetSelection() >= 0;
    bool en = lstTargets->GetSelection() >= 0;

    // files options
    XRCCTRL(*this, "btnFileOptions", wxButton)->Enable(filesEn);

    // target options
    XRCCTRL(*this, "btnBuildOrder",      wxButton)->Enable(lstTargets->GetCount() > 1);
    XRCCTRL(*this, "btnEditBuildTarget", wxButton)->Enable(en);
    XRCCTRL(*this, "btnCopyBuildTarget", wxButton)->Enable(en);
    XRCCTRL(*this, "btnDelBuildTarget",  wxButton)->Enable(en && lstTargets->GetCount() > 1);

    // edit project/target build options
    XRCCTRL(*this, "btnProjectBuildOptions", wxButton)->Enable(m_pCompiler);
    XRCCTRL(*this, "btnTargetBuildOptions",  wxButton)->Enable(m_pCompiler && en);
    XRCCTRL(*this, "btnExportTarget",        wxButton)->Enable(en);

    // disable some stuff if using a custom makefile
    bool customMake = XRCCTRL(*this, "chkCustomMakefile", wxCheckBox)->GetValue();
    XRCCTRL(*this, "rbPCHStrategy",       wxRadioBox)->Enable(!customMake);
    XRCCTRL(*this, "txtObjectDir",        wxTextCtrl)->Enable(!customMake && en);
    XRCCTRL(*this, "txtObjectDir",        wxTextCtrl)->Enable(!customMake && en);
    XRCCTRL(*this, "btnBrowseObjectDir",  wxButton)->Enable(!customMake && en);
    XRCCTRL(*this, "btnToggleCheckmarks", wxButton)->Enable(!customMake && en);
    XRCCTRL(*this, "btnCheckmarksOn",     wxButton)->Enable(!customMake && en);
    XRCCTRL(*this, "btnCheckmarksOff",    wxButton)->Enable(!customMake && en);
    list->Enable(!customMake);

    // enable some stuff if using a custom makefile
    XRCCTRL(*this, "txtExecutionDir", wxTextCtrl)->Enable(customMake);
    XRCCTRL(*this, "btnExecutionDir", wxButton)->Enable(customMake);

    // scripts page
    wxTreeCtrl* tc = XRCCTRL(*this, "tcOverview", wxTreeCtrl);
    tc->Enable(!customMake);
    bool scrsel = !customMake && tc->GetSelection().IsOk();

    wxListBox* lstPreScripts = XRCCTRL(*this, "lstPreScripts", wxListBox);
    lstPreScripts->Enable(scrsel);
    bool presel = lstPreScripts->GetSelection() != -1;
    XRCCTRL(*this, "btnAddPreScripts",    wxButton)->Enable(scrsel);
    XRCCTRL(*this, "btnRemovePreScripts", wxButton)->Enable(scrsel && presel);
    XRCCTRL(*this, "spnPreScripts",       wxSpinButton)->Enable(scrsel && presel && lstPreScripts->GetCount() > 1);
    XRCCTRL(*this, "btnCheckScripts",     wxButton)->Enable(!customMake);
}

void ProjectOptionsDlg::OnOK(wxCommandEvent& event)
{
    if (XRCCTRL(*this, "txtProjectName", wxTextCtrl)->GetValue().Trim().IsEmpty())
    {
        cbMessageBox(_("The project title (name) cannot be empty."), _("Error"),
                     wxOK | wxCENTRE | wxICON_ERROR, this);
        return; // Stop propagating the event
    }
    event.Skip();
}

void ProjectOptionsDlg::OnCreateDefFileClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);

    if (lstTargets->GetSelection() == -1)
        lstTargets->SetSelection(0);

    ProjectBuildTarget* target = m_Project->GetBuildTarget(lstTargets->GetSelection());
    if (!target)
        return;

    wxComboBox* cmb     = XRCCTRL(*this, "cmbProjectType", wxComboBox);
    wxCheckBox* chkSL   = XRCCTRL(*this, "chkCreateStaticLib", wxCheckBox);
    wxCheckBox* chkCD   = XRCCTRL(*this, "chkCreateDefFile", wxCheckBox);
    wxTextCtrl* txtI    = XRCCTRL(*this, "txtImportLibraryFilename", wxTextCtrl);
    wxTextCtrl* txtD    = XRCCTRL(*this, "txtDefinitionFileFilename", wxTextCtrl);
    wxButton*   browseI = XRCCTRL(*this, "btnBrowseImportLibraryFilename", wxButton);
    wxButton*   browseD = XRCCTRL(*this, "btnBrowseDefinitionFileFilename", wxButton);

    wxString platforms = GetStringFromPlatforms(target->GetPlatforms());

    txtI->Enable(chkSL->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    txtD->Enable(chkCD->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    browseI->Enable(chkSL->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    browseD->Enable(chkCD->IsChecked() && cmb->GetSelection() == ttDynamicLib);
}

void ProjectOptionsDlg::OnCreateImportFileClick(cb_unused wxCommandEvent& event)
{
    wxListBox* lstTargets = XRCCTRL(*this, "lstBuildTarget", wxListBox);

    if (lstTargets->GetSelection() == -1)
        lstTargets->SetSelection(0);

    ProjectBuildTarget* target = m_Project->GetBuildTarget(lstTargets->GetSelection());
    if (!target)
        return;

    wxComboBox* cmb     = XRCCTRL(*this, "cmbProjectType", wxComboBox);
    wxCheckBox* chkSL   = XRCCTRL(*this, "chkCreateStaticLib", wxCheckBox);
    wxCheckBox* chkCD   = XRCCTRL(*this, "chkCreateDefFile", wxCheckBox);
    wxTextCtrl* txtI    = XRCCTRL(*this, "txtImportLibraryFilename", wxTextCtrl);
    wxTextCtrl* txtD    = XRCCTRL(*this, "txtDefinitionFileFilename", wxTextCtrl);
    wxButton*   browseI = XRCCTRL(*this, "btnBrowseImportLibraryFilename", wxButton);
    wxButton*   browseD = XRCCTRL(*this, "btnBrowseDefinitionFileFilename", wxButton);

    wxString platforms = GetStringFromPlatforms(target->GetPlatforms());

    txtI->Enable(chkSL->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    txtD->Enable(chkCD->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    browseI->Enable(chkSL->IsChecked() && cmb->GetSelection() == ttDynamicLib);
    browseD->Enable(chkCD->IsChecked() && cmb->GetSelection() == ttDynamicLib);
}


namespace
{
static void UpdateNameInTree(cbProject *project)
{
    wxTreeCtrl* tree = Manager::Get()->GetProjectManager()->GetUI().GetTree();
    if (!tree || !project->GetProjectNode())
        return;
    tree->SetItemText(project->GetProjectNode(), project->GetTitle());
}
} // anonymous namespace

void ProjectOptionsDlg::EndModal(int retCode)
{
    if (retCode == wxID_OK)
    {
        m_Project->SetTitle(XRCCTRL(*this, "txtProjectName", wxTextCtrl)->GetValue());
        m_Project->SetPlatforms(GetPlatformsFromString(XRCCTRL(*this, "txtPlatformProj", wxTextCtrl)->GetValue()));
        UpdateNameInTree(m_Project);
        m_Project->SetMakefile(XRCCTRL(*this, "txtProjectMakefile", wxTextCtrl)->GetValue());
        m_Project->SetMakefileCustom(XRCCTRL(*this, "chkCustomMakefile", wxCheckBox)->GetValue());
        m_Project->SetMakefileExecutionDir(XRCCTRL(*this, "txtExecutionDir", wxTextCtrl)->GetValue());
        m_Project->SetTargetType(TargetType(XRCCTRL(*this, "cmbProjectType", wxComboBox)->GetSelection()));
        m_Project->SetModeForPCH((PCHMode)XRCCTRL(*this, "rbPCHStrategy", wxRadioBox)->GetSelection());
        m_Project->SetExtendedObjectNamesGeneration(XRCCTRL(*this, "chkExtendedObjNames", wxCheckBox)->GetValue());
        m_Project->SetShowNotesOnLoad(XRCCTRL(*this, "chkShowNotes", wxCheckBox)->GetValue());
        m_Project->SetCheckForExternallyModifiedFiles(XRCCTRL(*this, "chkCheckFiles", wxCheckBox)->GetValue());
        m_Project->SetNotes(XRCCTRL(*this, "txtNotes", wxTextCtrl)->GetValue());

        if (m_Current_Sel == -1)
            m_Current_Sel = 0; // force update of global options

        DoBeforeTargetChange(true);

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

    wxScrollingDialog::EndModal(retCode);
}

