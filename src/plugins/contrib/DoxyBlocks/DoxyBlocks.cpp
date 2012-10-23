/**************************************************************************//**
 * \file        DoxyBlocks.cpp
 * \author      Gary Harris
 * \date        01-02-2010
 *
 * DoxyBlocks - doxygen integration for Code::Blocks. \n
 * Copyright (C) 2010 Gary Harris.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/
#include "DoxyBlocks.h"

#include <sdk.h> // Code::Blocks SDK
#ifndef CB_PRECOMP
    #include <wx/menu.h>
    #include <wx/process.h>
    #include <wx/toolbar.h>
    #include <wx/xrc/xmlres.h>

    #include <cbeditor.h>
    #include <cbproject.h>
    #include <configmanager.h>
    #include <editormanager.h>
    #include <logmanager.h>
    #include <macrosmanager.h>
    #include <projectmanager.h>
#endif
#include <cbstyledtextctrl.h>
#include <configurationpanel.h>
#include <projectloader_hooks.h>
#include <tinyxml/tinywxuni.h>

#include <wx/tokenzr.h>
#include <wx/textfile.h>

#include "ConfigPanel.h"
#include "DoxyBlocksLogger.h"

// Register the plugin with Code::Blocks.
// We are using an anonymous namespace so we don't litter the global one.
namespace
{
    PluginRegistrant<DoxyBlocks> reg(wxT("DoxyBlocks"));
}

// events handling
BEGIN_EVENT_TABLE(DoxyBlocks, cbPlugin)
    // add any events you want to handle here
    EVT_UPDATE_UI(ID_TB_WIZARD,           DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_TB_EXTRACTPROJECT,   DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_TB_BLOCKCOMMENT,     DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_TB_LINECOMMENT,      DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_TB_RUNHTML,          DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_TB_RUNCHM,           DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_TB_CONFIG,           DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_DOXYBLOCKS,     DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_EXTRACTPROJECT, DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_BLOCKCOMMENT,   DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_LINECOMMENT,    DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_RUNHTML,        DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_RUNCHM,         DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_CONFIG,         DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_SAVE_TEMPLATE,  DoxyBlocks::OnUpdateUI)
    EVT_UPDATE_UI(ID_MENU_LOAD_TEMPLATE,  DoxyBlocks::OnUpdateUI)

    EVT_TEXT_URL(ID_LOG_DOXYBLOCKS, DoxyBlocks::OnTextURL)
END_EVENT_TABLE()

// constructor
DoxyBlocks::DoxyBlocks() :
    m_pToolbar(0l),
    m_DoxyBlocksLog(0l),
    m_LogPageIndex(0),
    m_bAutoVersioning(false)
{
    // Make sure our resources are available.
    // In the generated boilerplate code we have no resources but when
    // we add some, it will be nice that this code is in place already ;)
    if(!Manager::LoadResource(wxT("DoxyBlocks.zip"))){
        NotifyMissingFile(wxT("DoxyBlocks.zip"));
    }

    m_pConfig = new DoxyBlocksConfig;
}

// destructor
DoxyBlocks::~DoxyBlocks()
{
    wxDELETE(m_pConfig);
    wxASSERT(!m_pConfig);
}

void DoxyBlocks::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...

     // register event sinks
    Manager::Get()->RegisterEventSink(cbEVT_PROJECT_ACTIVATE, new cbEventFunctor<DoxyBlocks, CodeBlocksEvent>(this, &DoxyBlocks::OnProjectActivate));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_OPEN, new cbEventFunctor<DoxyBlocks, CodeBlocksEvent>(this, &DoxyBlocks::OnEditorOpen));
    Manager::Get()->RegisterEventSink(cbEVT_EDITOR_CLOSE, new cbEventFunctor<DoxyBlocks, CodeBlocksEvent>(this, &DoxyBlocks::OnEditorClose));

    // Create a new log window.
    if(LogManager *LogMan = Manager::Get()->GetLogManager()){
        m_DoxyBlocksLog = new DoxyBlocksLogger();
        m_LogPageIndex = LogMan->SetLog(m_DoxyBlocksLog);
        LogMan->Slot(m_LogPageIndex).title = wxT("DoxyBlocks");
        CodeBlocksLogEvent evtAdd(cbEVT_ADD_LOG_WINDOW, m_DoxyBlocksLog, LogMan->Slot(m_LogPageIndex).title);
        Manager::Get()->ProcessEvent(evtAdd);
    }
}

/*! \brief Hooks the IDE's project activated event.
 *
 * \param event CodeBlocksEvent&    A CodeBlocks event object.
 * \return void
 *
 * Checks whether AutoVersioning is active for the activated project. Loads settings
 * and inits the config object. Enables/disables toolbar and menu items.
 */
void DoxyBlocks::OnProjectActivate(CodeBlocksEvent& WXUNUSED(event))
{
    if(IsAttached()){
        if(m_pConfig != NULL){
            wxDELETE(m_pConfig);
            wxASSERT(!m_pConfig);
        }
        m_pConfig = new DoxyBlocksConfig;
        CheckForAutoVersioning();
        LoadSettings();
    }

       // Enable the menu and toolbar...
    m_pToolbar->Enable(true);
    wxMenuBar *menuBar =  Manager::Get()->GetAppFrame()->GetMenuBar();
    menuBar->FindItem(ID_MENU_DOXYWIZARD)->Enable(true);
    menuBar->FindItem(ID_MENU_EXTRACTPROJECT)->Enable(true);
    menuBar->FindItem(ID_MENU_RUNHTML)->Enable(true);
    menuBar->FindItem(ID_MENU_RUNCHM)->Enable(true);
    menuBar->FindItem(ID_MENU_CONFIG)->Enable(true);
    menuBar->FindItem(ID_MENU_LOAD_TEMPLATE)->Enable(true);
    menuBar->FindItem(ID_MENU_SAVE_TEMPLATE)->Enable(true);
    // ...but disable the comment controls if there are no editors open.
    if(Manager::Get()->GetEditorManager()->GetEditorsCount() > 0){
        menuBar->FindItem(ID_MENU_BLOCKCOMMENT)->Enable(true);
        menuBar->FindItem(ID_MENU_LINECOMMENT)->Enable(true);
    }
    else{
        m_pToolbar->EnableTool(ID_TB_BLOCKCOMMENT, false);
        m_pToolbar->EnableTool(ID_TB_LINECOMMENT, false);
        menuBar->FindItem(ID_MENU_BLOCKCOMMENT)->Enable(false);
        menuBar->FindItem(ID_MENU_LINECOMMENT)->Enable(false);
    }
}

/*! \brief Hooks the editor open event.
 *
 * \param event CodeBlocksEvent&    A CodeBlocks event object.
 * \return void
 *
 * Checks that there is at least one editor window open and enables the comment menu items and toolbar buttons if so.
 */
void DoxyBlocks::OnEditorOpen(CodeBlocksEvent& WXUNUSED(event))
{
    if(IsAttached()){
        // Only restore the menu and toolbar when the first editor window is opened to avoid
        // needlessly doing so for every window.
        if(Manager::Get()->GetEditorManager()->GetEditorsCount() == 1){
            m_pToolbar->EnableTool(ID_TB_BLOCKCOMMENT, true);
            m_pToolbar->EnableTool(ID_TB_LINECOMMENT, true);
            wxMenuBar *menuBar =  Manager::Get()->GetAppFrame()->GetMenuBar();
            menuBar->FindItem(ID_MENU_BLOCKCOMMENT)->Enable(true);
            menuBar->FindItem(ID_MENU_LINECOMMENT)->Enable(true);
        }
    }
}

/*! \brief Hooks the editor close event.
 *
 * \param event CodeBlocksEvent&    A CodeBlocks event object.
 * \return void
 *
 * Checks that there is at least one editor window open and disables the comment menu items and toolbar buttons if not.
 */
void DoxyBlocks::OnEditorClose(CodeBlocksEvent& WXUNUSED(event))
{
    if(IsAttached()){
        if(Manager::Get()->GetEditorManager()->GetEditorsCount() == 0){
            m_pToolbar->EnableTool(ID_TB_BLOCKCOMMENT, false);
            m_pToolbar->EnableTool(ID_TB_LINECOMMENT, false);
            wxMenuBar *menuBar =  Manager::Get()->GetAppFrame()->GetMenuBar();
            menuBar->FindItem(ID_MENU_BLOCKCOMMENT)->Enable(false);
            menuBar->FindItem(ID_MENU_LINECOMMENT)->Enable(false);
        }
    }
}

/*! \brief An UPDATE_UI  event was raised.
 *
 * \param event CodeBlocksEvent&    A CodeBlocks event object.
 * \return void
 *
 * Enables/disables the menu and toolbar items accordingly.
 */
void DoxyBlocks::OnUpdateUI(wxUpdateUIEvent& WXUNUSED(event))
{
    if(Manager::Get()->GetProjectManager()->GetProjects()->GetCount() == 0){
        m_pToolbar->Enable(false);
        wxMenuBar *menuBar =  Manager::Get()->GetAppFrame()->GetMenuBar();
        menuBar->FindItem(ID_MENU_DOXYWIZARD)->Enable(false);
        menuBar->FindItem(ID_MENU_EXTRACTPROJECT)->Enable(false);
        menuBar->FindItem(ID_MENU_BLOCKCOMMENT)->Enable(false);
        menuBar->FindItem(ID_MENU_LINECOMMENT)->Enable(false);
        menuBar->FindItem(ID_MENU_RUNHTML)->Enable(false);
        menuBar->FindItem(ID_MENU_RUNCHM)->Enable(false);
        menuBar->FindItem(ID_MENU_CONFIG)->Enable(false);
        menuBar->FindItem(ID_MENU_LOAD_TEMPLATE)->Enable(false);
        menuBar->FindItem(ID_MENU_SAVE_TEMPLATE)->Enable(false);
    }
}

void DoxyBlocks::OnRelease(bool /*appShutDown*/)
{
    // do de-initialization for your plugin
    // if appShutDown is true, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...

    // Remove the log window.
    if(Manager::Get()->GetLogManager()){
        if(m_DoxyBlocksLog){
            CodeBlocksLogEvent evt(cbEVT_REMOVE_LOG_WINDOW, m_DoxyBlocksLog);
            Manager::Get()->ProcessEvent(evt);
        }
    }
    m_DoxyBlocksLog = 0;
}

cbConfigurationPanel *DoxyBlocks::GetConfigurationPanel(wxWindow *parent)
{
    //create and display the configuration dialog for your plugin
    if(!IsAttached()){
        return 0;
    }

    // Get the version string before instantiating the panel so that it is recorded before
    // LoadSettings() is run in the constructor.
    if(m_bAutoVersioning){
        // If we're using autoversion for docs, get the value.
        if(m_pConfig->GetUseAutoVersion()){
            m_sAutoVersion = GetAutoVersion();
            m_pConfig->SetProjectNumber(m_sAutoVersion);
        }
    }

    // Create a configuration dialogue and hand it off to codeblocks
    ConfigPanel *pDlg = new ConfigPanel(parent, this);
    pDlg->SetAutoVersioning(m_bAutoVersioning);

    // Comments.
    pDlg->SetBlockComment(m_pConfig->GetBlockComment());
    pDlg->SetLineComment(m_pConfig->GetLineComment());
    // == Doxyfile defaults. ==
    // Project.
    pDlg->SetProjectNumber(m_pConfig->GetProjectNumber());
    pDlg->SetOutputDirectory(m_pConfig->GetOutputDirectory());
    pDlg->SetOutputLanguage(m_pConfig->GetOutputLanguage());
    pDlg->SetUseAutoVersion(m_pConfig->GetUseAutoVersion());
    // Build.
    pDlg->SetExtractAll(m_pConfig->GetExtractAll());
    pDlg->SetExtractPrivate(m_pConfig->GetExtractPrivate());
    pDlg->SetExtractStatic(m_pConfig->GetExtractStatic());
    // Warnings.
    pDlg->SetWarnings(m_pConfig->GetWarnings());
    pDlg->SetWarnIfDocError(m_pConfig->GetWarnIfDocError());
    pDlg->SetWarnIfUndocumented(m_pConfig->GetWarnIfUndocumented());
    pDlg->SetWarnNoParamdoc(m_pConfig->GetWarnNoParamdoc());
    //  Alphabetical Class Index.
    pDlg->SetAlphabeticalIndex(m_pConfig->GetAlphabeticalIndex());
    // Output.
    pDlg->SetGenerateHTML(m_pConfig->GetGenerateHTML());
    pDlg->SetGenerateHTMLHelp(m_pConfig->GetGenerateHTMLHelp());
    pDlg->SetGenerateCHI(m_pConfig->GetGenerateCHI());
    pDlg->SetBinaryTOC(m_pConfig->GetBinaryTOC());
    pDlg->SetGenerateLatex(m_pConfig->GetGenerateLatex());
    pDlg->SetGenerateRTF(m_pConfig->GetGenerateRTF());
    pDlg->SetGenerateMan(m_pConfig->GetGenerateMan());
    pDlg->SetGenerateXML(m_pConfig->GetGenerateXML());
    pDlg->SetGenerateAutogenDef(m_pConfig->GetGenerateAutogenDef());
    pDlg->SetGeneratePerlMod(m_pConfig->GetGeneratePerlMod());
    // Pre-processor.
    pDlg->SetEnablePreprocessing(m_pConfig->GetEnablePreprocessing());
    // Dot.
    pDlg->SetClassDiagrams(m_pConfig->GetClassDiagrams());
    pDlg->SetHaveDot(m_pConfig->GetHaveDot());
    // Paths.
    pDlg->SetPathDoxygen(m_pConfig->GetPathDoxygen());
    pDlg->SetPathDoxywizard(m_pConfig->GetPathDoxywizard());
    pDlg->SetPathHHC(m_pConfig->GetPathHHC());
    pDlg->SetPathCHMViewer(m_pConfig->GetPathCHMViewer());
    pDlg->SetPathDot(m_pConfig->GetPathDot());
    // General Options
    pDlg->SetOverwriteDoxyfile(m_pConfig->GetOverwriteDoxyfile());
    pDlg->SetPromptBeforeOverwriting(m_pConfig->GetPromptBeforeOverwriting());
    pDlg->SetUseAtInTags(m_pConfig->GetUseAtInTags());
    pDlg->SetLoadTemplate(m_pConfig->GetLoadTemplate());
    pDlg->SetUseInternalViewer(m_pConfig->GetUseInternalViewer());
    pDlg->SetRunHTML(m_pConfig->GetRunHTML());
    pDlg->SetRunCHM(m_pConfig->GetRunCHM());

    pDlg->Init();
    // when the configuration panel is closed with OK, OnDialogDone() will be called
    return pDlg;
}

cbConfigurationPanel* DoxyBlocks::GetProjectConfigurationPanel(wxWindow* /*parent*/, cbProject* /*project*/)
{
    return 0;
}

void DoxyBlocks::OnConfigure(wxCommandEvent & WXUNUSED(event))
{
    Configure();
}

int DoxyBlocks::Configure()
{
    //create and display the configuration dialog for your plugin
    cbConfigurationDialog dlg(Manager::Get()->GetAppWindow(), wxID_ANY, _("DoxyBlocks"));
    cbConfigurationPanel *panel = GetConfigurationPanel(&dlg);
    if(panel){
        dlg.AttachConfigurationPanel(panel);
        PlaceWindow(&dlg);
        return dlg.ShowModal() == wxID_OK ? 0 : -1;
    }
    return -1;
}

/*! \brief Save prefs before the dialogue closes.
 *
 * \param pDlg ConfigPanel*    DoxyBlocks' configuration panel.
 * \return void
 */
void DoxyBlocks::OnDialogueDone(ConfigPanel* pDlg)
{
    // Comments.
    m_pConfig->SetBlockComment(pDlg->GetBlockComment());
    m_pConfig->SetLineComment(pDlg->GetLineComment());
    // == Doxyfile defaults. ==
    // Project.
    m_pConfig->SetProjectNumber(pDlg->GetProjectNumber());
    m_pConfig->SetOutputDirectory(ValidateRelativePath(pDlg->GetOutputDirectory()));
    m_pConfig->SetOutputLanguage(pDlg->GetOutputLanguage());
    m_pConfig->SetUseAutoVersion(pDlg->GetUseAutoVersion());
    // Build.
    m_pConfig->SetExtractAll(pDlg->GetExtractAll());
    m_pConfig->SetExtractPrivate(pDlg->GetExtractPrivate());
    m_pConfig->SetExtractStatic(pDlg->GetExtractStatic());
    // Warnings.
    m_pConfig->SetWarnings(pDlg->GetWarnings());
    m_pConfig->SetWarnIfDocError(pDlg->GetWarnIfDocError());
    m_pConfig->SetWarnIfUndocumented(pDlg->GetWarnIfUndocumented());
    m_pConfig->SetWarnNoParamdoc(pDlg->GetWarnNoParamdoc());
    //  Alphabetical Class Index.
    m_pConfig->SetAlphabeticalIndex(pDlg->GetAlphabeticalIndex());
    // Output.
    m_pConfig->SetGenerateHTML(pDlg->GetGenerateHTML());
    m_pConfig->SetGenerateHTMLHelp(pDlg->GetGenerateHTMLHelp());
    m_pConfig->SetGenerateCHI(pDlg->GetGenerateCHI());
    m_pConfig->SetBinaryTOC(pDlg->GetBinaryTOC());
    m_pConfig->SetGenerateLatex(pDlg->GetGenerateLatex());
    m_pConfig->SetGenerateRTF(pDlg->GetGenerateRTF());
    m_pConfig->SetGenerateMan(pDlg->GetGenerateMan());
    m_pConfig->SetGenerateXML(pDlg->GetGenerateXML());
    m_pConfig->SetGenerateAutogenDef(pDlg->GetGenerateAutogenDef());
    m_pConfig->SetGeneratePerlMod(pDlg->GetGeneratePerlMod());
    // Pre-processor.
    m_pConfig->SetEnablePreprocessing(pDlg->GetEnablePreprocessing());
    // Dot.
    m_pConfig->SetClassDiagrams(pDlg->GetClassDiagrams());
    m_pConfig->SetHaveDot(pDlg->GetHaveDot());
    // Paths.
    m_pConfig->SetPathDoxygen(pDlg->GetPathDoxygen());
    m_pConfig->SetPathDoxywizard(pDlg->GetPathDoxywizard());
    m_pConfig->SetPathHHC(pDlg->GetPathHHC());
    m_pConfig->SetPathDot(pDlg->GetPathDot());
    m_pConfig->SetPathCHMViewer(pDlg->GetPathCHMViewer());
    // General Options
    m_pConfig->SetOverwriteDoxyfile(pDlg->GetOverwriteDoxyfile());
    m_pConfig->SetPromptBeforeOverwriting(pDlg->GetPromptBeforeOverwriting());
    m_pConfig->SetUseAtInTags(pDlg->GetUseAtInTags());
    m_pConfig->SetLoadTemplate(pDlg->GetLoadTemplate());
    m_pConfig->SetUseInternalViewer(pDlg->GetUseInternalViewer());
    m_pConfig->SetRunHTML(pDlg->GetRunHTML());
    m_pConfig->SetRunCHM(pDlg->GetRunCHM());

    cbProject *prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(prj){
        SaveSettings();
        // Update the config object and save the project so prefs don't get lost if a problem occurs before closing the project.
        prj->Save();
    }
    else{
        AppendToLog(_("No active project found. Settings not saved."), LOG_WARNING);
    }
}

void DoxyBlocks::BuildMenu(wxMenuBar *menuBar)
{
    //The application is offering its menubar for your plugin,
    //to add any menu items you want...
    //Append any items you need in the menu...
    //NOTE: Be careful in here... The application's menubar is at your disposal.
//    NotImplemented(wxT("DoxyBlocks::BuildMenu()"));

    int idx = menuBar->FindMenu(_("P&lugins"));
    if(idx != wxNOT_FOUND){
        wxMenu *submenu = new wxMenu;
        wxString sDataFolder(ConfigManager::GetDataFolder());
        wxString prefix = sDataFolder + wxT("/images/DoxyBlocks/16x16/");

        wxMenuItem *MenuItemDoxywizard = new wxMenuItem(submenu, ID_MENU_DOXYWIZARD, _("&Doxywizard...\tCtrl-Alt-D"), _("Run doxywizard."));
        MenuItemDoxywizard->SetBitmap(wxBitmap(prefix + wxT("doxywizard.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemDoxywizard);
        wxMenuItem *MenuItemExtract = new wxMenuItem(submenu, ID_MENU_EXTRACTPROJECT, _("&Extract documentation\tCtrl-Alt-E"), _("Extract documentation for the current project."));
        MenuItemExtract->SetBitmap(wxBitmap(prefix + wxT("extract.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemExtract);
        submenu->AppendSeparator();
        wxMenuItem *MenuItemBlockComment = new wxMenuItem(submenu, ID_MENU_BLOCKCOMMENT, _("&Block comment\tCtrl-Alt-B"), _("Insert a comment block at the current line."));
        MenuItemBlockComment->SetBitmap(wxBitmap(prefix + wxT("comment_block.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemBlockComment);
        wxMenuItem *MenuItemLineComment = new wxMenuItem(submenu, ID_MENU_LINECOMMENT, _("&Line comment\tCtrl-Alt-L"), _("Insert a line comment at the current cursor position."));
        MenuItemLineComment->SetBitmap(wxBitmap(prefix + wxT("comment_line.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemLineComment);
        submenu->AppendSeparator();
        wxMenuItem *MenuItemRunHTML = new wxMenuItem(submenu, ID_MENU_RUNHTML, _("Run &HTML\tCtrl-Alt-H"), _("Run HTML documentation."));
        MenuItemRunHTML->SetBitmap(wxBitmap(prefix + wxT("html.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemRunHTML);
        wxMenuItem *MenuItemRunCHM = new wxMenuItem(submenu, ID_MENU_RUNCHM, _("Run &CHM\tCtrl-Alt-C"), _("Run CHM documentation."));
        MenuItemRunCHM->SetBitmap(wxBitmap(prefix + wxT("chm.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemRunCHM);
        submenu->AppendSeparator();
        wxMenuItem *MenuItemConfig = new wxMenuItem(submenu, ID_MENU_CONFIG, _("Open &preferences...\tCtrl-Alt-P"), _("Open DoxyBlocks' preferences."));
        MenuItemConfig->SetBitmap(wxBitmap(prefix + wxT("configure.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemConfig);
        submenu->AppendSeparator();
        wxMenuItem *MenuItemLoadTemplate = new wxMenuItem(submenu, ID_MENU_LOAD_TEMPLATE, _("L&oad settings template"), _("Load saved settings template."));
        MenuItemLoadTemplate->SetBitmap(wxBitmap(sDataFolder + wxT("/images/16x16/fileopen.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemLoadTemplate);
        wxMenuItem *MenuItemSaveTemplate = new wxMenuItem(submenu, ID_MENU_SAVE_TEMPLATE, _("&Save settings template"), _("Save current settings for future use."));
        MenuItemSaveTemplate->SetBitmap(wxBitmap(sDataFolder + wxT("/images/16x16/filesave.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemSaveTemplate);

        Connect(ID_MENU_DOXYWIZARD,     wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnRunDoxywizard));
        Connect(ID_MENU_EXTRACTPROJECT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnExtractProject));
        Connect(ID_MENU_BLOCKCOMMENT,   wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnBlockComment));
        Connect(ID_MENU_LINECOMMENT,    wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnLineComment));
        Connect(ID_MENU_RUNHTML,        wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnRunHTML));
        Connect(ID_MENU_RUNCHM,         wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnRunCHM));
        Connect(ID_MENU_CONFIG,         wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnConfigure));
        Connect(ID_MENU_LOAD_TEMPLATE,  wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnReadPrefsTemplate));
        Connect(ID_MENU_SAVE_TEMPLATE,  wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnWritePrefsTemplate));

        menuBar->Insert(idx + 1, submenu, wxT("Do&xyBlocks"));
    }
}

void DoxyBlocks::BuildModuleMenu(const ModuleType type, wxMenu *menu, const FileTreeData * /*data*/)
{
    //Check the parameter \"type\" and see which module it is
    //and append any items you need in the menu...
    //TIP: for consistency, add a separator as the first item...

    // Add the comment functions to the editor's context-sensitive menu.
    if(type == mtEditorManager){
        wxMenu *submenu = new wxMenu;
        wxString prefix = ConfigManager::GetDataFolder() + wxT("/images/DoxyBlocks/16x16/");

        menu->AppendSeparator();
        wxMenuItem *MenuItemBlockComment = new wxMenuItem(submenu, ID_MENU_BLOCKCOMMENT, _("&Block Comment"), _("Insert a comment block at the current line."));
        MenuItemBlockComment->SetBitmap(wxBitmap(prefix + wxT("comment_block.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemBlockComment);
        wxMenuItem *MenuItemLineComment = new wxMenuItem(submenu, ID_MENU_LINECOMMENT, _("&Line Comment"), _("Insert a line comment at the current cursor position."));
        MenuItemLineComment->SetBitmap(wxBitmap(prefix + wxT("comment_line.png"), wxBITMAP_TYPE_PNG));
        submenu->Append(MenuItemLineComment);
        menu->AppendSubMenu(submenu, wxT("Do&xyBlocks"));
        // Events are already attached to these IDs in BuildMenu().
    }
}

bool DoxyBlocks::BuildToolBar(wxToolBar *toolBar)
{
    if(!IsAttached() || !toolBar)
        return false;

    m_pToolbar = toolBar;
    wxString prefix;
    ConfigManager *cfg = Manager::Get()->GetConfigManager(wxT("app"));
    if(cfg->ReadBool(wxT("/environment/toolbar_size"), true)){
        prefix = ConfigManager::GetDataFolder() + wxT("/images/DoxyBlocks/16x16/");
        m_pToolbar->SetToolBitmapSize(wxSize(16, 16));
    }
    else{
        prefix = ConfigManager::GetDataFolder() + wxT("/images/DoxyBlocks/");
        m_pToolbar->SetToolBitmapSize(wxSize(22, 22));
    }

    m_pToolbar->AddTool(ID_TB_WIZARD, _("Doxywizard"), wxBitmap(prefix + wxT("doxywizard.png"), wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL, _("Run doxywizard"));
    m_pToolbar->AddTool(ID_TB_EXTRACTPROJECT, _("Document project"), wxBitmap(prefix + wxT("extract.png"), wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL, _("Extract documentation for the current project"));
    m_pToolbar->AddSeparator();
    m_pToolbar->AddTool(ID_TB_BLOCKCOMMENT, _("Block Comment"), wxBitmap(prefix + wxT("comment_block.png"), wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL, _("Insert a comment block at the current line"));
    m_pToolbar->AddTool(ID_TB_LINECOMMENT, _("Line Comment"), wxBitmap(prefix + wxT("comment_line.png"), wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL, _("Insert a line comment at the current cursor position"));
    m_pToolbar->AddSeparator();
    m_pToolbar->AddTool(ID_TB_RUNHTML, _("Run HTML"), wxBitmap(prefix + wxT("html.png"), wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL, _("Run HTML documentation"));
    m_pToolbar->AddTool(ID_TB_RUNCHM, _("Run CHM"), wxBitmap(prefix + wxT("chm.png"), wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL, _("Run HTML Help documentation"));
    m_pToolbar->AddSeparator();
    m_pToolbar->AddTool(ID_TB_CONFIG, _("Open Preferences"), wxBitmap(prefix + wxT("configure.png"), wxBITMAP_TYPE_PNG), wxNullBitmap, wxITEM_NORMAL, _("Open DoxyBlocks' preferences"));
    m_pToolbar->Realize();
#if wxCHECK_VERSION(2, 8, 0)
    m_pToolbar->SetInitialSize();
#else
    m_pToolbar->SetBestFittingSize();
#endif

    Connect(ID_TB_WIZARD,         wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnRunDoxywizard));
    Connect(ID_TB_EXTRACTPROJECT, wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnExtractProject));
    Connect(ID_TB_BLOCKCOMMENT,   wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnBlockComment));
    Connect(ID_TB_LINECOMMENT,    wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnLineComment));
    Connect(ID_TB_RUNHTML,        wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnRunHTML));
    Connect(ID_TB_RUNCHM,         wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnRunCHM));
    Connect(ID_TB_CONFIG,         wxEVT_COMMAND_TOOL_CLICKED, wxCommandEventHandler(DoxyBlocks::OnConfigure));

    return true;
}

/*! \brief Run doxywizard. Load the project's doxyfile, if one exists.
 *
 * \return    bool    true on success, otherwise false.
 */
void DoxyBlocks::OnRunDoxywizard(wxCommandEvent & WXUNUSED(event))
{
    DoRunDoxywizard();
}

/*! \brief Run doxywizard. Load the project's doxyfile, if one exists.
 *
 * \return    bool    true on success, otherwise false.
 */
bool DoxyBlocks::DoRunDoxywizard()
{
    if(!IsProjectOpen()){
        return false;
    }

    AppendToLog(_("Running doxywizard..."));
    // Default command.
    wxString cmd = wxT("doxywizard");
    // If a path is configured, use that instead.
    wxString sDoxywizardPath = Manager::Get()->GetMacrosManager()->ReplaceMacros(m_pConfig->GetPathDoxywizard());
    if(!sDoxywizardPath.IsEmpty()){
        cmd = sDoxywizardPath;
    }
    // Get the doxyfile path.
    wxString sPathDoxyfile = wxT("doxygen");
    wxString sOutputDir =  m_pConfig->GetOutputDirectory();
    if(!sOutputDir.IsEmpty()){
        sPathDoxyfile = sPathDoxyfile + wxT("/") + sOutputDir;
    }
    wxString sCfgBaseFile = wxT("doxyfile");
    wxFileName fnDoxyfile(sPathDoxyfile + wxFileName::GetPathSeparator() + sCfgBaseFile);
    fnDoxyfile.Normalize();
    if(!sPathDoxyfile.IsEmpty()){
        cmd.Append(wxT(" \"") + fnDoxyfile.GetFullPath() + wxT("\""));
    }

    wxProcess *process = new wxProcess(this);
    // the PID of the last process we launched asynchronously
    long pid = wxExecute(cmd, wxEXEC_ASYNC, process);
    if(!pid){
        AppendToLog(wxString::Format(_("Execution of '%s' failed."), cmd.c_str()), LOG_ERROR);
        AppendToLog(_("Please ensure that the doxygen 'bin' directory is in your path or provide the specific path in DoxyBlocks' preferences.\n"));

        delete process;
    }
    else{
        AppendToLog(wxString::Format(_("Process %ld (%s) launched."), pid, cmd.c_str()));
    }
    return true;
}

/*! \brief Append a message to DoxyBlocks' log window.
 *
 * \param    sText           wxString&    The text to display.
 * \param    flag            eLogLevel    A flag controlling the entry's style. Defaults to LOG_NORMAL.
 * \param    bReturnFocus    bool         Whether to return the focus to the editor window after logging.
 *
 * The style flags are:
 *     \li LOG_NORMAL     - normal text
 *     \li LOG_WARNING    - italic text
 *     \li LOG_ERROR      - bold red text
 */
void DoxyBlocks::AppendToLog(const wxString &sText, eLogLevel flag /*  = LOG_NORMAL */, bool bReturnFocus /* = true */) const
{
    if(LogManager *LogMan = Manager::Get()->GetLogManager()){
        CodeBlocksLogEvent evtSwitch(cbEVT_SWITCH_TO_LOG_WINDOW, m_DoxyBlocksLog);
        Manager::Get()->ProcessEvent(evtSwitch);
        switch(flag){
            case LOG_NORMAL:
                LogMan->Log(sText, m_LogPageIndex);
                break;
            case LOG_WARNING:
                LogMan->LogWarning(sText, m_LogPageIndex);
                break;
            case LOG_ERROR:
                LogMan->LogError(sText, m_LogPageIndex);
                break;
        }

        // Stop the log window stealing the focus so that the caret remains positioned for comment entry...
        if(bReturnFocus){
            // ...but only do so if there are editor windows open.
            cbEditor *cbEd = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
            if(cbEd){
                cbEd->GetControl()->SetFocus();
            }
        }
    }
}

/*! \brief Check whether a project is open.
 *
 * \return    bool    true if a project is open, false otherwise.
 */
bool DoxyBlocks::IsProjectOpen() const
{
    const cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    // If no project open, exit
    if(!project)
    {
        const wxString msg = _("You need to open a project before using DoxyBlocks.");
        cbMessageBox(msg, wxString(wxT("DoxyBlocks ")) + _("Error"), wxICON_ERROR|wxOK, Manager::Get()->GetAppWindow());
        AppendToLog(msg, LOG_ERROR, false);
        return false;
    }
    return true;
}

void DoxyBlocks::OnRunHTML(wxCommandEvent & WXUNUSED(event))
{
    DoRunHTML();
}

/*! \brief Run the HTML documentation.
 *
 * This function checks configuration settings and either runs the HTML in the internal viewer or the default browser.
 */
void DoxyBlocks::DoRunHTML()
{
    if(!IsProjectOpen()){
        return;
    }

    wxString sDocPath = GetDocPath();
    if(sDocPath.IsEmpty()){
        AppendToLog(_("Unable to get the doxygen document path."), LOG_ERROR);
        return;
    }

    wxString sPath(sDocPath + wxT("html/index.html"));
    wxString sURL = wxT("file://") + sPath;
    bool bUseInternalViewer = m_pConfig->GetUseInternalViewer();
    if(wxFile::Exists(sPath)){
        if(bUseInternalViewer){
            // View HTML with the internal viewer.
            if(cbMimePlugin* p = Manager::Get()->GetPluginManager()->GetMIMEHandlerForFile(sPath)){
                p->OpenFile(sPath);
                AppendToLog(_("Internal viewer launched with path ") + sPath + wxT("."));
            }
            else{
                AppendToLog(_("Error getting MIME handler for ") + sPath, LOG_ERROR);
            }
        }
        else{
            if(!wxLaunchDefaultBrowser(sURL)){
                AppendToLog(_("Unable to launch the default browser."), LOG_WARNING);
            }
            else{
                AppendToLog(_("Default browser launched with URL ") + sURL + wxT("."));
            }
        }
    }
    else{
        AppendToLog(_("Index.html not found at ") + sPath + wxT("."), LOG_WARNING);
    }
}

void DoxyBlocks::OnRunCHM(wxCommandEvent & WXUNUSED(event))
{
    DoRunCHM();
}

/*! \brief Get parameters and call RunCompiledHelp() to run the Compiled Help documentation.
 */
void DoxyBlocks::DoRunCHM()
{
    if(!IsProjectOpen()){
        return;
    }

    const wxString sPrjName = GetProjectName();
    const wxString sDocPath = GetDocPath();
    if(sPrjName.IsEmpty())
    {
        AppendToLog(_("Unable to get the project name."), LOG_ERROR);
        return;
    }
    if(sDocPath.IsEmpty())
    {
        AppendToLog(_("Unable to get the doxygen document path."), LOG_ERROR);
        return;
    }
    RunCompiledHelp(sDocPath, sPrjName);
}

/*! \brief Run the Compiled Help documentation.
 *
 * \param    sDocPath wxString    The path to the CHM file.
 * \param    sPrjName wxString    The path to the project.
 *
 */
void DoxyBlocks::RunCompiledHelp(wxString sDocPath, wxString sPrjName)
{
    // Path to doc.
    wxString sPathCHM = sDocPath + sPrjName + wxT(".chm");
    if(wxFile::Exists(sPathCHM)){
        wxString cmdCHM;
        wxString sCHMViewer = Manager::Get()->GetMacrosManager()->ReplaceMacros(m_pConfig->GetPathCHMViewer());
        if(sCHMViewer.IsEmpty()){
            // No CHM viewer entered.
            if((wxPlatformInfo::Get().GetOperatingSystemId() & wxOS_WINDOWS) > 0){
                // On Windows, use the OS built-in capability.
                // Running this way avoids opening a command window.
                cmdCHM = wxT("CMD /C ") + sPathCHM;
            }
            else{
                cmdCHM = sPathCHM;
            }
        }
        else{
            cmdCHM = sCHMViewer + wxT(" ") + sPathCHM;
        }
        wxProcess *proc = wxProcess::Open(cmdCHM);
        if(proc == NULL){
            AppendToLog(wxString::Format(_("Execution of '%s' failed."), cmdCHM.c_str()), LOG_ERROR);
        }
        else{
            AppendToLog(wxString::Format(_("Process %ld (%s) launched."), proc->GetPid(), cmdCHM.c_str()));
        }
    }
    else{
        AppendToLog(_("HTML Help not found at ") + sPathCHM + wxT("."), LOG_WARNING);
    }
}

/*! \brief Get the project name.
 *
 * \return     wxString    The project name.
 */
wxString DoxyBlocks::GetProjectName()
{
    return Manager::Get()->GetProjectManager()->GetActiveProject()->GetTitle();
}

/*! \brief Get the document path.
 *
 * \return     wxString    The document path.
 *
 */
wxString DoxyBlocks::GetDocPath() const
{
    // killerbot : open question : in case no (active) project : what should the path be ??
    if(!IsProjectOpen())
    {
        return wxEmptyString;
    }

    const cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(!prj)
    {
        wxString sMsg = _("Failed to get the active project!");
        AppendToLog(sMsg, LOG_ERROR);
        return wxEmptyString;
    }
    // Get the doxfile path
    wxString sDoxygenDir = prj->GetBasePath();
    const wxString sOutputDir = m_pConfig->GetOutputDirectory();
    if(!sOutputDir.IsEmpty())
    {
        sDoxygenDir = sDoxygenDir + sOutputDir;
    }
    else
    {
        sDoxygenDir = sDoxygenDir + wxT("doxygen");
    }
    sDoxygenDir = sDoxygenDir + wxFileName::GetPathSeparator();
    wxFileName fnDoxygenDir(sDoxygenDir);
    fnDoxygenDir.Normalize();
    return fnDoxygenDir.GetPathWithSep();
}

/*! \brief Check whether the AutoVersioning plug-in is active.
 *
 * This function iterates over the Extensions node of the project's .cbp file looking for
 * the AutoVersioning node. It is run on project activation and sets the global flag bAutoVersioning.
 */
void DoxyBlocks::CheckForAutoVersioning()
{
    // Set the global variables.
    m_bAutoVersioning = false;
    m_sAutoVersion = wxEmptyString;
    m_sVersionHeader = wxEmptyString;
    cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
    TiXmlNode *node, *child;
    if(project){
        node = project->GetExtensionsNode();
        child = 0;
        while((child = node->IterateChildren(child))){
            wxString sNodeValue = wxString(child->Value(), wxConvUTF8);
            if(sNodeValue.Cmp(wxT("AutoVersioning")) == 0){
                m_bAutoVersioning = true;
                // Get the version header path while we're here.
                TiXmlHandle Handle(child);
                if(const TiXmlElement* pElem = Handle.FirstChildElement("Settings").ToElement()){
                    m_sVersionHeader = wxString(pElem->Attribute("header_path"), wxConvUTF8);
                }
                else{
                    AppendToLog(_("Unable to get the AutoVersion header path."), LOG_ERROR);
                }
                break;
            }
        }
    }
}

/*! \brief Get the Autoversion string.
 *
 * \return wxString    Version string in the form MAJOR.MINOR.BUILD.
 *
 */
wxString DoxyBlocks::GetAutoVersion()
{
    wxString sAutoVersion(wxEmptyString);

    wxFileName fnVersionH(Manager::Get()->GetProjectManager()->GetActiveProject()->GetCommonTopLevelPath() + m_sVersionHeader);
    wxString sPathVersionH(fnVersionH.GetFullPath());
    if(wxFile::Exists(sPathVersionH)){
        wxTextFile fileVersionH(sPathVersionH);
        fileVersionH.Open();
        if(fileVersionH.IsOpened()){
            fileVersionH.GetFirstLine();
            wxString sLine;
            while(!fileVersionH.Eof()){
                sLine = fileVersionH.GetNextLine();
                // Go straight to the full version string.
                if(sLine.Find(wxT("FULLVERSION_STRING")) != wxNOT_FOUND){
                    // Get the text after the first double quote.
                    sLine = sLine.AfterFirst('"');
                    // Get the text before the last decimal point.
                    sAutoVersion = sLine.BeforeLast('.');
                    break;
                }
            }
        }
        else{
            AppendToLog(_("Unable to open the version header."), LOG_ERROR);
        }
    }
    else{
        AppendToLog(_("Version header ") + sPathVersionH + _(" not found."), LOG_ERROR);
    }
    return sAutoVersion;
}

/*! \brief Load configuration settings.
 *
 * \return void
 *
 */
void DoxyBlocks::LoadSettings()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT("editor"));
    int val = 0;
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();
    if(prj){
        TiXmlElement *elem = prj->GetExtensionsNode()->ToElement();
        const TiXmlElement* node = elem->FirstChildElement("DoxyBlocks");
        if(node){
            TiXmlHandle handle(const_cast<TiXmlElement*>(node));
            if(const TiXmlElement* pElem = handle.FirstChildElement("comment_style").ToElement()){
                if(pElem->QueryIntAttribute("block", &val) == TIXML_SUCCESS){
                    m_pConfig->SetBlockComment(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("line", &val) == TIXML_SUCCESS){
                    m_pConfig->SetLineComment(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("doxyfile_project").ToElement()){
                wxString s = wxString(pElem->Attribute("project_number", &val), wxConvUTF8);
                if(!s.IsEmpty()){
                    m_pConfig->SetProjectNumber(s);
                }
                s = wxString(pElem->Attribute("output_directory", &val), wxConvUTF8);
                if(!s.IsEmpty()){
                    m_pConfig->SetOutputDirectory(s);
                }
                s = wxString(pElem->Attribute("output_language", &val), wxConvUTF8);
                if(!s.IsEmpty()){
                    m_pConfig->SetOutputLanguage(s);
                }
                if(pElem->QueryIntAttribute("use_auto_version", &val) == TIXML_SUCCESS){
                    m_pConfig->SetUseAutoVersion(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("doxyfile_build").ToElement()){
                if(pElem->QueryIntAttribute("extract_all", &val) == TIXML_SUCCESS){
                    m_pConfig->SetExtractAll(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("extract_private", &val) == TIXML_SUCCESS){
                    m_pConfig->SetExtractPrivate(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("extract_static", &val) == TIXML_SUCCESS){
                    m_pConfig->SetExtractStatic(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("doxyfile_warnings").ToElement()){
                if(pElem->QueryIntAttribute("warnings", &val) == TIXML_SUCCESS){
                    m_pConfig->SetWarnings(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("warn_if_doc_error", &val) == TIXML_SUCCESS){
                    m_pConfig->SetWarnIfDocError(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("warn_if_undocumented", &val) == TIXML_SUCCESS){
                    m_pConfig->SetWarnIfUndocumented(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("warn_no_param_doc", &val) == TIXML_SUCCESS){
                    m_pConfig->SetWarnNoParamdoc(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("doxyfile_alpha_index").ToElement()){
                if(pElem->QueryIntAttribute("alphabetical_index", &val) == TIXML_SUCCESS){
                    m_pConfig->SetAlphabeticalIndex(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("doxyfile_output").ToElement()){
                if(pElem->QueryIntAttribute("html", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateHTML(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("html_help", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateHTMLHelp(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("chi", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateCHI(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("binary_toc", &val) == TIXML_SUCCESS){
                    m_pConfig->SetBinaryTOC(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("latex", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateLatex(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("rtf", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateRTF(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("man", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateMan(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("xml", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateXML(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("autogen_def", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGenerateAutogenDef(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("perl_mod", &val) == TIXML_SUCCESS){
                    m_pConfig->SetGeneratePerlMod(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("doxyfile_preprocessor").ToElement()){
                if(pElem->QueryIntAttribute("enable_preprocessing", &val) == TIXML_SUCCESS){
                    m_pConfig->SetEnablePreprocessing(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("doxyfile_dot").ToElement()){
                if(pElem->QueryIntAttribute("class_diagrams", &val) == TIXML_SUCCESS){
                    m_pConfig->SetClassDiagrams(static_cast<long>(val));
                }
                if(pElem->QueryIntAttribute("have_dot", &val) == TIXML_SUCCESS){
                    m_pConfig->SetHaveDot(static_cast<long>(val));
                }
            }
            if(const TiXmlElement* pElem = handle.FirstChildElement("general").ToElement()){
                if(pElem->QueryIntAttribute("use_at_in_tags", &val) == TIXML_SUCCESS){
                    m_pConfig->SetUseAtInTags(static_cast<long>(val));
                }
            }
        }
        else{
            // If the DoxyBlocks entry doesn't exist in the project file and "Load Settings Template If No Saved Settings Exist" is set,
            // try loading saved template settings.
            if(cfg->ReadBool(wxT("doxyblocks/load_template"))){
                DoReadPrefsTemplate();
            }
        }
    }
    m_pConfig->SetPathDoxygen(cfg->Read(wxT("doxyblocks/path_doxygen")));
    m_pConfig->SetPathDoxywizard(cfg->Read(wxT("doxyblocks/path_doxywizard")));
    m_pConfig->SetPathHHC(cfg->Read(wxT("doxyblocks/path_hhc")));
    m_pConfig->SetPathDot(cfg->Read(wxT("doxyblocks/path_dot")));
    m_pConfig->SetPathCHMViewer(cfg->Read(wxT("doxyblocks/path_chm_viewer")));
    m_pConfig->SetOverwriteDoxyfile(cfg->ReadBool(wxT("doxyblocks/overwrite_doxyfile"), false));
    m_pConfig->SetPromptBeforeOverwriting(cfg->ReadBool(wxT("doxyblocks/prompt_before_overwriting"), false));
    m_pConfig->SetLoadTemplate(cfg->ReadBool(wxT("doxyblocks/load_template"), false));
    m_pConfig->SetUseInternalViewer(cfg->ReadBool(wxT("doxyblocks/use_internal_viewer"), false));
    m_pConfig->SetRunHTML(cfg->ReadBool(wxT("doxyblocks/run_html"), false));
    m_pConfig->SetRunCHM(cfg->ReadBool(wxT("doxyblocks/run_chm"), false));
}

/*! \brief Save configuration settings.
 *
 * \return void
 *
 * Settings that might change between projects are stored in the project file.
 * Global settings are stored in C::B's config tree.
 * Default values are not written.
 * \todo (Gary#3#): There's probably a more elegant way to avoid writing default vals.
 */
void DoxyBlocks::SaveSettings()
{
    wxString    s;
    bool         bVal;
    ConfigManager* cfg = Manager::Get()->GetConfigManager(wxT("editor"));
   // since rev4332, the project keeps a copy of the <Extensions> element
    // and re-uses it when saving the project (so to avoid losing entries in it
    // if plugins that use that element are not loaded atm).
    // so, instead of blindly inserting the element, we must first check it's
    // not already there (and if it is, clear its contents)
    cbProject* prj = Manager::Get()->GetProjectManager()->GetActiveProject();

    TiXmlElement *elem = prj->GetExtensionsNode()->ToElement();
    TiXmlElement* node = elem->FirstChildElement("DoxyBlocks");
    if(!node){
        node = elem->InsertEndChild(TiXmlElement("DoxyBlocks"))->ToElement();
    }
    node->Clear();

    TiXmlElement Comment("comment_style");
    Comment.SetAttribute("block", m_pConfig->GetBlockComment());
    Comment.SetAttribute("line", m_pConfig->GetLineComment());
    node->InsertEndChild(Comment);
    TiXmlElement DoxyfileProject("doxyfile_project");
    s = m_pConfig->GetProjectNumber();
    if(!s.IsEmpty()){
        DoxyfileProject.SetAttribute("project_number", s.mb_str());
    }
    s = m_pConfig->GetOutputDirectory();
    if(!s.IsEmpty()){
        DoxyfileProject.SetAttribute("output_directory", s.mb_str());
    }
    s = m_pConfig->GetOutputLanguage();
    if(!s.IsSameAs(wxT("English"))){
        DoxyfileProject.SetAttribute("output_language", s.mb_str());
    }
    bVal = m_pConfig->GetUseAutoVersion();
    if(bVal){
        DoxyfileProject.SetAttribute("use_auto_version", bVal);
    }
    node->InsertEndChild(DoxyfileProject);
    TiXmlElement DoxyfileBuild("doxyfile_build");
    bVal = m_pConfig->GetExtractAll();
    if(bVal){
        DoxyfileBuild.SetAttribute("extract_all", bVal);
    }
    bVal = m_pConfig->GetExtractPrivate();
    if(bVal){
        DoxyfileBuild.SetAttribute("extract_private", bVal);
    }
    bVal = m_pConfig->GetExtractStatic();
    if(bVal){
        DoxyfileBuild.SetAttribute("extract_static", bVal);
    }
    node->InsertEndChild(DoxyfileBuild);
    TiXmlElement DoxyfileWarnings("doxyfile_warnings");
    bVal = m_pConfig->GetWarnings();
    if(!bVal){
        DoxyfileWarnings.SetAttribute("warnings", bVal);
        bVal = m_pConfig->GetWarnIfDocError();
    }
    if(!bVal){
        DoxyfileWarnings.SetAttribute("warn_if_doc_error", bVal);
    }
    bVal = m_pConfig->GetWarnIfUndocumented();
    if(bVal){
        DoxyfileWarnings.SetAttribute("warn_if_undocumented", bVal);
    }
    bVal = m_pConfig->GetWarnNoParamdoc();
    if(!bVal){
        DoxyfileWarnings.SetAttribute("warn_no_param_doc", bVal);
    }
    node->InsertEndChild(DoxyfileWarnings);
    bVal = m_pConfig->GetAlphabeticalIndex();
    if(!bVal){
        // Don't insert the node if using the default setting.
        TiXmlElement DoxyfileAlphaIndex("doxyfile_alpha_index");
        DoxyfileAlphaIndex.SetAttribute("alphabetical_index", bVal);
        node->InsertEndChild(DoxyfileAlphaIndex);
    }
    TiXmlElement DoxyfileOutput("doxyfile_output");
    bVal = m_pConfig->GetGenerateHTML();
    if(!bVal){
        DoxyfileOutput.SetAttribute("html", bVal);
    }
    bVal = m_pConfig->GetGenerateHTMLHelp();
    if(bVal){
        DoxyfileOutput.SetAttribute("html_help", bVal);
    }
    bVal = m_pConfig->GetGenerateCHI();
    if(bVal){
        DoxyfileOutput.SetAttribute("chi", bVal);
    }
    bVal = m_pConfig->GetBinaryTOC();
    if(bVal){
        DoxyfileOutput.SetAttribute("binary_toc", bVal);
    }
    bVal = m_pConfig->GetGenerateLatex();
    if(bVal){
        DoxyfileOutput.SetAttribute("latex", bVal);
    }
    bVal = m_pConfig->GetGenerateRTF();
    if(bVal){
        DoxyfileOutput.SetAttribute("rtf", bVal);
    }
    bVal = m_pConfig->GetGenerateMan();
    if(bVal){
        DoxyfileOutput.SetAttribute("man", bVal);
    }
    bVal = m_pConfig->GetGenerateXML();
    if(bVal){
        DoxyfileOutput.SetAttribute("xml", bVal);
    }
    bVal = m_pConfig->GetGenerateAutogenDef();
    if(bVal){
        DoxyfileOutput.SetAttribute("autogen_def", bVal);
    }
    bVal = m_pConfig->GetGeneratePerlMod();
    if(bVal){
        DoxyfileOutput.SetAttribute("perl_mod", bVal);
    }
    node->InsertEndChild(DoxyfileOutput);
    bVal = m_pConfig->GetEnablePreprocessing();
    if(!bVal){
        // Don't insert the node if using the default setting.
        TiXmlElement DoxyfilePreprocessor("doxyfile_preprocessor");
        DoxyfilePreprocessor.SetAttribute("enable_preprocessing", bVal);
        node->InsertEndChild(DoxyfilePreprocessor);
    }
    TiXmlElement DoxyfileDot("doxyfile_dot");
    bVal = m_pConfig->GetClassDiagrams();
    if(bVal){
        DoxyfileDot.SetAttribute("class_diagrams", bVal);
    }
    bVal = m_pConfig->GetHaveDot();
    if(bVal){
        DoxyfileDot.SetAttribute("have_dot", bVal);
    }
    node->InsertEndChild(DoxyfileDot);
    // Paths.
    TiXmlElement General("general");
    cfg->Write(wxT("doxyblocks/path_doxygen"), m_pConfig->GetPathDoxygen());
    cfg->Write(wxT("doxyblocks/path_doxywizard"), m_pConfig->GetPathDoxywizard());
    cfg->Write(wxT("doxyblocks/path_hhc"), m_pConfig->GetPathHHC());
    cfg->Write(wxT("doxyblocks/path_dot"), m_pConfig->GetPathDot());
    cfg->Write(wxT("doxyblocks/path_chm_viewer"), m_pConfig->GetPathCHMViewer());
    // General Options
    cfg->Write(wxT("doxyblocks/overwrite_doxyfile"), m_pConfig->GetOverwriteDoxyfile());
    cfg->Write(wxT("doxyblocks/prompt_before_overwriting"), m_pConfig->GetPromptBeforeOverwriting());
    bVal = m_pConfig->GetUseAtInTags();
    if(bVal){
        General.SetAttribute("use_at_in_tags", bVal);
    }
    cfg->Write(wxT("doxyblocks/load_template"), m_pConfig->GetLoadTemplate());
    cfg->Write(wxT("doxyblocks/use_internal_viewer"), m_pConfig->GetUseInternalViewer());
    cfg->Write(wxT("doxyblocks/run_html"), m_pConfig->GetRunHTML());
    cfg->Write(wxT("doxyblocks/run_chm"), m_pConfig->GetRunCHM());
    node->InsertEndChild(General);
}

/*! \brief Validate the doxygen sub-directory name, removing dots, slashes, colons and tildes.
 *
 * \param path wxString    The string to validate.
 * \return wxString
 *
 */
wxString DoxyBlocks::ValidateRelativePath(wxString path)
{
    path.Replace(wxT("."), wxT(""), true);
    path.Replace(wxT("~"), wxT(""), true);
    wxFileName fn(path, wxEmptyString);
    path = fn.GetPath(0);
    if(path.StartsWith(wxT("/")) || path.StartsWith(wxT("\\"))){
        path.Remove(0, 1);
    }

    return path;
}

/*! \brief A URL was clicked in the log window.
 *
 * \param event wxTextUrlEvent&        The event object.
 * \return void
 *
 */
void DoxyBlocks::OnTextURL(wxTextUrlEvent& event)
{
    if (event.GetId() == ID_LOG_DOXYBLOCKS && event.GetMouseEvent().ButtonDown(wxMOUSE_BTN_LEFT)){
        m_DoxyBlocksLog->OpenLink(event.GetURLStart(), event.GetURLEnd(), m_pConfig->GetUseInternalViewer());
    }
    else{
        event.Skip();
    }
}

void DoxyBlocks::OnWritePrefsTemplate(wxCommandEvent & WXUNUSED(event))
{
    DoWritePrefsTemplate();
}

/*! \brief Write the settings template.
 *
 * \return void
 *
 */
void DoxyBlocks::DoWritePrefsTemplate()
{
    if(m_pConfig->WritePrefsTemplate()){
        AppendToLog(_("Settings template saved."));
    }
    else{
        AppendToLog(_("Error savings settings template."), LOG_ERROR);
    }
}

void DoxyBlocks::OnReadPrefsTemplate(wxCommandEvent & WXUNUSED(event))
{
    DoReadPrefsTemplate();
}

/*! \brief Read the settings template.
 *
 * \return void
 *
 */
void DoxyBlocks::DoReadPrefsTemplate()
{
    if(m_pConfig->ReadPrefsTemplate()){
        AppendToLog(_("Settings template loaded."));
    }
    else{
        AppendToLog(_("Settings template not found."), LOG_WARNING);
    }
}


