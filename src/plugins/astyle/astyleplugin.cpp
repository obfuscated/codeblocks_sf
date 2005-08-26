/***************************************************************
 * Name:      astyle.cpp
 * Purpose:   Code::Blocks plugin
 * Author:    Yiannis Mandravellos<mandrav@codeblocks.org>
 * Created:   05/25/04 10:06:40
 * Copyright: (c) Yiannis Mandravellos
 * License:   GPL
 **************************************************************/

#if defined(__GNUG__) && !defined(__APPLE__)
	#pragma implementation "astyle.h"
#endif

#include "astyleplugin.h"
#include <licenses.h>
#include "astyleconfigdlg.h"
#include <sstream>
#include <string>
#include "formattersettings.h"
#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <cbeditor.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>
#include <wx/strconv.h>

using std::istringstream;
using std::string;

cbPlugin* GetPlugin()
{
	return new AStylePlugin;
}

AStylePlugin::AStylePlugin()
{
	//ctor
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read(_T("data_path"), wxEmptyString);
    wxXmlResource::Get()->Load(resPath + _T("/astyle.zip#zip:*.xrc"));

	m_PluginInfo.name = _T("AStylePlugin");
	m_PluginInfo.title = _T("Source code formatter (AStyle)");
	m_PluginInfo.version = _T("1.1");
	m_PluginInfo.description = _T("Uses AStyle 1.17.0-dev to reformat your sources. Useful when copying code from the net or if you just want to reformat your sources based on a specific style.");
	m_PluginInfo.author = _T("Yiannis Mandravellos");
	m_PluginInfo.authorEmail = _T("mandrav@codeblocks.org");
	m_PluginInfo.authorWebsite = _T("http://www.codeblocks.org");
	m_PluginInfo.thanksTo = _T("AStyle team for the useful library.\nSee http://astyle.sourceforge.net");
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;
	
	ConfigManager::AddConfiguration(m_PluginInfo.title, _T("/astyle"));
}

AStylePlugin::~AStylePlugin()
{
	//dtor
}

void AStylePlugin::OnAttach()
{
	// do whatever initialization you need for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be TRUE...
	// You should check for it in other functions, because if it
	// is FALSE, it means that the application did *not* "load"
	// (see: does not need) this plugin...
}

void AStylePlugin::OnRelease(bool appShutDown)
{
	// do de-initialization for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}

int AStylePlugin::Configure()
{
    AstyleConfigDlg dlg(Manager::Get()->GetAppWindow());
    dlg.ShowModal();

    return 0;
}

int AStylePlugin::Execute()
{
    if (!IsAttached())
        return -1;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetBuiltinActiveEditor();
    if (!ed)
        return 0;
    string edText(ed->GetControl()->GetText().mb_str());
    wxString formattedText;

    astyle::ASFormatter formatter;
    
    // load settings
    FormatterSettings settings;
    settings.ApplyTo(formatter);

    wxString eolChars;
    switch (ed->GetControl()->GetEOLMode())
    {
        case wxSCI_EOL_CRLF: eolChars = _T("\r\n"); break;
        case wxSCI_EOL_CR: eolChars = _T("\r"); break;
        case wxSCI_EOL_LF: eolChars = _T("\n"); break;
    }
    
    //ASStreamIterator iter(edText, eolChars);
    istringstream iter(edText);
    formatter.init(iter);
    while (formatter.hasMoreLines())
    {
        formattedText << _U(formatter.nextLine().c_str());
        if (formatter.hasMoreLines())
            formattedText << eolChars;
    }

	int pos = ed->GetControl()->GetCurrentPos();
	ed->GetControl()->BeginUndoAction();
    ed->GetControl()->SetText(formattedText);
	ed->GetControl()->EndUndoAction();
	ed->GetControl()->GotoPos(pos);
    ed->SetModified(true);

	return 0;
}
