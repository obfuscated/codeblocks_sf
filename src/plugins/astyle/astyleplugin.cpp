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
#include "licenses.h"
#include "astyleconfigdlg.h"
#include "asstreamiterator.h"
#include "formattersettings.h"
#include <manager.h>
#include <editormanager.h>
#include <configmanager.h>
#include <cbeditor.h>
#include <wx/msgdlg.h>
#include <wx/xrc/xmlres.h>
#include <wx/fs_zip.h>

cbPlugin* GetPlugin()
{
	return new AStylePlugin;
}

AStylePlugin::AStylePlugin()
{
	//ctor
    wxFileSystem::AddHandler(new wxZipFSHandler);
    wxXmlResource::Get()->InitAllHandlers();
    wxString resPath = ConfigManager::Get()->Read("data_path", wxEmptyString);
    wxXmlResource::Get()->Load(resPath + "/astyle.zip");

	m_PluginInfo.name = "AStylePlugin";
	m_PluginInfo.title = "AStylePlugin source code formatter";
	m_PluginInfo.version = "1.0";
	m_PluginInfo.description = "Uses AStylePlugin 1.15.3 to reformat your sources. Useful when copying code from the net and it pastes non-formatted.";
	m_PluginInfo.author = "Yiannis Mandravellos";
	m_PluginInfo.authorEmail = "mandrav@codeblocks.org";
	m_PluginInfo.authorWebsite = "http://www.codeblocks.org";
	m_PluginInfo.thanksTo = "AStylePlugin team for the beautiful tool.";
	m_PluginInfo.license = LICENSE_GPL;
	m_PluginInfo.hasConfigure = true;
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

void AStylePlugin::OnRelease()
{
	// do de-initialization for your plugin
	// NOTE: after this function, the inherited member variable
	// m_IsAttached will be FALSE...
}

int AStylePlugin::Configure()
{
    AstyleConfigDlg dlg(Manager::Get()->GetAppWindow());
    if (dlg.ShowModal() == wxID_OK)
    {
    }
    return 0;
}

int AStylePlugin::Execute()
{
    if (!IsAttached())
        return -1;
    cbEditor* ed = Manager::Get()->GetEditorManager()->GetActiveEditor();
    if (!ed)
        return 0;
    wxString edText = ed->GetControl()->GetText();
    wxString formattedText;

    astyle::ASFormatter formatter;
    
    // load settings
    FormatterSettings settings;
    settings.ApplyTo(formatter);

    wxString eolChars;
    switch (ed->GetControl()->GetEOLMode())
    {
        case wxSTC_EOL_CRLF: eolChars = "\r\n"; break;
        case wxSTC_EOL_CR: eolChars = "\r"; break;
        case wxSTC_EOL_LF: eolChars = "\n"; break;
    }
    
    formatter.init(new ASStreamIterator(edText, eolChars));
    while (formatter.hasMoreLines())
    {
        formattedText << formatter.nextLine().c_str();
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
