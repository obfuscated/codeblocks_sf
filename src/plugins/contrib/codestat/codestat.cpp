/***************************************************************
 * Name:      codestat.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Zlika
 * Created:   11/09/2005
 * Copyright: (c) Zlika
 * License:   GPL
  **************************************************************/

#include "codestat.h"

CB_IMPLEMENT_PLUGIN(CodeStat);

CodeStat::CodeStat()
{
	 wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::GetDataFolder();
    wxXmlResource::Get()->Load(resPath + _T("/codestat.zip#zip:*.xrc"));

    m_PluginInfo.name = _T("CodeStatistics");
    m_PluginInfo.title = _("Code Statistics");
    m_PluginInfo.version = _("0.1");
    m_PluginInfo.description = _("A simple plugin for counting code, comments and empty lines of a project.");
    m_PluginInfo.author = _("Zlika");
    m_PluginInfo.authorEmail = _("");
    m_PluginInfo.authorWebsite = _("");
    m_PluginInfo.thanksTo = _("All the Code::Blocks team!");
    m_PluginInfo.license = LICENSE_GPL;
}

CodeStat::~CodeStat()
{

}

void CodeStat::OnAttach()
{
    // do whatever initialization you need for your plugin
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be TRUE...
    // You should check for it in other functions, because if it
    // is FALSE, it means that the application did *not* "load"
    // (see: does not need) this plugin...
}

void CodeStat::OnRelease(bool appShutDown)
{
    // do de-initialization for your plugin
    // if appShutDown is false, the plugin is unloaded because Code::Blocks is being shut down,
    // which means you must not use any of the SDK Managers
    // NOTE: after this function, the inherited member variable
    // m_IsAttached will be FALSE...
}

cbConfigurationPanel* CodeStat::GetConfigurationPanel(wxWindow* parent)
{
    // if not attached, exit
    if (!m_IsAttached)
        return 0;

    LanguageDef languages[NB_FILETYPES];
    LoadSettings(languages);
    CodeStatConfigDlg* dlg = new CodeStatConfigDlg(parent, languages);
    return dlg;
}

int CodeStat::Execute()
{
    // if not attached, exit
    if (!m_IsAttached)
        return -1;

   cbProject* project = Manager::Get()->GetProjectManager()->GetActiveProject();
   // if no project open, exit
	if (!project)
	{
		wxString msg = _("You need to open a project\nbefore using the plugin!");
		wxMessageBox(msg, _("Error"), wxICON_ERROR | wxOK);
		Manager::Get()->GetMessageManager()->DebugLog(msg);
		return -1;
	}

    dlg = new CodeStatExecDlg(Manager::Get()->GetAppWindow());

    // Load the language settings and launch the main function
    LanguageDef languages[NB_FILETYPES];
    LoadSettings(languages);
    if(dlg->Execute(languages) != 0)
        return -1;

    return 0;
}

// Definition of the comments for each language
void CodeStat::LoadSettings(LanguageDef languages[NB_FILETYPES])
{
	// C/C++ style comments
	languages[0].name = _T("C/C++");
	languages[0].ext.Add(_T("c"));
	languages[0].ext.Add(_T("cpp"));
	languages[0].ext.Add(_T("h"));
	languages[0].ext.Add(_T("hpp"));
	languages[0].single_line_comment = _T("//");
    languages[0].multiple_line_comment[0] = _T("/*");
    languages[0].multiple_line_comment[1] = _T("*/");

   // Java style comments
	languages[1].name = _T("Java");
	languages[1].ext.Add(_T("java"));
	languages[1].single_line_comment = _T("//");
    languages[1].multiple_line_comment[0] = _T("/*");
    languages[1].multiple_line_comment[1] = _T("*/");

   // Python style comments
	languages[2].name = _T("Python");
	languages[2].ext.Add(_T("py"));
	languages[2].single_line_comment = _T("#");
    languages[2].multiple_line_comment[0] = _T("");
    languages[2].multiple_line_comment[1] = _T("");

   // Perl style comments
	languages[3].name = _T("Perl");
	languages[3].ext.Add(_T("pl"));
	languages[3].single_line_comment = _T("#");
    languages[3].multiple_line_comment[0] = _T("");
    languages[3].multiple_line_comment[1] = _T("");

   // ASM style comments
	languages[4].name = _T("ASM");
	languages[4].ext.Add(_T("asm"));
	languages[4].single_line_comment = _T(";");
    languages[4].multiple_line_comment[0] = _T("");
    languages[4].multiple_line_comment[1] = _T("");

   // Pascal style comments
	languages[5].name = _T("Pascal");
	languages[5].ext.Add(_T("pas"));
	languages[5].single_line_comment = _T("");
    languages[5].multiple_line_comment[0] = _T("{");
    languages[5].multiple_line_comment[1] = _T("}");

   // Matlab style comments
	languages[6].name = _T("Matlab");
	languages[6].ext.Add(_T("m"));
	languages[6].single_line_comment = _T("%");
    languages[6].multiple_line_comment[0] = _T("");
    languages[6].multiple_line_comment[1] = _T("");
}
